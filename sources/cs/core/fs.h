////////////////////////////////////////////////////////////////////////////
//	Module 		: fs.h
//	Created 	: 26.08.2006
//  Modified 	: 02.09.2006
//	Author		: Dmitriy Iassenev
//	Description : file system
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_FS_H_INCLUDED
#define CS_CORE_FS_H_INCLUDED

#include "fs_readers_base.h"
#include "fs_writers_base.h"

namespace fs {
	reader CS_CORE_API file_reader	(pcstr path);
	writer CS_CORE_API file_writer	(pcstr path);
}

#endif // #ifndef CS_CORE_FS_H_INCLUDED