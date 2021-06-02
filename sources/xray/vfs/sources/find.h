////////////////////////////////////////////////////////////////////////////
//	Created		: 03.05.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_FINDING_H_INCLUDED
#define VFS_FINDING_H_INCLUDED

#include "branch_locks.h"
#include <xray/vfs/find_results.h>
#include "find_environment.h"
#include "find_async_callbacks.h"

namespace xray {
namespace vfs {

enum traverse_enum { traverse_branch, traverse_node, traverse_child };

typedef boost::function<void ()>		dispatch_callbacks_function;

result_enum		find_async							(pcstr						path_to_find,
													 vfs_locked_iterator *		out_iterator,
													 find_enum					find_flags,
													 virtual_file_system *		file_system,
													 memory::base_allocator *	allocator,
													 dispatch_callbacks_function const & user_dispatch_callback);

void			try_find_async						(pcstr						path_to_find,
													 find_callback				callback,
													 find_enum					find_flags,
													 virtual_file_system *		file_system,
													 memory::base_allocator *	allocator,
													 u32						start_mount_operation_id = 0,
													 u32						start_path_part_index = (u32)-1);

result_enum		try_find_sync						(pcstr						path_to_find,
													 vfs_locked_iterator *		out_iterator,
 													 find_enum					find_flags,
													 virtual_file_system *		file_system,
 													 memory::base_allocator *	allocator);

void			try_find_tree						(find_env &					env);

result_enum		can_traverse_tree_sync				(base_node<> *				node, 
													 find_enum					find_flags, 
													 base_node<> *				root_node,
													 virtual_file_system *		file_system,
													 memory::base_allocator *	allocator);

void			find_async_across_link				(find_env &					env);

bool			need_physical_mount_or_async		(base_node<> *				node, 
													 find_enum					find_flags, 
													 traverse_enum				traverse_type);

bool			need_physical_mount					(bool *						is_out_of_memory,
													 base_node<> *				node, 
										 			 find_enum					find_flags, 
													 traverse_enum				traverse_type, 
										 			 memory::base_allocator *	allocator);

vfs_iterator::type_enum		iterator_type			(find_enum					find_flags);
void			make_iterator						(vfs_locked_iterator & out_iterator, find_env & env);

void			find_async_expand_sub_fat			(base_node<> *				node, 
													 base_node<> *				node_parent, 
													 async_callbacks_data *		async_data,
													 u32						increment);

void			find_async_expand_physical			(base_node<> *				node, 
													 base_node<> *				node_parent, 
													 async_callbacks_data *		async_data, 
													 recursive_bool				recursive, 
													 u32						increment);

void			query_recursive_mount_of_sub_tree	(base_node<> *				sub_tree_root, 
													 base_node<> *				sub_tree_parent,
													 async_callbacks_data *		parent_callbacks_data);

void			query_expand_nodes					(node_to_expand_list & list, async_callbacks_data * async_data);
void			free_nodes_to_expand				(node_to_expand_list & list, memory::base_allocator * allocator);

result_enum		fill_nodes_to_expand_from_overlapped	(base_node<> *			topmost_node, 
													 base_node<> *				topmost_parent,
													 node_to_expand_list &		list, 
													 memory::base_allocator *	allocator,
													 find_enum					find_flags,
													 traverse_enum				traverse_type, 
													 u32						increment);

result_enum		fill_expand_nodes_and_incref		(base_node<> *				node, 
													 base_node<> *				parent, 
													 base_node<> *				expand_root, 
													 node_to_expand_list &		list, 
													 async_callbacks_data *		async_data,
													 u32						increment);


base_node<> *   find_overlapped_by_mount_id			(base_node<> * node, u32 mount_id);
bool			find_across_mount_if_needed			(find_env & env, base_node<> * topmost_parent);

void			skip_path_iterator_to_start_node	(fs_new::path_part_iterator	&		it,
													 fs_new::path_part_iterator	const & it_end,
													 base_node<> *						start_node);


// if change == -1 does the change only if sub-fat's mount operation id <= in_out_mount_operation_id
// if change == +1 and sub-fat's mount operation id > in_out_mount_operation_id, 
//		in_out_mount_operation_id will be set to sub-fat's mount operation id
void			change_subfat_ref_for_node			(int change, base_node<> * node, u32 * in_out_mount_operation_id);

void			change_subfat_ref_for_overlapped	(int change, base_node<> * topmost_node, u32 * in_out_mount_operation_id);


void			change_subfat_ref_for_branch		(int						change, 
													 base_node<> *				node, 
													 u32 *						in_out_mount_operation_id);

void			decref_children						(base_node<> *				node, 
													 find_enum					find_flags, 
													 vfs_hashset *				hashset,
													 u32 						mount_operation_id,
													 bool						is_root_node = true);

void			unlock_and_decref_recursively		(base_node<> *				node, 
													 lock_type_enum				branch_lock_type,
											 		 find_enum					find_flags, 
											 		 vfs_hashset *				hashset, 
											 		 u32 						mount_operation_id);

void			unlock_and_decref_branch			(base_node<> * node, lock_type_enum unlock_type, u32 mount_operation_id);


} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_FINDING_H_INCLUDED
