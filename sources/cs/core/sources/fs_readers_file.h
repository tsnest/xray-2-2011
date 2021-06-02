////////////////////////////////////////////////////////////////////////////
//	Module 		: fs_readers_file.h
//	Created 	: 02.09.2006
//  Modified 	: 02.09.2006
//	Author		: Dmitriy Iassenev
//	Description : file system reader file class
////////////////////////////////////////////////////////////////////////////

#ifndef FS_READERS_FILE_H_INCLUDED
#define FS_READERS_FILE_H_INCLUDED

#include <cs/core/fs_readers_base.h>

namespace fs { namespace readers {

class file : public base {
public:
					file	(int const file_handle, pcstr path);
			virtual	~file	();
};

}}

#endif // #ifndef FS_READERS_FILE_H_INCLUDED