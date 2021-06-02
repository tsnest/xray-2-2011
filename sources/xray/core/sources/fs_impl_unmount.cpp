////////////////////////////////////////////////////////////////////////////
//	Created		: 21.12.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <boost/type_traits/is_same.hpp>
#include <xray/fs_utils.h>
#include <xray/resources_unmanaged_allocator.h>
#include "game_resman.h"
#include "fs_impl.h"
#include "fs_helper.h"

namespace xray {
namespace fs   {

//-----------------------------------------------------------------------------------
// unmounting disk/db
//-----------------------------------------------------------------------------------

struct helper_folder_unmount_pred
{
	helper_folder_unmount_pred(file_system_impl * pimpl) : pimpl(pimpl) {}
	bool   operator () (fat_node<> * work_node) const
	{
		if ( work_node->is_db() || work_node->is_disk() )
			return			false;

		if ( !work_node->is_folder() )
			return			false;

		fat_folder_node<> * folder = work_node->cast_folder();
		return				!folder->get_first_child();
	}

	file_system_impl *	pimpl;
};

void   file_system_impl::unmount_disk (pcstr logical_path, pcstr disk_path, respect_mount_history_bool respect_mount_history)
{	
	if ( m_on_unmount_started_callback )
		m_on_unmount_started_callback	();

	if ( respect_mount_history )
		on_unmount						(mount_history::mount_type_disk, logical_path, disk_path);

	threading::mutex_raii		raii	(m_mount_mutex);

	verify_path_is_portable				(logical_path);
	verify_path_is_portable				(disk_path);
	u32		hash;	
	fat_node<> *	work_node		=	find_node_no_mount(logical_path, & hash);
	if ( !work_node )
		return;

	LOGI_INFO							("fs", "unmount disk started '%s' from '%s'", disk_path, logical_path);

	unmount_node						(work_node, hash, disk_unmount_pred(disk_path));

	LOGI_INFO							("fs", "unmount disk ended '%s' from '%s'", disk_path, logical_path);
}

void   file_system_impl::unmount_sub_dbs (sub_fat_resource * sub_fat, bool store_in_mount_history)
{
	while ( !sub_fat->children.empty() )
	{
		sub_fat_resource * child_sub_fat	=	sub_fat->children.front();
		unmount_db						(child_sub_fat->virtual_path.c_str(), child_sub_fat->archive_physical_path.c_str(), store_in_mount_history);
		// make sure, unmount_db removed this child from sub_fat and children list become shorter
		R_ASSERT_CMP					(child_sub_fat, !=, sub_fat->children.front()); 
	}
}

void   file_system_impl::unmount_root_db (sub_fat_resource * root_db)
{
	R_ASSERT						(root_db->is_no_delete());
	unmount_db						(root_db->virtual_path.c_str(), 
									 root_db->archive_physical_path.c_str(), 
									 !root_db->is_automatically_mounted);
}

void   file_system_impl::unmount_db (pcstr logical_path, pcstr disk_path, bool store_in_mount_history)
{
	if ( m_on_unmount_started_callback )
		m_on_unmount_started_callback	();

	threading::mutex_raii	raii	(m_mount_mutex);

	find_by_db_path	find_predicate	(disk_path, logical_path);
	bool is_root_db				=	false;
	if ( sub_fat_resource_ptr root_db = m_root_dbs.find_if(find_predicate) )
	{
		root_db->schedule_unmount_on_dying	=	false;
		m_root_dbs.erase			(root_db);
		is_root_db				=	true;
	}

	if ( store_in_mount_history )
		on_unmount					(mount_history::mount_type_db, logical_path, disk_path);

	verify_path_is_portable			(disk_path);
	verify_path_is_portable			(logical_path);

	sub_fat_resource * sub_fat	=	get_db(logical_path, disk_path);
	sub_fat->schedule_unmount_on_dying	=	false;
	
// 	if ( !is_root_db )
// 		resources::g_game_resources_manager->release_resource	(sub_fat);

	u32				hash;
	fat_node<> *	work_node	=	find_node_no_mount(logical_path, & hash);
	if ( !work_node )
		return;

	pcstr const fat_string		=	is_root_db ? "fat" : "sub-fat";
	LOGI_INFO						("fs", "unmount %s started: '%s' from '%s'", fat_string, disk_path, logical_path);

	unmount_sub_dbs					(sub_fat, store_in_mount_history);

	unmount_node					(work_node, hash, db_unmount_pred(this, sub_fat->root_node));

	m_dbs_tree.erase				(* sub_fat);
	//LOGI_INFO						("fstemp", "m_dbs_tree.erase %s %s", sub_fat->logical_path, sub_fat->root_node->get_name());

	R_ASSERT						(sub_fat);
	sub_fat->remove_from_parent		();

	if ( !is_root_db )
	{
		R_ASSERT					(sub_fat->sub_fat_node);
		deassociate_last_link_if_needed	(sub_fat->sub_fat_node);

		if ( sub_fat->is_pinned_by_grm() )
			resources::g_game_resources_manager->release_resource	(sub_fat);
	}

	LOGI_INFO						("fs", "unmount %s ended '%s' from '%s'", fat_string, disk_path, logical_path);

	R_ASSERT						(!sub_fat->is_captured());

	if ( sub_fat->reference_count() == 1 )
		R_ASSERT					(sub_fat->is_pinned_for_safe_destroying() || sub_fat->is_automatically_mounted);
	else 
		R_ASSERT_CMP				(sub_fat->reference_count(), ==, 0);

 	sub_fat->~sub_fat_resource		();
 	UNMANAGED_FREE					(sub_fat);
}

void   file_system_impl::deassociate_last_link_if_needed (fat_node<> * work_node)
{
	resources::unmanaged_resource_ptr	unmanaged_resource;
	resources::managed_resource_ptr		managed_resource;
	bool is_associated			=	(work_node->is_link() || work_node->is_folder()) ? 
									NULL : work_node->is_associated();
	resources::resource_base *	base_resource	=	NULL;

	//LOGI_INFO	("fstemp", "with node: %s associated: %d", work_node->get_name(), is_associated);
	if ( is_associated )
	{
		if ( unmanaged_resource	=	work_node->get_associated_unmanaged_resource_ptr() )
		{
			base_resource			=	unmanaged_resource.c_ptr();
			if ( unmanaged_resource->is_inlined_in_fat() )
			{
				unmanaged_resource->unset_as_inlined_in_fat	();
				work_node->set_associated					(NULL);
				is_associated							=	false;
			}
			else if ( unmanaged_resource->is_captured() )
			{
				resources::resource_base * const resource_base	=	unmanaged_resource.c_ptr();
				unmanaged_resource				=	NULL;
				LOGI_INFO							("fs", "informing grm of resource to release: %s", resources::log_string(resource_base).c_str());
				m_on_resource_leaked_callback		(resource_base);
				is_associated					=	work_node->is_associated();
			}
		}
		else if (  managed_resource	=	work_node->get_associated_managed_resource_ptr() )
		{
			base_resource			=	managed_resource.c_ptr();
			if ( managed_resource->is_captured() )
			{
				resources::resource_base * const resource_base	=	managed_resource.c_ptr();
				managed_resource				=	NULL;
				LOGI_INFO							("fs", "informing grm of resource to release: %s", resources::log_string(resource_base).c_str());
				m_on_resource_leaked_callback		(resource_base);
				is_associated					=	work_node->is_associated();
			}
		}
	}

	if ( !work_node->is_folder() && is_associated )
		LOGI_ERROR									("fs", "resource is being held by user (leak?): %s", 
													 base_resource->log_string().c_str());

// 	R_ASSERT										(work_node->is_folder() ||
// 													 !is_associated,
// 													 "omg! trying to free node, while it's used by "
// 													 "resources manager!");
}

template <class UnmountPredicate>
void   file_system_impl::unmount_node (fat_node<> *				work_node, 
									   u32 const				hash,
									   UnmountPredicate const &	unmount_pred)
{
	deassociate_last_link_if_needed		(work_node);
	
	fat_node<> *	 parent_of_work_node		=	work_node->m_parent;
	fat_folder_node<> * parent_folder_of_work_node	=	parent_of_work_node ? 
													parent_of_work_node->cast_folder() : NULL;
	 
	fat_folder_node<> * work_folder				=	work_node->cast_folder_if_not_link();

	bool			 need_replace				=	false;
	fat_node<> *	 freeing_node				=	NULL;

	fat_node<> *	 cur_node					=	* m_hash_set.find(hash);

	ASSERT											(cur_node);

	for ( ; cur_node; cur_node = cur_node->get_next_overlapped() )
	{
		if ( cur_node != work_node )
		{
			if ( !cur_node->same_path(work_node) )
				continue;
	 		if ( is_erased_node(cur_node) )
 				continue;
		}

		if ( !unmount_pred(cur_node) )
			continue;

		freeing_node				=	cur_node;
 		need_replace				=	(freeing_node == work_node);
		
		m_hash_set.erase				(hash, freeing_node);

		break;
	}

	fat_node<> *	replacing_node	=	NULL;
	fat_node<> *	relink_parent	=	NULL;
	cur_node						=	need_replace ? *m_hash_set.find(hash) : NULL;

	for ( ;cur_node; cur_node = cur_node->get_next_overlapped() )
	{
		if ( is_erased_node(cur_node) )
			continue;

		if ( cur_node == work_node )
			continue;

		if ( !cur_node->same_path(work_node) )
			continue;

		if ( !replacing_node )
		{
			replacing_node			=	cur_node;	// found replacing node
			if ( !freeing_node->is_folder() )
				break;							// no relinking if freeing file
		}
		if ( cur_node->is_folder() )
		{
			relink_parent			=	cur_node;	// found relink_parent
			break;
		}
	}

	fat_folder_node<> * recurse_folder	=	!work_folder && freeing_node && freeing_node->is_folder() ?
											freeing_node->cast_folder_if_not_link() : work_folder;
	
	if ( recurse_folder )
	{
		fat_node<> * child			=	recurse_folder->get_first_child(false);
		while ( child )
		{
			u32 child_hash			=	crc32(child->m_name, strings::length(child->m_name), hash);
			if ( relink_parent )
				relink_hidden_nodes_to_parent(child, child_hash, relink_parent);

			fat_node<> * next_child	=	child->get_next(false);
			unmount_node				(child, child_hash, unmount_pred);
			child					=	next_child;
		}
	}

	if ( need_replace )
		parent_folder_of_work_node->unlink_child	(freeing_node, true);

// 	if ( need_replace )
// 		ASSERT( replacing_node || freeing_node->is_link() || !freeing_node->get_first_child() );
	//if ( !need_replace && freeing_node && freeing_node->is_folder() )
	//	ASSERT ( freeing_node->is_link() || !freeing_node->get_first_child() );

	if ( need_replace && replacing_node )
	{
		if ( !freeing_node->is_link() && !replacing_node->is_link() && 
			 freeing_node->is_folder() && replacing_node->is_folder() )
		{
			freeing_node->cast_folder()->transfer_children (replacing_node->cast_folder(), false);
		}

		if ( !parent_folder_of_work_node->find_child(replacing_node) )
			parent_folder_of_work_node->prepend_child	(replacing_node);
	}

	// check if need to join 
	if ( !need_replace &&
		  work_folder &&
		  freeing_node &&
		 !freeing_node->is_folder() )
	{
		fat_node<> *	checking_node	=	work_node->get_next_overlapped();
		while ( checking_node )
		{
			if ( checking_node->same_path(work_node) )
			{
				if ( !checking_node->is_folder() )
					break;
				
				checking_node->cast_folder()->transfer_children(work_folder, true);
			}

			checking_node	=	checking_node->get_next_overlapped();
		}
	}

//	if ( freeing_node )
//  	LOGI_INFO		("fstemp", "freeing node: %s (0x%x, flags: %d)", freeing_node->get_name(), freeing_node, freeing_node->get_flags());

	if ( freeing_node && (freeing_node->is_disk() || !freeing_node->is_db()) )
	{
		if ( fat_disk_file_node<> * disk_file = freeing_node->cast_disk_file() )
			if ( disk_file->checked_is_db() && disk_file->is_db_file() )
				unmount_automatic_db			(freeing_node, hash, disk_file);

		if ( !freeing_node->is_disk() && !freeing_node->is_db() )
			ASSERT								(((bool)boost::is_same<UnmountPredicate, helper_folder_unmount_pred>::value));

		free_node								(freeing_node, false);
	}
}

void   file_system_impl::unmount_automatic_db (fat_node<> *	freeing_node, u32 hash, fat_disk_file_node<> * disk_file)
{
	R_ASSERT							(disk_file->checked_is_db() && disk_file->is_db_file());
	LOGI_INFO							("fs", "automatic unmount of db '%s'", disk_file->disk_path());

	path_string							logical_path;
	freeing_node->get_full_path			(logical_path);

	find_by_db_path	find_predicate		(disk_file->disk_path(), logical_path.c_str());
	sub_fat_resource * db			=	& * m_root_dbs.find_if(find_predicate);
	ASSERT								(db->is_automatically_mounted);
	ASSERT_U							(db);
	unmount_db							(logical_path.c_str(), disk_file->disk_path(), false);

	fat_node<> * helper				=	find_node_no_mount(logical_path.c_str(), NULL);
	helper_folder_unmount_pred			helper_predicate(this);
//	ASSERT								(helper_predicate(helper));
	unmount_node						(helper, hash, helper_predicate);
}

} // namespace fs 
} // namespace xray 
