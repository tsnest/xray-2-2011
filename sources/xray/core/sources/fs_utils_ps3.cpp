////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs_utils.h>
#include <xray/fs_path.h>
#include <sys/paths.h>
#include "fs_watcher_internal.h"

namespace xray {
namespace fs {

signalling_bool   open_file_impl (FILE** const						out_file,
								  enum_flags<open_file_enum> const	open_flags, 
								  pcstr const						portable_path,
								  bool const						assert_on_fail)
{
	ASSERT							(out_file);
	verify_path_is_portable			(portable_path);
	path_string	native_path		=	convert_to_native(portable_path);

	pcstr file_open_flags		=	"";
	if ( (open_flags & open_file_read) && (open_flags & open_file_write) )
		file_open_flags			=	"wb+";
	else if ( (open_flags & open_file_read) )
		file_open_flags			=	"rb";
	else if ( (open_flags & open_file_write) )
		file_open_flags			=	"wb";
	else 
		FATAL("%s", "one or both read or write flags must be specified");

	pcstr file_open_flags_final	=	file_open_flags;
	if ( open_flags & open_file_append )
		STR_JOINA				( file_open_flags_final, file_open_flags, "a" );

	*out_file					=	fopen( native_path.c_str(), file_open_flags_final );

	if ( assert_on_fail )
		ASSERT						(*out_file,
									 "failed opening file:%s",
									 native_path.c_str());

	return							!!* out_file;
}

signalling_bool   calculate_file_size (file_size_type * const out_file_size, FILE * const file)
{
	ASSERT						(out_file_size);
	CURE_ASSERT					(file, return false);

	long const current_position	= ftell( file );

	fseek						( file, 0, SEEK_END );
	* out_file_size				= (file_size_type)ftell(file);

	fseek						( file, current_position, SEEK_SET );

	return						true;
}

bool   make_dir (pcstr portable_path)
{
	verify_path_is_portable			(portable_path);
	path_string	native_path		=	convert_to_native(portable_path);

	unlink						( native_path.c_str() );
	if ( mkdir(native_path.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) == -1 )
	{
		return					false;//errno() == EEXIST;
	}

	return true;
}

path_info::type_enum   get_path_info (path_info * out_path_info, pcstr path)
{
	verify_path_is_portable			( path );

	struct stat local_stat;
	std::stat						( convert_to_native(path).c_str(), &local_stat );

	path_info::type_enum const out_path_type	=	local_stat.st_mode & S_IFDIR ? path_info::type_folder : path_info::type_file;
	if ( out_path_info ) {
		out_path_info->type			=	out_path_type;
		out_path_info->file_size	=	local_stat.st_size;
		out_path_info->file_last_modify_time	= local_stat.st_mtime;
	}

	return							out_path_type;
}

pcstr get_default_user_data_folder	( )
{
	return							SYS_APP_HOME "/user_data";
}

path_string   convert_to_relative_path (pcstr physical_path)
{
	XRAY_UNREFERENCED_PARAMETER(physical_path);
	NOT_IMPLEMENTED();
	path_string	result;
	return result;
}

signalling_bool		convert_to_absolute_native_path (buffer_string * out_result, 
													 pcstr relative_portable_path)
{
	XRAY_UNREFERENCED_PARAMETER(relative_portable_path);
	NOT_IMPLEMENTED();
	return			true;
}

void   set_file_size_and_close (file_type * file, file_size_type size)
{
	int filedes				=	fileno( file_type_to_FILE(file) );
    int const error			=	ftruncate64(filedes, size);
	R_ASSERT					(!error);
	close_file					(file);
}

void   set_end_of_file_and_close (file_type * file)
{
	file_size_type const file_pos	= ftell( file_type_to_FILE(file) );
	set_file_size_and_close			(file, file_pos);
}

} // namespace fs
} // namespace xray