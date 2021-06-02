////////////////////////////////////////////////////////////////////////////
//	Created		: 11.02.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/sound/single_sound.h>
#include <xray/sound/world_user.h>
#include "sound_world.h"

namespace xray {
namespace sound {

single_sound::single_sound	( encoded_sound_with_qualities_ptr const& encoded_sound, sound_rms_ptr const& rms, configs::binary_config_ptr const& spl, sound_spl_ptr const& spl_curve ) :
	m_rms					( rms ),
	m_spl_config			( spl ),
	m_spl					( spl_curve )
{
	m_old_address									= (u64)get_sound_propagator_emitter	( );

	m_encoded_sound.initialize_and_set_parent( this, encoded_sound.c_ptr() );
	LOG_DEBUG					( "ss: 0x%8x", get_sound_propagator_emitter() );
//	LOG_DEBUG					( "single_sound address: 0x%8x", this );
}

single_sound::~single_sound	( )
{
}

void single_sound::emit_sound_propagators	(	sound_instance_proxy_internal& proxy,
												playback_mode mode,
												u32 before_playing_offset,
												u32 after_playing_offset,
												sound_producer const* const producer,
												sound_receiver const* const ignorable_receiver
											) const
{

	sound_propagator* new_propagator	= proxy.get_sound_scene( ).create_sound_propagator
										(
											*this,
											proxy,
											mode,
											0,
											before_playing_offset,
											after_playing_offset,
											producer,
											ignorable_receiver
										);

	if ( new_propagator )
		proxy.add_sound_propagator		( *new_propagator );
}


sound_propagator_emitter const* single_sound::get_sound_propagator_emitter	( ) const
{
	return this;
}


void single_sound::serialize	( memory::writer& w ) const
{
	m_old_address				= (u64)get_sound_propagator_emitter( );
	w.write_u64					( m_old_address );
}

void single_sound::deserialize	( memory::reader& r )
{
	m_old_address				= r.r_u64( );
}

encoded_sound_child_ptr const& single_sound::get_quality_for_resource( ) const
{
	return m_encoded_sound->get_encoded_sound( );
}

} // namespace sound
} // namespace xray