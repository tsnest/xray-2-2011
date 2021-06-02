////////////////////////////////////////////////////////////////////////////
//	Created		: 01.12.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_impl.h"
#include "fs_impl_finding.h"

namespace xray {
namespace fs {

void   pin_sub_fat_if_node_is_root_of_sub_fat (int change, fat_node<> * node)
{
	if ( sub_fat_resource * sub_fat = g_fat->impl()->get_db(node) )
	{
		if ( node == sub_fat->root_node )
		{
			u32 const result_reference_count =	sub_fat->change_reference_count_change_may_destroy_this	(change);
			path_string	const sub_fat_logical_path	=	sub_fat->virtual_path;
			XRAY_UNREFERENCED_PARAMETER			(sub_fat_logical_path);
			LOGI_INFO							("fs", 
												 "%s sub_fat: %s (%d)", 
												 change == +1 ? "pinned" : "unpinned",
												 sub_fat_logical_path.c_str(),
												 result_reference_count);
		}
	}
}

static
void   pin_sub_fats_of_self_and_children_impl (int change, fat_node<> * node, find_enum find_flags, fat_node<> * root_node)
{
	pin_sub_fat_if_node_is_root_of_sub_fat		(change, node);

	if ( node->is_soft_link() )
	{
		fat_node<> * const referenced_node	=	node->find_referenced_link_node();
		R_ASSERT								(referenced_node);
		pin_sub_fats_of_ancestors				(change, referenced_node);
		pin_sub_fats_of_self_and_children		(change, referenced_node, find_flags);
		return;
	}

	if ( !node->is_folder() )
		return;

	if ( !(find_flags & find_recursively) && (node != root_node) )
		return;

	for ( fat_node<> *	it_child	=	node->get_first_child();
						it_child;
						it_child	=	it_child->get_next() )
	{
		pin_sub_fats_of_self_and_children_impl	(change, it_child, find_flags, root_node);
	}
}

void   pin_sub_fats_of_self_and_children (int change, fat_node<> * node, find_enum find_flags)
{
	pin_sub_fats_of_self_and_children_impl	(change, node, find_flags, node);
}

void   pin_sub_fats_of_ancestors (int change, fat_node<> * node)
{
	for	( fat_node<> *	it_ancestor	=	node->get_parent();
						it_ancestor;
						it_ancestor	=	it_ancestor->get_parent() )
	{
		pin_sub_fat_if_node_is_root_of_sub_fat	(change, it_ancestor);
	}
}

void   pin_sub_fats_of_node	(int change, fat_node<> * node, find_enum find_flags)
{
	pin_sub_fats_of_ancestors				(change, node);
	pin_sub_fats_of_self_and_children		(change, node, find_flags);
}

} // namespace fs
} // namespace xray
