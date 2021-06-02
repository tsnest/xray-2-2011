////////////////////////////////////////////////////////////////////////////
//	Created		: 17.04.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_STREAM_INLINE_H_INCLUDED
#define XRAY_MEMORY_STREAM_INLINE_H_INCLUDED

inline xray::memory::stream::stream				( memory::base_allocator* allocator ) :
	m_allocator		( allocator ),
	m_buffer		( 0 ),
	m_buffer_size	( 0 ),
	m_allocated_size( 0 )
{
}

inline xray::memory::stream::~stream			( )
{
	XRAY_FREE_IMPL	( *m_allocator, m_buffer );
}

inline void xray::memory::stream::append		( u32 const value )
{
	append			( &value, sizeof(value) );
}

inline void xray::memory::stream::append		( int const value )
{
	append			( &value, sizeof(value) );
}

inline void xray::memory::stream::append		( pcstr const value )
{
	append			( value, (strings::length(value) + 1)*sizeof(char) );
}

inline pbyte xray::memory::stream::get_buffer	( ) const
{
	return			m_buffer;
}

inline u32 xray::memory::stream::get_buffer_size( ) const
{
	return			m_buffer_size;
}

inline void xray::memory::stream::clear			( )
{
	m_buffer_size	= 0;
}

#endif // #ifndef XRAY_MEMORY_STREAM_INLINE_H_INCLUDED