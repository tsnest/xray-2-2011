////////////////////////////////////////////////////////////////////////////
//	Created		: 17.06.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "find.h"
#include "find_async_callbacks.h"

namespace xray {
namespace vfs {

using namespace fs_new;

async_callbacks_data::async_callbacks_data	(type_enum type, find_env const & in_env) 
	:	env								(in_env),
		callbacks_count					(0), 
		callbacks_called_count			(0), 
		result							(result_success), 
		all_queries_done				(false),
		type							(type)
{
	R_ASSERT								(env.path_to_find);
	u32 const path_to_find_length		=	strings::length(in_env.path_to_find);
	u32 const partial_path_length		=	strings::length(in_env.partial_path);

	strings::copy							(path_to_find, path_to_find_length + 1, env.path_to_find);
	env.path_to_find					=	path_to_find;

	pstr const partial_path_holder		=	path_to_find + path_to_find_length + 1;

	strings::copy							(partial_path_holder, partial_path_length + 1, env.partial_path);
	env.partial_path					=	partial_path_holder;
}

void	async_callbacks_data::on_lazy_mounted	(mount_result mount)
{
	on_callback_may_destroy_this			(mount.result);	
}

void	async_callbacks_data::on_automatic_archive_or_subfat_mounted	(mount_result mount, u32 increment)
{
	if ( mount.result == result_success )
	{
		base_node<> * const mount_root	=	node_cast< base_node >(mount.mount->get_mount_root());
		change_subfat_ref_for_node			(increment, mount_root, & env.mount_operation_id);
	}

	on_callback_may_destroy_this			(mount.result);	
}

void	async_callbacks_data::on_callback_may_destroy_this	(result_enum in_result)
{
	++callbacks_called_count;
	if ( in_result == result_out_of_memory )
	{
		result							=	result_out_of_memory;
	}
	else if ( in_result == result_cannot_lock )
	{
		if ( result != result_out_of_memory && result != result_fail )
			result						=	result_cannot_lock;
	}
	else if ( in_result == result_fail )
		result							=	result_fail;
	
	try_finish_may_destroy_this				();
}

void	async_callbacks_data::try_finish_may_destroy_this	()
{
	if ( !all_queries_done )
		return;
	if ( callbacks_called_count != callbacks_count )
		return;

	env.node							=	env.hashset()->find_no_lock(env.partial_path);
	R_ASSERT								(env.node);

	if ( type == type_branch )
		finish_branch_destroy_this			();
	else
		finish_tree_may_destroy_this		();
}

void	async_callbacks_data::continue_find_tree	()
{
	callbacks_count						=	0;
	callbacks_called_count				=	0;

	node_to_expand_list						new_to_expand;

	virtual_path_string						previous_path;
	virtual_path_string						current_path;

	result_enum	result					=	result_success;
	for ( node_to_expand *	it_expanded	=	nodes_to_expand.front();
							it_expanded;
							it_expanded	=	nodes_to_expand.get_next_of_object(it_expanded) )
	{
		it_expanded->node->get_full_path	(& current_path);
		if ( current_path == previous_path )
			continue;

		base_node<> * topmost_node		=	env.hashset()->find_no_lock(current_path.c_str(), vfs_hashset::check_locks_false);

		result							=	fill_expand_nodes_and_incref(topmost_node, 
																		 node_cast<base_node>(topmost_node->get_parent()), 
																		 topmost_node,
																		 new_to_expand, 
																		 this,
																		 it_expanded->increment);
		if ( result == result_out_of_memory )
			break;

		previous_path					=	current_path;
	}

	free_nodes_to_expand					(nodes_to_expand, env.allocator);
	if ( result == result_out_of_memory )
	{
		free_nodes_to_expand				(new_to_expand, env.allocator);
		env.callback						(vfs_locked_iterator::end(), result_out_of_memory);
	}
	else
		nodes_to_expand.swap				(new_to_expand);

	query_expand_nodes						(nodes_to_expand, this);
}

void	async_callbacks_data::finish_branch_destroy_this	()
{
	if ( result == result_success )
	{
		upgrade_branch						(env.node, lock_type_write, lock_type_read);
		try_find_async						(env.path_to_find, env.callback, env.find_flags, env.file_system, 
											 env.allocator, env.mount_operation_id, env.path_part_index);
	}
	else
	{
		unlock_and_decref_branch			(env.node, lock_type_write, env.mount_operation_id);
		env.callback						(vfs_locked_iterator::end(), result);
	}

	delete_this								();
}

void	async_callbacks_data::finish_tree_may_destroy_this	()
{
	if ( result == result_success )
	{
		bool const processed_whole_tree	=	nodes_to_expand.empty();
		if ( !processed_whole_tree )
			upgrade_branch					(env.node, lock_type_write, lock_type_read);

		if ( !processed_whole_tree )
		{
			continue_find_tree				();
			return;
		}
		else
		{
			vfs_locked_iterator				out_iterator;
			make_iterator					(out_iterator, env);
			env.callback					(out_iterator, result_success);
		}
	}
	else
	{
		free_nodes_to_expand				(nodes_to_expand, env.allocator);
		unlock_and_decref_recursively		(env.node, lock_type_write, env.find_flags, env.hashset(), env.mount_operation_id);
		env.callback						(vfs_locked_iterator::end(), result);
	}

	delete_this								();
}

void	async_callbacks_data::delete_this	()
{
	async_callbacks_data * this_ptr		=	this;
	memory::base_allocator * my_allocator	=	env.allocator;
	this_ptr->~async_callbacks_data			();
	XRAY_FREE_IMPL							(my_allocator, this_ptr);
}

void	async_callbacks_data::finish_with_out_of_memory	()
{
	unlock_and_decref_branch				(env.node, lock_type_read, env.mount_operation_id);
	env.callback							(vfs_locked_iterator::end(), result_out_of_memory);
	R_ASSERT								(callbacks_count == callbacks_called_count);
	delete_this								();
}

} // namespace vfs
} // namespace xray
