////////////////////////////////////////////////////////////////////////////
//	Created		: 29.11.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "vision_sensor.h"
#include "hearing_sensor.h"
#include "damage_sensor.h"
#include "smell_sensor.h"
#include "interaction_sensor.h"
#include "brain_unit.h"

namespace xray {
namespace ai {

static sensors::active_sensor_base* create_active_sensor_by_type	(
		npc& npc,
		ai_world& world,
		pcstr active_sensor_type,
		brain_unit& brain
	)
{
	if ( strings::equal( active_sensor_type, "vision" ) )
		return NEW( sensors::vision_sensor )( npc, world, brain );

	if ( strings::equal( active_sensor_type, "interaction" ) )
		return NEW( sensors::interaction_sensor )( npc, world, brain );
	
	return 0;
}

static sensors::passive_sensor_base* create_passive_sensor_by_type	(
		npc& npc,
		ai_world& world,
		pcstr passive_sensor_type,
		brain_unit& brain
	)
{
	if ( strings::equal( passive_sensor_type, "hearing" ) )
		return NEW( sensors::hearing_sensor )( npc, world, brain );

	if ( strings::equal( passive_sensor_type, "smell" ) )
		return NEW( sensors::smell_sensor )( npc, world, brain );

	if ( strings::equal( passive_sensor_type, "damage" ) )
		return NEW( sensors::damage_sensor )( npc, world, brain );
	
	return 0;
}

void brain_unit::create_sensors		( configs::binary_config_value const& options )
{
	configs::binary_config_value const& sensors_value			= options["sensors"];
	
	configs::binary_config_value::const_iterator it				= sensors_value.begin();
	configs::binary_config_value::const_iterator it_end			= sensors_value.end();

	for ( ; it != it_end; ++it )
	{
		configs::binary_config_value const& sensor_type_value	= *it;
		pcstr sensor_type										= (pcstr)sensor_type_value;
		if ( sensors::active_sensor_base* active_sensor			=
				create_active_sensor_by_type					(
					m_npc,
					m_world,
					sensor_type,
					*this
				)
			)
		{
			m_active_sensors.push_back							( active_sensor );
		}
		else if ( sensors::passive_sensor_base* passive_sensor	=
				create_passive_sensor_by_type					(
					m_npc,
					m_world,
					sensor_type,
					*this
				)
			)
		{
			m_passive_sensors.push_back							( passive_sensor );
		}
		else
		{
			LOG_ERROR	( "Unknown sensor type was declared - %s", sensor_type );
		}
	}
}

struct sensor_parameters_predicate : private boost::noncopyable
{
	inline sensor_parameters_predicate	( behaviour_ptr const& new_behaviour ) :
		m_behaviour						( new_behaviour )	 
	{
	}

	template < typename T >
	inline void operator()				( T* object ) const
	{
		object->set_parameters			( *m_behaviour.c_ptr() );
	}

	behaviour_ptr const&				m_behaviour;
};

void brain_unit::set_sensors_parameters	( )
{
	sensor_parameters_predicate			sensor_parameters_setter( m_behaviour );
	m_active_sensors.for_each			( sensor_parameters_setter );
	m_passive_sensors.for_each			( sensor_parameters_setter );
}

} // namespace ai
} // namespace xray