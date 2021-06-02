////////////////////////////////////////////////////////////////////////////
//	Created		: 21.12.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_file_system_impl.h"
#include <xray/fs_utils.h>
#include "fs_helper.h"

namespace xray {
namespace fs   {

//-----------------------------------------------------------------------------------
// unmounting disk/db
//-----------------------------------------------------------------------------------

struct disk_unmount_pred
{	
	disk_unmount_pred(pcstr disk_dir) : disk_dir(disk_dir) {}	
	bool   operator () (fat_node<> * work_node) const	
	{		
		if ( !work_node->is_disk() )			
			return					false;		
		
		pcstr node_disk_dir			=	work_node->is_folder() ? work_node->cast_disk_folder()->m_disk_path : 	
																 work_node->cast_disk_file()->m_disk_path;		
		return strstr(node_disk_dir, disk_dir) == node_disk_dir;	
	}	
	
	pcstr disk_dir;
};

void   file_system_impl::unmount_disk (pcstr fat_dir, pcstr disk_dir, respect_mount_history_bool respect_mount_history)
{	
	if ( respect_mount_history )
		on_unmount						(mount_history::mount_type_disk, fat_dir, disk_dir);

	threading::mutex_raii		raii	(m_mount_mutex);

	verify_path_is_portable				(fat_dir);
	verify_path_is_portable				(disk_dir);
	u32		hash;	
	fat_node<> *	work_node		=	find_node(fat_dir, & hash);
	if ( !work_node )
		return;

	LOGI_INFO							("file_system", "unmount disk started '%s' from '%s'", disk_dir, fat_dir);

	unmount_node						(work_node, hash, disk_unmount_pred(disk_dir));

	LOGI_INFO							("file_system", "unmount disk ended '%s' from '%s'", disk_dir, fat_dir);
}

void   file_system_impl::unmount_db (pcstr fat_dir, pcstr disk_file)
{
	threading::mutex_raii		raii	(m_mount_mutex);

	on_unmount							(mount_history::mount_type_db, fat_dir, disk_file);

	verify_path_is_portable				(disk_file);
	verify_path_is_portable				(fat_dir);

	u32				hash;
	fat_node<> *	work_node	=	find_node(fat_dir, & hash);
	if ( !work_node )
		return;

	db_data::iterator db_it	=	std::find_if(m_db_data.begin(), m_db_data.end(), find_db_by_path(fat_dir, disk_file));
	if ( db_it == m_db_data.end() )
		return;

	db_record db				=	* db_it;
	LOGI_INFO					("file_system", "unmount fat started: '%s' from '%s'", disk_file, fat_dir);

	unmount_node				(work_node, hash, db_unmount_pred(this, db.root_node));

	db.destroy					();

	m_db_data.erase				(db_it);

	LOGI_INFO					("file_system", "unmount fat ended '%s' from '%s'", disk_file, fat_dir);
}

template <class UnmountPredicate>
void   file_system_impl::unmount_node (fat_node<> *				work_node, 
									   u32 const				hash,
									   UnmountPredicate const &	unmount_pred)
{
	resources::unmanaged_resource_ptr	unmanaged_resource;
	resources::managed_resource_ptr		managed_resource;
	bool is_associated		=	work_node->is_folder() ? NULL : work_node->is_associated();
	if ( is_associated )
	{
		if ( unmanaged_resource	=	work_node->get_associated_unmanaged_resource_ptr() )
		{
			if ( unmanaged_resource->is_inlined_in_fat() )
			{
				unmanaged_resource->unset_as_inlined_in_fat	();
				work_node->set_associated					(NULL);
				is_associated							=	false;
			}
			else if ( unmanaged_resource->pinned_by_game_resource_manager() )
			{
				resources::resource_base * const resource_base	=	unmanaged_resource.c_ptr();
				unmanaged_resource				=	NULL;
				m_on_resource_leaked_callback		(resource_base);
				is_associated					=	work_node->is_associated();
			}
		}
		else if (  managed_resource	=	work_node->get_associated_managed_resource_ptr() )
		{
			if ( managed_resource->pinned_by_game_resource_manager() )
			{
				resources::resource_base * const resource_base	=	managed_resource.c_ptr();
				managed_resource				=	NULL;
				m_on_resource_leaked_callback		(resource_base);
				is_associated					=	work_node->is_associated();
			}
		}
	}

	R_ASSERT										(work_node->is_folder() ||
													 !is_associated,
													 "omg! trying to free node, while it's used by "
													 "resources manager!");

	fat_node<> *	 parent_of_work_node		=	work_node->m_parent;
	fat_folder_node<> * parent_folder_of_work_node	=	parent_of_work_node ? 
													parent_of_work_node->cast_folder() : NULL;
	 
	fat_folder_node<> * work_folder				=	work_node->cast_folder();

	bool			 need_replace				=	false;
	fat_node<> *	 freeing_node				=	NULL;

	fat_node<> *	 cur_node					=	* m_hash_set.find(hash);

	ASSERT											(cur_node);
	while ( cur_node )
	{
		if ( cur_node->same_path(work_node) )
		{
			if ( unmount_pred(cur_node) )
 			{
				freeing_node		=	cur_node;
		 		need_replace		=	(freeing_node == work_node);
		 		m_hash_set.erase		(hash, freeing_node);
				break;
			}
		}

		cur_node					=	cur_node->get_next_hash_node();
	}

	fat_node<> *	replacing_node	=	NULL;
	fat_node<> *	relink_parent	=	NULL;
	cur_node						=	need_replace ? *m_hash_set.find(hash) : NULL;
	while ( cur_node )
	{
		if ( cur_node->same_path(work_node) )
		{
			if ( !replacing_node )
			{
				replacing_node		=	cur_node;	// found replacing node
				if ( !freeing_node->is_folder() )
					break;							// no relinking if freeing file
			}
			if ( cur_node->is_folder() )
			{
				relink_parent		=	cur_node;	// found relink_parent
				break;
			}
		}

		cur_node					=	cur_node->get_next_hash_node();
	}

	fat_folder_node<> * recurse_folder	=	!work_folder && freeing_node && freeing_node->is_folder() ?
										freeing_node->cast_folder() : work_folder;
	
	if ( recurse_folder )
	{
		fat_node<> * child			=	recurse_folder->get_first_child();
		while ( child )
		{
			u32 child_hash			=	crc32(child->m_name, strings::length(child->m_name), hash);
			if ( relink_parent )
				relink_hidden_nodes_to_parent(child, child_hash, relink_parent);

			fat_node<> * next_child		=	child->get_next();
			unmount_node				(child, child_hash, unmount_pred);
			child					=	next_child;
		}
	}

	if ( need_replace )
	{
		parent_folder_of_work_node->unlink_child	(freeing_node);
	}

	if ( need_replace )
		ASSERT( replacing_node || !freeing_node->get_first_child() );

	if ( !need_replace && freeing_node && freeing_node->is_folder() )
		ASSERT ( !freeing_node->get_first_child() );

	if ( need_replace && replacing_node )
	{
		if ( freeing_node->is_folder() && replacing_node->is_folder() )
			freeing_node->cast_folder()->transfer_children (replacing_node->cast_folder(), false);

		parent_folder_of_work_node->prepend_child	(replacing_node);
	}

	// check if need to join 
	if ( !need_replace &&
		  work_folder &&
		  freeing_node &&
		 !freeing_node->is_folder() )
	{
		fat_node<> *	checking_node	=	work_node->get_next_hash_node();
		while ( checking_node )
		{
			if ( checking_node->same_path(work_node) )
			{
				if ( !checking_node->is_folder() )
					break;
				
				checking_node->cast_folder()->transfer_children(work_folder, true);
			}

			checking_node	=	checking_node->get_next_hash_node();
		}
	}

	if ( freeing_node && freeing_node->is_disk() )
		free_node	(freeing_node, false);
}

} // namespace fs 
} // namespace xray 
