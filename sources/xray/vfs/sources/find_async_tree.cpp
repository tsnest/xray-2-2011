////////////////////////////////////////////////////////////////////////////
//	Created		: 09.12.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "find.h"
#include "find_async_callbacks.h"
#include <xray/type_predicates.h>

namespace xray {
namespace vfs {

void	free_nodes_to_expand			(node_to_expand_list & list, memory::base_allocator * allocator)
{
	while ( !list.empty() )
	{
		node_to_expand * node			=	list.pop_front();
		XRAY_DELETE_IMPL					(allocator, node);
	}
}

void   query_expand_nodes				(node_to_expand_list & list, async_callbacks_data * async_data)
{
	async_data->callbacks_count			=	list.size();
	async_data->all_queries_done		=	false;

	if ( !list.empty() )
		upgrade_branch						(async_data->env.node, lock_type_read, lock_type_write);

	for ( node_to_expand *	it_expand	=	list.front();
							it_expand;
							it_expand	=	list.get_next_of_object(it_expand) )
	{
		if ( it_expand->type == node_to_expand::type_subfat )
		{
			find_async_expand_sub_fat		(it_expand->node, it_expand->node_parent, async_data, it_expand->increment);
		}
		else if ( it_expand->type == node_to_expand::type_physical )
		{
			recursive_bool const recursive	=	
				(recursive_bool)(async_data->env.is_full_path() && 
								(async_data->env.find_flags & find_recursively));

			find_async_expand_physical		(it_expand->node, it_expand->node_parent, async_data, recursive, it_expand->increment);
		}
		else
			NODEFAULT						();
	}
	
	async_data->all_queries_done		=	true;
	async_data->try_finish_may_destroy_this	();
}

result_enum   fill_nodes_to_expand_from_overlapped	(base_node<> *				topmost_node, 
													 base_node<> *				topmost_parent,
													 node_to_expand_list &		list, 
													 memory::base_allocator *	allocator,
													 find_enum					find_flags,
													 traverse_enum				traverse_type, 
													 u32						increment)
{
	for ( base_node<> * it_node			=	topmost_node;
						it_node;
						it_node			=	it_node->get_next_overlapped() )
	{
		if ( it_node->is_link() )
			continue;

		bool is_out_of_memory			=	false;
		if ( it_node->is_sub_fat() || need_physical_mount(& is_out_of_memory, it_node, find_flags, traverse_type, allocator) )
		{
			base_node<> * parent		=	find_overlapped_by_mount_id(topmost_parent, mount_id_of_node(it_node));
			R_ASSERT						(parent || !it_node->get_parent());

			if ( node_to_expand * const item = list.find_if(member_equal_to(& node_to_expand::node, it_node)) )
			{
				item->increment			+=	increment;
				continue; // already added to expand it from a link
			}

			node_to_expand::type_enum type	=	it_node->is_sub_fat() ? node_to_expand::type_subfat : node_to_expand::type_physical;

			node_to_expand * new_item	=	XRAY_NEW_WITH_CHECK_IMPL(allocator, new_item, node_to_expand) 
																	(type, it_node, parent, increment);
			if ( !new_item )
				return						result_out_of_memory;

			list.push_back					(new_item);
		}
	}

	return									result_success;
}

result_enum   fill_expand_nodes_and_incref	(base_node<> *			node, 
											 base_node<> *			parent, 
											 base_node<> *			expand_root, 
											 node_to_expand_list &	list, 
											 async_callbacks_data *	async_data,
											 u32					increment)
{
	if ( node->is_soft_link() )
		return								result_success;

	if ( node->is_hard_link() )
	{
		hard_link_node<> * link_node	=	node_cast<hard_link_node>(node);
		base_node<> * referenced		=	find_referenced_link_node(node_cast<base_node>(link_node));
		R_ASSERT							(referenced->is_archive());
		return								fill_expand_nodes_and_incref(referenced, node_cast<base_node>(referenced->get_parent()), 
																		 expand_root, list, async_data, increment);
	}

	if ( node != expand_root )
		change_subfat_ref_for_overlapped	(increment, node, & async_data->env.mount_operation_id);

	u32 const saved_size				=	list.size();
	traverse_enum const traverse_type	=	(node == expand_root) ? traverse_node : traverse_child;

	result_enum const result			=	fill_nodes_to_expand_from_overlapped(
		node, parent, list, async_data->env.allocator, async_data->env.find_flags, traverse_type, increment
		);

	if ( result != result_success )
		return								result;

	bool const added_to_expand			=	list.size() != saved_size;

	if ( added_to_expand )
		return								result_success;

	bool const recursive				=	(node == async_data->env.node) || (async_data->env.find_flags & find_recursively);
	if ( !recursive )
		return								result_success;

	for ( base_node<> *	it_child		=	node->get_first_child(); 
						it_child; 
						it_child		=	it_child->get_next() )
	{
		result_enum const child_result	=	fill_expand_nodes_and_incref(it_child, node, expand_root, list, async_data, increment);
		if ( child_result != result_success )
			return							child_result;
	}

	return									result_success;	
}

void   try_find_tree					(find_env & env)
{
	async_callbacks_data * async_data	=	(async_callbacks_data *)XRAY_MALLOC_IMPL(env.allocator, sizeof(async_callbacks_data) + 
																					 strings::length(env.path_to_find) + 1 +
																					 strings::length(env.path_to_find) + 1, 
																					 "async_callbacks_data");
	if ( !async_data )
	{
		async_data->finish_with_out_of_memory	();
		return;
	}

	new (async_data)						async_callbacks_data(async_callbacks_data::type_tree, env);

	result_enum const result			=	fill_expand_nodes_and_incref(async_data->env.node, 
																		 async_data->env.node_parent, 
																		 async_data->env.node, 
																		 async_data->nodes_to_expand, 
																		 async_data,
																		 1);
	if ( result == result_out_of_memory )
	{
		async_data->finish_with_out_of_memory	();
		return;
	}

	query_expand_nodes						(async_data->nodes_to_expand, async_data);
}

} // namespace vfs
} // namespace xray
