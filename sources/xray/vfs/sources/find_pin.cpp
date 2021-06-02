////////////////////////////////////////////////////////////////////////////
//	Created		: 01.12.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "nodes.h"
#include "find.h"
#include <xray/vfs/hashset.h>

namespace xray {
namespace vfs {

using namespace fs_new;

void   change_subfat_ref_for_node				(int change, base_node<> * node, u32 * in_out_mount_operation_id)
{
	vfs_mount * sub_fat					=	mount_of_node(node);
	if ( !sub_fat )
		return;

	mount_root_node_base<> * mount_root	=	sub_fat->get_mount_root();
	if ( node != node_cast<base_node>(mount_root) )
		return;

	virtual_path_string const virtual_path	=	mount_root->virtual_path;
	u32 const mount_operation_id		=	mount_root->mount_operation_id;
	
	if ( mount_operation_id > (* in_out_mount_operation_id) )
	{
		if ( change < 0 )
			return;
		* in_out_mount_operation_id		=	mount_operation_id;
	}

	u32 const result_reference_count	=	sub_fat->change_reference_count_change_may_destroy_this	(change);

	XRAY_UNREFERENCED_PARAMETER				(virtual_path);
	LOG_TRACE								("%s sub_fat: '%s' (%d)", 
											 change == +1 ? "pinned" : "unpinned",
											 virtual_path.c_str(),
											 result_reference_count);
}

void  change_subfat_ref_for_overlapped	(int change, base_node<> * topmost_node, u32 * in_out_mount_operation_id)
{
	for ( base_node<> * it_node			=	topmost_node;
						it_node;
						it_node			=	it_node->get_next_overlapped() )
	{
		change_subfat_ref_for_node			(change, it_node, in_out_mount_operation_id);
	}
}


void   decref_children					(base_node<> *	node, 
										 find_enum		find_flags, 
										 vfs_hashset *	hashset,
										 u32 			mount_operation_id,
										 bool			is_root_node)
{
	bool const traverse_children		=	(find_flags & find_recursively) || is_root_node;
	
	if ( node->is_folder() && traverse_children )
	{
		base_folder_node<> * node_folder	=	node_cast<base_folder_node>(node);
		for ( base_node<> *	it_child	=	node_folder->get_first_child();
							it_child;	)
		{
			base_node<> * next_child	=	it_child->get_next();
			decref_children		(it_child, find_flags, hashset, mount_operation_id, false);
			it_child					=	next_child;
		}
	}

	if ( is_root_node )
		return;

	if ( node->is_hard_link() )
	{
		base_node<> * const ref_node	=	find_referenced_link_node(node);
		R_ASSERT							(ref_node);
		decref_children						(ref_node, find_flags, hashset, mount_operation_id, false);
	}

	change_subfat_ref_for_overlapped		(-1, node, & mount_operation_id);
}

void   unlock_and_decref_branch			(base_node<> * node, lock_type_enum unlock_type, u32 mount_operation_id)
{
	if ( !node )
		return;

	change_subfat_ref_for_overlapped		(-1, node, & mount_operation_id);
	base_folder_node<> * parent			=	node->get_parent();

	unlock_node								(node, unlock_type);

	if ( parent )
	{
		unlock_and_decref_branch			(node_cast<base_node>(parent), soften_lock(unlock_type), mount_operation_id);
	}
}

void   unlock_and_decref_recursively	(base_node<> *	node, 
										 lock_type_enum	branch_lock_type,
										 find_enum		find_flags, 
										 vfs_hashset *	hashset, 
										 u32 			mount_operation_id)
{
	decref_children							(node, find_flags, hashset, mount_operation_id);
	unlock_and_decref_branch				(node, branch_lock_type, mount_operation_id);
}

} // namespace vfs
} // namespace xray
