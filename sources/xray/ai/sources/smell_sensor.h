////////////////////////////////////////////////////////////////////////////
//	Created		: 23.11.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SMELL_SENSOR_H_INCLUDED
#define SMELL_SENSOR_H_INCLUDED

#include "passive_sensor_base.h"
#include "smell_sensor_parameters.h"

namespace xray {
namespace ai {
namespace sensors {

class smell_sensor : public passive_sensor_base
{
public:
						smell_sensor	( npc& npc, ai_world& world, brain_unit& brain );

	virtual void		set_parameters	( behaviour const& behaviour_parameters );
	virtual void		dump_state		( npc_statistics& stats ) const;

private:
	smell_sensor_parameters				m_parameters;
}; // class smell_sensor

} // namespace sensors
} // namespace ai
} // namespace xray

#endif // #ifndef SMELL_SENSOR_H_INCLUDED