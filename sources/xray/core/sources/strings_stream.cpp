////////////////////////////////////////////////////////////////////////////
//	Created		: 08.04.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/strings_stream.h>
#include <stdarg.h>

using xray::strings::stream;

void stream::append				( pcstr format, ... )
{
	va_list						mark;
	va_start					( mark, format );

	string4096					buffer;
	int	const size				= vsnprintf(
		buffer,
		sizeof(buffer) - 1,
		format,
		mark
	);
    va_end						( mark );

	if ( !size )
		return;

	if ( m_allocated_size <= (m_buffer_size + size) ) {
		if ( !m_allocated_size )
			m_allocated_size	= 1024;

		while ( m_allocated_size <= (m_buffer_size + size) )
			m_allocated_size	*= 2;

		m_buffer				= static_cast<pbyte>(XRAY_REALLOC_IMPL( *m_allocator, m_buffer, m_allocated_size, "xray::strings::stream"));
	}

	memory::copy				(m_buffer + m_buffer_size, m_allocated_size - m_buffer_size, buffer, size);
	m_buffer_size				+= size;
}