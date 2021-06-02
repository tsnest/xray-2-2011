////////////////////////////////////////////////////////////////////////////
//	Created		: 09.12.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_impl.h"
#include "fs_impl_finding.h"
#include <xray/fs_path_iterator.h>
#include <xray/fs_sub_fat_resource.h>
#include "resources_helper.h"
#include "fs_impl_finding_async_callbacks_data.h"

namespace xray {
namespace fs {

using namespace		resources;


void   query_recursive_mount_of_sub_tree	(fat_node<> *			tree_root, 
											 async_callbacks_data *	callbacks_data, 
											 enum_flags<find_enum>	find_flags);

void   on_link_ready (fs_iterator				it, 
					  find_results_struct *		link_results,
					  async_callbacks_data *	callbacks_data)
{
	R_ASSERT_U										(!!it);
	link_results->pin_iterator.clear_without_unpin	();
	callbacks_data->on_callback_may_destroy_this	();	
	XRAY_DELETE_IMPL								(&memory::g_resources_helper_allocator, link_results);
}

void   on_disk_mount_finished (bool						result, 
							   fat_node<> * const		node,
							   async_callbacks_data *	parent_callbacks_data,
							   enum_flags<find_enum>	find_flags)
{
	R_ASSERT_U									(result);

	fat_node<> * tree_root					=	NULL;
	if ( !node->is_folder() ) // automatic db
	{
		path_string								full_path;
		node->get_full_path						(full_path);
		tree_root							=	g_fat->impl()->find_node_no_mount	(full_path.c_str());
		R_ASSERT								(tree_root != node);
	}
	else
	{
		tree_root							=	node;
	}
	
	query_recursive_mount_of_sub_tree			(tree_root, parent_callbacks_data, find_flags);
}

void   on_sub_fat_loaded (resources::queries_result &	results, 
						  async_callbacks_data *		parent_callbacks_data,
						  enum_flags<find_enum>			find_flags)
{
	R_ASSERT									(results.is_successful());
	sub_fat_resource_ptr sub_fat			=	static_cast_resource_ptr<sub_fat_resource_ptr>
												(results[0].get_unmanaged_resource());
	g_fat->impl()->mount_sub_fat_resource			(sub_fat);

	query_recursive_mount_of_sub_tree			(sub_fat->root_node, parent_callbacks_data, find_flags);
}

static
void   query_recursive_sub_fat (fat_node<> * node, enum_flags<find_enum> find_flags, async_callbacks_data * callbacks_data)
{
	path_string									db_fat_full_path;
	node->get_full_path							(db_fat_full_path);
	resources::query_resource					(db_fat_full_path.c_str(), 
												 resources::sub_fat_class, 
												 boost::bind( & on_sub_fat_loaded, 
																_1, 
																callbacks_data,
																find_flags), 
												 & memory::g_resources_helper_allocator);
}

static
void   query_recursive_disk_mount (fat_node<> * node, enum_flags<find_enum>	find_flags, async_callbacks_data * callbacks_data)
{
	path_string									node_path;
	node->get_full_path							(node_path);
	path_string									disk_path;
	g_fat->impl()->get_disk_path					(node, disk_path);

	resources::query_continue_disk_mount		(node_path.c_str(), 
									 			 disk_path.c_str(), 
									 			 (find_flags & find_recursively),
									 			 boost::bind(& on_disk_mount_finished,
															 _1, 
															 node,
															 callbacks_data,
															 find_flags),
								 		 		 & memory::g_resources_helper_allocator);
}

static
void   query_async_pin_link (fat_node<> *			node, 
							 enum_flags<find_enum>	find_flags, 
							 async_callbacks_data *	callbacks_data)
{
	path_string									link_target_path;
	node->find_link_target_path					(& link_target_path);
	find_results_struct * const link_results =	XRAY_NEW_IMPL(&memory::g_resources_helper_allocator, find_results_struct)
															(find_results_struct::type_pin);
	link_results->callback					=	boost::bind(& on_link_ready, _1, link_results, callbacks_data);
	g_fat->impl()->find_async_pin					(link_results, link_target_path.c_str(), find_flags); 			
}

struct tree_load_and_pin_predicate
{
	tree_load_and_pin_predicate	(async_callbacks_data * callbacks_data) : callbacks_data(callbacks_data) {}

	bool operator () (fat_node<> * const node, fat_node<> * const root_node, enum_flags<find_enum> const find_flags)
	{
		pin_sub_fat_if_node_is_root_of_sub_fat	(+1, node);

		if ( node->is_link() )
		{
			++callbacks_data->callbacks_count;
			query_async_pin_link				(node, find_flags, callbacks_data);
			return								false;
		}

		if ( node->is_sub_fat() )
		{
			++callbacks_data->callbacks_count;
			query_recursive_sub_fat				(node, find_flags, callbacks_data);
			return								false;
		}

		bool const is_full_path				=	(node == root_node);
		bool const is_child_of_root			=	(node->get_parent() == root_node);
		bool const is_leaf_node				=	!is_full_path && !is_child_of_root;

 		if ( need_disk_mount(node, find_flags, is_full_path, is_leaf_node) )
		{
			if ( !try_sync_mounting(node, find_flags, is_full_path, is_leaf_node) )
			{
				++callbacks_data->callbacks_count;
				query_recursive_disk_mount		(node, find_flags, callbacks_data);	
 				return							false;
			}
		}

		return									(find_flags & find_recursively) || (node == root_node);
	}

	async_callbacks_data *						callbacks_data;
};

template <class action_predicate>
void   traverse_tree_with_action (fat_node<> * const			node, 
								  fat_node<> * const			root_node, 
								  enum_flags<find_enum> const	find_flags, 
								  action_predicate				action)
{
	if ( !action(node, root_node, find_flags) )
		return;

	for ( fat_node<> * it_child = node->get_first_child(); it_child; )
	{
		fat_node<> * const it_next			=	it_child->get_next();
		traverse_tree_with_action				(it_child, root_node, find_flags, action);
		it_child							=	it_next;
	}
}

void   on_recursive_tree_loaded (fs_iterator				it, 
								 async_callbacks_data *		parent_callbacks_data,
								 find_results_struct *		find_results_to_delete)
{
	XRAY_UNREFERENCED_PARAMETER					(it);
	R_ASSERT									(!find_results_to_delete->pin_iterator);
	parent_callbacks_data->on_callback_may_destroy_this	();	
	XRAY_DELETE_IMPL							(&memory::g_resources_helper_allocator, find_results_to_delete);
}

void   query_recursive_mount_of_tree_impl	(fat_node<> *			tree_root, 
											 async_callbacks_data * callbacks_data,
											 enum_flags<find_enum>	find_flags)
{
	tree_load_and_pin_predicate	action			(callbacks_data);
	traverse_tree_with_action					(tree_root, tree_root, find_flags, action);

	callbacks_data->all_queries_done		=	true;
	callbacks_data->try_finish_may_destroy_this	();
}

void   query_recursive_mount_of_sub_tree (fat_node<> *				sub_tree_root, 
										  async_callbacks_data *	parent_callbacks_data, 
										  enum_flags<find_enum>		find_flags)
{
	find_results_struct * const sub_tree_results	=	XRAY_NEW_IMPL(& memory::g_resources_helper_allocator, find_results_struct)
																(find_results_struct::type_pin);
	
	sub_tree_results->callback			=	boost::bind(& on_recursive_tree_loaded, _1, parent_callbacks_data, sub_tree_results);
	
	async_callbacks_data * const callbacks_data	=	XRAY_NEW_IMPL(& memory::g_resources_helper_allocator, 
																async_callbacks_data) (sub_tree_results);

	query_recursive_mount_of_tree_impl			(sub_tree_root, callbacks_data, find_flags);
}

void   query_recursive_mount_of_tree	(fat_node<> *			tree_root, 
										 find_results_struct *	root_find_results,
										 enum_flags<find_enum>	find_flags)
{
	async_callbacks_data * const callbacks_data	=	XRAY_NEW_IMPL(& memory::g_resources_helper_allocator, async_callbacks_data)
																	(root_find_results);

	query_recursive_mount_of_tree_impl			(tree_root, callbacks_data, find_flags);
}


} // namespace fs
} // namespace xray
