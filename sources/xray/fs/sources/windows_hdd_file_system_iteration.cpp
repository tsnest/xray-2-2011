////////////////////////////////////////////////////////////////////////////
//	Created		: 24.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs/windows_hdd_file_system.h>
#include <sys/stat.h>				// for _S_IREAD, _S_IWRITE
#include <sys/types.h>
#include <xray/fs/path_string_utils.h>
#include <xray/fs/physical_path_info.h>

namespace xray {
namespace fs_new {

physical_path_info 	 windows_hdd_file_system::get_physical_path_info (native_path_string const & native_physical_path)
{
	native_path_string						absolute_physical_path;
	bool const convert_result			=	convert_to_absolute_path(& absolute_physical_path, native_physical_path, assert_on_fail_true);
	R_ASSERT_U								(convert_result);

	struct _stat32i64						local_stat;
	if ( _stat32i64(absolute_physical_path.c_str(), & local_stat) == -1 )
		return								physical_path_info();

	physical_path_initializer				initializer;
	initializer.device					=	this;
	initializer.data.path				=	absolute_physical_path.c_str();
	initializer.data.path_type			=	physical_path_info_data::path_type_contains_full_path;
	initializer.parent					=	NULL;
	initializer.data.type				=	local_stat.st_mode & S_IFDIR ? 
												physical_path_info_data::type_folder : physical_path_info_data::type_file;
	initializer.data.file_size			=	local_stat.st_size;
	initializer.data.last_time_of_write	=	local_stat.st_mtime;
	return									physical_path_info(initializer);
}

static
void   init_from_os_struct (physical_path_info_data * out_data, _finddata32i64_t & in_struct)
{
	out_data->file_size					=	in_struct.size;
	out_data->type						=	(in_struct.attrib & _A_SUBDIR) ? 
												physical_path_info_data::type_folder : physical_path_info_data::type_file;

	out_data->last_time_of_write		=	in_struct.time_write;
	out_data->path						=	in_struct.name;
	out_data->path_type					=	physical_path_info_data::path_type_contains_name;
}

bool   windows_hdd_file_system::find_first (u64 *						out_search_handle,
											physical_path_info_data	*	out_data,
											pcstr						absolute_native_physical_path_mask)
{
	_finddata32i64_t						file_desc;
	intptr_t const handle				=	_findfirst32i64(absolute_native_physical_path_mask, & file_desc);
	if ( handle == intptr_t(-1) )
	{
		* out_search_handle				=	u64(-1);
		return								false;
	}

	while ( file_desc.name[0] == '.' )
	{
		if ( _findnext32i64(handle, & file_desc) == u32(-1) )
		{
			find_close						(handle);
			* out_search_handle			=	u64(-1);
			return							true;
		}
	}

	* out_search_handle					=	(u64)handle;
	init_from_os_struct						(out_data, file_desc);	
	return									true;
}

bool   windows_hdd_file_system::find_next (u64 * in_out_search_handle, physical_path_info_data * out_data)
{
	R_ASSERT								(* in_out_search_handle != u64(-1));
	_finddata32i64_t						file_desc;

	if ( _findnext32i64((intptr_t)* in_out_search_handle, & file_desc) == u32(-1) )
	{
		* in_out_search_handle			=	u64(-1);
		return								false;
	}

	init_from_os_struct						(out_data, file_desc);
	return									true;
}

void   windows_hdd_file_system::find_close (u64 search_handle)
{
	_findclose								((intptr_t)search_handle);
}

} // namespace fs_new
} // namespace xray
