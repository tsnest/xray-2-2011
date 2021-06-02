////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs_utils.h>			// for fs::open_file_enum, ...
#include <xray/fs_path.h>			// for fs::convert_to_native
#include <fcntl.h>					// for _O_RDWR, _O_RDONLY, _O_WRONLY, ...
#include <share.h>					// for _SH_DENYWR, _SH_DENYNO
#include <sys/stat.h>				// for _S_IREAD, _S_IWRITE
#include <direct.h>					// for _mkdir
#include "fs_watcher_internal.h"	// for file_type_to_FILE function

namespace xray {
namespace fs {

#ifdef DEBUG
static 
fixed_string512   file_open_flags_to_string (u32 const file_open_flags)
{
	fixed_string512					out_string;
	if ( file_open_flags & _O_RDWR )
		out_string				+=	" RDWR";
	if ( file_open_flags == _O_RDONLY )
		out_string				+=	" RD";
	if ( file_open_flags & _O_WRONLY )
		out_string				+=	" WR";
	if ( file_open_flags & _O_CREAT )
		out_string				+=	" CREATE";
	if ( file_open_flags & _O_TRUNC )
		out_string				+=	" TRUNC";
	if ( file_open_flags & _O_APPEND )
		out_string				+=	" APPEND";

	return							out_string;
}
#endif // #ifdef DEBUG

signalling_bool   open_file_impl (FILE** const						out_file,
								  enum_flags<open_file_enum> const	open_flags, 
								  pcstr const						portable_path,
								  bool const						assert_on_fail)
{
	ASSERT							(out_file);
	verify_path_is_portable			(portable_path);
	path_string						absolute_native_path;
	if ( !convert_to_absolute_native_path(& absolute_native_path, portable_path) )
		return						false;

	int	file_open_flags			=	_O_BINARY | _O_SEQUENTIAL;
	int permission_flags		=	((open_flags & open_file_read)  ? _S_IREAD  : 0) | 
									((open_flags & open_file_write) ? _S_IWRITE : 0);
	int sh_permission_flags		=	(open_flags & open_file_write) ? _SH_DENYWR : _SH_DENYNO;

	if ( (open_flags & open_file_read) && (open_flags & open_file_write) )
		file_open_flags			|=	_O_RDWR;
	else if ( (open_flags & open_file_read) )
		file_open_flags			|=	_O_RDONLY;
	else if ( (open_flags & open_file_write) )
		file_open_flags			|=	_O_WRONLY;
	else 
		FATAL("%s", "one or both read or write flags must be specified");

	if ( open_flags & open_file_create )
		file_open_flags			|=	_O_CREAT;
	if ( open_flags & open_file_truncate )
		file_open_flags			|=	_O_TRUNC;
	if ( open_flags & open_file_append )
		file_open_flags			|=	_O_APPEND;

	int		file_handle			=	0;
	errno_t error_code			=	_sopen_s(& file_handle, absolute_native_path.c_str(), file_open_flags, 
											 sh_permission_flags, permission_flags);

	if ( error_code ) 
	{
		threading::yield		(1);
		error_code				=	_sopen_s(& file_handle, absolute_native_path.c_str(), file_open_flags,		
											 sh_permission_flags, permission_flags);
	}

	bool const success			=	!error_code && file_handle != -1;
	pcstr const fopen_flags		=	"r+b";

	* out_file					=	success ? _fdopen(file_handle, fopen_flags) : NULL;

	if ( assert_on_fail )
		ASSERT						(*out_file,
									 "failed opening file:%s with open flags:%s",
									 absolute_native_path.c_str(),
									 file_open_flags_to_string(file_open_flags).c_str());
	
	return							!!* out_file;
}

signalling_bool   calculate_file_size (file_size_type * const out_file_size, FILE * const file)
{
	ASSERT						(out_file_size);
	CURE_ASSERT					(file, return false);
	* out_file_size			=	(file_size_type)_filelengthi64(_fileno(file));
	return						true;
}

bool   make_dir (pcstr portable_path)
{
	verify_path_is_portable			(portable_path);
	path_string						absolute_native_path;
	if ( !convert_to_absolute_native_path(& absolute_native_path, portable_path) )
		return						false;

	if ( _unlink(absolute_native_path.c_str()) != 0 ) {
	}

	if ( _mkdir (absolute_native_path.c_str()) == -1 )
	{
		errno_t err;
		_get_errno(&err);
		return err == EEXIST ? true : false;
	}

	return true;
}

path_info::type_enum   get_path_info (path_info * out_path_info, pcstr portable_path)
{
	verify_path_is_portable			(portable_path);
	path_string						absolute_native_path;
	if ( !convert_to_absolute_native_path(& absolute_native_path, portable_path) )
		return						path_info::type_nothing;

	_finddata32i64_t				file_desc;
	intptr_t const handle		=	_findfirst32i64(absolute_native_path.c_str(), & file_desc);
	if ( handle	==	-1 )
	{
		path_info::type_enum const out_path_type	=	path_info::type_nothing;
		if ( out_path_info )
			out_path_info->type	=	out_path_type;
		return						out_path_type;
	}
	
	path_info::type_enum const out_path_type	=	!!(file_desc.attrib & _A_SUBDIR) ? path_info::type_folder : path_info::type_file;
	if ( out_path_info )
	{
		out_path_info->type		=	out_path_type;
		out_path_info->file_last_modify_time	=	file_desc.time_write;
		out_path_info->file_size				=	(file_size_type)file_desc.size;
	}

	_findclose						(handle);

	return							out_path_type;
}

static inline bool skip_notifications	( file_type * file )
{
#if XRAY_FS_WATCHER_ENABLED
	return						file->skip_notifications;
#else // #if XRAY_FS_WATCHER_ENABLED
	XRAY_UNREFERENCED_PARAMETER	( file );
	return						false;
#endif // #if XRAY_FS_WATCHER_ENABLED
}

void   set_file_size_and_close (file_type * file, file_size_type size)
{
	R_ASSERT							(file_type_to_FILE(file));

	if ( skip_notifications( file ) ) {
		file_size_type				previous_size;
		bool const got_file_size	=	calculate_file_size(& previous_size, file);
		R_ASSERT_U					(got_file_size);
		
		u32 notifications_to_skip	=	0;
		if ( previous_size < size )
			notifications_to_skip	=	1;
		else if ( previous_size > size )
			notifications_to_skip	=	2;

		skip_deassociation_notifications	(file, notifications_to_skip);
	}

	int filedes				=	_fileno( file_type_to_FILE(file) );
    int const error			=	_chsize_s(filedes, size);
	R_ASSERT_U					(!error);
	close_file					(file);
}

void   set_end_of_file_and_close (file_type * file)
{
	s64 const file_pos		=	_ftelli64( file_type_to_FILE(file) );
	set_file_size_and_close		(file, (file_size_type)file_pos);
}

} // namespace fs
} // namespace xray