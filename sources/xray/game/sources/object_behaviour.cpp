////////////////////////////////////////////////////////////////////////////
//	Created		: 16.05.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_behaviour.h"
#include "object_scene.h"
#include "logic_action.h"
#include "event_manager.h"
#include "camera_behaviour.h"
#include "event_generator_behaviour.h"
#include "light_behaviour.h"
#include "proximity_trigger_behaviour.h"
#include "values_storage_behaviour.h"
#include "composite_object_behaviour.h"
#include "npc_play_animation_behaviour.h"
#include "npc_reach_position_behaviour.h"
#include "npc_empty_switcher_behaviour.h"

namespace stalker2
{

object_behaviour::object_behaviour( object_scene_job* owner, configs::binary_config_value const& data, pcstr name )
:m_data	( data ),
m_owner	( owner ),
m_prev_time ( 0.0f ),
m_name	( name ),
m_controlled_object( NULL )
{	
	//LOG_INFO( "object_behaviour construct [%x]", this );

	m_timing_enabled = m_data.value_exists("timing");

	if ( m_timing_enabled )  {
		// load event generator data
		configs::binary_config_value t_event_generator		= m_data["timing"];
		configs::binary_config_value::const_iterator it		= t_event_generator.begin();
		configs::binary_config_value::const_iterator it_e	= t_event_generator.end();

		for( ;it!=it_e; ++it)
		{
			timer_item						itm;
			itm.m_time						= (*it)["time"];
			itm.m_event						= (pcstr)(*it)["value"];
			m_time_events_list.push_back	( itm );
		}
		std::sort(m_time_events_list.begin(), m_time_events_list.end());
		m_length		= m_data["length"];
		m_cycled		= m_data["cycled"];
	}
};

void object_behaviour::attach_to_object( object_controlled* o )
{
	m_controlled_object = o;
	
	if ( m_timing_enabled )
		m_timer.start			( );

	m_prev_time				= 0.0f;

	if(m_data.value_exists("events"))
	{
		configs::binary_config_value t_event_handlers = m_data["events"];
		configs::binary_config_value::const_iterator it = t_event_handlers.begin();
		configs::binary_config_value::const_iterator it_e = t_event_handlers.end();

		for( ;it!=it_e; ++it)
		{
			pcstr event_name	= (*it)["event_name"];
			bool bglobal		= (*it)["is_global"];
			configs::binary_config_value::const_iterator action_it = (*it)["actions"].begin();
			configs::binary_config_value::const_iterator action_it_e = (*it)["actions"].end();

			game_event e( event_name );

			fixed_string<256> desc = "eh:";
			desc.appendf( "behaviour[%s] event_name[%s]" , full_name().c_str(), event_name );

			for( ;action_it!=action_it_e; ++action_it )
			{
				logic_action_ptr a = NULL;			
				
				pcstr action_type = (*action_it)["action_type"];

				desc.appendf( " type[%s]" , action_type );

				if(strings::equal(action_type, "switch_to_behaviour" ))
				{
					pcstr bevaviour_name		= (*action_it)["switching_to_behaviour"];
					object_behaviour* behaviour	= m_owner->get_behaviour			( bevaviour_name );
					a							= NEW(action_set_object_behaviour)( m_owner->get_event_manager(), o, behaviour, m_owner );
					desc.appendf( " to_behaviour[%s]", bevaviour_name );									
				}else
				if(strings::equal(action_type, "filtered_switch_to_behaviour" ))
				{
					a							= NEW(action_filtered_set_object_behaviour)( m_owner->get_event_manager(), o, *action_it, m_owner );
				}else
				if(strings::equal(action_type, "raise_event" ))
				{
					pcstr raise_event_name		= (*action_it)["raised_event"];
					game_event e( raise_event_name );
					bool is_global =  ( *action_it ).value_exists("is_global") && (bool)(*action_it)["is_global"] == false;
					if ( is_global )
						a							= NEW(action_emit_event)( m_owner->get_event_manager(), e, m_controlled_object );
					else
						a							= NEW(action_emit_event)( m_owner->get_event_manager(), e );

					desc.appendf( " raised_event [%s]", raise_event_name, is_global );

				}else
				{
					UNREACHABLE_CODE();
				}
				desc.appendf( " [%x]", a );
			//	LOG_INFO( desc.c_str() );
				m_owner->get_event_manager()->subscribe_event_handler	( e, (bglobal) ? NULL : o, a, desc.c_str(), true );
				m_actions.push_back										( a );
			}
		}
	}

	start( );
}

void object_behaviour::detach_from_object( object_controlled* o )
{
	XRAY_UNREFERENCED_PARAMETERS	( o );
	logic_action_list::const_iterator it		= m_actions.begin();
	logic_action_list::const_iterator it_e		= m_actions.end();

	for( ;it!=it_e; ++it)
	{
		logic_action_ptr a								= *it;
		m_owner->get_event_manager()->unsubscribe_event_handler	( a.c_ptr() );
	}
	m_actions.clear									( );
	m_controlled_object = NULL;
}

void object_behaviour::start				(  )
{
	m_owner->get_event_manager()->emit_event( game_event( "on_started" ) , m_controlled_object );
}

fixed_string<128> object_behaviour::full_name( ) const
{
	fixed_string<128> result;
	result.assignf("%s:%s", m_owner->name(), m_name );
	return result;
}

void object_behaviour::tick( )
{	
	if ( !m_timing_enabled )
		return;

	float const t = m_timer.get_elapsed_sec();

	float const time_current = (m_cycled) ? fmod(t, m_length ) : t;

	bool b_was_cycle		= (m_cycled) && (time_current<m_prev_time);

	events_list::iterator it = std::lower_bound(m_time_events_list.begin(), m_time_events_list.end(), m_prev_time );

	while(it!=m_time_events_list.end())
	{
		timer_item& itm			= *it;
		if( itm.m_time>m_prev_time )
		{
			if(!b_was_cycle && itm.m_time>time_current)
				break;

			game_event ev(itm.m_event);
			m_owner->get_event_manager()->emit_event ( ev, m_controlled_object );
		}else
			break;
		++it;
	}

	if(b_was_cycle)
	{
		events_list::iterator it = m_time_events_list.begin();
		while(it!=m_time_events_list.end())
		{
			timer_item& itm			= *it;
			if( itm.m_time<=time_current )
			{
				game_event ev(itm.m_event);
				m_owner->get_event_manager()->emit_event ( ev, m_controlled_object );
			}else
				break;
			++it;
		}
	}

	m_prev_time						= time_current;
}

object_controlled::object_controlled( )
:m_current_behaviour(NULL)
{}	

void object_controlled::set_behaviour( object_behaviour* b )
{ 
	m_current_behaviour		= b; 
}

void object_controlled::detach_current_behaviour( )
{ 
	m_current_behaviour=NULL; 
}

object_behaviour* create_behaviour( object_scene_job* owner, configs::binary_config_value const& t, pcstr name )
{
	object_behaviour* result	= NULL;
	pcstr type					= t["behaviour_type"];

	if(strings::equal(type, "camera_track"))
	{
		result = NEW(camera_follow_by_track_behaviour)( owner, t, name );
	}else
	if(strings::equal(type, "camera_free_fly"))
	{
		result = NEW(camera_free_fly_behaviour)( owner, t, name );
	}else
	if(strings::equal(type, "active_camera"))
	{
		result = NEW(camera_director_behaviour)( owner, t, name );
	}else
	if(strings::equal(type, "event_generator"))
	{
		result = NEW(event_generator_behaviour)( owner, t, name );
	}else
	if(strings::equal(type, "light_static"))
	{
		result = NEW(light_static_behaviour)( owner, t, name );
	}else
	if(strings::equal(type, "light_anim"))
	{
		result = NEW(light_anim_behaviour)( owner, t, name );
	}else
	if(strings::equal(type, "proximity_trigger_default"))
	{
		result = NEW(proximity_trigger_behaviour)( owner, t, name );
	}else
	if(strings::equal(type, "values_storage_default"))
	{
		result = NEW(values_storage_behaviour)( owner, t, name );
	}else
	if(strings::equal(type, "composite_default"))
	{
		result = NEW(composite_object_behaviour)( owner, t, name );
	}else
	if(strings::equal(type, "npc_play_animation"))
	{
		result = NEW(npc_play_animation_behaviour)( owner, t, name );
	}else	
	if(strings::equal(type, "npc_reach_location"))
	{
		result = NEW(npc_reach_position_behaviour)( owner, t, name );
	}else	
	if(strings::equal(type, "npc_patrol"))
	{
		result = NEW(npc_empty_switcher_behaviour)( owner, t, name );
	}else	
	{
		UNREACHABLE_CODE();
	}
	

	return result;
}
}