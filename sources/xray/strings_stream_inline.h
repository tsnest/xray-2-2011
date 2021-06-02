////////////////////////////////////////////////////////////////////////////
//	Created		: 08.04.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_STRINGS_STREAM_INLINE_H_INCLUDED
#define XRAY_STRINGS_STREAM_INLINE_H_INCLUDED

inline xray::strings::stream::stream				( memory::base_allocator* allocator ) :
	m_allocator		( allocator ),
	m_buffer		( 0 ),
	m_buffer_size	( 0 ),
	m_allocated_size( 0 )
{
}

inline xray::strings::stream::~stream				( )
{
	XRAY_FREE_IMPL	( *m_allocator, m_buffer );
}

inline pcbyte xray::strings::stream::get_buffer		( ) const
{
	return			m_buffer;
}

inline u32 xray::strings::stream::get_buffer_size	( ) const
{
	return			m_buffer_size;
}

inline void xray::strings::stream::clear			( )
{
	m_buffer_size	= 0;
}

#endif // #ifndef XRAY_STRINGS_STREAM_INLINE_H_INCLUDED