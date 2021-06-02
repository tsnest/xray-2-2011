////////////////////////////////////////////////////////////////////////////
//	Created		: 30.11.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_impl.h"
#include "fs_impl_finding.h"
#include "resources_helper.h"

namespace xray {
namespace fs {

bool   check_is_db_file (pcstr disk_path)
{
	file_type * file			=	NULL;
	if ( !open_file(& file, open_file_read, disk_path, false) )
		return					false;

	u32 const char_count	=	10;
	fixed_string<char_count + 1> big_endian_string_part(big_endian_string, big_endian_string + char_count);
	fixed_string<char_count + 1> little_endian_string_part(little_endian_string, little_endian_string + char_count);

	fixed_string<char_count + 1> part;
	part.set_length				(char_count);
	bool is_db_file					=	false;
	if ( read_file(file, part.get_buffer(), char_count) == char_count )
		if ( part == big_endian_string_part || part == little_endian_string_part )
			is_db_file				=	true;
	
	close_file					(file);
	return						is_db_file;
}

bool   need_disk_folder_mount (fat_node<> * node, enum_flags<find_enum> find_flags, bool is_full_path)
{
	bool const need_recursive		=	is_full_path && (find_flags & find_recursively);

	fat_disk_folder_node<> * disk_folder	=	node->cast_disk_folder();
	if ( !disk_folder )
		return							false;
	
	if ( !disk_folder->is_scanned(need_recursive) )
		return							true;

	return								false;
}

bool   need_disk_db_mount (fat_node<> * node, enum_flags<find_enum> find_flags, bool is_full_path)
{
	bool const check_is_db			=	!(find_flags & find_file_only) || !is_full_path;
	
	if ( !check_is_db )
		return							false;

	fat_disk_file_node<> * disk_file	=	node->cast_disk_file();
	if ( disk_file->checked_is_db() )
	{
		return							disk_file->is_db_file();

// 		if ( disk_file->is_db_file() )
// 			env.other_thread_continuing_disk_mount	=	true;
//		return							false;
	}

	bool const is_db_file			=	check_is_db_file(disk_file->disk_path());
	if ( !is_db_file )
		return							false;
	
	disk_file->set_checked_is_db		(is_db_file);

	return								true;
}

bool   need_disk_mount (fat_node<> * node, enum_flags<find_enum> find_flags, bool is_full_path, bool traversing_leafs)
{
	for ( fat_node<> *	it_node		=	node; 
						it_node; 
						it_node		=	node_hash_set::get_next_of_object(it_node) )
	{
		if ( it_node->is_link() )
			continue;
		if ( it_node != node && !it_node->same_path(node) )
			continue;

		if ( it_node->cast_disk_folder() )
			return						need_disk_folder_mount	(it_node, find_flags, is_full_path);
		else if ( it_node->cast_disk_file() && (is_full_path || !traversing_leafs) )
			return						need_disk_db_mount		(it_node, find_flags, is_full_path);
	}

	return								false;
}

void   on_find_disk_mounted (bool						result,
							 find_results_struct *		find_results,
					         pcstr						path_to_find,
							 enum_flags<find_enum>		find_flags)
{
	R_ASSERT_U									(result);
	g_fat->impl()->find_async						(find_results, path_to_find, find_flags);
}

void   file_system_impl::find_async_across_mount_disk (find_env & env)
{
	resources::query_mount_callback	const mount_callback	
									= 	 boost::bind(& on_find_disk_mounted,
													 _1, 
													 env.find_results,
													 env.path_to_find, 
													 env.find_flags);
	path_string							node_path;
	env.node->get_full_path				(node_path);
	path_string							disk_path;
	g_fat->impl()->get_disk_path			(env.node, disk_path);

	bool const is_full_path			=	strings::equal(env.path_to_find, env.partial_path);
	bool const is_recursive			=	is_full_path && (env.find_flags & find_recursively);

	resources::query_continue_disk_mount(node_path.c_str(), 
										 disk_path.c_str(), 
										 is_recursive,
										 mount_callback,
								 		 & memory::g_resources_helper_allocator);
}

} // namespace fs
} // namespace xray
