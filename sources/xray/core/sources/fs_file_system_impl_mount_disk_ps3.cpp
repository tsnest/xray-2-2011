////////////////////////////////////////////////////////////////////////////
//	Created		: 07.05.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_file_system_impl.h"
#include "fs_helper.h"
#include <sys/paths.h>
#include <dirent.h>
#include <xray/fs_utils.h>

bool   file_system_impl::mount_disk_file (fat_folder_node<> * const	work_folder, 
										  pcstr const				physical_path, 
										  u32 const					hash)
{
	XRAY_UNREFERENCED_PARAMATERS(work_folder, physical_path, hash);
	NOT_IMPLEMENTED(return false);
}
void xray::fs::file_system_impl::mount_disk_folder (
		fat_folder_node<> * const	work_folder,
		pcstr const					physical_path,
		u32 const					hash
	)
{
	DIR* const directory			= opendir( physical_path.c_str() );
	R_ASSERT						( directory );

	u32 const physical_path_length	= strings::length( physical_path.c_str() );
	
	while ( dirent* const entry = readdir( directory ) ) {
		if ( entry->d_name[0] == '.' )
			continue;

		bool const found_folder		= ( entry->d_type == DT_DIR );
		u32 const name_length		= strings::length( entry->d_name );
		u32 const child_hash		= crc32( entry->d_name, name_length, hash );

		pstr full_name;
		STR_JOINA					( full_name, physical_path.c_str(), "/", entry->d_name );
		u32 const full_name_length	= strings::length( full_name );

		file_size_type file_size	= 0;
		if ( !found_folder ) {
			bool const result		= fs::calculate_file_size( &file_size, full_name );
			R_ASSERT				( result, "cannot open file \'%s\'", full_name );
		}

		fat_node<> * work_node		=
			found_folder ? 
				create_disk_folder_node(
					child_hash,
					entry->d_name,
					name_length,
					full_name,
					full_name_length
				)->cast_node( )
			:
				create_disk_file_node(
					child_hash,
					entry->d_name,
					name_length,
					full_name,
					full_name_length,
					file_size
				)->cast_node( );

		actualize_node				( work_node, child_hash, work_folder );
		R_ASSERT_CMP				( found_folder, ==, work_node->is_folder() );
		if ( !found_folder )
			continue;

		mount_disk_folder			( work_node->cast_folder(), full_name, child_hash );
	}

	closedir						( directory );
}