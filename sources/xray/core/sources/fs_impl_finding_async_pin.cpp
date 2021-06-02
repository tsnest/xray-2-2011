////////////////////////////////////////////////////////////////////////////
//	Created		: 03.12.2010
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

void   file_system_impl::find_async_pin	(find_results_struct *	find_results,
										 pcstr					path_to_find,
										 enum_flags<find_enum>	find_flags)
{
	if ( try_find_sync (& find_results->pin_iterator, path_to_find, find_flags) )
	{
		find_results->callback					(find_results->pin_iterator);
		return;
	}

	find_env									env;
	path_string									partial_path;
	env.partial_path						=	partial_path.c_str();
	env.path_to_find						=	path_to_find;
	env.find_flags							=	find_flags;
	env.find_results						=	find_results;

	for ( path_iterator	it = path_to_find, end_it = path_iterator::end(); it != end_it; ++it )
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
				find_async_across_mount_disk		(env);
 				return;
			}
		}
	}

	if ( can_traverse_tree_sync(env.node, find_flags, env.node) )
	{
		find_results->pin_iterator.assign		(make_iterator(env.node, find_flags));
		find_results->callback					(find_results->pin_iterator);
		return;
	}

	find_results->pin_iterator.assign_no_pin	(make_iterator(env.node, find_flags));
	pin_sub_fats_of_ancestors					(+1, env.node);

	query_recursive_mount_of_tree				(env.node, find_results, find_flags);
}


} // namespace fs
} // namespace xray
