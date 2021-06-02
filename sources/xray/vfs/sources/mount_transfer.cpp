////////////////////////////////////////////////////////////////////////////
//	Created		: 19.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mount_transfer.h"
#include <xray/vfs/hashset.h>
#include "overlapped_node_iterator.h"

namespace xray {
namespace vfs {

using namespace fs_new;	

void   transfer_children::find_first_and_last_overlapper	(base_node<> * * const			out_first_overlapper,
															 base_node<> * * const			out_last_overlapper,
															 virtual_path_string const &	path, 
															 u32 const						hash,
															 base_node<> * const			child)
{
	vfs_hashset::begin_end_pair const begin_end	=	m_hashset.equal_range(path.c_str(), hash, lock_type_read);

	overlapped_node_iterator	it			(begin_end.first);
	overlapped_node_iterator	it_end		(begin_end.second);

	u32 const dest_start_mount_id		=	mount_id_of_node(m_dest_start);
	base_node<> * first_overlapper		=	NULL;
	base_node<> * last_overlapper		=	NULL;
	for ( ; it != it_end; ++it )
	{
		base_node<> * const it_node		=	it.c_ptr();

		if ( it_node == child )
			break;

		if ( mount_id_of_node(it_node) <= dest_start_mount_id )
		{
			if ( !first_overlapper )
				first_overlapper		=	it_node;

			if ( it_node->is_file() )
				first_overlapper		=	NULL;

			last_overlapper				=	it_node;
		}
	}

	* out_first_overlapper				=	first_overlapper;
	* out_last_overlapper				=	last_overlapper;
}

void   transfer_children::transfer_child	(virtual_path_string const &	path, 
											 u32 const						hash,
											 base_node<> * const			child)
{
	base_node<> * first_overlapper		=	NULL;
	base_node<> * last_overlapper		=	NULL;
	find_first_and_last_overlapper			(& first_overlapper, & last_overlapper, path, hash, child);

	if ( !last_overlapper )
	{
		base_folder_node<> * const dest_folder	=	node_cast<base_folder_node>(m_dest_start);
		dest_folder->prepend_child			(child);
		return;
	}
	
	R_ASSERT								(!last_overlapper->get_next_overlapped());
	last_overlapper->set_next_overlapped	(child);
	u32 const last_overlapper_mount_id	=	mount_id_of_node(last_overlapper);

	base_node<> * parent_to_link		=	NULL;
	for ( base_node<> * it_parent		=	m_dest_start;
						it_parent		!=	NULL;
						it_parent		=	it_parent->get_next_overlapped() )
	{
		if ( mount_id_of_node(it_parent) < last_overlapper_mount_id )
		{
			parent_to_link				=	it_parent;
			break;
		}
	}

	if ( parent_to_link->is_file() || parent_to_link == node_cast<base_node>(m_source_folder) )
	{
		m_new_source_nodes.push_back		(child);
	}
	else
	{
		base_folder_node<> * const parent_to_link_folder	=	node_cast<base_folder_node>(parent_to_link);
		parent_to_link_folder->prepend_child	(child);
	}

	if ( child->is_folder() && first_overlapper )
		transfer_children					(m_hashset, path, hash, first_overlapper, child);
}

transfer_children::transfer_children	(vfs_hashset &					hashset,
										 virtual_path_string const &	path, 
										 u32 const						hash,
										 base_node<> * const			dest_start, 
										 base_node<> * const			source_folder)
	: m_hashset(hashset), m_path(path), m_hash(hash), m_dest_start(dest_start)
{
	R_ASSERT								(dest_start->is_folder());
	m_source_folder						=	node_cast<base_folder_node>(source_folder);

	u32 const path_length				=	m_path.length();
	
	for ( base_node<> *		it_child	=	source_folder->get_first_child();
							it_child	!=	NULL;	)
	{
		base_node<> * const it_next		=	it_child->get_next();

		u32 const child_hash			=	path_crc32(virtual_path_string(it_child->get_name()), hash);
		virtual_path_string	&	child_path	=	m_path;
		child_path.appendf					("%c%s", virtual_path_string::separator, it_child->get_name());

		transfer_child						(child_path, child_hash, it_child);
	
		m_path.set_length					(path_length);
		it_child						=	it_next;
	}

	m_source_folder->set_first_child		(m_new_source_nodes.front());
}

} // namespace vfs
} // namespace xray