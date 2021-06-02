////////////////////////////////////////////////////////////////////////////
//	Created		: 03.08.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "patch.h"

namespace xray {
namespace vfs {

using namespace fs_new;

bool   equal_nodes						(base_node<> * const 					first, 
										 base_node<> * const 					second,
										 synchronous_device_interface * const	device,
										 compressor * const						compressor,
										 memory::base_allocator * const			allocator);

void   mount_nodes_for_patch::insert_new_for_children	(virtual_path_string const &	path, 
														 vfs_iterator &					from_iterator, 
														 vfs_iterator &					to_iterator)
{		
	for ( vfs_iterator	to_child	=	to_iterator.children_begin(),
						end_child	=	to_iterator.children_end();
						to_child	!=	end_child;
						++to_child )
	{
		pcstr const child_name			=	to_child.get_name();
		vfs_iterator from_child			=	from_iterator ? 
											from_iterator.find_child(child_name) : vfs_iterator::end();

		virtual_path_string child_path	=	path;
		child_path.append_path				(child_name);
		
		insert_new							(child_path, from_child, to_child);
	}
}

void   mount_nodes_for_patch::insert_new	(virtual_path_string const &	path, 
											 vfs_iterator &					from_iterator, 
											 vfs_iterator &					to_iterator)
{
	base_node<> * const from_node		=	from_iterator.get_node();
	base_node<> * const to_node			=	to_iterator.get_node();

	bool should_insert					=	false;

	if ( !from_iterator )
		should_insert					=	true;
	else if ( from_iterator.is_folder() && to_iterator.is_folder() )
		should_insert					=	false;
	else if ( !equal_nodes(from_node, to_node, & m_args.synchronous_device, m_compressor, m_args.allocator) )
		should_insert					=	true;
		
	if ( !should_insert && from_iterator.is_link() )
	{
		base_node<> * const target		=	from_iterator.get_link_target();
		virtual_path_string					target_path;
		target->get_full_path				(& target_path);
		vfs_iterator const to_target	=	m_to_root.find_child(target_path);
		base_node<> * const to_target_node	=	to_target.get_node();

		if ( !equal_nodes(from_node, to_target_node, & m_args.synchronous_device, m_compressor, m_args.allocator) )
			should_insert				=	true;
	}

	if ( should_insert )
	{
		base_node<> * const parent		=	create_branch_for_node(path);
		base_folder_node<> * const parent_folder	=	node_cast<base_folder_node>(parent);
		
		if ( !parent_folder->find_child(to_node->get_name(), NULL) )
			duplicate_to_patch				(to_node, parent_folder, path);
	}

	if ( to_iterator.is_folder() )
		insert_new_for_children		(path, from_iterator, to_iterator);
}

} // namespace vfs
} // namespace xray
