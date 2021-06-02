////////////////////////////////////////////////////////////////////////////
//	Created		: 08.04.2011
//	Author		: 
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "free_node.h"
#include "branch_locks.h"
#include <xray/vfs/virtual_file_system.h>

namespace xray {
namespace vfs {

enum erase_from_hashset_enum			{ erase_from_hashset_false, erase_from_hashset_true };

void   free_node_impl					(virtual_file_system & 		file_system,
										 base_node<> *				node, 
										 base_node<> * &			root_write_lock,
										 u32						hash,
										 memory::base_allocator *	allocator,
										 erase_from_hashset_enum	erase_from_hashset)
{
	if ( file_system.on_node_unmount )
	{
		vfs_iterator	iterator			(node, NULL, & file_system.hashset, vfs_iterator::type_non_recursive);
		file_system.on_node_unmount			(iterator);
	}

	ASSERT									(root_write_lock);
	fixed_string512	addr;
	addr.assignf							("'%s' [0x%08x]", node->get_name(), node);
	if ( node->is_folder() )
		addr.appendf						(" folder[0x%08x]", node_cast<base_folder_node>(node));
	else
		addr.appendf						(" file");
	if ( node->is_mount_root() )
		addr.append							(" mount-root");
	else if ( node->is_mount_helper() )
		addr.append							(" mount-helper");

	LOG_TRACE								("freing %s", addr.c_str());

	mount_root_node_base<> * mount_root_base	=	node->is_link() ? NULL : node_cast<mount_root_node_base>(node);
	if ( mount_root_base )
	for ( base_node<> * it_erased_node	=	mount_root_base->first_erased;
						it_erased_node;	)
	{
		base_node<> * const	next		=	it_erased_node->get_next();

		free_node_impl						(file_system, it_erased_node, root_write_lock, 0, allocator, erase_from_hashset_false);	

		it_erased_node					=	next;
	}
	
	bool const node_is_root_lock		=	(node == root_write_lock);
	if ( node_is_root_lock )
	{
		if ( node->get_next_overlapped() )
		{
			base_node<> * overlapped	=	node->get_next_overlapped();
			lock_node						(overlapped, lock_type_write, lock_operation_lock);
			root_write_lock				=	overlapped;
		}
		else
			root_write_lock				=	NULL;
	}

	bool const node_is_erased_mount_root	=	mount_root_base && mount_root_base->erased;

	if ( erase_from_hashset == erase_from_hashset_true && !node_is_erased_mount_root )
		file_system.hashset.erase						(hash, node);
	
	if ( node_is_root_lock )
		unlock_node							(node, lock_type_write);
	
	if ( node->is_universal_file() )
	{
		universal_file_node<> * uni_node	=	node_cast<universal_file_node>(node);
		uni_node->~universal_file_node		();
		XRAY_FREE_IMPL						(allocator, uni_node);
	}
	else if ( node->is_physical() )
 	{
		if ( node->is_erased() )
 		{
 			erased_node<> * erase_node	=	node_cast<erased_node>(node);
			erase_node->~erased_node		();
			XRAY_FREE_IMPL					(allocator, erase_node);
		}
 		else if ( node->is_file() )
 		{
 			if ( node->is_mount_root() )
 			{
 				physical_file_mount_root_node<> * mount_root	=	cast_physical_file_mount_root(node);
				mount_root->~physical_file_mount_root_node();
 				XRAY_FREE_IMPL				(allocator, mount_root);
 			}
 			else
 			{
 				physical_file_node<> * file	=	node_cast<physical_file_node>(node);
 				if ( !file->is_allocated_in_parent_arena() )
 				{
					file->~physical_file_node();
 					XRAY_FREE_IMPL			(allocator, file);
 				}
 			}
 		}
 		else if ( node->is_folder() )
 		{
 			physical_folder_node<> * folder	=	node_cast<physical_folder_node>(node);
			node_cast<base_folder_node>(folder)->set_first_child	(NULL);

 			pvoid children_arena		=	(pvoid)folder->children_arena.c_ptr();
 			XRAY_FREE_IMPL					(allocator, children_arena);
			folder->children_arena.reset	();
 
 			if ( node->is_mount_root() )
 			{
 				physical_folder_mount_root_node<> * mount_root	=	node_cast<physical_folder_mount_root_node>(node);
				mount_root->~physical_folder_mount_root_node();
 				XRAY_FREE_IMPL				(allocator, mount_root);
 			}
 			else if ( !folder->is_allocated_in_parent_arena() )
 			{
				folder->~physical_folder_node();
 				XRAY_FREE_IMPL				(allocator, folder);
 			}
 		}
 	}
	else if ( node->is_archive() )
	{
		if ( node->is_hard_link() )
 		{
 			hard_link_node<> * hard_link	=	node_cast<hard_link_node>(node);
			hard_link->~hard_link_node		();
			XRAY_UNREFERENCED_PARAMETER		(hard_link);
		}
		else if ( node->is_soft_link() )
 		{
 			soft_link_node<> * soft_link	=	node_cast<soft_link_node>(node);
			soft_link->~soft_link_node		();
			XRAY_UNREFERENCED_PARAMETER		(soft_link);
		}
		else if ( node->is_erased() )
 		{
 			erased_node<> * erase_node	=	node_cast<erased_node>(node);
			erase_node->~erased_node		();
			XRAY_UNREFERENCED_PARAMETER		(erase_node);
		}
		else if ( node->is_mount_root() )
		{
			R_ASSERT						(node->is_folder());
			archive_folder_mount_root_node<> * mount_root	=	node_cast<archive_folder_mount_root_node>(node);
			pcstr buffer				=	(pcstr)mount_root->nodes_buffer;
			XRAY_FREE_IMPL					(allocator, buffer); // freeing all archive nodes at once
		}
	}
	else if ( node->is_mount_helper() )
	{
		mount_helper_node<> * folder	=	node_cast<mount_helper_node>(node);
		folder->~mount_helper_node			();
		XRAY_FREE_IMPL						(allocator, folder);
	}
	else if ( node->is_folder() )
	{
		R_ASSERT							(node->get_flags() == vfs_node_is_folder);
		base_folder_node<> * folder		=	node_cast<base_folder_node>(node);
		folder->~base_folder_node			();
		XRAY_FREE_IMPL						(allocator, folder);
	}
	else
		NOT_IMPLEMENTED						();
}

void   free_node						(virtual_file_system & 		file_system,
										 base_node<> *				node, 
										 base_node<> * &			root_write_lock,
										 u32						hash,
										 memory::base_allocator *	allocator)
{
	free_node_impl							(file_system, 
											 node, 
											 root_write_lock,
											 hash,
											 allocator,
											 erase_from_hashset_true);
}

} // namespace vfs
} // namespace xray