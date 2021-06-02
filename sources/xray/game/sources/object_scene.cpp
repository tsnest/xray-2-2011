////////////////////////////////////////////////////////////////////////////
//	Created		: 19.05.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_scene.h"
#include "object_behaviour.h"
#include "game_world.h"
#include "logic_action.h"
#include "object_behaviour.h"
#include "event_manager.h"


namespace stalker2 {

object_scene::object_scene( game_world& w )
:super( w )
{}

object_scene::~object_scene( )
{
	jobs_list::iterator it		= m_jobs.begin();
	jobs_list::iterator it_e	= m_jobs.end();
	for(; it!=it_e; ++it)
	{
		object_scene_job* job	= *it;
		DELETE					( job );
	}

	m_jobs.clear				( );
}

void object_scene::load( configs::binary_config_value const& t )
{
	super::load		( t );
	m_data			= t;
	m_name			= m_data["name"];
	m_event_manager.load( m_data["events"] );
}

void object_scene::start( )
{
	get_game_world().on_scene_start						( this );

	configs::binary_config_value t_jobs					= m_data["jobs"];
	configs::binary_config_value::const_iterator it		= t_jobs.begin();
	configs::binary_config_value::const_iterator it_e	= t_jobs.end();
	for( ;it!=it_e; ++it)
	{
		object_scene_job* job	= NEW(object_scene_job)( this, *it );
		m_jobs.push_back	( job );
	}

	configs::binary_config_value t_scenes	= m_data["sub_scenes"];
	it										= t_scenes.begin();
	it_e									= t_scenes.end();
	for( ;it!=it_e; ++it)
	{
		pcstr sub_scene_name	= *it;
		game_object_ptr_ s		= m_game_world.get_object_by_name(sub_scene_name);
		object_scene_ptr scene	= static_cast_resource_ptr<object_scene_ptr>(s);
		m_sub_scenes.push_back	( scene );
	}

	jobs_list::iterator j_it			= m_jobs.begin();
	jobs_list::const_iterator j_it_e	= m_jobs.end();
	for( ;j_it!=j_it_e; ++j_it)
	{
		object_scene_job* job	= *j_it;
		job->start				( )	;
	}
	j_it			= m_jobs.begin();
	for( ;j_it!=j_it_e; ++j_it)
	{
		object_scene_job* job	= *j_it;
		job->job_resource().c_ptr()->on_scene_start();
	}


// load scene event handlers 
	if(m_data.value_exists("event_handlers"))
	{
		configs::binary_config_value t_event_handlers = m_data["event_handlers"];
		configs::binary_config_value::const_iterator it = t_event_handlers.begin();
		configs::binary_config_value::const_iterator it_e = t_event_handlers.end();

		for( ;it!=it_e; ++it)
		{
			pcstr event_name	= (*it)["event_name"];
			configs::binary_config_value::const_iterator action_it = (*it)["actions"].begin();
			configs::binary_config_value::const_iterator action_it_e = (*it)["actions"].end();

			game_event e( event_name );
			
			fixed_string<128> desc;

			for( ;action_it!=action_it_e; ++action_it )
			{
				logic_action_ptr a				= NULL;
				pcstr action_type			= (*action_it)["action_type"];

				if(strings::equal(action_type, "start_scene" ))
				{
					pcstr scene_to_start_name	= (*action_it)["scene"];
					game_object_ptr_ s			= m_game_world.get_object_by_name( scene_to_start_name );
					object_scene_ptr scene_to_start	= static_cast_resource_ptr<object_scene_ptr>(s);

					desc.assignf("start scene handler: parent=%s scene_to_start=%s event=%s", m_name, scene_to_start->m_name, event_name );

					a						= NEW(action_start_scene)( get_event_manager(), scene_to_start );					
				}
				else if (strings::equal(action_type, "raise_event" ))
				{
					pcstr scene_to_start_name	= (*action_it)["event_parent_scene"];
					game_object_ptr_ s			= m_game_world.get_object_by_name( scene_to_start_name );
					object_scene_ptr event_scene= static_cast_resource_ptr<object_scene_ptr>(s);
					pcstr raise_event_name		= (*action_it)["raised_event"];
					game_event e( raise_event_name );

					desc.assignf("start scene handler: parent=%s raise_event_name=%s event=%s", m_name, raise_event_name, event_name );

					a						= NEW(action_emit_event)( event_scene->get_event_manager(), e );
				}
				else
				{
					UNREACHABLE_CODE();
				}

				if(strings::equal(event_name, start_scene_event_name) )
				{
					a->execute	( e );
					a			= NULL;
				}else
				{					
					get_event_manager()->subscribe_event_handler	( e, NULL, a, desc.c_str(), true );
					m_actions.push_back								( a );
				}

			}
		}
	}

}

void object_scene::stop( bool unloading )
{
	jobs_list::iterator j_it				= m_jobs.begin();
	jobs_list::const_iterator j_it_e		= m_jobs.end();
	for( ;j_it!=j_it_e; ++j_it)
	{
		object_scene_job* job				= *j_it;
		job->stop							( )	;
	}

	logic_action_list::const_iterator it	= m_actions.begin();
	logic_action_list::const_iterator it_e	= m_actions.end();

	for( ;it!=it_e; ++it)
	{
		logic_action_ptr a					= *it;
		get_event_manager()->unsubscribe_event_handler	( a.c_ptr() );
	}
	m_actions.clear							( );

	if(!unloading)
		get_game_world().on_scene_stop		( this );
}

void object_scene::tick( )
{
	jobs_list::iterator it			= m_jobs.begin();
	jobs_list::const_iterator it_e	= m_jobs.end();
	for ( ; it != it_e; ++it )
	{
		object_scene_job* job		= *it;
		job->tick					( )	;
	}
}

object_scene_job::object_scene_job	( object_scene* owner, configs::binary_config_value const& data ) :
	m_owner							( owner ),
	m_data							( data ),
	m_active_behaviour				( NULL ),
	m_name							( m_data["name"] )
{
	//LOG_INFO( "object_scene_job construct [%x]", this );
}

object_scene_job::~object_scene_job	( )
{
	//LOG_INFO( "object_scene_job destruct [%x]", this );
	while ( !m_behaviours.empty() )
	{
		behaviours_list::iterator first	= m_behaviours.begin();
		object_behaviour* beh			= *first;
		m_behaviours.erase				( first );
		DELETE							( beh );
	}
}

void object_scene_job::start( )
{
	configs::binary_config_value t_behaviours			= m_data["behaviours"];
	configs::binary_config_value::const_iterator it		= t_behaviours.begin();
	configs::binary_config_value::const_iterator it_e	= t_behaviours.end();
	for( ;it!=it_e; ++it)
	{
		object_behaviour* beh			= create_behaviour( this, *it, it->key() );
		m_behaviours.push_back			( beh );
	}

	pcstr job_resource_name				= m_data["job_resource"];
	pcstr start_behaviour_name			= m_data["start_behaviour_name"];
	pcstr start_event_name				= m_data["start_event_name"];

	m_job_resource						= get_game_world().get_object_by_name(job_resource_name);
	object_controlled* o				= dynamic_cast<object_controlled*>(m_job_resource.c_ptr());

	// start initial behaviour
	object_behaviour* start_behaviour	= get_behaviour( start_behaviour_name );
	logic_action_ptr a		= NEW(action_set_object_behaviour)( get_event_manager(), o, start_behaviour, this );
	game_event e( start_event_name );

	if(strings::equal(start_event_name, start_scene_event_name) )
	{
		a->execute		( e );
		a				= NULL;
	}else
	{

		fixed_string<128> desc;
		desc.assignf("start behaviour: beh=%s event=%s", start_behaviour->name(), start_event_name );
		get_event_manager()->subscribe_event_handler	( e, NULL, a, desc.c_str(), true );
	}

}

void object_scene_job::stop( )
{
	if(m_active_behaviour)
	{
		// tmp
		object_controlled* o					= dynamic_cast<object_controlled*>(m_job_resource.c_ptr());

		m_active_behaviour->detach_from_object	( o );
		o->set_behaviour						( NULL );
	}
}

object_behaviour* object_scene_job::get_behaviour( pcstr name ) const
{
	behaviours_list::const_iterator it		= m_behaviours.begin();
	behaviours_list::const_iterator it_e	= m_behaviours.end();
	
	for( ;it!=it_e; ++it)
	{
		object_behaviour* b = *it;
		if(strings::equal( b->name(), name))
			return b;
	}
	R_ASSERT(false, "unknown behaviour");
	return NULL;
}

game_world& object_scene_job::get_game_world( )
{
	return m_owner->get_game_world( );
}

event_manager* object_scene_job::get_event_manager( ) const
{
	return m_owner->get_event_manager( );
}

void object_scene_job::tick( )
{
	if(m_active_behaviour)
		m_active_behaviour->tick( );
}

void object_scene_job::switch_to_behaviour( object_behaviour* b )
{
	m_active_behaviour = b;
}

}
