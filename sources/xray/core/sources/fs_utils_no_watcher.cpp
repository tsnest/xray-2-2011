////////////////////////////////////////////////////////////////////////////
//	Created		: 12.01.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs_utils.h>

namespace xray {
namespace fs {

#if !XRAY_FS_WATCHER_ENABLED

u32   open_file_notifications_count (pcstr const portable_path, enum_flags<open_file_enum> const open_flags)
{
	XRAY_UNREFERENCED_PARAMETERS		( portable_path, &open_flags )	;
	return								0;
}

FILE *   file_type_to_FILE(file_type * file)
{
	return								file;
}

signalling_bool   open_file_impl (FILE * * const					out_file,
								  enum_flags<open_file_enum> const	open_flags, 
								  pcstr	const 						portable_path,
								  bool const						assert_on_fail);

signalling_bool   open_file (file_type * * const				out_file,
							 enum_flags<open_file_enum> const	open_flags, 
							 pcstr const						portable_path,
							 bool const							assert_on_fail,
							 bool const							skip_notifications)
{
	XRAY_UNREFERENCED_PARAMETER	(skip_notifications);
	verify_path_is_portable		( portable_path );
	if ( (open_flags & open_file_create) == open_file_create )
		fs::make_dir_r			( portable_path, false );
	
	return						open_file_impl(out_file, open_flags, portable_path, assert_on_fail );
}

size_t   write_file (file_type * file, pcvoid data, size_t size)
{
	return						fwrite(data, 1, size, file);
}

size_t	  read_file	(file_type * file, pvoid data, size_t size)
{
	return						fread(data, 1, size, file);
}

void   close_file (file_type * file)
{
	fclose						(file);
}

#endif // #if !XRAY_FS_WATCHER_ENABLED

} // namespace fs
} // namespace xray

