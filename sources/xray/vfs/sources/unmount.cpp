////////////////////////////////////////////////////////////////////////////
//	Created		: 08.04.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/vfs/virtual_file_system.h>
#include <xray/vfs/mount_history.h>
#include <xray/vfs/mount_args.h>
#include <xray/fs/path_string_utils.h>
#include <xray/fs/path_part_iterator.h>
#include <xray/fs/path_string_utils.h>
#include <xray/fs/physical_path_iterator.h>
#include "unmount.h"
#include "mount_physical_path.h"
#include "branch_locks.h"
#include "free_node.h"
#include "mount_utils.h"

namespace xray {
namespace vfs {


threading::atomic32_type	s_global_unmounts_counter	=	0; // for debug

u32 get_global_unmounts_count	()
{
	return	s_global_unmounts_counter;
}

base_node<> *   get_attach_node			(mount_root_node_base<> * node)
{
	archive_folder_mount_root_node<> const * archive_mount	=	node_cast<archive_folder_mount_root_node>(node);
	if ( !archive_mount )
		return								NULL;
	return									archive_mount->attach_node;
}
	
unmounter::unmounter					(query_mount_arguments & m_args, virtual_file_system & file_system)
		: m_args(m_args), m_hashset(file_system.hashset), m_root_node_to_unmount(NULL), m_file_system(file_system)
{	
	if ( file_system.on_unmount_started )
		file_system.on_unmount_started		();

	m_args.convert_pathes_to_absolute		();
	R_ASSERT								(m_args.mount_ptr);
	R_ASSERT								(m_args.mount_ptr->get_reference_count() == 0 
												|| 
											 m_args.submount_type == submount_type_hot_unmount);

	m_root_node_to_unmount				=	m_args.mount_ptr->get_mount_root();

	R_ASSERT								(m_root_node_to_unmount);
	R_ASSERT								(m_args.root_write_lock);

	if ( m_args.submount_type == submount_type_hot_unmount )
		hot_unmount							();
	else
		unmount								();

	if ( m_args.unlock_after_mount )
		unlock_branch						(m_args.root_write_lock, lock_type_write);
	
	if ( m_args.callback )
		m_args.callback						(mount_result(NULL, result_success));
}

pcstr   get_mount_log_type				(base_node<> * attach_node, mount_root_node_base<> * mount_root_base)
{
	base_node<> * const mount_root_node	=	node_cast<base_node>(mount_root_base);
	if ( attach_node )
	{
		if ( attach_node->is_external_sub_fat() )
			return							"external sub_fat";
		else if ( attach_node->is_sub_fat() )
			return							"sub_fat";
		else
			return							"auto-archive";
	}
	else if ( mount_root_node->is_physical() )
	{
		return								mount_root_node->is_file() ? "physical file" : "physical folder";
	}

	R_ASSERT								(mount_root_node->is_folder());
	return									"archive folder";
}

void   unmounter::unmount				()
{
	u32 const hash						=	fs_new::path_crc32(m_args.virtual_path);
	base_node<> * attach_node			=	get_attach_node(m_root_node_to_unmount);

	pcstr const mount_log_type			=	get_mount_log_type(attach_node, m_root_node_to_unmount);

	LOG_DEBUG								("unmounting %s started '%s' on '%s'", 
											 mount_log_type,
											 m_args.get_physical_path().c_str(),
											 m_args.virtual_path.c_str());
	
	base_node<> * node_to_unmount		=	NULL;
	base_node<> * overlap_of_node_to_unmount	=	NULL;	
	is_part_of_mount predicate				(m_root_node_to_unmount);
	recursive_unmount_node					(m_args.virtual_path, hash, predicate, node_to_unmount, overlap_of_node_to_unmount);

	R_ASSERT								(node_to_unmount == node_cast<base_node>(m_root_node_to_unmount));
	
	base_node<> * mount_root_node		=	node_to_unmount;
	mount_helper_node<> * mount_root_helper_parent	=	mount_root_node->get_mount_helper_parent();
	
	if ( attach_node )
	{
		unlink_from_attach_node				(mount_root_node, hash, overlap_of_node_to_unmount, attach_node);
		
		if ( attach_node->is_physical() )
		{
			physical_file_node<> * file_node	=	node_cast<physical_file_node>(attach_node);
			R_ASSERT						(file_node);
			file_node->set_is_mounted		(false); // mark file node that automatic-archive is unmounted
		}
	}
	else
	{
		LOG_DEBUG							("unmounting helper branch");
		unmount_helper_branch				(mount_root_helper_parent, mount_root_node, overlap_of_node_to_unmount, hash);
	}

	threading::interlocked_increment		(s_global_unmounts_counter);
	LOG_INFO								("unmounted %s '%s' on '%s'", 
											 mount_log_type,
											 m_args.get_physical_path().c_str(),
											 m_args.virtual_path.c_str());
}

void   unmounter::unlink_from_attach_node	(base_node<> * mount_root, u32 hash, base_node<> * overlapper, base_node<> * attach_node)
{
	vfs::replace_and_free_what_node			(m_args.virtual_path,
											 hash,
											 m_file_system,
											 mount_root,
											 attach_node, 
											 overlapper,
											 m_args.root_write_lock, 
											 m_args.allocator);
}

mount_root_node_base<> *   unmounter::find_mount_root (u32 virtual_path_hash)
{
	std::pair<overlapped_node_initializer, overlapped_node_initializer> begin_end	= m_hashset.equal_range(m_args.virtual_path.c_str(), virtual_path_hash, lock_type_write);
	overlapped_node_iterator it			=	begin_end.first;
	overlapped_node_iterator it_end		=	begin_end.second;
	for ( ;it != it_end; ++it )
	{
		base_node<> * node				=   & * it;
		if ( mount_root_node_base<> * mount_root = node_cast<mount_root_node_base>(node) )
		{
			if ( m_args.virtual_path != mount_root->virtual_path )
				continue;
			if ( m_args.type == mount_type_physical_path )
			{
				if ( m_args.physical_path != mount_root->physical_path )
					continue;
			}
			else
			{
				if ( m_args.fat_physical_path != mount_root->physical_path )
					continue;
			}

			return							mount_root;
		}
	}

	return									NULL;
}

} // namespace vfs
} // namespace xray