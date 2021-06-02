////////////////////////////////////////////////////////////////////////////
//	Module 		: fs_readers_base.cpp
//	Created 	: 02.09.2006
//  Modified 	: 02.09.2006
//	Author		: Dmitriy Iassenev
//	Description : file system reader base class
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <cs/core/fs_readers_base.h>

u32 fs::readers::base::advance_string	()
{
	pcstr			B = (pcstr)m_buffer + m_position;
	pcstr			E = (pcstr)m_buffer + m_buffer_size;
	pcstr			I = B;
	for ( ; (I != E) && !terminal_character(*I); ++I);

	u32				result = u32(I - B);

	if (I == E) {
		m_position	+= result;
		return		result;
	}

	for (++I; (I != E) && terminal_character(*I); ++I);

	if (I == E) {
		m_position	= m_buffer_size;
		return		result;
	}

	m_position		+= u32(I - B);
	return			result;
}
