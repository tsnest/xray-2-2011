////////////////////////////////////////////////////////////////////////////
//	Created		: 23.11.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "smell_sensor.h"
#include "behaviour.h"

namespace xray {
namespace ai {
namespace sensors {

smell_sensor::smell_sensor			( npc& npc, ai_world& world, brain_unit& brain ) :
	passive_sensor_base				( npc, world, brain )
{
}

void smell_sensor::set_parameters	( behaviour const& behaviour_parameters )
{
	m_parameters					= behaviour_parameters.get_smell_sensor_parameters();
}

void smell_sensor::dump_state		( npc_statistics& stats ) const
{
	XRAY_UNREFERENCED_PARAMETER		( stats );
}

} // namespace sensors
} // namespace ai
} // namespace xray