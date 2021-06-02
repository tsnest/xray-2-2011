////////////////////////////////////////////////////////////////////////////
//	Created		: 30.11.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs_path_iterator.h>
#include "find.h"
#include "nodes.h"
#include <xray/vfs/hashset.h>

namespace xray {
namespace vfs {

using namespace fs_new;

result_enum   try_find_sync_link		(find_env & env)
{
	virtual_path_string						path_across_link;
	find_link_target_path					(env.node, & path_across_link);
	path_across_link					+=	env.path_to_find + strings::length(env.partial_path);

	result_enum const sync_result		=	try_find_sync(path_across_link.c_str(), 
														  env.out_iterator,
														  env.find_flags, 
														  env.file_system, 
														  env.allocator);
	return									sync_result;
}

result_enum   overlapped_chain_is_expanded	(base_node<> * topmost_node, find_env & env)
{
	for ( base_node<> * it_node			=	topmost_node;
						it_node;
						it_node			=	it_node->get_next_overlapped() )
	{
		if ( it_node->is_sub_fat() )
			return							result_need_async;

		traverse_enum traverse_type		=	traverse_branch;
		if ( env.is_full_path() )
			traverse_type				=	(topmost_node == env.node) ? traverse_node : traverse_child;
		
		if ( need_physical_mount_or_async(it_node, env.find_flags, traverse_type) )
			return							result_need_async;
	}

	return									result_success;
}

result_enum   try_pin_tree				(base_node<> * node, find_env & env)
{
	if ( node != env.node )
	{
		result_enum const result		=	overlapped_chain_is_expanded(node, env);
		if ( result != result_success )
			return							result;

		change_subfat_ref_for_overlapped	(+1, node, & env.mount_operation_id);

		if ( node->is_link() )
		{
			virtual_path_string				link_target_path;
			find_link_target_path			(node, & link_target_path);

			vfs_locked_iterator		link_iterator;
			result_enum const link_result	=	try_find_sync(link_target_path.c_str(), & link_iterator, env.find_flags, env.file_system, env.allocator);
			link_iterator.clear_without_unpin	();
			return							link_result;
		}
	}

	if ( !node->is_folder() )
		return								result_success;

	bool const go_recursive				=	(env.find_flags & find_recursively) || (node == env.node);
	if ( !go_recursive )
		return								result_success;

	result_enum child_result			=	result_success;

	base_node<> * node_which_failed		=	NULL;
	base_folder_node<> * const folder	=	node_cast<base_folder_node>(node);
	for ( base_node<> * it_child		=	folder->get_first_child();
						it_child;
						it_child		=	it_child->get_next() )
	{
		child_result					=	try_pin_tree(it_child, env);
		if ( child_result != result_success )
		{
			node_which_failed			=	it_child;
			break;
		}
	}	

	if ( !node_which_failed )
		return								result_success;

	for ( base_node<> * it_child		=	folder->get_first_child();
						it_child		!=	node_which_failed;
						it_child		=	it_child->get_next() )
	{
		decref_children			(it_child, env.find_flags, env.hashset(), env.mount_operation_id, false);
	}	

	return									child_result;
}

result_enum   try_find_sync				(pcstr						path_to_find,
										 vfs_locked_iterator *		out_iterator,
										 find_enum					find_flags,
										 virtual_file_system *		file_system,
										 memory::base_allocator *	allocator)
{
	R_ASSERT								(strings::is_lower_case(path_to_find), "fs supports only lowercase pathes: '%s'", path_to_find);

	virtual_path_string						partial_path;
	find_env								env;
	env.out_iterator					=	out_iterator;
	env.partial_path					=	partial_path.c_str();
	env.path_to_find					=	path_to_find;
	env.find_flags						=	find_flags;
	env.file_system						=	file_system;
	env.allocator						=	allocator;

	path_part_iterator	it				=	path_part_iterator(path_to_find, virtual_path_string::separator, include_empty_string_in_iteration_true);
	path_part_iterator	it_end			=	path_part_iterator::end();

	for ( ; it != it_end; ++it, ++env.path_part_index )
	{
		partial_path.assign					(path_to_find, it.cur_end());

		base_node<> * previous_node		=	env.node;
		env.node_parent					=	env.node;
		bool const lock_result			=	env.hashset()->find_no_branch_lock(env.node, partial_path.c_str(), lock_type_read, lock_operation_try_lock);
		if ( !lock_result )
		{
			unlock_and_decref_branch		(env.node, lock_type_read, env.mount_operation_id);
			return							result_cannot_lock;
		}

		if ( !env.node )
		{
			out_iterator->clear				();
			unlock_and_decref_branch		(previous_node, lock_type_read, env.mount_operation_id);
			return							result_success;
		}

		upgrade_node						(env.node_parent, lock_type_read, lock_type_soft_read);

		result_enum const result		=	overlapped_chain_is_expanded(env.node, env);
		change_subfat_ref_for_overlapped	(+1, env.node, & env.mount_operation_id);
		
		if ( result != result_success )
		{
			unlock_and_decref_branch		(env.node, lock_type_read, env.mount_operation_id);
			return							result;
		}

		if ( env.node->is_link() )
		{
			result_enum const link_result	=	try_find_sync_link(env);
			unlock_and_decref_branch		(env.node, lock_type_read, env.mount_operation_id);
			return							link_result;
		}
	}

	result_enum const pin_tree_result	=	try_pin_tree(env.node, env);

	if ( pin_tree_result == result_success )
		out_iterator->assign				(env.node, env.hashset(), iterator_type(find_flags), env.mount_operation_id);
	else
		unlock_and_decref_branch			(env.node, lock_type_read, env.mount_operation_id);

	return									pin_tree_result;
}

} // namespace vfs
} // namespace xray
