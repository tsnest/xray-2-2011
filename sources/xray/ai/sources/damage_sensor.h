////////////////////////////////////////////////////////////////////////////
//	Created		: 30.11.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef DAMAGE_SENSOR_H_INCLUDED
#define DAMAGE_SENSOR_H_INCLUDED

#include "passive_sensor_base.h"
#include "damage_subscriber.h"
#include "damage_sensor_parameters.h"

namespace xray {
namespace ai {
namespace sensors {

class damage_sensor : public passive_sensor_base
{
public:
					damage_sensor	( npc& npc, ai_world& world, brain_unit& brain );
	virtual			~damage_sensor	( );
	
	virtual void	set_parameters	( behaviour const& behaviour_parameters );
	virtual void	dump_state		( npc_statistics& stats ) const;

private:
	inline damage_sensor* return_this	( );

			void	on_perceive_hit	( sensed_hit_object const& sensed_hit ) const;
	
private:
	damage_subscriber				m_subscription;
	damage_sensor_parameters		m_parameters;
}; // class damage_sensor

} // namespace sensors
} // namespace ai
} // namespace xray

#endif // #ifndef DAMAGE_SENSOR_H_INCLUDED