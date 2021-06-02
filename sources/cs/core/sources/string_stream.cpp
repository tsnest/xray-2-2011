////////////////////////////////////////////////////////////////////////////
//	Module 		: string_stream.cpp
//	Created 	: 15.06.2005
//  Modified 	: 25.08.2007
//	Author		: Dmitriy Iassenev
//	Description : string stream class
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <cs/core/string_stream.h>
#include <stdarg.h>

string_stream::string_stream	() :
	m_buffer					(0),
	m_buffer_size				(0),
	m_allocated_size			(0)
{
}

string_stream::~string_stream	()
{
	cs_free						(m_buffer);
}

void string_stream::msg			(pcstr format, ...)
{
	va_list						mark;
	va_start					(mark, format);

	string4096					buffer;
	int							size = cs::vsnprintf(
		buffer,
		sizeof(buffer),
		sizeof(buffer) - 1,
		format,
		mark
	);
    va_end						(mark);

	if (!size)
		return;

//	OUTPUT_DEBUG_STRING			(buffer);

	// should be <, not <=, fix this later
	if (m_allocated_size <= (m_buffer_size + size)) {
		if (!m_allocated_size)
			m_allocated_size	= 1024;

		while (m_allocated_size <= (m_buffer_size + size))
			m_allocated_size	*= 2;

		m_buffer				= (char*)cs_realloc(m_buffer,m_allocated_size, "string_stream");
	}

	mem_copy					(m_buffer + m_buffer_size, buffer, size);
	m_buffer_size				+= size;
}
