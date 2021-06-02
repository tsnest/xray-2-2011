////////////////////////////////////////////////////////////////////////////
//	Module 		: fs.cpp
//	Created 	: 02.09.2006
//  Modified 	: 02.09.2006
//	Author		: Dmitriy Iassenev
//	Description : file system
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <cs/core/fs.h>
#include "fs_readers_file.h"
#include "fs_writers_file.h"

#ifdef _MSC_VER
#	include <io.h>
#	include <share.h>
#endif // #ifdef _MSC_VER

#include <fcntl.h>
#include <sys/stat.h>

fs::reader fs::file_reader	(pcstr path)
{
#ifdef _MSC_VER
	int	file_handle;
	errno_t	error_code	= 
		_sopen_s(
			&file_handle,
			path,
			_O_RDONLY | _O_BINARY,
			_SH_DENYNO, 
            _S_IREAD | _S_IWRITE
		);

	if (error_code) {
		threading::yield( 1 );
		error_code		= 
			_sopen_s(
				&file_handle,
				path,
				_O_RDONLY | _O_BINARY | _O_SEQUENTIAL,
				_SH_DENYNO, 
				_S_IREAD | _S_IWRITE
			);
	}

	if (error_code)
		return			0;

	return				cs_new<readers::file>(file_handle,path);
#else // #ifdef _MSC_VER
	FILE* const file	= fopen( path, "rb" );
	R_ASSERT			( file );
	return				cs_new<readers::file>(fileno(file),path);
#endif // #ifdef _MSC_VER
}

fs::writer fs::file_writer	(pcstr path)
{
	return				cs_new<writers::file>(path);
}