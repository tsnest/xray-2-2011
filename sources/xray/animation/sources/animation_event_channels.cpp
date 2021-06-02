////////////////////////////////////////////////////////////////////////////
//	Created		: 27.09.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/animation/animation_event_channels.h>

namespace xray {
namespace animation {

event_channel			&animation_event_channels::channel( u32 id )
{
	ASSERT( m_channels_count != 0 );
	ASSERT( m_channels_count != u32(-1) );
	ASSERT( id < m_channels_count );
	return get_shift_ptr( (event_channel*) (this), m_internal_memory_position )[id];
}

event_channel	const	&animation_event_channels::channel( u32 id ) const
{
	ASSERT( m_channels_count != 0 );
	ASSERT( m_channels_count != u32(-1) );
	ASSERT( id < m_channels_count );
	return get_shift_ptr( (event_channel const*) (this), m_internal_memory_position )[id];
}

struct find_predicate
{
	find_predicate( pcstr name ): m_name( name ){}
	
	bool operator () ( const event_channel &channel )
	{
		return strings::equal( channel.name(), m_name );
	}

	pcstr m_name;
};

u32			animation_event_channels::get_channel_id( pcstr name ) const 
{
	if ( m_channels_count == u32(-1) || m_channels_count == 0 )
		return u32(-1);
	
	const event_channel  *begin = &channel( 0 ), *end = ( &channel(m_channels_count-1) + 1 );

	find_predicate p( name ); 

	const event_channel  * r = std::find_if ( begin, end,  p );

	if ( r == end )
		return u32(-1);

	return u32( r - begin );

}

animation_event_channels::animation_event_channels():
m_channels_count( u32(-1) ),
m_internal_memory_position( size_t(-1) )
{

}

animation_event_channels::~animation_event_channels()
{
	if ( m_channels_count ==  u32(-1) || m_channels_count == 0 )
		return;
	ASSERT( m_channels_count !=  u32(-1) );
	ASSERT( m_internal_memory_position != size_t(-1) );
	for ( u32 i = 0; i < m_channels_count; ++i )
		channel( i ).~event_channel();
}

size_t	animation_event_channels::internal_memory_size()const
{
	
	if ( m_channels_count == u32(-1) )
		return 0;
	
	u32 mem_size = channels_self_memory_size ( m_channels_count );

	for ( u32 i = 0 ; i < m_channels_count ; ++i )
		mem_size += channel( i ).internal_memory_size	();
	return mem_size;

}

void	animation_event_channels::copy_internals( void* mem_buff )const
{
	memory::copy( mem_buff, internal_memory_size(),   get_shift_ptr(  (void const*) (this), m_internal_memory_position ),internal_memory_size() )  ;
}

void	animation_event_channels::create_in_place_internals ( animation_event_channels const& event_channels, void* memory_buff )
{
	m_internal_memory_position = bytes_dist(  memory_buff, this );
	m_channels_count = event_channels.channels_count( );

	event_channels.copy_internals( memory_buff );
}

} // namespace animation
} // namespace xray
