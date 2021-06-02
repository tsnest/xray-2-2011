////////////////////////////////////////////////////////////////////////////
//	Created		: 03.12.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "find.h"

namespace xray {
namespace vfs {

using namespace fs_new;

base_node<> *   find_overlapped_by_mount_id	(base_node<> * node, u32 mount_id)
{
	for ( base_node<> * it_node			=	node;
						it_node;
						it_node			=	it_node->get_next_overlapped() )
	{
		if ( mount_id_of_node(it_node) == mount_id )
			return							it_node;
	}

	return									NULL;
}

bool   mount_overlapped_if_needed		(find_env & env)
{
	node_to_expand_list						mounts;

	if ( fill_nodes_to_expand_from_overlapped(
			env.node, 
			env.node_parent, 
			mounts, 
			env.allocator, 
			env.find_flags, 
			env.is_full_path() ? traverse_node : traverse_branch, 
			1) ==	result_out_of_memory )
	{
		unlock_and_decref_branch			(env.node, lock_type_read, env.mount_operation_id);
		env.callback						(vfs_locked_iterator::end(), result_out_of_memory);
		return								true;
	}

	if ( mounts.empty() )
		return								false;

	async_callbacks_data * async_data	=	(async_callbacks_data *)XRAY_MALLOC_IMPL(env.allocator, sizeof(async_callbacks_data) + 
																				strings::length(env.path_to_find) + 1 +
																				strings::length(env.partial_path) + 1, 
																				"async_callbacks_data");
	if ( !async_data )
	{
		unlock_and_decref_branch			(env.node, lock_type_read, env.mount_operation_id);
		env.callback						(vfs_locked_iterator::end(), result_out_of_memory);
		return								true;
	}

	new (async_data)						async_callbacks_data(async_callbacks_data::type_branch, env);

	query_expand_nodes						(mounts, async_data);

	free_nodes_to_expand					(mounts, env.allocator);
	
	return									true;
}

void   try_find_async					(pcstr						path_to_find,
										 find_callback				callback,
										 find_enum					find_flags,
										 virtual_file_system *		file_system,
										 memory::base_allocator *	allocator,
										 u32						start_mount_operation_id,
										 u32						start_path_part_index)
{
	find_env								env;
	virtual_path_string						partial_path;
	env.callback						=	callback;
	env.partial_path					=	partial_path.c_str();
	env.path_to_find					=	path_to_find;
	env.find_flags						=	find_flags;
	env.file_system						=	file_system;
	env.allocator						=	allocator;
	env.mount_operation_id				=	start_mount_operation_id;

	path_part_iterator	it				=	path_part_iterator(path_to_find, virtual_path_string::separator, include_empty_string_in_iteration_true);
	path_part_iterator	it_end			=	path_part_iterator::end();

	for ( ; it != it_end; ++it, ++env.path_part_index )
	{
		partial_path.assign					(path_to_find, it.cur_end());

		if ( start_path_part_index != u32(-1) )
		{
			if ( env.path_part_index < start_path_part_index )
				continue;
			
			if ( env.path_part_index == start_path_part_index )
			{
				env.node					=	env.hashset()->find_no_lock(partial_path.c_str());
				R_ASSERT						(has_lock(env.node, lock_type_read));
				continue;
			}
		}

		base_node<> * previous_node		=	env.node;
		env.node_parent					=	env.node;
		bool const lock_result			=	env.hashset()->find_no_branch_lock(env.node, partial_path.c_str(), lock_type_read, lock_operation_try_lock);
		if ( !lock_result )
		{
			callback						(vfs_locked_iterator::end(), result_cannot_lock);
			unlock_and_decref_branch		(env.node, lock_type_read, env.mount_operation_id);
			return;
		}
									
		if ( !env.node )
		{
			callback						(vfs_locked_iterator::end(), result_success);
			unlock_and_decref_branch		(previous_node, lock_type_read, env.mount_operation_id);
			return;
		}

		upgrade_node						(env.node_parent, lock_type_read, lock_type_soft_read);

		change_subfat_ref_for_overlapped	(+1, env.node, & env.mount_operation_id);

		if ( env.node->is_link() )
		{
			find_async_across_link			(env);
			return;
		}

		if ( mount_overlapped_if_needed(env) )
			return;
	}

	if ( env.node->is_file() )
	{
		vfs_locked_iterator					out_iterator;
		make_iterator						(out_iterator, env);
		env.callback						(out_iterator, result_success);
		return;
	}

	try_find_tree							(env);
}

void	helper_callback					(vfs_locked_iterator const &	iterator, 
										 result_enum					result, 
										 vfs_locked_iterator *			out_iterator,
										 result_enum *					out_result,
										 bool *							callback_ready)
{
	out_iterator->grab						(iterator);
	* out_result						=	result;
	* callback_ready					=	true;
}

result_enum		find_async				(pcstr									path_to_find,
										 vfs_locked_iterator *					out_iterator,
										 find_enum								find_flags,
										 virtual_file_system *					file_system,
										 memory::base_allocator *				allocator,
										 dispatch_callbacks_function const &	user_dispatch_callback)
{
	bool callback_ready					=	false;
	result_enum	result					=	result_success;
	find_callback const callback		=	boost::bind(& helper_callback, _1, _2, out_iterator, & result, & callback_ready);
	try_find_async							(path_to_find, callback, find_flags, file_system, allocator);

	while ( !callback_ready )
	{
		file_system->dispatch_callbacks		();
		if ( user_dispatch_callback )
			user_dispatch_callback			();
	}

	return									result;
}

} // namespace vfs
} // namespace xray
