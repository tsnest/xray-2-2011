////////////////////////////////////////////////////////////////////////////
//	Created		: 23.11.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef INTERACTION_SENSOR_H_INCLUDED
#define INTERACTION_SENSOR_H_INCLUDED

#include "active_sensor_base.h"
#include "interaction_sensor_parameters.h"

namespace xray {
namespace ai {
namespace sensors {

class interaction_sensor : public active_sensor_base
{
public:
						interaction_sensor	( npc& npc, ai_world& world, brain_unit& brain );
	
	virtual	void		tick				( );	
	virtual void		set_parameters		( behaviour const& behaviour_parameters );
	virtual void		dump_state			( npc_statistics& stats ) const;

private:
	interaction_sensor_parameters			m_parameters;
}; // class interaction_sensor

} // namespace sensors
} // namespace ai
} // namespace xray

#endif // #ifndef INTERACTION_SENSOR_H_INCLUDED