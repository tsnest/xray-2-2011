////////////////////////////////////////////////////////////////////////////
//	Created		: 12.01.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs_utils.h>
#include "fs_watcher_internal.h"

namespace xray {
namespace fs {

#if XRAY_FS_WATCHER_ENABLED

FILE * file_type_to_FILE(file_type * file)
{
	return								file->handle;
}

u32   open_file_notifications_count (pcstr const portable_path, enum_flags<open_file_enum> const open_flags)
{
	path_info::type_enum const path_type	=	get_path_info(NULL, portable_path);
	if ( path_type == path_info::type_folder )
		return							0;

	if ( path_type == path_info::type_nothing )
	{
		if ( !(open_flags & u32(open_file_create | open_file_truncate | open_file_append)) )
			return						0;
		return							1;
	}

	// here  path_type == path_info::type_file
	if ( open_flags & open_file_truncate )
		return							1;

	return								0;
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
	verify_path_is_portable		( portable_path );
	if ( (open_flags & open_file_create) == open_file_create )
		fs::make_dir_r			( portable_path, false );

	u32 const notifications_to_skip	=	skip_notifications ? 
											open_file_notifications_count(portable_path, open_flags) : 0;
	skip_deassociation_notifications	(portable_path, notifications_to_skip);

	FILE * out_handle		=	NULL;
	bool const out_result	=	open_file_impl(& out_handle, open_flags, portable_path, assert_on_fail );
	if ( !out_result )
	{
		bool const removed	=	try_remove_deassociation_notifications(portable_path, notifications_to_skip);
		R_ASSERT				(removed);
	}
	else
	{
		u32 const path_length	=	strings::length(portable_path);
		file_type * file		=	(file_type *)DEBUG_ALLOC(char, sizeof(file_type) + path_length + 1);
		new (file)				file_type;
		strings::copy			(file->file_name, path_length + 1, portable_path);
		file->handle		=	out_handle;
		file->skip_notifications	=	skip_notifications;

		* out_file			=	file;
	}

	return						out_result;
}

bool is_watcher_enabled ();

size_t   write_file (file_type * file, pcvoid data, size_t size)
{
	R_ASSERT					(file->handle);

	if ( !file->done_write )
	{
		if ( file->skip_notifications )
			skip_deassociation_notifications	(file->file_name, 1);

		file->done_write	=	true;
	}

	return						fwrite(data, 1, size, file->handle);
}

size_t	  read_file	(file_type * file, pvoid data, size_t size)
{
	return						fread(data, 1, size, file->handle);
}

void   close_file (file_type * file)
{
	fclose						(file->handle);
	file->~file_type			();
	DEBUG_FREE					(file);
}

#endif // #if XRAY_FS_WATCHER_ENABLED

} // namespace fs
} // namespace xray
