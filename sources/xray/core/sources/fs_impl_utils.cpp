////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/resources_managed_resource.h>
#include <xray/resources_unmanaged_resource.h>
#include <xray/fs_utils.h>
#include "fs_impl.h"
#include "fs_impl_finding.h"
#include "fs_helper.h"
#include "game_resman.h"

namespace xray {
namespace fs   {

//-----------------------------------------------------------------------------------
// file_system_impl implementation
//-----------------------------------------------------------------------------------

file_system_impl::file_system_impl () : m_root(NULL), m_on_resource_leaked_callback(NULL), m_on_unmount_started_callback(NULL)
{	
	create_root();
}

void   file_system_impl::clear ()
{
	while ( !m_mount_history.empty() )
	{
		mount_history * const	history	=	m_mount_history.back();
		// we need to copy in paths because on_unmount operation will destroy history item
		path_string const logical_path	=	history->logical_path;
		path_string const physical_path	=	history->physical_path;

		if ( history->mount_type == mount_history::mount_type_disk )
			unmount_disk			(logical_path.c_str(), physical_path.c_str());
		else
			unmount_db				(logical_path.c_str(), physical_path.c_str());
	}

// 	while ( !m_root_dbs.empty() )
// 		unmount_root_db				(m_root_dbs.front().c_ptr());

	m_hash_set.clear				();
	free_node						(m_root->cast_node(), true);

	m_root						=	NULL;
	create_root						();
}

file_system_impl::~file_system_impl ()
{
 	clear				();
 	m_hash_set.clear	();
 	free_node			(m_root->cast_node(), true);

	history_deleter_predicate	history_deleter_predicate;
	m_mount_history.for_each	(history_deleter_predicate);
	m_mount_history.pop_all_and_clear	();
}

struct compare_with_node
{
	bool operator () (sub_fat_resource const & sub_fat, fat_node<> const * work_node) const
	{
		return					!sub_fat.contains(work_node) && (pvoid)& sub_fat < (pvoid)work_node;
	}
};

sub_fat_resource *   file_system_impl::get_db (fat_node<> const * work_node) 
{
	if ( !work_node )
		return					NULL;

	find_node_in_db				find_predicate(work_node);

	sub_fat_resource_tree_type::iterator	it	=	m_dbs_tree.lower_bound(work_node, compare_with_node());
	if ( it == m_dbs_tree.end() )
		return					NULL;
	sub_fat_resource * const	out_db	=	& * it;
	if ( !out_db->contains(work_node) )
		return					NULL;

	return						out_db;
}

sub_fat_resource *   file_system_impl::get_db (pcstr logical_path, pcstr disk_path)
{
	find_by_db_path	find_predicate		(disk_path, logical_path);
	sub_fat_resource_tree_type::iterator	it	=	std::find_if(m_dbs_tree.begin(), 
																 m_dbs_tree.end(), 
																 find_predicate);
	if ( it == m_dbs_tree.end() )
		return							NULL;
	return								& * it;
}

void   file_system_impl::get_disk_path (fat_node<> const * work_node, path_string & out_path, const bool replicate) const
{
	out_path.clear				();
	pcstr						file_path;

	fat_disk_file_node<> const * disk_file_node	=	NULL;
	fat_disk_folder_node<> const * disk_folder_node	=	NULL;
	if ( work_node->is_disk() )
	{
		if ( work_node->is_folder() )
		{
			disk_folder_node	=	work_node->cast_disk_folder();
			file_path			=	disk_folder_node->m_disk_path;
		}
		else
		{
			disk_file_node	=	work_node->cast_disk_file();
			file_path		=	disk_file_node->m_disk_path;
		}
	}
	else
	{
		sub_fat_resource const * db =	get_db(work_node);
		ASSERT					(db);
		file_path			=	db->archive_physical_path.c_str();
	}

	if ( work_node->is_replicated() && replicate )
		replicate_path			(file_path, out_path);
	else
		out_path			=	file_path;
}

bool   file_system_impl::operator == (file_system_impl const & f) const
{
	return						* m_root->cast_node() == * f.m_root->cast_node();
}

bool   file_system_impl::is_main_fat () const
{
	return						g_fat->impl() == this;
}

void   file_system_impl::unmount_disk_node (pcstr physical_path)
{
	path_string					logical_path;
	if ( !convert_physical_to_logical_path(& logical_path, physical_path, false) )
		return;

	unmount_disk				(logical_path.c_str(), physical_path, respect_mount_history_false);
}

void   deassociate_from_grm_and_fat_if_needed (fat_node<> * node)
{
	if ( node->is_link() || node->is_folder() )
		return;

	resources::managed_resource_ptr	const 	managed_resource	=	node->get_associated_managed_resource_ptr();
	resources::unmanaged_resource_ptr const unmanaged_resource	=	node->get_associated_unmanaged_resource_ptr();
	resources::resource_base * const		resource			=	managed_resource ? (resources::resource_base *)managed_resource.c_ptr() : unmanaged_resource.c_ptr();
	
	if ( !resource )
		return;

	if ( resource->is_pinned_by_grm() )
		resources::g_game_resources_manager->release_resource	(resource);

	node->set_associated					(NULL);
	LOGI_INFO								("fs", "deassociated resource from node: '%s'", node->get_name());
}

void   file_system_impl::mark_disk_node_as_erased (pcstr physical_path)
{
	path_string					logical_path;
	if ( !convert_physical_to_logical_path(& logical_path, physical_path, false) )
		return;

	u32 hash				=	0;
	fat_node<> * node_with_path	=	find_node_no_mount(logical_path.c_str(), & hash);
	if ( !node_with_path )
		return;

	fat_node<> * node_to_erase	=	NULL;
	fat_node<> * node_to_relink	=	NULL;
	fat_node<> * cur_node	=	* m_hash_set.find(hash);
	for ( ;cur_node; cur_node = cur_node->get_next_overlapped() )
	{
		if ( !cur_node->same_path(node_with_path) )
			continue;

		if ( is_erased_node(cur_node) )
			continue;

		path_string					cur_node_path;
		get_disk_path				(cur_node, cur_node_path);

		if ( node_to_erase )
		{
			if ( cur_node_path == physical_path )
				continue;

			node_to_relink		=	cur_node;
			break;
		}

		if ( !cur_node->is_disk() )
			continue;


		if ( cur_node_path != physical_path )
			continue;
		
		R_ASSERT					(!node_to_erase);
		node_to_erase			=	cur_node;
	}

	if ( !node_to_erase )
		return;
	
	fat_disk_node_base * disk_node_base	=	node_to_erase->cast_disk_node_base();
	ASSERT						(disk_node_base);
	disk_node_base->set_is_erased	();

	deassociate_from_grm_and_fat_if_needed	(node_to_erase);

	if ( node_to_relink )
	{
		fat_folder_node<> * parent		=	node_to_erase->get_parent()->cast_folder();
		if ( parent->find_child(node_to_erase) )
			parent->prepend_child			(node_to_relink);
	}

}

} // namespace fs 
} // namespace xray 
