////////////////////////////////////////////////////////////////////////////
//	Created		: 02.12.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_impl.h"
#include "fs_impl_finding.h"
#include <xray/fs_path_iterator.h>
#include "resources_helper.h"

namespace xray {
namespace fs {

using namespace resources;

void   on_async_no_pin_across_link_received (fs_iterator			result, 
									  		 fat_node<> *			original_node,
									  		 find_results_struct * 	original_find_results,
									  		 find_results_struct * 	link_find_results,
											 pstr					allocated_link_path,
									  		 enum_flags<find_enum>	find_flags)
{
	fs_iterator									out_result;
	if ( original_node )
	{
		out_result							=	make_iterator(original_node, find_flags);
		original_find_results->sub_fat		=	g_fat->impl()->get_db(original_node);
		original_find_results->sub_fat_of_link	=	g_fat->impl()->get_db(result.get_fat_node());
	}
	else
	{
		out_result							=	make_iterator(result.get_fat_node(), find_flags);
		original_find_results->sub_fat		=	link_find_results->sub_fat;
		R_ASSERT								(g_fat->impl()->get_db(result.get_fat_node()) == link_find_results->sub_fat.c_ptr());
		original_find_results->sub_fat_of_link	=	link_find_results->sub_fat_of_link;
	}

	if ( original_find_results->sub_fat_of_link == original_find_results->sub_fat )
		original_find_results->sub_fat_of_link	=	NULL;

	original_find_results->callback				(out_result);
	XRAY_DELETE_IMPL							(memory::g_resources_helper_allocator, link_find_results);
	XRAY_FREE_IMPL								(memory::g_resources_helper_allocator, allocated_link_path);
}

void   on_async_pin_across_link_received  (fs_iterator				result, 
										   fat_node<> *				original_node,
										   find_results_struct * 	original_find_results,
										   find_results_struct * 	link_find_results,
										   pstr						allocated_link_path,
										   enum_flags<find_enum>	find_flags)
{
	R_ASSERT_U									(!!result);
	if ( original_node )
	{
		original_find_results->pin_iterator.assign_no_pin	(make_iterator(original_node, find_flags));
		link_find_results->pin_iterator.clear_without_unpin	();
	}
	else
	{
		original_find_results->pin_iterator.grab	(link_find_results->pin_iterator);
	}

	original_find_results->callback				(original_find_results->pin_iterator);
	XRAY_DELETE_IMPL							(memory::g_resources_helper_allocator, link_find_results);
	XRAY_FREE_IMPL								(memory::g_resources_helper_allocator, allocated_link_path);
}

void   find_async_across_link (find_env & env)
{
	path_string									path_across_link;
	env.node->find_link_target_path				(& path_across_link);
	path_across_link						+=	env.path_to_find + strings::length(env.partial_path);
	

	pstr const allocated_path_across_link	=	strings::duplicate(memory::g_resources_helper_allocator, path_across_link.c_str());

	bool const is_full_path					=	strings::equal(env.path_to_find, env.partial_path);

	find_results_struct * link_find_results	=	XRAY_NEW_IMPL(memory::g_resources_helper_allocator, find_results_struct)
												(env.find_results->type);

	link_find_results->callback				=	boost::bind(env.find_results->is_no_pin() ? 
																& on_async_no_pin_across_link_received :
																& on_async_pin_across_link_received,
															_1, 
															is_full_path ? env.node : NULL,
															env.find_results, 
															link_find_results,
															allocated_path_across_link,
															env.find_flags);

	g_fat->impl()->find_async					(link_find_results, allocated_path_across_link, env.find_flags);
}

void   file_system_impl::find_async_no_pin (find_results_struct *	find_results,
											pcstr const				path_to_find,
											enum_flags<find_enum>	find_flags)
{
	resources::fs_iterator						it;
	if ( try_find_sync_no_pin(& it, find_results, path_to_find, find_flags) )
	{
		find_results->callback					(it);
		return;
	}

	find_env									env;
	path_string									partial_path;
	env.partial_path						=	partial_path.c_str();
	env.path_to_find						=	path_to_find;
	env.find_flags							=	find_flags;
	env.find_results						=	find_results;

	for ( path_iterator	it = path_to_find; it != path_iterator::end(); ++it )
	{
		partial_path.assign						(path_to_find, it.cur_end());

		env.node							=	find_node_no_mount(partial_path.c_str(), NULL);
		ASSERT									(env.node);

  		if ( env.node->is_link() )
		{
			find_async_across_link				(env);
			return;
		}

		if ( env.node->is_sub_fat() )
		{
			R_ASSERT							(!(env.find_flags & find_sub_fat));
			find_async_query_sub_fat			(env);
			return;
		}

 		if ( need_disk_mount(env.node, env.find_flags, env.is_full_path(), false) )
		{
			if ( (env.node = try_sync_mounting(env.node, env.find_flags, env.is_full_path(), false)) == NULL )
			{
				find_async_across_mount_disk	(env);
 				return;
			}
		}
	}

	find_results->sub_fat					=	get_db(env.node);
	find_results->callback						(fs_iterator(env.node, find_referenced_link_node(env.node)));
}

void   file_system_impl::find_async (find_results_struct *		find_results,
									 pcstr						path_to_find,
									 enum_flags<find_enum>		find_flags)
{
	R_ASSERT									(strings::is_lower_case(path_to_find), "fs supports only lowercase pathes: '%s'", path_to_find);
	if ( find_results->is_pin() )
		find_async_pin							(find_results, path_to_find, find_flags);
	else
		find_async_no_pin						(find_results, path_to_find, find_flags);
}

} // namespace fs
} // namespace xray 