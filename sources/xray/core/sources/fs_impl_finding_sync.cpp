////////////////////////////////////////////////////////////////////////////
//	Created		: 30.11.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs_path_iterator.h>
#include "fs_impl.h"
#include "fs_impl_finding.h"

namespace xray {
namespace fs {

bool   can_traverse_path_sync (pcstr path_to_find, find_enum find_flags);

using namespace resources;

fat_node<> *   find_referenced_link_node (fat_node<> * node)
{
	return										node ? node->find_referenced_link_node() : NULL;
}

bool   try_find_sync_link_no_pin (fs_iterator * out_iterator, find_env & env)
{
	path_string									path_across_link;
	env.node->find_link_target_path				(& path_across_link);
	path_across_link						+=	env.path_to_find + strings::length(env.partial_path);

	fs_iterator									target_iterator;

	find_results_struct							find_link_struct(find_results_struct::type_no_pin);
	g_fat->impl()->try_find_sync_no_pin			(& target_iterator,
												 & find_link_struct,
												 path_across_link.c_str(), 
												 env.find_flags);
	if ( !target_iterator )
		return									false;

	bool const is_full_path					=	strings::equal(env.partial_path, env.path_to_find);

	if ( is_full_path )
	{		
		ASSERT									(!find_link_struct.sub_fat_of_link, "link to a link found! - call Lain");
		env.find_results->sub_fat			=	g_fat->impl()->get_db(env.node);
		env.find_results->sub_fat_of_link	=	find_link_struct.sub_fat;

		if ( env.find_results->sub_fat_of_link == env.find_results->sub_fat )
			env.find_results->sub_fat_of_link	=	NULL;
	}
	else
	{
		env.find_results->sub_fat			=	find_link_struct.sub_fat;
		env.find_results->sub_fat_of_link	=	find_link_struct.sub_fat_of_link;
		* out_iterator						=	target_iterator;
		return									true;
	}

	* out_iterator							=	fs_iterator(env.node, find_referenced_link_node(env.node));
	return										true;
}

bool   file_system_impl::try_find_sync_no_pin (fs_iterator * const				out_iterator,
											   find_results_struct *			find_results,
											   pcstr const						path_to_find,
											   enum_flags<find_enum> const		find_flags)
{
	R_ASSERT									(strings::is_lower_case(path_to_find), "fs supports only lowercase pathes: '%s'", path_to_find);
	find_env									env;
	path_string									partial_path;
	env.partial_path						=	partial_path.c_str();
	env.path_to_find						=	path_to_find;
	env.find_flags							=	find_flags | find_sync;
	env.find_results						=	find_results;
	* out_iterator							=	fs_iterator::end();
	
	for ( path_iterator	it = path_to_find; it != path_iterator::end(); ++it )
	{
		partial_path.assign						(path_to_find, it.cur_end());

		env.node							=	find_node_no_mount(partial_path.c_str(), NULL);
		if ( !env.node )
		{
			* out_iterator					=	fs_iterator::end();
			return								true;
		}

		if ( env.node->is_link() )
			return								try_find_sync_link_no_pin(out_iterator, env);

		if ( !try_sync_mounting(env.node, env.find_flags, env.is_full_path(), false) )
			return								false;
	}

	find_results->sub_fat					=	get_db(env.node);
	* out_iterator							=	fs_iterator(env.node, find_referenced_link_node(env.node));
	return										true;
}

bool   can_traverse_tree_sync (fat_node<> * const			node, 
							   enum_flags<find_enum> const	find_flags, 
							   fat_node<> * const			root_node)
{
	if ( node->is_link() )
	{
		path_string								link_target;
		node->find_link_target_path				(& link_target);
		return									can_traverse_path_sync(link_target.c_str(), find_flags);
	}

	bool const is_full_path					=	(node == root_node);
	bool const is_child_of_root				=	(node->get_parent() == root_node);
	bool const is_leaf_node					=	!is_full_path && !is_child_of_root;

	if ( !try_sync_mounting(node, find_flags, is_full_path, is_leaf_node) )
		return									false;

	if ( !(find_flags & find_recursively) && !is_full_path )
		return									true;

	for ( fat_node<> *	it_child	=	node->get_first_child();
						it_child;
						it_child	=	it_child->get_next() )
	{
		if ( !can_traverse_tree_sync(it_child, find_flags, root_node) )
			return								false;
	}	

	return										true;
}

bool   can_traverse_path_sync (pcstr path_to_find, find_enum find_flags)
{
	find_env									env;
	path_string									partial_path;
	env.partial_path						=	partial_path.c_str();
	env.path_to_find						=	path_to_find;
	env.find_flags							=	find_flags;
	
	for ( path_iterator	it = path_to_find; it != path_iterator::end(); ++it )
	{
		partial_path.assign						(path_to_find, it.cur_end());

		env.node							=	g_fat->impl()->find_node_no_mount(partial_path.c_str(), NULL);
		if ( !env.node )
			return								true;

		if ( env.node->is_link() )
		{
			path_string							path_across_link;
			env.node->find_link_target_path		(& path_across_link);
			path_across_link				+=	env.path_to_find + strings::length(env.partial_path);

			return								can_traverse_path_sync(path_across_link.c_str(), env.find_flags);
		}

		if ( !try_sync_mounting(env.node, env.find_flags, env.is_full_path(), false) )
			return								false;
	}

	return										can_traverse_tree_sync(env.node, find_flags, env.node);
}

bool   find_sync_link_helper (sub_fat_pin_fs_iterator * out_pin_iterator, find_env & env)
{
	path_string									path_across_link;
	env.node->find_link_target_path				(& path_across_link);
	path_across_link						+=	env.path_to_find + strings::length(env.partial_path);

	sub_fat_pin_fs_iterator						target_iterator;
	if ( !g_fat->impl()->try_find_sync(& target_iterator, path_across_link.c_str(), env.find_flags) )
		return									false;

	if ( strings::equal(env.partial_path, env.path_to_find) )
	{
		out_pin_iterator->assign				(env.node, find_referenced_link_node(env.node));
		target_iterator.clear_without_unpin		();
	}
	else
	{
		out_pin_iterator->grab					(target_iterator);
	}

	return										true;
}

bool   file_system_impl::try_find_sync (sub_fat_pin_fs_iterator *	out_pin_iterator,													 
									    pcstr const					path_to_find,
										enum_flags<find_enum>		find_flags)
{
	R_ASSERT									(strings::is_lower_case(path_to_find), "fs supports only lowercase pathes: '%s'", path_to_find);
	if ( !can_traverse_path_sync(path_to_find, find_flags) )
		return									false;

	find_env									env;
	path_string									partial_path;
	env.out_pin_iterator					=	out_pin_iterator;
	env.partial_path						=	partial_path.c_str();
	env.path_to_find						=	path_to_find;
	env.find_flags							=	find_flags;
	
	for ( path_iterator	it = path_to_find; it != path_iterator::end(); ++it )
	{
		partial_path.assign						(path_to_find, it.cur_end());
		env.node							=	find_node_no_mount(partial_path.c_str(), NULL);

		if ( !env.node )
		{
			out_pin_iterator->assign			(fs_iterator::end());
			return								true;
		}

		if ( env.node->is_link() )
		{
			bool const sync_link_result		=	find_sync_link_helper(out_pin_iterator, env);
			R_ASSERT_U							(sync_link_result);
			return								true;
		}
	}

	out_pin_iterator->assign					(env.node, find_referenced_link_node(env.node), 
												 (find_flags & find_recursively) ?
													 fs_iterator::type_recursive : fs_iterator::type_non_recursive);

	return										true;
}

} // namespace fs
} // namespace xray