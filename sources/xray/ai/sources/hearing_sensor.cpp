////////////////////////////////////////////////////////////////////////////
//	Created		: 24.11.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "hearing_sensor.h"
#include "ai_world.h"
#include "sensed_object_types.h"
#include "sensed_object.h"
#include <xray/ai/npc.h>
#include <xray/ai/sensed_sound_object.h>
#include "behaviour.h"

namespace xray {
namespace ai {
namespace sensors {

hearing_sensor::hearing_sensor		( npc& npc, ai_world& world, brain_unit& brain ) :
	passive_sensor_base				( npc, world, brain ),
	m_subscription					( boost::bind( &hearing_sensor::on_perceive_sound, return_this(), _1 ) )
{
	m_world.sounds_manager().subscribe	( m_npc, m_subscription );
}

hearing_sensor::~hearing_sensor	( )
{
	m_world.sounds_manager().unsubscribe( m_npc, m_subscription );
}

inline hearing_sensor* hearing_sensor::return_this	( )
{
	return	this;
}

void hearing_sensor::on_perceive_sound	( sensed_sound_object const& perceived_sound ) const
{
	sensed_object sound_object			(
		m_npc.get_position( float3( 0, 0, 0 ) ),
		perceived_sound.position,
		c_ptr( perceived_sound.object ),
		m_world.get_current_time_in_ms(),
		sensed_object_type_sound,
		perceived_sound.power / 100.f
	);
	m_brain_unit.on_sensed_sound		( sound_object );
}

void hearing_sensor::set_parameters		( behaviour const& behaviour_parameters )
{
	m_parameters						= behaviour_parameters.get_hearing_sensor_parameters();
}

void hearing_sensor::dump_state			( npc_statistics& stats ) const
{
	XRAY_UNREFERENCED_PARAMETER			( stats );
}

} // namespace sensors
} // namespace ai
} // namespace xray
