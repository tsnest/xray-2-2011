////////////////////////////////////////////////////////////////////////////
//	Module 		: fs_writers_file.cpp
//	Created 	: 02.09.2006
//  Modified 	: 02.09.2006
//	Author		: Dmitriy Iassenev
//	Description : file system writer file class
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_writers_file.h"

#ifdef _MSC_VER
#	include <io.h>
#	include <share.h>
#	include <direct.h>
#endif // #ifdef _MSC_VER

#include <fcntl.h>
#include <sys/stat.h>

fs::writers::file::file		(pcstr path)
{
	m_path				= path;

	size_t const length = sz_len(path);
	pstr const temp		= (pstr)ALLOCA((length + 1)*sizeof(char));
	pcstr i				= path;
	pstr j				= temp;
	for ( ; *i; ++i, ++j) {
		*j	= *i;

		if ( (*i != '\\') && (*i != '/') )
			continue;

		if ( i == path )
			continue;

		*j				= 0;
#if CS_PLATFORM_WINDOWS | CS_PLATFORM_XBOX_360
		_mkdir			(temp);
#elif CS_PLATFORM_PS3 // #if CS_PLATFORM_WINDOWS | CS_PLATFORM_XBOX_360
		mkdir			(temp, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
#else // #ifdef _MSC_VER
#	error 
#endif // #ifdef _MSC_VER
		*j				= '\\';
	}

#if CS_PLATFORM_WINDOWS | CS_PLATFORM_XBOX_360
		_unlink			(path);
#elif CS_PLATFORM_PS3 // #if CS_PLATFORM_WINDOWS | CS_PLATFORM_XBOX_360
		unlink			(path);
#else // #ifdef _MSC_VER
#	error 
#endif // #ifdef _MSC_VER

#if defined(_MSC_VER)
	errno_t	error_code = 
		_sopen_s(
			&m_file_handle,
			path,
			_O_CREAT | _O_WRONLY | _O_BINARY,
			_SH_DENYWR, 
            _S_IREAD | _S_IWRITE
		);

	if (error_code) {
		threading::yield(1);
		error_code = 
			_sopen_s(
				&m_file_handle,
				path,
				_O_CREAT | _O_WRONLY | _O_BINARY,
				_SH_DENYWR, 
				_S_IREAD | _S_IWRITE
			);
	}

	R_ASSERT	(!error_code,"Cannot open file \"%s\" for writing",path);
#elif defined(__GNUC__) // #if defined(_MSC_VER)
	m_file_handle	= fileno( fopen( path, "rb" ) );
	R_ASSERT		( m_file_handle );
#else // #elif defined(__GNUC__)
#	error implement file open for your platform here
#endif // #if defined(_MSC_VER)
}

fs::writers::file::~file	()
{
#if CS_PLATFORM_WINDOWS | CS_PLATFORM_XBOX_360
	R_ASSERT	(!_close(m_file_handle),"Cannot close file \"%s\" after writing",*m_path);
#elif CS_PLATFORM_PS3 // #if CS_PLATFORM_WINDOWS | CS_PLATFORM_XBOX_360
	R_ASSERT	(!close(m_file_handle),"Cannot close file \"%s\" after writing",*m_path);
#else // #ifdef _MSC_VER
#	error implement file close for your platform here
#endif // #ifdef _MSC_VER
}

void fs::writers::file::w	(pcvoid buffer, u32 const buffer_size)
{
#if CS_PLATFORM_WINDOWS | CS_PLATFORM_XBOX_360
	R_ASSERT	(_write(m_file_handle,buffer,buffer_size) == (int)buffer_size,"Cannot write to file \"%s\"",*m_path);
#elif CS_PLATFORM_PS3 // #if CS_PLATFORM_WINDOWS | CS_PLATFORM_XBOX_360
	R_ASSERT	(write(m_file_handle,buffer,buffer_size) == (int)buffer_size,"Cannot write to file \"%s\"",*m_path);
#else // #ifdef _MSC_VER
#	error implement file close for your platform here
#endif // #ifdef _MSC_VER
}