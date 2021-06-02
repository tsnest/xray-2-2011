////////////////////////////////////////////////////////////////////////////
//	Created		: 21.12.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_impl.h"
#include "fs_watcher_internal.h"
#include "fs_impl_finding.h"
#include <xray/fs_utils.h>
#include <xray/resources_fs.h>
#include "fs_helper.h"

namespace xray {

namespace resources {

void   query_update_disk_node_name 		(fs::file_change_info const &	file_change_info, 
										 query_mount_callback const 	callback, 
										 memory::base_allocator * const allocator);
void   query_mount_disk_node			(fs::file_change_info const &	file_change_info, 
										 query_mount_callback const		callback, 
										 memory::base_allocator * const allocator);
void   query_unmount_disk_node			(fs::file_change_info const &	file_change_info, 
										 query_mount_callback const		callback, 
										 memory::base_allocator * const allocator);
void   query_update_file_size_in_fat	(fs::file_change_info const &	file_change_info, 
										 query_mount_callback const		callback, 
										 memory::base_allocator * const allocator);

} // namespace resources

namespace fs   {

//-----------------------------------------------------------------------------------
// file_system watching
//-----------------------------------------------------------------------------------

bool				is_watcher_initialized	();
threading::mutex &  mutex_folder_watcher	();

void   file_system_impl::action_for_folder_watchers (folder_watchers_action_enum const folder_watchers_action)
{
	if ( !is_watcher_initialized() )
		return;

	if ( !is_main_fat() )
		return;

	threading::mutex_raii	raii		(mutex_folder_watcher ());

	action_predicate					action_predicate(folder_watchers_action);
	m_mount_history.for_each			(action_predicate);
}

void   file_system_impl::on_mount (mount_history::mount_type_enum type, pcstr logical_path, pcstr disk_path, watch_directory_bool const watch_directory)
{
	// update hot load fs watchers
	if ( type == mount_history::mount_type_disk )
		action_for_folder_watchers	(folder_watchers_action_remove);
	
	pstr lower_case_disk_path		= 0;
	STR_DUPLICATEA					( lower_case_disk_path, disk_path );
	strlwr							( lower_case_disk_path, (strings::length( lower_case_disk_path ) + 1)*sizeof(char) );

	mount_history * const new_history_element	=	FS_NEW(mount_history)(type, logical_path, lower_case_disk_path, watch_directory);
	m_mount_history.push_back		(new_history_element);

	if ( type == mount_history::mount_type_disk )
		action_for_folder_watchers	(folder_watchers_action_add);
}

void   file_system_impl::on_unmount (mount_history::mount_type_enum type, pcstr logical_path, pcstr disk_path)
{
	// update hot load fs watchers
	if ( type == mount_history::mount_type_disk )
		action_for_folder_watchers	(folder_watchers_action_remove);

	mount_history * last_history_element	=	m_mount_history.back(); 
	R_ASSERT_U						(last_history_element->logical_path == logical_path);
	R_ASSERT_U						(last_history_element->physical_path == disk_path);
	R_ASSERT						(last_history_element->mount_type == type);
	m_mount_history.pop_back		();
	FS_DELETE						(last_history_element);

	if ( type == mount_history::mount_type_disk )
		action_for_folder_watchers	(folder_watchers_action_add);
}

bool   file_system_impl::convert_physical_to_logical_path (buffer_string *	out_logical_path, 
														   pcstr			in_physical_path, 
														   bool				assert_on_fail)
{
	verify_path_is_portable					(in_physical_path);
	path_string	file_physical_path	=	convert_to_relative_path(in_physical_path);

	threading::mutex_raii		raii		(m_mount_mutex);

	find_disk_mount_by_physical_path_predicate	predicate(file_physical_path);
	m_mount_history.for_each				(predicate);
	mount_history *	const mount_history	=	predicate.best_history_item;
	if ( !assert_on_fail && !mount_history )
		return								false;

	CURE_ASSERT								(mount_history, return false);

	* out_logical_path					=	mount_history->logical_path;

	if ( file_physical_path.length() > mount_history->physical_path.length() )
	{
		* out_logical_path					+=	'/';
		out_logical_path->append				(file_physical_path.c_str() + mount_history->physical_path.length() + 1,
									 			 file_physical_path.c_str() + file_physical_path.length());
	}

	return									true;
}

void   deassociate_from_grm_and_fat_if_needed (fat_node<> * node);

bool   file_system_impl::update_file_size_impl (pcstr physical_path, fat_node<> * node, bool do_resource_deassociation)
{
	fat_disk_file_node<> * disk_file_node	= node->cast_disk_file();
	R_ASSERT								(disk_file_node);

	file_size_type	new_file_size;
	if ( !calculate_file_size(& new_file_size, physical_path) )
	{
		LOGI_INFO							("fs", "update_file_size_in_fat '%s' cannot calculate file size", physical_path);
		return								false;
	}

	file_size_type const old_file_size	=	threading::interlocked_exchange(disk_file_node->m_size, (u32)new_file_size);

	if ( old_file_size != new_file_size || old_file_size )
	{
		// check if the notification is caused by out own activity
		if ( do_resource_deassociation )
			deassociate_from_grm_and_fat_if_needed	(node);

		LOGI_INFO							("fs", "update_file_size_in_fat '%s' updated file size (%d->%d)", physical_path, u32(old_file_size), u32(new_file_size));
	}

	return									true;
}
bool   file_system_impl::update_file_size_in_fat (pcstr in_logical_path, 
												  pcstr in_physical_path,
												  bool	do_resource_deassociation)
{
	verify_path_is_portable					(in_logical_path);
	threading::mutex_raii	raii			(m_mount_mutex);
	
	u32 node_on_path_hash				=	0;
	fat_node<> * const node_on_path		=	find_node_no_mount(in_logical_path, & node_on_path_hash);
	if ( !node_on_path )
		return								false;
	
	// iterate nodes that have same hash as node on logical_path 
	for ( fat_node<> *	it_node			=	* m_hash_set.find(node_on_path_hash);
						it_node; 
						it_node			=	it_node->get_next_overlapped() )
	{
		if ( it_node != node_on_path && !it_node->same_path(node_on_path) )
			continue;

		fat_disk_file_node<> * disk_file_node	= it_node->cast_disk_file();
		if ( !disk_file_node )
			continue;

		if ( disk_file_node->is_erased() )
			continue;

		if ( !strings::equal(disk_file_node->disk_path(), in_physical_path) )
			continue;

		if ( get_path_info(NULL, in_physical_path) != path_info::type_file )
			continue;

		if ( !update_file_size_impl(in_physical_path, it_node, do_resource_deassociation) )
			return							false;

		break;
	}

	return									true;
}

bool   file_system_impl::mount_disk_node (pcstr const						in_logical_path, 
										  pcstr const						in_physical_path, 
										  resources::fs_iterator * const	out_result_it,
										  bool const						assert_on_fail)
{
	threading::mutex_raii	raii			(m_mount_mutex);
	verify_path_is_portable					(in_logical_path);

	if ( out_result_it )
		* out_result_it					=	resources::fs_iterator::end();

	bool already_mounted				=	false;

	u32	hash =	0;
	fat_node<> * const node				=	find_node_no_mount(in_logical_path, & hash);
	if ( node && !node->is_link() && node->cast_disk_file() )
	{
		if ( !in_physical_path || strings::equal(node->cast_disk_file()->disk_path(), in_physical_path) )
		{
			if ( out_result_it )
				* out_result_it			=	resources::fs_iterator(node,  find_referenced_link_node(node));
			already_mounted				=	true;
		}
	}

	path_string			physical_path	=	in_physical_path;
	mount_history *							history_item;

	if ( !physical_path.length() )
	{
		if ( !get_disk_path_to_store_file(in_logical_path, & physical_path, & history_item, assert_on_fail) )
		{
			if ( !assert_on_fail )
				return						false;

			R_ASSERT						(!already_mounted);
			LOGI_INFO						("fs", "mount_disk_node '%s' no disk mounts with this path", physical_path.c_str());
			return							false;
		}
	}

	path_info::type_enum const path_type		=	get_path_info(NULL, physical_path.c_str());

	file_size_type file_size = 0;
	if ( path_type == path_info::type_file )
	{
		if ( !calculate_file_size(& file_size, physical_path.c_str()) )
			LOGI_INFO						("fs", "mount_disk_node '%s' cannot calculate file size", physical_path.c_str());
	}

	if ( path_type == path_info::type_nothing ) 
		return								false;
	
	if ( already_mounted )
	{
		fat_disk_file_node<> * const disk_file_node	=	node->cast_disk_file();

		file_size_type const old_file_size	=	threading::interlocked_exchange	(disk_file_node->m_size, (u32)file_size);
		if ( old_file_size != file_size )
			LOGI_INFO						("fs", "mount_disk_node '%s' already mounted, only updating size to (%d)", in_logical_path, file_size);
		return								true;
	}

	path_string	logical_path			=	in_logical_path;

	path_string folder_path;
	path_string file_name;
	u32 const slash_pos					=	logical_path.rfind('/');
	if ( slash_pos != logical_path.npos )
	{
		logical_path.substr					(slash_pos + 1, logical_path.npos, & file_name);
		logical_path.substr					(0, slash_pos, & folder_path);
	}
	else
		file_name						=	logical_path;

	u32				  folder_hash;
	fat_folder_node<> * folder_node		=	find_or_create_folder(folder_path.c_str(), & folder_hash);

	u32 const		  file_hash			=	crc32(file_name.c_str(), file_name.length(), folder_hash);


	fat_node<> * const new_node				=	(path_type == path_info::type_folder) ? 
											create_disk_folder_node( file_hash,
																	 file_name.c_str(),
																 	 file_name.length(),
																	 physical_path.c_str(),
																	 physical_path.length() )->cast_node() :
											create_disk_file_node(	 file_hash, 
																	 file_name.c_str(), 
																	 file_name.length(), 
																	 physical_path.c_str(),
																	 physical_path.length(),
																	 (u32)file_size )->cast_node();


	actualize_node							(new_node, file_hash, folder_node);
	if ( out_result_it )
		* out_result_it					=	resources::fs_iterator(new_node, find_referenced_link_node(new_node));

	LOGI_INFO								("fs", "mount_disk_node '%s' with size (%d)", 
													physical_path.c_str(), file_size);
	return									true;
}

void   file_system_impl::rename_disk_node (pcstr const in_logical_path, bool do_resource_deassociation, pcstr const in_renamed_old_path, pcstr const in_renamed_new_path)
{
	threading::mutex_raii	raii			(m_mount_mutex);
	rename_disk_node						(in_logical_path, do_resource_deassociation, in_renamed_old_path, in_renamed_new_path, 0);
}

void   file_system_impl::rename_disk_node (pcstr const in_logical_path, bool do_resource_deassociation, pcstr const in_renamed_old_path, pcstr const in_renamed_new_path, u32 const in_full_name_of_parent_hash)
{
	u32 node_on_path_hash				=	0;
	fat_node<> * const node_on_path		=	find_node_no_mount(in_logical_path, & node_on_path_hash);
	if ( !node_on_path )
		return;

	fat_node<> *	 it_node			=	* m_hash_set.find(node_on_path_hash);
	ASSERT									(it_node);

	for ( ; it_node; it_node = it_node->get_next_overlapped() )
	{
		path_string	logical_path_of_it_node;
		it_node->get_full_path				(logical_path_of_it_node);
		if ( logical_path_of_it_node.find(in_logical_path) != 0 )
			continue;

		if ( !it_node->is_disk() )
			continue;

		if ( is_erased_node(it_node) )
			continue;

		pstr const disk_path_of_it_node		=	 it_node->is_folder() ? 
												 it_node->cast_disk_folder()->m_disk_path : 
												 it_node->cast_disk_file()->m_disk_path;

		buffer_string disk_path_of_it_node_string	(disk_path_of_it_node,
													 path_string::fixed_size,
													 strings::length(disk_path_of_it_node));

		if ( disk_path_of_it_node_string.find(in_renamed_old_path) != 0 )
			continue;

		u32 full_new_name_hash			=	0;

		pcstr new_name					=	NULL;

		bool const exactly_this_node_was_renamed = strings::equal(disk_path_of_it_node, in_renamed_old_path);

		if ( exactly_this_node_was_renamed )
		{
			ASSERT							(!in_full_name_of_parent_hash);
			new_name					=	file_name_from_path(in_renamed_new_path);
			fs::path_string new_full_path;
			it_node->get_full_path			(new_full_path);
			fs::path_string directory_part;
			directory_part_from_path		(& directory_part, new_full_path.c_str());
			new_full_path				=	directory_part;
			new_full_path				+=	'/';
			new_full_path				+=	new_name;
			full_new_name_hash			=	path_crc32(new_full_path.c_str(), new_full_path.length());
		}
		else
		{
			full_new_name_hash			=	path_crc32(it_node->m_name, strings::length(it_node->m_name), in_full_name_of_parent_hash);
		}

		if ( it_node->is_folder() )
		{
			path_string child_logical_path	=	in_logical_path;
			child_logical_path			+=	'/';
			u32 const child_logical_path_saved_size	=	child_logical_path.length();

			fat_node<> * it_child		=	it_node->get_first_child();
			while ( it_child )
			{
				child_logical_path		+=	it_child->m_name;

				fat_node<>* next_child	=	it_child->get_next();
				rename_disk_node			(child_logical_path.c_str(), do_resource_deassociation, in_renamed_old_path, in_renamed_new_path, full_new_name_hash);
				it_child				=	next_child;

				child_logical_path.set_length	(child_logical_path_saved_size);
			}
		}

		path_string new_disk_path		=	in_renamed_new_path;
		new_disk_path					+=	disk_path_of_it_node + strings::length(in_renamed_old_path);

		if ( exactly_this_node_was_renamed )
			LOGI_INFO						("fs", "renamed '%s' to '%s'", disk_path_of_it_node, new_disk_path.c_str());

		strings::copy						(disk_path_of_it_node, path_string::fixed_size, new_disk_path.c_str());

		if ( exactly_this_node_was_renamed )
		{
			ASSERT							(new_name);
			strings::copy					(it_node->m_name, path_string::fixed_size, new_name);
		}

		if ( !it_node->is_folder() )
			update_file_size_impl			(new_disk_path.c_str(), it_node, do_resource_deassociation);
			
		m_hash_set.erase					(node_on_path_hash, it_node);
		m_hash_set.insert					(full_new_name_hash, it_node);
	}
}

void   file_system_impl::on_disk_file_change (file_change_info const & info)
{
	fixed_string4096	notification_string;
	if ( info.type == file_change_info::change_type_added )
		notification_string.assignf					("add '%s'", info.physical_file_path.c_str());
	else if ( info.type == file_change_info::change_type_removed )
		notification_string.assignf					("removed '%s'", info.physical_file_path.c_str());
	else if ( info.type == file_change_info::change_type_modified )
		notification_string.assignf					("modified '%s'", info.physical_file_path.c_str());
	else if ( info.type == file_change_info::change_type_renamed )
		notification_string.assignf					("renamed '%s' to '%s'", info.physical_renamed_old_file_path.c_str(), info.physical_renamed_new_file_path.c_str());

	LOGI_INFO										("fs:watcher", "received notification: %s [caused by host program: %d]", notification_string.c_str(), !info.do_resource_deassociation);

	if ( info.type == file_change_info::change_type_added )
		resources::query_mount_disk_node			(info, NULL, & memory::g_mt_allocator);
	else if ( info.type == file_change_info::change_type_removed )
		resources::query_unmount_disk_node			(info, NULL, & memory::g_mt_allocator);
	else if ( info.type == file_change_info::change_type_modified )
		resources::query_update_file_size_in_fat	(info, NULL, & memory::g_mt_allocator);
	else if ( info.type == file_change_info::change_type_renamed )
		resources::query_update_disk_node_name		(info, NULL, & memory::g_mt_allocator);
	else
		UNREACHABLE_CODE();
}

} // namespace fs 
} // namespace xray 
