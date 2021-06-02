////////////////////////////////////////////////////////////////////////////
//	Created		: 22.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef FS_PHYSICAL_PATH_INFO_DATA_H_INCLUDED
#define FS_PHYSICAL_PATH_INFO_DATA_H_INCLUDED

#include <xray/fs/native_path_string.h>

namespace xray {
namespace fs_new {

struct physical_path_info_data
{
	enum type_enum					{ type_error_no_path, type_file, type_folder };

	bool							exists		() const { return type != type_error_no_path; }
	bool							is_file		() const { return type == type_file; }
	bool							is_folder	() const { return type == type_folder; }

    file_size_type					file_size;
    u32								last_time_of_write;
	type_enum						type;

	enum	path_type_enum			{ path_type_unitialized, path_type_contains_name, path_type_contains_full_path };
	mutable native_path_string		path;
	mutable path_type_enum			path_type;

	physical_path_info_data			() 
		:	type(type_error_no_path), file_size(file_size_type(-1)), last_time_of_write(u32(-1)), path_type(path_type_unitialized) {}

}; // struct physical_path_info

} // namespace fs_new
} // namespace xray

#endif // #ifndef FS_PHYSICAL_PATH_INFO_DATA_H_INCLUDED