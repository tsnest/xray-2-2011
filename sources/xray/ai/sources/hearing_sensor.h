////////////////////////////////////////////////////////////////////////////
//	Created		: 23.11.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef HEARING_SENSOR_H_INCLUDED
#define HEARING_SENSOR_H_INCLUDED

#include "passive_sensor_base.h"
#include "sound_subscriber.h"
#include "hearing_sensor_parameters.h"

namespace xray {
namespace ai {
namespace sensors {

class hearing_sensor : public passive_sensor_base
{
public:
						hearing_sensor	( npc& npc, ai_world& world, brain_unit& brain );
	virtual				~hearing_sensor	( );
	
	virtual void		set_parameters	( behaviour const& behaviour_parameters );
	virtual void		dump_state		( npc_statistics& stats ) const;

private:
	inline hearing_sensor* return_this	( );

			void	on_perceive_sound	( sensed_sound_object const& perceived_sound ) const;

private:
	sound_subscriber					m_subscription;
	hearing_sensor_parameters			m_parameters;
}; // class hearing_sensor

} // namespace sensors
} // namespace ai
} // namespace xray

#endif // #ifndef HEARING_SENSOR_H_INCLUDED