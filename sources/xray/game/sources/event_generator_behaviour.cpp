////////////////////////////////////////////////////////////////////////////
//	Created		: 31.05.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "event_generator_behaviour.h"
#include "object_scene.h"

namespace stalker2 {

event_generator_behaviour::event_generator_behaviour( object_scene_job* owner, 
													 configs::binary_config_value const& data, 
													 pcstr name )
:super			( owner, data, name ),
m_prev_time		( 0.0f ),
m_object		( NULL )
{
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

void event_generator_behaviour::attach_to_object( object_controlled* o )
{
	m_object				= o;
	super::attach_to_object	( o );
	m_timer.start			( );
	m_prev_time				= 0.0f;
}

void event_generator_behaviour::detach_from_object( object_controlled* o )
{
	super::detach_from_object( o );
	m_object				= NULL;
}

void event_generator_behaviour::tick( )
{	
	super::tick( );
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
			m_owner->get_event_manager()->emit_event ( ev, m_object );
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
				m_owner->get_event_manager()->emit_event ( ev, m_object );
			}else
				break;
			++it;
		}
	}

	m_prev_time						= time_current;
}

} // namespace stalker2
