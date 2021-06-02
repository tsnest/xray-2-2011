////////////////////////////////////////////////////////////////////////////
//	Created		: 24.11.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "interaction_sensor.h"
#include "ai_world.h"
#include <xray/ai/npc.h>
#include <xray/ai/collision_object.h>
#include "sensed_object_types.h"
#include "sensed_object.h"
#include "behaviour.h"

namespace xray {
namespace ai {
namespace sensors {

interaction_sensor::interaction_sensor	( npc& npc, ai_world& world, brain_unit& brain ) :
	active_sensor_base					( npc, world, brain )
{
}

void interaction_sensor::tick			( )
{
	ai_objects_type	objects				( g_allocator );
	m_world.get_colliding_objects		( m_npc.get_aabb(), objects );
	
	for ( ai_objects_type::const_iterator iter = objects.begin(); iter != objects.end(); ++iter )
	{
		collision_object const* const ai_object = (*iter)->get_collision_object();

		sensed_object pickup_object		(
			m_npc.get_position( float3( 0, 0, 0 ) ),
			ai_object->get_origin(),
			&ai_object->get_game_object(),
			m_world.get_current_time_in_ms(),
			sensed_object_type_interaction,
			1.f
		);
		m_brain_unit.on_interacting_object( pickup_object );
	}
}

void interaction_sensor::set_parameters	( behaviour const& behaviour_parameters )
{
	m_parameters						= behaviour_parameters.get_interaction_sensor_parameters();
}

void interaction_sensor::dump_state		( npc_statistics& stats ) const
{
	XRAY_UNREFERENCED_PARAMETER			( stats );
}

} // namespace sensors
} // namespace ai
} // namespace xray