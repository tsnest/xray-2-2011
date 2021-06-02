////////////////////////////////////////////////////////////////////////////
//	Created		: 26.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "patch.h"
#include <xray/compressor_ppmd.h>
#include <xray/memory_allocated_buffer.h>
#include <xray/fs/file_type_pointer.h>
#include <xray/fs/device_utils.h>
#include <xray/vfs/virtual_file_system.h>
#include "saving_info_tree.h"
#include <xray/vfs/unpack.h>
#include "test.h"
#include "free_node.h"

namespace xray {
namespace vfs {

using namespace		fs_new;
fat_node_info *   make_info_tree		(pvoid &						out_allocated_nodes_buffer, 
										 base_node<> *					root_node, 
										 synchronous_device_interface &	device,
										 memory::base_allocator *		allocator);


mount_nodes_for_patch::mount_nodes_for_patch	(patch_args &					args,
												 compressor * const				compressor,
												 virtual_file_system * const	in_out_patch_vfs,
												 vfs_iterator &					from_iterator, 
												 vfs_iterator &					to_iterator)
	:	m_vfs(in_out_patch_vfs), m_args(args), m_compressor(compressor), m_mount_root(NULL)
{
	m_from_root							=	from_iterator;
	m_to_root							=	to_iterator;

	query_mount_arguments mount_args	=	query_mount_arguments::mount_physical_path
													(args.allocator, "", "!", "", NULL, & args.synchronous_device, 
													 NULL, recursive_true);
	mount_args.physical_path			=	"";

	physical_folder_mount_root_node<> * const mount_root	=	mount_root_node_functions
												::create<physical_folder_mount_root_node, platform_pointer_default>
												("", 0, m_vfs, mount_args);
	mount_root->folder.set_is_scanned		(true);
	m_mount_root						=	mount_root;
	
	m_vfs->hashset.insert					(path_crc32("", 0), node_cast<base_node>(m_mount_root), m_mount_root->mount_id);

	insert_new								("", from_iterator, to_iterator);

	vfs_locked_iterator						patch_iterator;
	result_enum const patch_result		=	m_vfs->find_async("", & patch_iterator, find_recursively, m_args.allocator, NULL);
	R_ASSERT_U								(patch_result == result_success);

	vfs_iterator parent_patch_iterator	=	vfs_iterator::end();
	insert_erased							("", patch_iterator, parent_patch_iterator, from_iterator);
}

fat_node_info *   make_sorted_info_tree	(vfs_hashset &					hashset,
										 pvoid &						out_allocated_nodes_buffer, 
										 synchronous_device_interface &	device,
										 memory::base_allocator *		allocator)
{
	base_node<> * const root_node		=	hashset.find_no_lock("", vfs_hashset::check_locks_false);
	R_ASSERT								(root_node);
	fat_inline_data	null_inline_data		(NULL);

	fat_node_info * const out_tree		=	make_info_tree(out_allocated_nodes_buffer, root_node, device, allocator);
	calculate_sizes_for_info_tree			(root_node, out_tree, null_inline_data);
	sort_info_tree							(root_node, out_tree);
	return									out_tree;
}

bool   make_patch						(patch_args & args)
{
	u32 const from_sources_size			=	args.from_sources.count_of(';') + 1;
	sources_array							from_sources(ALLOCA(sizeof(fs_new::native_path_string) * from_sources_size), from_sources_size);
	parse_sources							(args.from_sources, & from_sources);
		
	virtual_file_system * from_vfs		=	XRAY_NEW_IMPL(args.allocator, virtual_file_system);
	mount_ptrs_array	from_ptrs			((vfs_mount_ptr *)ALLOCA(sizeof(vfs_mount_ptr) * from_sources.size()), from_sources.size());
	mount_sources							(* from_vfs, args.synchronous_device, from_sources, from_ptrs, args.allocator, args.log_format, args.log_flags);

	u32 const to_sources_size			=	args.to_sources.count_of(';') + 1;
 	sources_array							to_sources(ALLOCA(sizeof(fs_new::native_path_string) * to_sources_size), to_sources_size);
 	parse_sources							(args.to_sources, & to_sources);
 
 	virtual_file_system * to_vfs		=	XRAY_NEW_IMPL(args.allocator, virtual_file_system);
 	mount_ptrs_array	to_ptrs				((vfs_mount_ptr *)ALLOCA(sizeof(vfs_mount_ptr) * to_sources.size()), to_sources.size());
 	mount_sources							(* to_vfs, args.synchronous_device, to_sources, to_ptrs, args.allocator, args.log_format, args.log_flags);
 
	// find to expand all subfats
	vfs_locked_iterator						from_iterator;
	result_enum const from_result		=	from_vfs->find_async("", & from_iterator, find_recursively, args.allocator, NULL);
	R_ASSERT_U								(from_result == result_success);
	vfs_locked_iterator						to_iterator;

	LOGI_INFO								("test", "FROM VFS:");
	log_vfs_root							(* from_vfs);

	result_enum const to_result			=	to_vfs->find_async("", & to_iterator, find_recursively, args.allocator, NULL);
	R_ASSERT_U								(to_result == result_success);
  
 	ppmd_compressor		compressor			(args.allocator, 1);
 	virtual_file_system	* patch_vfs		=	XRAY_NEW_IMPL(args.allocator, virtual_file_system);

	pvoid	to_vfs_info_buffer			=	NULL;
	fat_node_info * const to_vfs_info_tree		=	make_sorted_info_tree(to_vfs->hashset, to_vfs_info_buffer, args.synchronous_device, args.allocator);
	pvoid	from_vfs_info_buffer		=	NULL;
	fat_node_info * const from_vfs_info_tree	=	make_sorted_info_tree(from_vfs->hashset, from_vfs_info_buffer, args.synchronous_device, args.allocator);
 
 	mount_nodes_for_patch					(args, & compressor, patch_vfs, from_iterator, to_iterator);
 
	LOGI_INFO								("test", "PATCH VFS (not yet saved): ");
 	log_vfs_root							(* patch_vfs);
 
 	args.vfs							=	patch_vfs;
 	args.from_vfs						=	from_vfs;
	args.from_vfs_info_tree				=	from_vfs_info_tree;
 	args.to_vfs							=	to_vfs;
	args.to_vfs_info_tree				=	to_vfs_info_tree;
 	bool const pack_result				=	pack_archive(args);
 
 	unmount_nodes_for_patch					(* patch_vfs);

	to_iterator.clear						();
	from_iterator.clear						();

	from_ptrs.clear							();
	to_ptrs.clear							();

	from_vfs->user_finalize					();
	to_vfs->user_finalize					();
	patch_vfs->user_finalize				();

	XRAY_DELETE_IMPL						(args.allocator, from_vfs);
	XRAY_DELETE_IMPL						(args.allocator, to_vfs);
	XRAY_DELETE_IMPL						(args.allocator, patch_vfs);

	XRAY_FREE_IMPL							(args.allocator, to_vfs_info_buffer);
	XRAY_FREE_IMPL							(args.allocator, from_vfs_info_buffer);


	return									pack_result;
}

void   free_patch_node_recursively		(base_node<> * node, virtual_file_system & file_system, base_node<> * & root_node, memory::base_allocator * const allocator)
{
	for ( base_node<> *	it_child		=	node->get_first_child();
						it_child;
						 )
	{
		base_node<> * const next_child	=	it_child->get_next();
		free_patch_node_recursively			(it_child, file_system, root_node, allocator);
		it_child						=	next_child;
	}

	virtual_path_string						path;
	node->get_full_path						(& path);
	u32 const hash						=	path_crc32(path);

	free_node								(file_system, node, root_node, hash, allocator);
}

void   unmount_nodes_for_patch			(virtual_file_system & patch_vfs)
{
	base_node<> * root_node				=	NULL;
	
	patch_vfs.hashset.find_and_lock_branch	(root_node, "", lock_type_write);

	mount_root_node_base<> * const mount_root	=	node_cast<mount_root_node_base>(root_node);

	free_patch_node_recursively				(root_node, patch_vfs, root_node, mount_root->allocator);
}

} // namespace vfs
} // namespace xray
