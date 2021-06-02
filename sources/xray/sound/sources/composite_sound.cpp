////////////////////////////////////////////////////////////////////////////
//	Created		: 11.02.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "composite_sound.h"

namespace xray {
namespace sound {

composite_sound::composite_sound	( pvoid buffer, size_t max_count, u32 last_time ) :
m_collection					( buffer, (buffer_vector< sounds_type >::size_type)max_count ),
m_random_number					( last_time )
{
	m_old_address				= (u64)get_sound_propagator_emitter	( );
	LOG_DEBUG					( "composite_sound address: 0x%8x", get_sound_propagator_emitter() );
}

composite_sound::~composite_sound	( )
{
}

void composite_sound::emit_sound_propagators	(
		sound_instance_proxy_internal& proxy,
		playback_mode mode,
		u32 before_playing_offset,
		u32 after_playing_offset,
		sound_producer const* const producer,
		sound_receiver const* const ignorable_receiver
	) const
{
	for ( u32 i = 0; i < m_collection.size(); ++i )
	{
		if ( sound_propagator_emitter const* const generator = m_collection[i].first->get_sound_propagator_emitter() )
		{
			LOG_DEBUG					( "sp_address: 0x%8x", generator );
			if ( m_collection[i].second.first == m_collection[i].second.second )
				generator->emit_sound_propagators	( proxy, mode, before_playing_offset + m_collection[i].second.first, after_playing_offset, producer, ignorable_receiver );
			else
			{
				u32 const offset	= m_random_number.random( m_collection[i].second.second - m_collection[i].second.first ) + m_collection[i].second.first;
				generator->emit_sound_propagators	( proxy, mode, before_playing_offset + offset, after_playing_offset, producer, ignorable_receiver );
			}
		}
	}
}

sound_propagator_emitter const* composite_sound::get_sound_propagator_emitter( ) const
{
	return this;
}

void composite_sound::add_sound		( sound_emitter_ptr sound, std::pair< u32, u32 > const& offsets )
{
	m_collection.push_back			( std::make_pair( sound, offsets ) );
}

void composite_sound::serialize	( memory::writer& w ) const
{
	m_old_address				= (u64)get_sound_propagator_emitter( );
	w.write_u64					( m_old_address );

	buffer_vector< sounds_type >::const_iterator begin		= m_collection.begin( ); 
	buffer_vector< sounds_type >::const_iterator end		= m_collection.end( ); 
	
	for ( ; begin != end; ++ begin )
		begin->first->serialize			( w );
}

void composite_sound::deserialize	( memory::reader& r )
{
	m_old_address					= r.r_u64( );

	buffer_vector< sounds_type >::const_iterator begin		= m_collection.begin( ); 
	buffer_vector< sounds_type >::const_iterator end		= m_collection.end( ); 
	
	for ( ; begin != end; ++ begin )
		begin->first->deserialize		( r );
}

sound_propagator_emitter const* composite_sound::get_sound_propagator_emitter( u64 address ) const
{
	if ( address == m_old_address )
		return this;

	for ( u32 i = 0; i < m_collection.size(); ++i )
	{
		if ( sound_propagator_emitter const* const generator = m_collection[i].first->get_sound_propagator_emitter() )
		{
			sound_propagator_emitter const* emt	= generator->get_sound_propagator_emitter( address );
			if ( (u64)emt != 0 )
				return emt;
		}
	}

	//if ( address == (u64)get_sound_propagator_emitter( ) )
	//	return this;

	//for ( u32 i = 0; i < m_collection.size(); ++i )
	//{
	//	if ( sound_propagator_emitter const* const generator = m_collection[i].first->get_sound_propagator_emitter() )
	//	{
	//		sound_propagator_emitter const* emt	= generator->get_sound_propagator_emitter( address );
	//		if ( (u64)emt == address )
	//			return emt;
	//	}
	//}

	return 0;
}

} // namespace sound
} // namespace xray