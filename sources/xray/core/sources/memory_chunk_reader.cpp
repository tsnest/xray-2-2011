////////////////////////////////////////////////////////////////////////////
//	Created 	: 20.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

using xray::memory::chunk_reader;
using xray::memory::reader;

chunk_reader::chunk_reader					( pcbyte const data, u32 const size ) :
	m_reader			( data + sizeof( u32 ), size - sizeof( u32 ) ),
	m_chunks			( 0 ),
	m_chunk_count		( 0 )
{
	m_type				= chunk_type( *( u32* )data );
	ASSERT				( m_type < chunk_type_count );
	construct			( );
}

chunk_reader::chunk_reader					( pcbyte const data, u32 const size, chunk_type const type ) :
	m_reader			( data, size ),
	m_type				( type ),
	m_chunks			( 0 ),
	m_chunk_count		( 0 )

{
	construct			( );
}

void chunk_reader::construct				( )
{
	switch ( m_type ) {
		case chunk_type_sequential :	return	( sequential_chunk_reader< chunk_reader >::construct( ) );
		case chunk_type_array :			return	( array_chunk_reader< chunk_reader >::construct( ) );
		case chunk_type_associative :	return	( associative_chunk_reader< chunk_reader >::construct( ) );
		default	:						NODEFAULT();
	}
}

u32 chunk_reader::chunk_position			( u32 const chunk_id )
{
	switch ( m_type ) {
		case chunk_type_sequential :	return	( sequential_chunk_reader< chunk_reader >::chunk_position( chunk_id ) );
		case chunk_type_array :			return	( array_chunk_reader< chunk_reader >::chunk_position( chunk_id ) );
		case chunk_type_associative :	return	( associative_chunk_reader< chunk_reader >::chunk_position( chunk_id ) );
		default	:						NODEFAULT(return u32(-1));
	}
}

u32 chunk_reader::chunk_size				( u32 const chunk_id, chunk_type& type )
{
	u32 const position	= chunk_position( chunk_id );
	ASSERT				( position != u32( -1 ) );
	m_reader.seek		( position );
	u32 const id		= m_reader.r_u32( );
	ASSERT_U			( id == chunk_id );

	u32 const temp		= m_reader.r_u32( );
	u32 const size		= temp & 0x3fffffff;
	type				= chunk_type( temp >> 30 );
	return				( size );
}

chunk_reader chunk_reader::open_chunk_reader( u32 const chunk_id )
{
	chunk_type			type;
	u32 const size		= chunk_size( chunk_id, type );
	ASSERT				( type < chunk_type_count );
	return				( chunk_reader( m_reader.pointer( ), size, type ) );
}

reader chunk_reader::open_reader			( u32 const chunk_id )
{
	chunk_type			type;
	u32 const size		= chunk_size( chunk_id, type );
//	ASSERT				( type == chunk_type_count );
	return				( memory::reader( m_reader.pointer( ), size ) );
}

bool chunk_reader::chunk_exists				( u32 const chunk_id )
{
	u32 const position	= chunk_position( chunk_id );
	return				( position != u32( -1 ) );
}