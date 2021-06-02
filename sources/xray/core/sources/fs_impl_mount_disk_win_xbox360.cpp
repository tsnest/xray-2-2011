////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_impl.h"
#include "fs_helper.h"
#include <xray/fs_utils.h>
#include <io.h>

#include <xray/os_preinclude.h>
#	undef	NOUSER            			// All USER undefs and routines
#	undef	NOMSG             			// typedef MSG and associated routines
#include <xray/os_include.h>

namespace xray {
namespace fs   {

bool   file_system_impl::mount_disk_file (fat_folder_node<> * const	work_folder, 
										  pcstr const				physical_path, 
										  u32 const					hash)
{
	path_info::type_enum const path_type	=	get_path_info(NULL, physical_path);
	file_size_type file_size = 0;
	CURE_ASSERT_CMP							(path_type, ==, path_info::type_file, return false);
	
	if ( !calculate_file_size(& file_size, physical_path) )
		LOGI_INFO							("fs", "mount_disk_file '%s' cannot calculate file size", physical_path);

	path_string const file_name			=	file_name_from_path(physical_path);
	u32 const file_hash					=	crc32(file_name.c_str(), file_name.length(), hash);

	fat_node<> * const new_node			=	create_disk_file_node(	 file_hash, 
																	 file_name.c_str(), 
																	 file_name.length(), 
																	 physical_path,
																	 strings::length(physical_path),
																	 (u32)file_size )->cast_node();
	
	actualize_node							(new_node, file_hash, work_folder);
	return									true;
}

bool   file_system_impl::physical_node_mounted	(u32 const hash, pcstr const physical_path)
{
	fat_node<> *	it_node		=	* m_hash_set.find(hash);
	
	for ( ; it_node; it_node = node_hash_set::get_next_of_object(it_node) )
	{
		if ( it_node->is_link() )
			continue;

		if ( fat_disk_file_node<> * disk_file = it_node->cast_disk_file() )
		{
			if ( strings::equal(disk_file->disk_path(), physical_path) )
				return						true;
		}
	}

	return									false;
}

void   file_system_impl::mount_disk_folder_impl (fat_folder_node<> * const	work_folder, 
										    	pcstr const					physical_path, 
												u32 const					hash,
												recursive_bool const		recursive)
{
	_finddata32i64_t						file_desc;

	path_string								absolute_physical_path;
	bool const convert_result			=	convert_to_absolute_native_path(& absolute_physical_path, physical_path);
	R_ASSERT_U								(convert_result);

	path_string	relative_path			=	convert_to_relative_path(physical_path);
	relative_path						+=	"/";

	path_string								file_spec;
	file_spec							+=	absolute_physical_path;
	file_spec							+=	"\\" "*";

	fat_disk_folder_node<> * const disk_folder_node	=	work_folder->cast_disk_folder_node();

	path_string							disk_folder_path;
	if ( disk_folder_node )
		get_disk_path						(disk_folder_node->cast_node(), disk_folder_path);
	bool const is_disk_path_of_node		=	(disk_folder_path == physical_path);

	if ( disk_folder_node && disk_folder_node->is_scanned(false) && is_disk_path_of_node )
	{
		if ( disk_folder_node->is_scanned(true) )
			return;

		if( !recursive )
			return;

		continue_mount_disk_folder_recursively		(disk_folder_node, hash);
		return;
	}

	intptr_t const handle				=	_findfirst32i64(file_spec.c_str(), & file_desc);
	file_spec.rtrim							('*');

	if ( handle							==	-1 )
	{
		if ( disk_folder_node && !disk_folder_node->is_scanned(recursive) && is_disk_path_of_node )
			disk_folder_node->set_is_scanned	(recursive);
		return;
	}

	do
	{
		if ( file_desc.name[0] == '.' )
			continue;

		const bool   found_folder		=	(file_desc.attrib & _A_SUBDIR) != 0;
		u32 const	saved_size			=	relative_path.length();
		relative_path					+=	file_desc.name;

		path_string lower_case_name		=	file_desc.name;
		lower_case_name.make_lowercase		();

		u32 const child_hash			=	crc32(lower_case_name.c_str(), lower_case_name.length(), hash);

		u32 const full_name_length		=	relative_path.length();

		if ( physical_node_mounted(child_hash, relative_path.c_str()) )
		{
			relative_path.set				(saved_size, NULL);
			continue;
		}

		fat_node<> * work_node			=	found_folder ? 
											create_disk_folder_node (child_hash,
																	 lower_case_name.c_str(),
																 	 lower_case_name.length(),
																	 relative_path.c_str(),
																	 full_name_length)->cast_node() :
										    create_disk_file_node 	(child_hash,
																	 lower_case_name.c_str(),
																	 lower_case_name.length(),
																	 relative_path.c_str(),
																	 full_name_length,
																	 (u32)file_desc.size)->cast_node();

		actualize_node						(work_node, child_hash, work_folder);

		if ( work_node->is_folder() && recursive )
			mount_disk_folder				(work_node->cast_folder(), relative_path.c_str(), child_hash, recursive);

		relative_path.set					(saved_size, NULL);
	}
	while ( _findnext32i64 (handle, &file_desc) != -1 );

	_findclose								(handle);

	if ( disk_folder_node && !disk_folder_node->is_scanned(recursive) && is_disk_path_of_node )
		disk_folder_node->set_is_scanned	(recursive);

}

} // namespace fs
} // namespace xray