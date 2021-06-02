////////////////////////////////////////////////////////////////////////////
//	Created		: 30.11.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "damage_sensor.h"
#include "ai_world.h"
#include <xray/ai/sensed_hit_object.h>
#include "sensed_object_types.h"
#include "sensed_object.h"
#include "behaviour.h"

namespace xray {
namespace ai {
namespace sensors {

damage_sensor::damage_sensor	( npc& npc, ai_world& world, brain_unit& brain ) :
	passive_sensor_base			( npc, world, brain ),
	m_subscription				( boost::bind( &damage_sensor::on_perceive_hit, return_this(), _1 ) )
{
	m_world.damage_manager().subscribe	( m_npc, m_subscription );
}

damage_sensor::~damage_sensor	( )
{
	m_world.damage_manager().unsubscribe( m_npc, m_subscription );
}

inline damage_sensor* damage_sensor::return_this	( )
{
	return this;
}

void damage_sensor::on_perceive_hit			( sensed_hit_object const& sensed_hit ) const
{
	sensed_object hit_object				(
		sensed_hit.own_position,
		sensed_hit.direction,
		c_ptr( sensed_hit.object ),
		m_world.get_current_time_in_ms(),
		sensed_object_type_hit,
		sensed_hit.extent_of_damage
	);
	
	m_brain_unit.on_sensed_hit				( hit_object );
}

void damage_sensor::set_parameters	( behaviour const& behaviour_parameters )
{
	m_parameters					= behaviour_parameters.get_damage_sensor_parameters();
}

void damage_sensor::dump_state		( npc_statistics& stats ) const
{
	XRAY_UNREFERENCED_PARAMETER		( stats );
}

} // namespace sensors
} // namespace ai
} // namespace xray