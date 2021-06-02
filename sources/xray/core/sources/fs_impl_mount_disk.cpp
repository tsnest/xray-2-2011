////////////////////////////////////////////////////////////////////////////
//	Created		: 21.12.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_impl.h"
#include "fs_helper.h"
#include <xray/fs_utils.h>
#include <xray/fs_path_iterator.h>

namespace xray {
namespace fs   {

//-----------------------------------------------------------------------------------
// mounting disk
//-----------------------------------------------------------------------------------

void   file_system_impl::mount_disk (pcstr const logical_path, 
									 pcstr const in_physical_path, 
									 watch_directory_bool const watch_directory,
									 recursive_bool const recursive)
{
	path_string	const physical_path_storage	=	convert_to_relative_path(in_physical_path);
	pcstr const physical_path			=	physical_path_storage.c_str();

	m_mount_mutex.lock						();

	verify_path_is_portable					(logical_path);
	verify_path_is_portable					(physical_path);

	u32					hash;
	fat_folder_node<> *	folder			=	find_or_create_folder(logical_path, & hash);

	bool const mount_file				=	get_path_info(NULL, physical_path) == path_info::type_file;
	if ( mount_file )
	{
		mount_disk_file						(folder, physical_path, hash);
		LOGI_INFO							("fs", "mounted file '%s' on '%s'", physical_path, logical_path);
	}
	else
	{
		mount_disk_folder					(folder, physical_path, hash, recursive);
		LOGI_INFO							("fs", "mounted dir '%s' on '%s'", physical_path, logical_path);
	}

	m_mount_mutex.unlock					();

	if ( !mount_file )
		on_mount							(mount_history::mount_type_disk, logical_path, physical_path, watch_directory);

}

fat_node<> *   file_system_impl::continue_mount_disk_db (fat_disk_file_node<> * disk_file, pcstr logical_path)
{
	LOGI_INFO							("fs", "automatic mount of db '%s'", disk_file->disk_path());
	bool const success				=	mount_db(logical_path, disk_file->disk_path(), disk_file->disk_path(), false, false, true);
	
	fat_node<> * const db_root		=	find_node_no_mount(logical_path, 0);
	R_ASSERT							(db_root != disk_file->cast_node());

	R_ASSERT_U							(success);
	return								db_root;
}

fat_node<> *   file_system_impl::continue_mount_disk (fat_node<> * const opt_node, pcstr const opt_logical_path, recursive_bool const recursive)
{
	threading::mutex_raii		raii		(m_mount_mutex);
	path_string	logical_path			=	opt_logical_path;	
	if ( opt_node )
	{
		R_ASSERT							(!opt_logical_path, "passing the node is enough, Winston.");
		opt_node->get_full_path				(logical_path);
	}
	
	verify_path_is_portable					(logical_path.c_str());
	u32		hash;
	fat_node<> * const node				=	find_node_no_mount(logical_path.c_str(), & hash);

	if ( fat_disk_file_node<> * disk_file = node->cast_disk_file() )
	{
		fat_node<> * out_node			=	continue_mount_disk_db(disk_file, logical_path.c_str());
		mount_overlapped					(out_node->cast_folder(), NULL, hash, recursive);
		return								out_node;
	}

	fat_disk_folder_node<> * disk_folder	=	node->cast_disk_folder();
	if ( !disk_folder )
	{
		R_ASSERT							(node->cast_folder());
		mount_overlapped					(node->cast_folder(), NULL, hash, recursive);
		return								node;
	}

	if ( disk_folder->is_scanned(recursive) )
		return								node; // already mounted

	LOGI_INFO								("fs", "lazy mount of disk folder started '%s' (recursive:%d)", disk_folder->disk_path(), recursive);

	if ( disk_folder->is_scanned(false) )
		continue_mount_disk_folder_recursively	(disk_folder, hash);
	else
		mount_disk_folder					(node->cast_folder(), disk_folder->disk_path(), hash, recursive);

	LOGI_INFO								("fs", "lazy mount of disk folder ended '%s' (recursive:%d)", disk_folder->disk_path(), recursive);

	return									node;
}

void   file_system_impl::continue_mount_disk_folder_recursively (fat_disk_folder_node<> * const	work_folder, 
																 u32 const						hash)
{
	R_ASSERT								(work_folder->is_scanned(false));
	R_ASSERT								(!work_folder->is_scanned(true));

	path_string  child_physical_path	=	work_folder->disk_path();
	child_physical_path					+=	'/';
	u32 const	 saved_length			=	child_physical_path.length();
	fat_node<> * it_child				=	work_folder->cast_node()->get_first_child();
	while ( it_child )
	{
		if ( fat_disk_folder_node<> * const child_folder_node = it_child->cast_disk_folder() )
		{
			u32 const child_hash		=	crc32(it_child->get_name(), strings::length(it_child->get_name()), hash);
			child_physical_path			+=	it_child->get_name();
			mount_disk_folder				(it_child->cast_folder(), child_physical_path.c_str(), child_hash, file_system::recursive_true);
			child_physical_path.set_length	(saved_length);
		}

		it_child						=	it_child->get_next();
	}

	work_folder->set_is_scanned				(true);
}

u32   count_of_path_parts_that_physically_exist (pcstr dir_path)
{
	pcstr colon_pos				=	strchr(dir_path, ':');
	bool const absolute_path	=	!!colon_pos;
	pcstr	disk_letter_end		=	absolute_path ? colon_pos + 1 : dir_path;
	path_string		cur_path;
	cur_path.append					(dir_path, disk_letter_end);
	if ( absolute_path )
		cur_path				+=	'/';
	u32 const drive_part_length	=	absolute_path ? (u32(disk_letter_end - dir_path) + 1) : 0; // with '/'

	path_iterator	it			(dir_path + drive_part_length, // skip '/' if absolute path
									int(strings::length(dir_path) - drive_part_length));
	path_iterator	end_it	=	path_iterator::end();

	u32 out_result_count		=	0;
	while ( it != end_it )
	{
		path_string					part;
		it.to_string				(part);
		cur_path				+=	part;

		if ( get_path_info(NULL, cur_path.c_str()) == path_info::type_folder )
			++out_result_count;
		else
			return					out_result_count;

		cur_path				+=	"/";

		++it;
	}

	return							out_result_count;
}

struct find_best_physical_path_by_logical_path_predicate : private boost::noncopyable 
{
	find_best_physical_path_by_logical_path_predicate	(buffer_string & file_logical_path,
														 mount_filter const &	filter) 
		:	best_history_item(NULL), best_folders_count(0), 
			file_logical_path(&file_logical_path), filter(filter) {;}

	void operator () (file_system_impl::mount_history * const history)
	{
		path_string history_item_logical_path_with_slash	=	history->logical_path;
		if ( history_item_logical_path_with_slash.length() )
			history_item_logical_path_with_slash				+=	'/';

		if ( history->mount_type == file_system_impl::mount_history::mount_type_disk && 
			(file_logical_path->find(history_item_logical_path_with_slash.c_str()) == 0 ||
			history->logical_path == * file_logical_path) )
		{
			if ( filter && !filter(history->physical_path.c_str(), history->logical_path.c_str()) )
				return;

			path_string	physical_path	=	history->physical_path;
			physical_path				+=	'/';
			physical_path.append			(file_logical_path->c_str() + history->logical_path.length() + 1,
											 file_logical_path->c_str() + file_logical_path->length());

			u32 const new_count			=	count_of_path_parts_that_physically_exist(physical_path.c_str());

			if ( !best_history_item || best_folders_count < new_count )
			{
				best_folders_count		=	new_count;
				best_history_item		=	history;
				best_physical_path		=	physical_path;
			}
		}
	}

	mount_filter const &				filter;
	buffer_string *						file_logical_path;
	u32									best_folders_count;
	file_system_impl::mount_history *	best_history_item;
	path_string							best_physical_path;
}; // find_best_physical_path_by_logical_path_predicate

bool   file_system_impl::get_disk_path_to_store_file (pcstr					logical_path, 
													  buffer_string *		out_physical_path, 
													  mount_history * *		out_opt_mount_history,
													  bool					assert_on_fail,
													  mount_filter const &	filter)
{
	verify_path_is_portable					(logical_path);
	threading::mutex_raii		raii		(m_mount_mutex);
	path_string	file_logical_path		=	logical_path;

	find_best_physical_path_by_logical_path_predicate	predicate(file_logical_path, filter);
	m_mount_history.for_each				(predicate);

	if ( !predicate.best_history_item && !assert_on_fail )
		return								false;

	CURE_ASSERT								(predicate.best_history_item, return false);

	* out_physical_path					=	predicate.best_physical_path;
	if ( out_opt_mount_history )
		* out_opt_mount_history			=	predicate.best_history_item;

	return									true;
}

path_info::type_enum   file_system_impl::get_physical_path_info_by_logical_path (path_info * out_path_info, pcstr logical_path)
{
	verify_path_is_portable					(logical_path);
	threading::mutex_raii		raii		(m_mount_mutex);

	bool found							=	false;
	path_info	archive_info;
	out_path_info->type					=	path_info::type_nothing;
	for ( mount_history *	it_mount	=	m_mount_history.front();
							it_mount;
							it_mount	=	m_mount_history.get_next_of_object(it_mount) )
	{
		path_string history_with_slash	=	it_mount->logical_path;
		if ( history_with_slash.length() )
			history_with_slash			+=	'/';

		if ( strstr(logical_path, history_with_slash.c_str()) == 0 && it_mount->logical_path != logical_path )
			continue;

		if ( it_mount->mount_type == file_system_impl::mount_history::mount_type_db )
		{
			get_path_info					(& archive_info, it_mount->physical_path.c_str());
			continue;
		}

		path_string	physical_path		=	it_mount->physical_path;
		physical_path					+=	'/';
		physical_path.appendf				(logical_path + it_mount->logical_path.length() + 1,
											 logical_path + strings::length(logical_path));

		path_info							info;
		bool is_full_path				=	true;
		path_string	cur_physical_path	=	physical_path;
		while ( identity(true) ) 
		{
			if ( get_path_info(& info, cur_physical_path.c_str()) != path_info::type_nothing )
				break;

			if ( it_mount->physical_path == cur_physical_path )
				break;

			directory_part_from_path_inplace	(& cur_physical_path);
			is_full_path				=	false;
		}

		if ( info.type == path_info::type_nothing )
			continue;

		if ( info.type == path_info::type_folder && !is_full_path )
			continue;

		if ( !is_full_path )
		{
			archive_info				=	info;
			continue;
		}

		* out_path_info					=	info;

		R_ASSERT							(!found, "more then one file corresponds to virtual path '%s'!", logical_path);
		found							=	true;
	}

	if ( !found )
	{
		if ( archive_info.type != path_info::type_nothing )
		{
			archive_info.type			=	path_info::type_archive;
			* out_path_info				=	archive_info;
		}

		return								out_path_info->type;
	}

	return									out_path_info->type;
}

bool   check_is_db_file (pcstr disk_path);

void   file_system_impl::mount_disk_folder (fat_folder_node<> * const	work_folder, 
										   	pcstr const					physical_path, 
											u32 const					hash,
											recursive_bool const		recursive)
{
	mount_disk_folder_impl				(work_folder, physical_path, hash, recursive);
	mount_overlapped					(work_folder, physical_path, hash, recursive);
}

void   file_system_impl::mount_overlapped  (fat_folder_node<> * const	work_folder, 
										   	pcstr const					physical_path, 
											u32 const					hash,
											recursive_bool const		recursive)
{
	fat_node<> *	it_node		=	* m_hash_set.find(hash);
	
	for ( ; it_node; it_node = node_hash_set::get_next_of_object(it_node) )
	{
		if ( it_node->is_link() )
			continue;
		if ( it_node->cast_folder() == work_folder )
			continue;
		if ( !it_node->same_path(work_folder->cast_node()) )
			continue;

		if ( fat_disk_file_node<> * disk_file = it_node->cast_disk_file() )
		{
			if ( disk_file->checked_is_db() )
				continue;

			bool const is_db_file			=	check_is_db_file(disk_file->disk_path());
			disk_file->set_checked_is_db		(is_db_file);
			path_string							logical_path;
			it_node->get_full_path				(logical_path);
			if ( is_db_file )
				continue_mount_disk_db			(disk_file, logical_path.c_str());
			continue;
		}

		fat_disk_folder_node<> * disk_folder_node	= it_node->cast_disk_folder();
		if ( !disk_folder_node )
			continue;

		if ( disk_folder_node->is_scanned(recursive) )
			continue;

		pcstr const it_physical_path	=	disk_folder_node->disk_path();
		if ( physical_path && strings::equal(it_physical_path, physical_path) )
			continue;

		mount_disk_folder_impl				(work_folder, it_physical_path, hash, recursive);
		disk_folder_node->set_is_scanned	(recursive);
	}	
}

} // namespace fs 
} // namespace xray 
