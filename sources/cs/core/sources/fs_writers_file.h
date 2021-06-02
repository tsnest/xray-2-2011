////////////////////////////////////////////////////////////////////////////
//	Module 		: fs_writers_file.h
//	Created 	: 02.09.2006
//  Modified 	: 02.09.2006
//	Author		: Dmitriy Iassenev
//	Description : file system writer file class
////////////////////////////////////////////////////////////////////////////

#ifndef FS_WRITERS_FILE_H_INCLUDED
#define FS_WRITERS_FILE_H_INCLUDED

#include <cs/core/fs_writers_base.h>

namespace fs { namespace writers {

class file : public base {
private:
	int				m_file_handle;
	shared_string	m_path;

public:
					file	(pcstr path);
	virtual			~file	();
	virtual	void	w		(pcvoid buffer, u32 const buffer_size);
};

}}

#endif // #ifndef FS_WRITERS_FILE_H_INCLUDED