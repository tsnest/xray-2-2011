////////////////////////////////////////////////////////////////////////////
//	Created 	: 19.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_READER_INLINE_H_INCLUDED
#define XRAY_MEMORY_READER_INLINE_H_INCLUDED

namespace xray {
namespace memory {

inline reader::reader			( non_null< u8 const, u8 const* >::ptr const data, u32 const size, u32 const position ) :
	m_data		( &*data ),
	m_pointer	( m_data + position ),
	m_size		( size )
{
	ASSERT		( m_size >= position );
}

inline void reader::r			( pvoid const destination, u32 const destination_size, u32 const size )
{
	ASSERT		( m_pointer >= m_data );
	ASSERT		( m_pointer <= (m_data + m_size) );
	ASSERT		( (m_data + m_size) >= (m_pointer + size) );

	copy		( destination, destination_size, m_pointer, size);
	m_pointer	+= size;
}

inline void reader::advance		( u32 offset )
{
	ASSERT		( m_pointer >= m_data );
	ASSERT		( m_pointer <= (m_data + m_size) );
	ASSERT		( (m_data + m_size) >= (m_pointer + offset) );

	m_pointer	+= offset;
}

inline void reader::seek		( u32 new_position )
{
	ASSERT		( m_pointer >= m_data );
	ASSERT		( m_pointer <= (m_data + m_size) );
	ASSERT		( m_size >= new_position );

	m_pointer	= m_data + new_position;
}

inline void	reader::rewind		( )
{
	seek		( 0 );
}

inline bool	reader::eof			( ) const
{
	ASSERT		( m_pointer >= m_data );
	ASSERT		( m_pointer <= (m_data + m_size) );

	return		( u32(m_pointer - m_data) >= m_size );
}

inline u32 reader::elapsed		( ) const
{
	ASSERT		( m_pointer >= m_data );
	ASSERT		( m_pointer <= (m_data + m_size) );

	return		( m_size - tell( ) );
}

inline u32 reader::tell			( ) const
{
	ASSERT		( m_pointer >= m_data );
	ASSERT		( m_pointer <= (m_data + m_size) );

	return		( u32(m_pointer - m_data) );
}

inline u32 reader::length		( ) const
{
	ASSERT		( m_pointer >= m_data );
	ASSERT		( m_pointer <= (m_data + m_size) );

	return		( m_size );
}

inline pcbyte reader::pointer	( ) const
{
	ASSERT		( m_pointer >= m_data );
	ASSERT		( m_pointer <= (m_data + m_size) );

	return		( m_pointer );
}

inline pcbyte reader::data		( ) const
{
	return		( m_data );
}

} // namespace memory
} // namespace xray

#endif // #ifndef XRAY_MEMORY_READER_INLINE_H_INCLUDED