////////////////////////////////////////////////////////////////////////////
//	Created		: 08.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mount_archive.h"
#include "branch_locks.h"
#include <xray/fs/file_type_pointer.h>
#include <xray/fs/device_file_system_proxy.h>
#include <xray/fs/path_string_utils.h>
#include <xray/fs/synchronous_device_interface.h>
#include "fat_header.h"
#include "mount_utils.h"
#include <xray/vfs/virtual_file_system.h>

namespace xray {
namespace vfs {

using namespace fs_new;

sub_fat_resource *	find_sub_fat_parent	(sub_fat_resource * sub_fat, virtual_file_system & vfs_data);

archive_mounter::archive_mounter		(query_mount_arguments & args, virtual_file_system & vfs_data)
	:	mounter(args, vfs_data), m_mount_root_base(NULL), m_reverse_byte_order(false), m_nodes_buffer(NULL), m_branch_lock(NULL)
{
	m_args.convert_pathes_to_absolute			();
 
	if ( try_mount_from_history() )
		return;
		
	vfs_mount * mount_ptr_raw			=	XRAY_NEW_WITH_CHECK_IMPL(m_args.allocator, mount_ptr_raw, vfs_mount)(NULL, m_args.allocator);
	if ( !mount_ptr_raw )
	{
		finish_with_out_of_memory	();
		return;
	}

	m_mount_ptr							=	mount_ptr_raw;

	if ( m_args.submount_node )
	{
		vfs_mount_ptr parent_mount		=	mount_of_node(m_args.submount_node);
		m_mount_id						=	parent_mount->get_mount_root()->mount_id;
	}

	if ( m_args.submount_type == submount_type_subfat )
	{
		mount_sub_fat						();
	}
	else
	{
		if ( m_args.mount_id )
			m_mount_id					=	m_args.mount_id;

		if ( !m_mount_id )
			m_mount_id					=	next_mount_id();

		mount_archive						();
	}
}

void   archive_mounter::mount_archive	()
{
	if ( m_args.asynchronous_device )
	{
		synchronous_device_interface		device(m_args.asynchronous_device, m_args.allocator);
		if ( device.out_of_memory() )
		{
			finish_with_out_of_memory		();
			return;
		}

		mount_archive_impl					(device);
	}
	else
		mount_archive_impl					(* m_args.synchronous_device);

	if ( m_result == result_out_of_memory )
		m_mount_ptr						=	NULL;

	finish									(mount_result(m_mount_ptr, m_result));
}

void   archive_mounter::mount_archive_impl	(synchronous_device_interface & device)
{
	file_type_pointer fat_file				(m_args.fat_physical_path.c_str(), device, file_mode::open_existing, file_access::read, assert_on_fail_false);
	if ( !fat_file)
	{
		m_result						=	result_fail;
 		return;
	}

	if ( m_args.submount_node )
	{
		base_folder_node<> * root_parent	=	node_cast<base_folder_node>(m_args.parent_of_submount_node);
		R_ASSERT							(root_parent);
		mount_archive_to_parent				(fat_file, root_parent, device);

		if ( m_result != result_out_of_memory )
		{
			physical_file_node<> * file_node	=	node_cast<physical_file_node>(m_args.submount_node);
			R_ASSERT						(file_node);
			file_node->set_is_mounted		(true);
		}
	}
	else
	{
		u32 const max_helper_nodes		=	m_args.virtual_path.count_of(virtual_path_string::separator) + 1;
		buffer_vector< mount_helper_node<> * >	helper_nodes(ALLOCA(sizeof(mount_helper_node<> *) * max_helper_nodes), max_helper_nodes);
		if ( !allocate_mount_branch(& helper_nodes) )
		{
			finish_with_out_of_memory		();
			return;
		}

		base_node<> * 	branch_node		=	NULL;
		u32				hash			=	0;
		add_mount_branch					(helper_nodes, branch_node, m_args.root_write_lock, & hash);

		mount_archive_to_parent				(fat_file, branch_node ? node_cast<base_folder_node>(branch_node) : NULL, device);
	}
}

void   archive_mounter::mount_archive_to_parent	(file_type *						fat_file, 
												 base_folder_node<> *				parent_of_mount_root, 
												 synchronous_device_interface &		device)
{
	fat_header								header;
 	device->read							(fat_file, & header, sizeof(header));
 	m_reverse_byte_order				=	header.is_big_endian() != platform::big_endian();
 	if ( m_reverse_byte_order )
 		header.reverse_bytes				();
 
	// allocating +1 for secure access to last node with interlocked operations on node's association lock
	m_nodes_buffer						=	(pstr)XRAY_MALLOC_IMPL(m_args.allocator, header.buffer_size + 1, "archive nodes");
	if ( !m_nodes_buffer )
	{
		m_result						=	result_out_of_memory;
		return;
	}

 	device->read							(fat_file, m_nodes_buffer, header.buffer_size);

	archive_folder_mount_root_node<> *		mount_root	=	reinterpret_cast<archive_folder_mount_root_node<> *>
											(m_nodes_buffer);

	mount_root->mount_size				=	header.buffer_size;

	mount_fat								(mount_root, parent_of_mount_root);
	
	add_to_mount_history					(m_mount_ptr.c_ptr(), m_file_system);
 
	pcstr mount_type					=	m_args.submount_node ? "auto-archive" : "archive";
 	LOG_INFO								("mounted %s '%s' on '%s'", 
											 mount_type,
											 m_args.archive_physical_path.c_str(),
 											 m_args.virtual_path.c_str());
}

void   archive_mounter::mount_fat		(archive_folder_mount_root_node<> *	mount_root, base_folder_node<> * parent)
{
	m_mount_root_base					=	mount_root;

	m_mount_root_base->mount_id			=	m_mount_id;
	m_mount_root_base->mount_operation_id	=	next_mount_operation_id();

	mount_root->nodes_buffer			=	m_nodes_buffer;
	mount_root->attach_node				=	m_args.submount_node; 

	recursive_fixup_node					(node_cast<base_node>(mount_root), (pstr)mount_root);

	mount_root->attach_node				=	m_args.submount_node;
	mount_root->allocator				=	m_args.allocator;
	mount_root->file_system				=	& m_file_system;
	strings::copy							(mount_root->virtual_path_holder, m_args.virtual_path.c_str());
	strings::copy							(mount_root->archive_path_holder, m_args.archive_physical_path.c_str());
	strings::copy							(mount_root->descriptor, m_args.descriptor.c_str());
	strings::copy							(mount_root->fat_path_holder, m_args.get_physical_path().c_str());
	mount_root->device					=	m_args.synchronous_device ? m_args.synchronous_device->get_device()->get_device_file_system_interface() : NULL;
	mount_root->async_device			=	m_args.asynchronous_device;
	mount_root->watcher_enabled			=	m_args.watcher_enabled;
	m_mount_root_base->virtual_path		=	mount_root->virtual_path_holder;
	m_mount_root_base->physical_path	=	mount_root->fat_path_holder;

	if ( m_args.submount_type == submount_type_automatic_archive )
		mount_root->node->set_name			(m_args.submount_node->get_name());

	u32	const hash						=	path_crc32(m_args.virtual_path);

	base_node<> * mount_root_base		=	node_cast<base_node>(mount_root);
	recursive_merge							(m_args.virtual_path, hash, mount_root_base, parent);

	m_mount_ptr->set_mount_root				(mount_root);
	m_mount_root_base->mount			=	m_mount_ptr.c_ptr();
}

void   archive_mounter::recursive_merge	(virtual_path_string &	path, 
										 u32 					hash,
										 base_node<> *			node,
										 base_folder_node<> *	parent)
{
	base_node<> * node_children			=	node->get_first_child();
	if ( node->is_folder() )
		node_cast<base_folder_node>(node)->set_first_child	(NULL);
	if ( parent )
	{
		merge_node_with_tree				(path, hash, node, parent);
	}
	else
	{
		R_ASSERT							(path.length() == 0);
		R_ASSERT							(node_cast<base_node>(m_mount_root_base) == node);
		
		merge_root_node						(hash, node, m_args.root_write_lock);
	}

	u32 saved_path_length				=	path.length();
	base_folder_node<> * node_folder	=	node->is_link() ? NULL : node_cast<base_folder_node>(node);
	for ( base_node<> * child			=	node_children;
						child; )
	{
		base_node<> * next_child		=	child->get_next();

		virtual_path_string	& child_path	=	path;
		child_path.append_path				(child->get_name());
		u32 child_hash					=	crc32(child->get_name(), strings::length(child->get_name()), hash);
		recursive_merge						(child_path, child_hash, child, node_folder);

		path.set_length						(saved_path_length);
		child							=	next_child;
	}

	if ( parent )
		remove_marked_to_unlink_from_parent	(parent);
}

} // namespace vfs
} // namespace xray