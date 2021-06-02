////////////////////////////////////////////////////////////////////////////
//	Created		: 04.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "enemy_perceptor.h"
#include "brain_unit.h"
#include "sensed_object.h"

namespace xray {
namespace ai {
namespace perceptors {

enemy_perceptor::enemy_perceptor	( npc& npc, brain_unit& brain, working_memory& memory )
	:	perceptor_base				( npc, brain, memory ),
		m_subscription				( boost::bind( &enemy_perceptor::on_sensed_object_retrieval, return_this(), _1 ) )
{
	m_brain_unit.subscribe_on_sensors_events	( m_subscription );
}

enemy_perceptor::~enemy_perceptor	( )
{
	m_brain_unit.unsubscribe_from_sensors_events( m_subscription );
}

inline enemy_perceptor* enemy_perceptor::return_this	( )
{
	return this;
}

void enemy_perceptor::on_sensed_object_retrieval( sensors::sensed_object const& memory_object )
{
	if ( m_brain_unit.is_object_ignored( c_ptr( memory_object.object ) ) )
		return;
	
	switch ( memory_object.type )
	{
		case sensors::sensed_object_type_hit :
		case sensors::sensed_object_type_sound :
		case sensors::sensed_object_type_visual :
		{
			if ( memory_object.confidence == 1.f )
				m_working_memory.forget_all_about_object( *memory_object.object );
			
			percept_memory_object* memory_fact	= m_working_memory.create_memory_object( percept_memory_object_type_enemy );
			memory_fact->object					= memory_object.object;
			memory_fact->owner_position			= memory_object.position;
			memory_fact->target_position		= memory_object.direction;
			memory_fact->update_time			= memory_object.update_time;
			memory_fact->confidence				= memory_object.confidence;
		}
		break;

		default :
			return;
	}
}

} // namespace perceptors
} // namespace ai
} // namespace xray