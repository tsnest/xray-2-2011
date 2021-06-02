////////////////////////////////////////////////////////////////////////////
//	Created		: 10.05.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "logic_action.h"
#include "object_behaviour.h"
#include "event_manager.h"
#include "object_scene.h"
#include <xray/math_randoms_generator.h>

namespace stalker2 {

logic_action::logic_action( event_manager* event_manager ) 
:m_event_manager(event_manager),
m_reference_count(0)
{ 
};
logic_action::~logic_action		( )
{
}

void logic_action::destroy( logic_action* action_to_destroy )
{
	DELETE	( action_to_destroy );
}

action_emit_event::action_emit_event( event_manager* event_manager, game_event const& e, object_controlled* o )
:super	( event_manager ),
m_event	( e ),
m_object_controlled ( o )
{
	m_priority = 0;
}

void action_emit_event::execute( game_event const& sender_event )
{
	XRAY_UNREFERENCED_PARAMETER	( sender_event );
	m_event_manager->emit_event ( m_event, m_object_controlled );
}


action_set_object_behaviour::action_set_object_behaviour(	event_manager* event_manager, 
															object_controlled* o, 
															object_behaviour* b, 
															object_scene_job* job )
:super			( event_manager ),
m_object		( o ),
m_behaviour		( b ),
m_job			( job )
{
	R_ASSERT	(m_object);
	m_priority	= 10000;// last
}
u32 uid = 0;
void action_set_object_behaviour::execute( game_event const& e )
{
	XRAY_UNREFERENCED_PARAMETER	( e );
	
	LOG_INFO("%d activate behaviour: %s", ++uid, m_behaviour->full_name().c_str());

	object_behaviour*  prev = m_object->get_current_behaviour();
	if(prev)
	{
		if(prev==m_behaviour)
		{
		
		}
		LOG_INFO("%d prev behaviour was: %s", uid, prev->full_name().c_str());
		m_object->detach_current_behaviour	( );
		prev->detach_from_object			( m_object ); 
	}

	m_object->set_behaviour			( m_behaviour );
	m_behaviour->attach_to_object	( m_object ); 

	m_job->switch_to_behaviour( m_behaviour );//tmp
}



action_filtered_set_object_behaviour::action_filtered_set_object_behaviour(	event_manager* event_manager, 
														 object_controlled* o, 
														 configs::binary_config_value const& action_data, 
														 object_scene_job* job )
														 :super			( event_manager ),
														 m_object		( o ),
														 m_action_data	( action_data ),
														 m_job			( job )														 
{
	R_ASSERT	(m_object);
	m_priority	= 10000;// last
}

static math::random32 random(100500);

void action_filtered_set_object_behaviour::execute( game_event const& e )
{
	XRAY_UNREFERENCED_PARAMETER	( e );

	object_behaviour* new_behaviour = NULL;

	u32 random_value = random.random( 100 );
	
	configs::binary_config_value::const_iterator behaviours_it		= m_action_data["behaviours"].begin( );
	configs::binary_config_value::const_iterator behaviours_it_end	= m_action_data["behaviours"].end( );
	
	u32 prev_val = 0;
	u32 next_val = 0;
	pcstr current_behaviour = NULL;
	for ( ; behaviours_it != behaviours_it_end ; ++behaviours_it )
	{
		next_val = u32(*behaviours_it) + prev_val;
		current_behaviour = behaviours_it->key();
		if ( random_value >= prev_val && random_value <= next_val ){
			new_behaviour = m_job->get_behaviour( current_behaviour );		
			break;
		}
		prev_val = next_val;
	}

	R_ASSERT( new_behaviour != NULL );

	LOG_INFO("%d activate behaviour: %s", ++uid, new_behaviour->full_name().c_str());

	object_behaviour*  prev = m_object->get_current_behaviour();
	if(prev)
	{
		if(prev==new_behaviour)
		{

		}
		LOG_INFO("%d prev behaviour was: %s", uid, prev->full_name().c_str());
		m_object->detach_current_behaviour	( );
		prev->detach_from_object			( m_object ); 
	}

	m_object->set_behaviour			( new_behaviour );
	new_behaviour->attach_to_object	( m_object ); 

	m_job->switch_to_behaviour( new_behaviour );//tmp
}




action_start_scene::action_start_scene( event_manager* event_manager, object_scene_ptr scene )
:super( event_manager ),
m_scene_to_start( scene )
{
}

void action_start_scene::execute( game_event const& )
{
	m_scene_to_start->start();
}

} // namespace stalker2