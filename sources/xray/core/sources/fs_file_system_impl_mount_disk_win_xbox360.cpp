////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_file_system_impl.h"
#include "fs_helper.h"
#include <xray/fs_utils.h>
#include <io.h>

namespace xray {
namespace fs   {

bool   file_system_impl::mount_disk_file (fat_folder_node<> * const	work_folder, 
										  pcstr const				physical_path, 
										  u32 const					hash)
{
	path_info::type_enum const path_type	=	get_path_info(NULL, physical_path);
	file_size_type file_size = 0;
	CURE_ASSERT_CMP								(path_type, ==, path_info::type_file, return false);
	
	if ( !calculate_file_size(& file_size, physical_path) )
		LOGI_INFO							("file_system", "mount_disk_file '%s' cannot calculate file size", physical_path);

	path_string const file_name				=	file_name_from_path(physical_path);
	u32 const file_hash						=	crc32(file_name.c_str(), file_name.length(), hash);

	fat_node<> * const new_node				=	create_disk_file_node(	 file_hash, 
																		 file_name.c_str(), 
																		 file_name.length(), 
																		 physical_path,
																		 strings::length(physical_path),
																		 (u32)file_size )->cast_node();
	
	actualize_node								(new_node, file_hash, work_folder);
	return										true;
}

void   file_system_impl::mount_disk_folder (fat_folder_node<> * const	work_folder, 
											pcstr const					physical_path, 
											u32 const					hash)
{
	_finddata32i64_t						file_desc;
	path_string								file_spec;
	file_spec							+=	physical_path;
	file_spec							+=	"/*";

	intptr_t const handle				=	_findfirst32i64(convert_to_native(file_spec.c_str()).c_str(), 
																  & file_desc);
	file_spec.rtrim							('*');

	if ( handle							==	-1 )
		return;

	do
	{
		if ( file_desc.name[0] == '.' )
			continue;

		const bool   found_folder		=	(file_desc.attrib & _A_SUBDIR) != 0;
		u32 const	saved_size			=	file_spec.length();
		file_spec						+=	file_desc.name;

		u32 const name_length			=	strings::length(file_desc.name);
		u32 const child_hash			=	crc32(file_desc.name, name_length, hash);

		u32 const full_name_length		=	file_spec.length();
		fat_node<> * work_node			=	found_folder ? 
											create_disk_folder_node (child_hash,
																	 file_desc.name,
																 	 name_length,
																	 file_spec.c_str(),
																	 full_name_length)->cast_node() :
										    create_disk_file_node 	(child_hash,
																	 file_desc.name,
																	 name_length,
																	 file_spec.c_str(),
																	 full_name_length,
																	 (u32)file_desc.size)->cast_node();

		actualize_node						(work_node, child_hash, work_folder);

		if ( work_node->is_folder() )
			mount_disk_folder				(work_node->cast_folder(), file_spec.c_str(), child_hash);

		file_spec.set						(saved_size, NULL);
	}
	while ( _findnext32i64 (handle, &file_desc) != -1 );

	_findclose								(handle);
}

} // namespace fs
} // namespace xray