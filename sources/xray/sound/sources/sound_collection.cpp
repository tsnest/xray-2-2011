////////////////////////////////////////////////////////////////////////////
//	Created		: 08.02.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "sound_collection.h"

namespace xray {
namespace sound {

sound_collection::sound_collection		(
		collection_playing_types type,
		bool can_repeat_successively,
		u16 cyclic_repeating_index,
		pvoid buffer,
		size_t max_count,
		u32 last_time
	) :	
	m_sounds							( buffer, (buffer_vector< sound_emitter_ptr >::size_type) max_count ),
	m_type								( type ),
	m_random_number						( last_time ),
	m_cyclic_repeating_from_index		( cyclic_repeating_index ),
	m_previously_played_sound_index		( (u32)-1 ),
	m_can_repeat_successively			( can_repeat_successively )
{
	LOG_DEBUG					( "sound_collection address: 0x%8x", this );
}

sound_collection::~sound_collection		( )
{
}

sound_emitter const* sound_collection::get_sound	( ) const
{
	if ( m_sounds.empty() )
		return 0;

	switch ( m_type )
	{
		case collection_playing_type_random:
		{
			u32 const sounds_size		= m_sounds.size();

			for ( ;; )
			{
				u32 const sound_index	= m_random_number.random( sounds_size );
				if ( ( sound_index != m_previously_played_sound_index ) || m_can_repeat_successively || ( sounds_size == 1 ) )
				{
					m_previously_played_sound_index	= sound_index;
					return m_sounds[sound_index].c_ptr();
				}
			}
		}

		case collection_playing_type_sequential:
		{
			u32 sound_index = m_previously_played_sound_index++;
			return m_sounds[++sound_index % m_sounds.size()].c_ptr();
		}
		
		default:
			NODEFAULT( return 0 );
	}
}

sound_propagator_emitter const* sound_collection::get_sound_propagator_emitter( ) const
{
	sound_emitter const* const emitter = get_sound();
	return emitter ? emitter->get_sound_propagator_emitter() : 0;
}

void sound_collection::add_sound		( sound_emitter_ptr sound )
{
	m_sounds.push_back					( sound );
	m_old_address						= (u64)get_sound_propagator_emitter();
}

sound_propagator_emitter const* sound_collection::get_sound_propagator_emitter( u64 address ) const
{
	buffer_vector< sound_emitter_ptr >::const_iterator begin	= m_sounds.begin( );
	buffer_vector< sound_emitter_ptr >::const_iterator end		= m_sounds.end( );

	for ( ; begin != end; ++begin )
	{
		sound_propagator_emitter const* const emitter		= (*begin)->get_sound_propagator_emitter( address );
		if ( (u64)emitter != 0 )
			return emitter;
	}

	return 0;
}

void sound_collection::serialize	( memory::writer& w ) const
{
//	w.write_u64					( (u64)get_sound_propagator_emitter( ) );
	w.write						( &m_type, sizeof( m_type ) );
	m_random_number.serialize	( w );
	w.write_u32					( m_cyclic_repeating_from_index );
	w.write_u32					( m_previously_played_sound_index );
	w.write						( &m_can_repeat_successively, sizeof( bool ) );

	buffer_vector< sound_emitter_ptr >::const_iterator begin	= m_sounds.begin( );
	buffer_vector< sound_emitter_ptr >::const_iterator end		= m_sounds.end( );

	for ( ; begin != end; ++begin )
	{
		(*begin)->serialize			( w );
	}
}

void sound_collection::deserialize	( memory::reader& r )
{
//	m_old_address						= r.r_u64( );
	r.r				( &m_type, sizeof( collection_playing_types ), sizeof( collection_playing_types ) );
	m_random_number.deserialize( r );
	m_cyclic_repeating_from_index		= r.r_u32( );
	m_previously_played_sound_index		= r.r_u32( );
	r.r				( &m_can_repeat_successively, sizeof( bool ), sizeof( bool ) );

	buffer_vector< sound_emitter_ptr >::const_iterator begin	= m_sounds.begin( );
	buffer_vector< sound_emitter_ptr >::const_iterator end		= m_sounds.end( );

	for ( ; begin != end; ++begin )
	{
		(*begin)->deserialize			( r );
	}
}

//sound_propagator_emitter const* sound_collection::get_sound_propagator_emitter( u64 address ) const
//{
//	buffer_vector< sound_emitter_ptr >::const_iterator begin	= m_sounds.begin( );
//	buffer_vector< sound_emitter_ptr >::const_iterator end		= m_sounds.end( );
//	for ( ; begin != end; ++begin )
//	{
//		sound_propagator_emitter const* emt	= (*begin)->get_sound_propagator_emitter( address );
//		if ( (u64)emt == address )
//			return emt;
//	}
//
//	return 0;
//}

} // namespace sound
} // namespace xray