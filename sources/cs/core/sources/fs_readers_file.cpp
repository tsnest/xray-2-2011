////////////////////////////////////////////////////////////////////////////
//	Module 		: fs_readers_file.cpp
//	Created 	: 02.09.2006
//  Modified 	: 02.09.2006
//	Author		: Dmitriy Iassenev
//	Description : file system reader file class
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_readers_file.h"

#ifdef _MSC_VER
#	include <io.h>
#	include <share.h>
#endif // #ifdef _MSC_VER

#include <fcntl.h>
#include <sys/stat.h>

fs::readers::file::file		(int const file_handle, pcstr path)
{
#ifdef _MSC_VER
	u32			buffer_size = _filelength(file_handle);
	pvoid		buffer = cs_malloc(buffer_size, "file reader data");

	u32			result = _read(file_handle,buffer,buffer_size);
	R_ASSERT	(buffer_size == result,"Cannot read from file \"%s\"",path);
	R_ASSERT	(!_close(file_handle),"Cannot close file \"%s\" after reading",path);
#else // #ifdef _MSC_VER
	u32			buffer_size = lseek(file_handle, SEEK_END, 0);
	lseek		(file_handle, SEEK_SET, 0);
	pvoid		buffer = cs_malloc(buffer_size, "file reader data");

	u32			result = read(file_handle,buffer,buffer_size);
	R_ASSERT	(buffer_size == result,"Cannot read from file \"%s\"",path);
	R_ASSERT	(!close(file_handle),"Cannot close file \"%s\" after reading",path);
#endif // #ifdef _MSC_VER

	setup		(buffer,result,0);
}

fs::readers::file::~file	()
{
	seek		(0);
	pcvoid		temp = pointer();
	cs_free		(temp);
}
