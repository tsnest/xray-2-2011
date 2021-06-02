////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "unmount.h"
#include "branch_locks.h"
#include <xray/vfs/virtual_file_system.h>

namespace xray {
namespace vfs {

using namespace		fs_new;

void   unmounter::hot_unmount_node		(base_node<> * const node, u32 const hash)
{
	m_hashset.erase							(hash, node);
	if ( node_cast<mount_root_node_base>(node) == m_root_node_to_unmount )
	{
		m_root_node_to_unmount->erased	=	true;
		return;
	}

	if ( physical_folder_node<> * physical_folder	=	node_cast<physical_folder_node>(node) )
	{
		if ( !physical_folder->is_allocated_in_parent_arena() )
			m_root_node_to_unmount->prepend_erased_node	(node);
	}
	else
	{
		physical_file_node<> * const file	=	node_cast<physical_file_node>(node);
		R_ASSERT							(file);
		if ( !file->is_allocated_in_parent_arena() )
			m_root_node_to_unmount->prepend_erased_node	(node);
	}
}

void   unmounter::hot_unmount			()
{
	R_ASSERT								(m_args.submount_node);

	LOG_INFO								("hot unmount '%s' from '%s'", 
											 m_args.get_physical_path().c_str(),
											 m_args.virtual_path.c_str());

	if ( m_file_system.on_node_hides )
	{
		vfs_iterator	iterator			(m_args.submount_node, NULL, & m_file_system.hashset, vfs_iterator::type_non_recursive);
		m_file_system.on_node_hides			(iterator);
	}
	
	u32 const hash						=	fs_new::path_crc32(m_args.virtual_path);

	base_node<> * node_to_unmount		=	NULL;
	base_node<> * overlap_of_node_to_unmount	=	NULL;	
	is_part_of_mount predicate				(m_root_node_to_unmount);
	recursive_unmount_node					(m_args.virtual_path, hash, predicate, node_to_unmount, overlap_of_node_to_unmount);

	virtual_path_string						parent_path;
	get_path_without_last_item				(& parent_path, m_args.virtual_path.c_str());
	u32 const parent_path_hash			=	fs_new::path_crc32(parent_path);

	base_folder_node<> * const parent_to_traverse	=	node_to_unmount->get_parent();

	recursive_traverse_folder				(parent_path, parent_path_hash, is_exact_node(node_to_unmount), parent_to_traverse);
}

} // namespace xray
} // namespace vfs

