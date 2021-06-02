////////////////////////////////////////////////////////////////////////////
//	Created		: 25.06.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mount_utils.h"
#include <xray/vfs/hashset.h>
#include <xray/vfs/virtual_file_system.h>
#include "free_node.h"
#include "overlapped_node_iterator.h"

namespace xray {
namespace vfs {

using namespace		fs_new;

//void   relink_child						(base_node<> * const child, base_node<> * const parent)
//{
//	base_folder_node<> * const parent_folder	=	node_cast<base_folder_node>(parent);
//	base_node<> * current_child			=	parent_folder->find_child(child->get_name(), NULL);
//	if ( !current_child )
//	{
//		parent_folder->prepend_child		(child);
//		return;
//	}
//
//	base_node<> * const overlapped_parent	=	parent->get_next_overlapped();
//	R_ASSERT								(overlapped_parent);
//	R_ASSERT								(overlapped_parent->is_folder());
//
//	parent_folder->unlink_child				(current_child);
//	relink_child							(current_child, overlapped_parent);
//	parent_folder->prepend_child			(child);
//}

void   relink_children_of_folder		(base_node<> * overlapper, u32 separator_mount_id)
{
	base_folder_node<> * overlapper_folder	=	node_cast<base_folder_node>(overlapper);
	
	base_folder_node<> * overlapped_folder	=	NULL;
	for ( base_node<> * it_node			=	overlapper;
						it_node			!=	NULL;
						it_node			=	it_node->get_next_overlapped() )
	{
		if ( mount_id_of_node(it_node) < separator_mount_id )
		{
			overlapped_folder			=	node_cast<base_folder_node>(it_node);
			break;
		}
	}

	node_list	new_children;
	for ( base_node<> *	it_child		=	overlapper_folder->get_first_child();
						it_child;	)
	{
		base_node<> * next_child		=	it_child->get_next();
		
		if ( mount_id_of_node(it_child) < separator_mount_id )
		{
			R_ASSERT						(!overlapped_folder->find_child(it_child->get_name(), NULL));
			overlapped_folder->prepend_child	(it_child);
		}
		else
		{
			new_children.push_back			(it_child);
			if ( it_child->is_folder() )
				relink_children_of_folder	(it_child, separator_mount_id);
		}

		it_child						=	next_child;
	}

	overlapper_folder->set_first_child		(new_children.front());
}

void   relink_children_of_folder_range	(base_node<> * const	it_overlapper, 
										 base_node<> * const	last_overlapper,
										 u32 const				separator_mount_id)
{
	if ( it_overlapper != last_overlapper )
		relink_children_of_folder_range		(it_overlapper->get_next_overlapped(), last_overlapper, separator_mount_id);

	relink_children_of_folder				(it_overlapper, separator_mount_id);
}

void   break_separated_links_of_folder	(base_node<> * overlapper, u32 separator_mount_id)
{
	base_folder_node<> * overlapper_folder	=	node_cast<base_folder_node>(overlapper);
	for ( base_node<> *	it_child		=	overlapper_folder->get_first_child();
						it_child;
						it_child		=	it_child->get_next() )
	{
		R_ASSERT							(mount_id_of_node(it_child) > separator_mount_id);

		base_node<> * const child_overlapped	=	it_child->get_next_overlapped();
		if ( child_overlapped && mount_id_of_node(child_overlapped) < separator_mount_id )
			it_child->set_next_overlapped	(NULL);

		if ( it_child->is_folder() )
			break_separated_links_of_folder	(it_child, separator_mount_id);
	}
}

void   break_separated_links_of_folder_range	(base_node<> * const	it_overlapper, 
												 base_node<> * const	last_overlapper,
												 u32 const				separator_mount_id)
{
	if ( it_overlapper != last_overlapper )
		break_separated_links_of_folder_range	(it_overlapper->get_next_overlapped(), last_overlapper, separator_mount_id);

	break_separated_links_of_folder			(it_overlapper, separator_mount_id);
}

void   separate_folders_by_file_node	(vfs_hashset & hashset, pcstr path, u32 hash, base_node<> * last_overlapper, u32 separator_mount_id)
{
	vfs_hashset::begin_end_pair begin_end	=	hashset.equal_range(path, hash, lock_type_write);

	overlapped_node_iterator	it		=	begin_end.first;
	overlapped_node_iterator	it_end	=	begin_end.second;
	base_node<> * first_overlapper		=	NULL;

	for ( ; it != it_end;	++it )
	{
		base_node<> * it_overlapper		=	it.c_ptr();
		if ( it_overlapper->is_file() )
		{
			first_overlapper			=	NULL;
		}
		else if ( !first_overlapper )
			first_overlapper			=	it_overlapper;

		if ( it_overlapper == last_overlapper )
			break;
	}

	relink_children_of_folder_range			(first_overlapper, last_overlapper, separator_mount_id);
	break_separated_links_of_folder_range	(first_overlapper, last_overlapper, separator_mount_id);
}

enum	exchange_nodes_action			{ exchange_nodes_replace, exchange_nodes_insert };

void   exchange_nodes_impl				(fs_new::virtual_path_string const &	virtual_path,
										 u32 									virtual_path_hash,
										 exchange_nodes_action 					action,
										 virtual_file_system & 					file_system,
										 base_node<> * 							what_node, 
										 base_node<> * 							with_node, 
										 base_node<> * 							overlapper,
										 base_node<> * 							root_write_lock,
										 memory::base_allocator *				allocator)
{
	base_folder_node<> * parent_folder	=	what_node->get_parent();
	parent_folder->unlink_child				(what_node);
	parent_folder->prepend_child			(with_node);

	base_node<> * overlapped			=	what_node->get_next_overlapped();
	with_node->set_next_overlapped		(overlapped);

	if ( overlapper )
		overlapper->set_next_overlapped		(with_node);

	if ( action == exchange_nodes_insert )
	{
		file_system.hashset.insert			(virtual_path_hash, with_node, mount_id_of_node(with_node));
		free_node							(file_system, what_node, root_write_lock, virtual_path_hash, allocator);
	}
	else
		file_system.hashset.replace			(virtual_path_hash, with_node, what_node, mount_id_of_node(with_node));

	if ( overlapper && overlapper->is_folder() && 
		 overlapped && overlapped->is_folder() )
	{
		separate_folders_by_file_node		(file_system.hashset, virtual_path.c_str(), virtual_path_hash, overlapper, 
											 mount_id_of_node(with_node));
	}
}

void   replace_and_free_what_node		(fs_new::virtual_path_string const &	virtual_path,
										 u32 									virtual_path_hash,
										 virtual_file_system & 					file_system,
										 base_node<> * 							what_node, 
										 base_node<> * 							with_node, 
										 base_node<> * 							overlapper,
										 base_node<> * 							root_write_lock,
										 memory::base_allocator *				allocator)
{
	exchange_nodes_impl						(virtual_path, virtual_path_hash, exchange_nodes_insert, file_system, 
											 what_node, with_node, overlapper, root_write_lock, allocator);
}

void   exchange_nodes					(fs_new::virtual_path_string const &	virtual_path,
										 u32 									virtual_path_hash,
										 virtual_file_system & 					file_system,
										 base_node<> * 							what_node, 
										 base_node<> * 							with_node, 
										 base_node<> * 							overlapper)
{
	exchange_nodes_impl						(virtual_path, virtual_path_hash, exchange_nodes_replace,
											 file_system, what_node, with_node, overlapper, NULL, NULL);
}

base_node<> *   find_node_of_mount		(vfs_hashset &					hashset,
										 virtual_path_string const &	virtual_path,
										 u32 const						virtual_path_hash,
										 u32 const						mount_id)
{
	vfs_hashset::begin_end_pair	begin_end	=	hashset.equal_range(virtual_path.c_str(), virtual_path_hash, lock_type_read);
	for ( overlapped_node_iterator	it		=	begin_end.first,
									it_end	=	begin_end.second;
									it		!=	it_end;
									++it )
	{
		base_node<> * node				=	it.c_ptr();
		if ( mount_id_of_node(node) == mount_id )
			return							node;
	}

	return									NULL;
}

} // namespace vfs
} // namespace xray