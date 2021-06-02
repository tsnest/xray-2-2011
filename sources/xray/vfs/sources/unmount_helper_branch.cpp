////////////////////////////////////////////////////////////////////////////
//	Created		: 28.03.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "unmount.h"
#include "mount_physical_path.h"
#include <xray/vfs/mount_history.h>
#include <xray/fs/path_string_utils.h>
#include "free_node.h"
#include <xray/fs/path_part_iterator.h>
#include <xray/fs/path_string_utils.h>
#include <xray/fs/physical_path_iterator.h>

namespace xray {
namespace vfs {

using namespace fs_new;

struct node_with_hash
{
	base_node<> *						node;
	u32									hash;
	node_with_hash *					next;
	node_with_hash						(base_node<> * node, u32 hash) : node(node), hash(hash) {}
};

typedef	intrusive_list<node_with_hash, node_with_hash*, &node_with_hash::next, threading::single_threading_policy>
	node_with_hash_list;

void   unmounter::unmount_helper		(virtual_path_string &	path, 
										 u32 const				hash,
										 base_node<> *			parent_to_unmount,
										 base_node<> *			node_to_unmount,
										 base_node<> * &		out_overlap_of_node_to_unmount)
{
	base_node<> *	first_to_unmount	=	NULL;
	base_node<> *	last_to_unmount		=	NULL;
	base_node<> *	next_to_last		=	NULL;
	find_range_to_unmount					(path, hash, is_exact_node(parent_to_unmount), 
											 first_to_unmount, last_to_unmount, next_to_last);
	out_overlap_of_node_to_unmount		=	next_to_last;

	R_ASSERT								(last_to_unmount && last_to_unmount == parent_to_unmount);
	R_ASSERT								(last_to_unmount->is_folder());

	is_exact_node predicate					(node_to_unmount);
	recursive_unmount_folder_range			(path, hash, predicate, first_to_unmount, last_to_unmount);

	node_to_unmount						=	last_to_unmount;
}

void   unmounter::unmount_helper_branch	(mount_helper_node<> *	parent_to_unmount_helper, 
										 base_node<> *			child_to_unmount,
										 base_node<> *			overlap_of_unmount,
										 u32					child_to_unmount_hash)
{
	virtual_path_string						branch_path;
	get_path_without_last_item				(& branch_path, m_args.virtual_path.c_str());

	u32 hash							=	child_to_unmount_hash;
	bool root_visited					=	false;
	while ( branch_path.length() || !root_visited )
	{
		if ( !parent_to_unmount_helper )
			break;
		R_ASSERT							(parent_to_unmount_helper);
		base_node<> * parent_to_unmount	=	node_cast<base_node>(parent_to_unmount_helper);
		mount_helper_node<> * next_parent	=	parent_to_unmount->get_mount_helper_parent();

		hash							=	path_crc32(branch_path);
		if ( child_to_unmount )
			unmount_helper					(branch_path, hash, parent_to_unmount, child_to_unmount, overlap_of_unmount);

		child_to_unmount				=	parent_to_unmount;

		if ( !branch_path.length() && !root_visited )
			root_visited				=	true;
	
		parent_to_unmount_helper		=	next_parent;
		cut_last_item_from_path				(& branch_path);
	}

	if ( child_to_unmount )
	{
		if ( overlap_of_unmount )
			overlap_of_unmount->set_next_overlapped	(child_to_unmount->get_next_overlapped());
		
		free_node							(m_file_system, child_to_unmount, m_args.root_write_lock, hash, m_args.allocator);
	}
	else
		R_ASSERT							(!overlap_of_unmount);
}

} // namespace vfs
} // namespace xray
