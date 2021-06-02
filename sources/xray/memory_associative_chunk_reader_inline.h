////////////////////////////////////////////////////////////////////////////
//	Created 	: 18.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_ASSOCIATIVE_CHUNK_READER_INLINE_H_INCLUDED
#define XRAY_MEMORY_ASSOCIATIVE_CHUNK_READER_INLINE_H_INCLUDED

#define TEMPLATE_SIGNATURE			template < typename implementation >
#define ASSOCIATIVE_CHUNK_READER	xray::memory::associative_chunk_reader< implementation >

TEMPLATE_SIGNATURE
inline void	ASSOCIATIVE_CHUNK_READER::construct			( )
{
	impl( ).m_last_position	= 0;
	reader&	reader			= impl( ).reader( );
	impl( ).m_chunks		= reader.pointer( ) + reader.r_u32( );
	impl( ).m_chunk_count	= reader.r_u32( );
	ASSERT					( impl( ).m_chunks < reader.pointer( ) + reader.elapsed( ) );
}

TEMPLATE_SIGNATURE
inline u32 ASSOCIATIVE_CHUNK_READER::chunk_position		( u32 const chunk_id )
{
	reader&	reader			= impl( ).reader( );
	if ( impl( ).m_last_position && (impl( ).m_last_position < reader.length( )) ) {
		reader.seek			( impl( ).m_last_position );
		reader.r_u32		( );
		u32 const size		= reader.r_u32( ) & 0x3fffffff;
		reader.advance		( size );
		u32 result			= reader.tell( );
		if ( reader.r_u32( ) == chunk_id ) {
			impl( ).m_last_position	= result;
			return			( result );
		}
	}

	Pair const* const b		= ( Pair const* )impl( ).m_chunks;
	Pair const* const e		= (( Pair const* )impl( ).m_chunks) + impl( ).m_chunk_count;
	Pair const* i			= std::find_if( b, e, predicate( chunk_id ) );
	if ( i == e )
		return				( u32( -1 ) );

	u32 const result		= ( *i ).second;
	impl( ).m_last_position	= result;
	return					( result );
}

TEMPLATE_SIGNATURE
inline implementation& ASSOCIATIVE_CHUNK_READER::impl	( )
{
	return					( (implementation&)*this );
}

#undef ARRAY_CHUNK_READER
#undef TEMPLATE_SIGNATURE

#endif // #ifndef XRAY_MEMORY_ASSOCIATIVE_CHUNK_READER_INLINE_H_INCLUDED