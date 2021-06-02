////////////////////////////////////////////////////////////////////////////
//	Created		: 17.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mounter.h"
#include "mount_physical_path.h"
#include <xray/vfs/mount_history.h>
#include <xray/vfs/mount_args.h>
#include "mount_helper_node.h"
#include "branch_locks.h"
#include "mount_utils.h"
#include <xray/vfs/virtual_file_system.h>
#include <xray/fs/path_string_utils.h>
#include <xray/fs/path_part_iterator.h>
#include <xray/fs/path_string_utils.h>
#include <xray/fs/physical_path_iterator.h>

namespace xray {
namespace vfs {

using namespace fs_new;

physical_path_mounter::physical_path_mounter	(query_mount_arguments & args, virtual_file_system & vfs_data) 
	: mounter(args, vfs_data), m_device(NULL)
{
	m_args.convert_pathes_to_absolute		();

	if ( try_mount_from_history() )
		return;

	if ( args.submount_node == NULL )	// if root physical mount?
	{		
		vfs_mount * mount_ptr_raw		=	XRAY_NEW_WITH_CHECK_IMPL(m_args.allocator, mount_ptr_raw, vfs_mount)(NULL, m_args.allocator);
		if ( !mount_ptr_raw )
		{
			finish_with_out_of_memory	();
			return;
		}

		m_mount_ptr						=	mount_ptr_raw;
	}

	if ( m_args.asynchronous_device )
	{
		synchronous_device_interface	device(m_args.asynchronous_device, m_args.allocator);
		if ( device.out_of_memory() )
		{
			finish_with_out_of_memory	();
			return;
		}

		m_device						=	& device;
		mount_impl							();
	}	
	else
	{
		m_device						=	m_args.synchronous_device;
		mount_impl							();
	}

	finish									(mount_result(m_mount_ptr, m_result));
}

void   physical_path_mounter::mount_impl	()
{
	if ( m_args.submount_node )
	{
		if ( m_args.submount_type == submount_type_lazy )
			mount_lazy						();
		else if ( m_args.submount_type == submount_type_hot_mount )
			mount_hot						();
		else
			NOT_IMPLEMENTED					(return);
	}
	else
		mount_root							();
}

void   physical_path_mounter::mount_root	()
{
	m_mount_id							=	m_args.mount_id ? m_args.mount_id : next_mount_id();
	physical_path_info path_info		=	(* m_device)->get_physical_path_info(m_args.physical_path);

	if ( !path_info.exists() ) 
	{
		LOG_ERROR							("mount physical path doesnt exist : '%s'", m_args.physical_path.c_str());
		m_result						=	result_fail;
		return;
	}

	LOG_DEBUG								("mounting started %s '%s' on '%s'", 
											 path_info.is_folder() ? "folder" : "file",
											 m_args.physical_path.c_str(), m_args.virtual_path.c_str());

	u32 const max_helper_nodes			=	m_args.virtual_path.count_of(virtual_path_string::separator) + 1;
	buffer_vector< mount_helper_node<> * >	helper_nodes(ALLOCA(sizeof(mount_helper_node<> *) * max_helper_nodes), max_helper_nodes);
	if ( !allocate_mount_branch(& helper_nodes) )
	{
		finish_with_out_of_memory	();
		return;
	}

	mount_root_node_base<> *   root_node	=	create_mount_root(path_info.is_folder(), m_args.virtual_path);
	if ( !root_node )
	{
		free_mount_branch					(helper_nodes);
		finish_with_out_of_memory	();
		return;
	}

	base_node<> *	branch_node			=	NULL;
	u32				hash				=	0;
	add_mount_branch						(helper_nodes, branch_node, m_args.root_write_lock, & hash);

	add_mount_root							(root_node, m_args.virtual_path, & hash, branch_node, m_args.root_write_lock);
	
	m_mount_root_base					=	root_node;

	if ( path_info.is_file() )
	{
		physical_file_mount_root_node<> *	mount_root = node_cast<physical_file_mount_root_node>(m_mount_root_base);
		ASSERT								(mount_root);
		mount_root->file.m_size			=	(u32)path_info.get_file_size();
		mount_root->watcher_enabled		=	watcher_enabled_false;
	}
	else if ( m_args.recursive == recursive_true )
	{
		physical_folder_mount_root_node<> *	mount_root = node_cast<physical_folder_mount_root_node>(m_mount_root_base);

		native_path_string	absolute_path	=	m_args.physical_path;
		virtual_path_string	folder_path		=	m_args.virtual_path;
		mount_physical_folder				(folder_path, 
											 node_cast<physical_folder_node>(mount_root), 
											 absolute_path, 
											 hash);			
	}

	if ( path_info.is_file() )
		LOG_INFO							("mounting file '%s' on '%s'", m_args.physical_path.c_str(), m_args.virtual_path.c_str());
	else
		LOG_INFO							("mounted folder '%s' on '%s'", m_args.physical_path.c_str(), m_args.virtual_path.c_str());

	m_mount_ptr->set_mount_root				(m_mount_root_base);
	m_mount_root_base->mount			=	m_mount_ptr.c_ptr();

#pragma message(XRAY_TODO("how about checking for lazy mount ? mount_flags_enum"))
	add_to_mount_history					(m_mount_ptr.c_ptr(), m_file_system);
}

void   physical_path_mounter::mount_physical_folder	(virtual_path_string &		folder_path,
													 physical_folder_node<> *	folder, 
													 native_path_string &		absolute_path,
													 u32 const					folder_hash)
{
	physical_path_info const path_info	=	(* m_device)->get_physical_path_info(absolute_path);
	R_ASSERT								(path_info.exists());

	u32 folder_size						=	0;
	node_list								new_nodes;

	bool out_of_memory					=	false;

	for ( physical_path_iterator	it		=	path_info.children_begin(),
									it_end	=	path_info.children_end();
									it		!=	it_end;
											++it )
	{
		virtual_path_string					name;
		it.get_name							(& name);
		name.make_lowercase					();

		base_node<> * node				=	NULL;
		physical_file_node<> *		file_node	=	NULL;
		physical_folder_node<> *	folder_node	=	NULL;
		if ( it.is_file() )
		{
			file_node					=	physical_file_node<>::create
												(m_args.allocator, m_mount_root_base, name, (u32)it.get_file_size());
			if ( !file_node )
			{
				out_of_memory			=	true;
				break;
			}

			file_node->set_is_allocated_in_parent_arena	();

			node						=	node_cast<base_node>(file_node);																				 
		}
		else
		{
			folder_node					=	physical_folder_node<>::create(m_args.allocator, m_mount_root_base, name);
			if ( !folder_node )
			{
				out_of_memory			=	true;
				break;
			}

			folder_node->set_is_allocated_in_parent_arena	();

			node						=	node_cast<base_node>(folder_node);
		}

		folder_size						+=	node->sizeof_with_name();
		folder_size						=	math::align_up(folder_size, (u32)sizeof(pvoid));

		new_nodes.push_back					(node);
	}

	if ( out_of_memory )
	{
		m_result						=	result_out_of_memory;
		free_node_list						(new_nodes);
		return;
	}

	if ( !new_nodes.empty() )
	{
		flatten_helper_nodes_and_merge	(new_nodes, folder_path, folder, folder_hash, folder_size, absolute_path);
		remove_marked_to_unlink_from_parent	(node_cast<base_folder_node>(folder));
	}

	out_of_memory						=	(m_result == result_out_of_memory);
	folder->set_is_scanned					((m_args.recursive == recursive_true) && !out_of_memory);
}

void   physical_path_mounter::flatten_helper_nodes_and_merge	(node_list &				new_nodes,
																	 virtual_path_string &		folder_path,
																	 physical_folder_node<> *	folder, 
																	 u32						folder_hash,
																	 u32						folder_size,
																	 native_path_string &		absolute_path)
{
	u32 const saved_absolute_path_length	=	absolute_path.length();
	u32 const saved_folder_path			=	folder_path.length();

	ASSERT									(folder_size);
	mutable_buffer children_arena			(XRAY_MALLOC_IMPL(m_args.allocator, folder_size, "children_nodes_arena"), folder_size);
	if ( !children_arena )
	{
		m_result						=	result_out_of_memory;
		free_node_list						(new_nodes);
		return;
	}

	R_ASSERT								(!folder->children_arena);
	folder->children_arena				=	children_arena;
	u32 child_offs						=	0;
	while ( !new_nodes.empty() )
	{
		base_node<> * source_node		=	new_nodes.pop_front();
		physical_file_node<> * file_node	=	NULL;
		physical_folder_node<> * folder_node	=	NULL;
		base_node<> * node				=	NULL;

		mutable_buffer const child_buffer	=	children_arena + child_offs;

		u32 node_size					=	source_node->sizeof_with_name();

		if ( source_node->is_file() )
		{
			physical_file_node<> * source_file	=	node_cast<physical_file_node>(source_node);
			file_node					=	(physical_file_node<> *)child_buffer.c_ptr();
			memory::copy					(file_node, node_size, source_file, node_size);
			node						=	node_cast<base_node>(file_node);
			source_file->~physical_file_node();
			XRAY_FREE_IMPL					(m_args.allocator, source_file);
		}
		else
		{
			physical_folder_node<> * source_folder	=	node_cast<physical_folder_node>(source_node);
			folder_node					=	(physical_folder_node<> *)child_buffer.c_ptr();
			memory::copy					(folder_node, node_size, source_folder, node_size);
			node						=	node_cast<base_node>(folder_node);
			source_folder->~physical_folder_node();
			XRAY_FREE_IMPL					(m_args.allocator, source_folder);
		}

		u32 const hash					=	path_crc32(virtual_path_string(node->get_name()), folder_hash);

		virtual_path_string & node_path	=	folder_path;
		node_path.append_path				(node->get_name());

		merge_node_with_tree				(node_path, hash, node, node_cast<base_folder_node>(folder));
		
		if ( folder_node && m_args.recursive && m_result != result_out_of_memory )
		{
			absolute_path				+=	native_path_string::separator;
			absolute_path				+=	node->get_name();
		
			mount_physical_folder			(folder_path, folder_node, absolute_path, hash);

			absolute_path.set_length		(saved_absolute_path_length);
		}

		folder_path.set_length				(saved_folder_path);
		
		child_offs						+=	node_size;
		child_offs						=	math::align_up(child_offs, (u32)sizeof(pvoid));
	}
	R_ASSERT								(child_offs == children_arena.size());
}

void   physical_path_mounter::free_node_list (node_list & new_nodes)
{
	while ( !new_nodes.empty() )
	{
		base_node<> * source_node		=	new_nodes.pop_front();
		if ( source_node->is_file() )
		{
			physical_file_node<> * source_file	=	node_cast<physical_file_node>(source_node);
			source_file->~physical_file_node();
			XRAY_FREE_IMPL					(m_args.allocator, source_file);
		}
		else
		{
			physical_folder_node<> * source_folder	=	node_cast<physical_folder_node>(source_node);
			source_folder->~physical_folder_node();
			XRAY_FREE_IMPL					(m_args.allocator, source_folder);
		}
	}
}

} // namespace vfs
} // namespace xray
