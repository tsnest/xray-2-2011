////////////////////////////////////////////////////////////////////////////
//	Created		: 05.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/vfs/virtual_file_system.h>
#include "mount_physical_path.h"
#include "mount_utils.h"
#include "overlapped_node_iterator.h"

namespace xray {
namespace vfs {

using namespace fs_new;

base_node<> *   physical_path_mounter::add_physical_node (virtual_path_string const &	name, 
														  virtual_path_string const &	virtual_path, 
														  u32							virtual_path_hash,
														  native_path_string const &	physical_path,
														  base_node<> *					parent)
{

	physical_path_info const info		=	(* m_device)->get_physical_path_info(physical_path);
	if ( !info.exists() )
		return								NULL;

	base_node<> *	node				=	NULL;
	if ( info.is_file() )
	{
		physical_file_node<> * const file_node	=	physical_file_node<>::create
											(m_args.allocator, m_mount_root_base, name, (u32)info.get_file_size());
		node							=	node_cast<base_node>(file_node);
	}
	else
	{
		physical_folder_node<> * const folder_node	=	physical_folder_node<>::create(m_args.allocator, 
																					   m_mount_root_base, name);
		node							=	node_cast<base_node>(folder_node);
	}

	if ( !node )
	{
		m_result						=	result_out_of_memory;
		return								NULL;
	}

	merge_node_with_tree					(virtual_path, virtual_path_hash, node, node_cast<base_folder_node>(parent));
	return									node;
}

base_node<> *   physical_path_mounter::find_node_on_virtual_path (base_node<> * * out_overlapper)
{	
	vfs_hashset::begin_end_pair	begin_end	=	m_file_system.hashset.equal_range(m_args.virtual_path.c_str(), lock_type_read);
	overlapped_node_iterator	it		=	begin_end.first;
	overlapped_node_iterator	it_end	=	begin_end.second; 

	* out_overlapper					=	NULL;
	for ( ; it != it_end; ++it )
	{
		base_node<> * const node		=	it.c_ptr();
		if ( mount_id_of_node(node) == m_mount_id )
			return							node;

		* out_overlapper				=	node;
	}

	return									NULL;
}

void   physical_path_mounter::mount_hot ()
{
	R_ASSERT								(m_args.submount_node);
	R_ASSERT								(m_args.submount_node->is_physical() && 
											 m_args.submount_node->is_folder() && 
											 m_args.submount_node->is_mount_root());

	LOG_INFO								("hot mounting of '%s' on '%s'", 
											 m_args.physical_path.c_str(), m_args.virtual_path.c_str());

	m_mount_root_base					=	node_cast<mount_root_node_base>(m_args.submount_node);
	m_mount_id							=	m_mount_root_base->mount_id;

	base_node<> * overlapper			=	NULL;
	base_node<> * node					=	find_node_on_virtual_path(& overlapper);
	
	if ( !node )
		mount_hot_branch					();
	else
		LOG_INFO							("boo, node '%s' already exists...", m_args.virtual_path.c_str());
}

void   physical_path_mounter::mount_hot_branch ()
{
	R_ASSERT								(m_args.recursive == recursive_false);
	virtual_path_string						base_path;
	m_args.submount_node->get_full_path		(& base_path);

	virtual_path_string						relative_path;
	convert_to_relative_path				(& relative_path, m_args.virtual_path, base_path);
	u32 virtual_path_hash				=	path_crc32(base_path);
	
	virtual_path_string		it_virtual_path		=	base_path;
	native_path_string		it_physical_path	=	m_mount_root_base->physical_path;
	base_node<> * parent				=	NULL;
	for ( path_part_iterator			it		=	relative_path.begin_part(include_empty_string_in_iteration_true), 
										it_end	=	relative_path.end_part();
										it		!=	it_end; 
										++it )
	{
		virtual_path_string					path_part;
		it.assign_to_string					(path_part);

		it_virtual_path.append_path			(path_part);
		
		if ( path_part.length() )
			it_physical_path.appendf		("%c%s", native_path_string::separator, path_part.c_str());

		virtual_path_hash				=	path_crc32(path_part, virtual_path_hash);

		base_node<> * node				=	find_node_of_mount(m_file_system.hashset, it_virtual_path, virtual_path_hash, m_mount_id);

		if ( !node )
			node						=	add_physical_node(path_part, it_virtual_path, virtual_path_hash, it_physical_path, parent);

		if ( m_result == result_out_of_memory )
			return;

		if ( !node )
		{
			LOGI_INFO						("watcher", "hot mount of '%s' failed: nothing on such path", it_physical_path.c_str());
			return;
		}

		if ( node->is_physical() && node->is_folder() )
		{
			physical_folder_node<> * folder	=	node_cast<physical_folder_node>(node);
			if ( !folder->is_scanned(false) )
				mount_physical_folder		(it_virtual_path, folder, it_physical_path, virtual_path_hash);

			if ( m_result == result_out_of_memory )
				return;
		}

		parent							=	node;
	}

	m_mount_ptr							=	m_mount_root_base->mount;
}

} // namespace vfs
} // namespace xray


