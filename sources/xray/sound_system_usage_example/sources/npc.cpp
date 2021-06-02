////////////////////////////////////////////////////////////////////////////
//	Created		: 29.08.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "npc.h"
#include <xray/sound/playback_mode.h>
#include <xray/sound/world_user.h>

namespace xray {
namespace sound_system_usage_example {

npc::npc					(	float3 const& position,
								float3 const& direction,
								sound::sound_emitter_ptr sound_emitter,
								pcstr description
							) :
	m_sound_emitter			( sound_emitter ),
	m_description			( description ),
	m_position				( position ),
	m_direction				( direction )
{
}

npc::~npc	( )
{
	m_proxy			= 0;
	m_sound_emitter	= 0;
}

void npc::emit	( sound::sound_scene_ptr scene, sound::world_user& user )
{
	m_proxy								= m_sound_emitter->emit_spot_sound( scene, user, sound::human );
}

void npc::say	( )
{
	m_proxy->set_position_and_direction	( m_position, m_direction );
	m_proxy->play						( sound::once, this, this );
}

void npc::say_once	( sound::sound_scene_ptr scene, sound::world_user& user )
{
	m_sound_emitter->emit_and_play_once( scene, user, m_position );
}

pcstr npc::get_description	( ) const
{
	return m_description;
}

float3 npc::get_source_position	( float3 const& requester ) const
{
	XRAY_UNREFERENCED_PARAMETER	( requester );
	return m_position;
}

void npc::on_sound_event	( sound::sound_producer const& sound_source )
{
	LOG_DEBUG			( sound_source.get_description( ) );
}

void npc::register_as_sound_receiver	( sound::sound_scene_ptr scene, sound::world_user& user )
{
	user.register_receiver				( scene, *this );
	sound_receiver::set_position		( m_position );
}

void npc::unregister_as_sound_receiver	( sound::sound_scene_ptr scene, sound::world_user& user )
{
	user.unregister_receiver( scene, *this );
}

} // namespace sound_system_usage_example
} // namespace xray
