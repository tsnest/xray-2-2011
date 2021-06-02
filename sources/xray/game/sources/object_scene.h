////////////////////////////////////////////////////////////////////////////
//	Created		: 19.05.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_SCENE_H_INCLUDED
#define OBJECT_SCENE_H_INCLUDED

#include "object.h"
#include "object_behaviour.h"
#include "event_manager.h"

namespace stalker2 {

class object_scene_job;
class object_behaviour;
class object_scene;

typedef	intrusive_ptr<
	object_scene,
	resources::unmanaged_intrusive_base,
	threading::single_threading_policy
> object_scene_ptr;


class object_scene : public game_object_
{
	typedef game_object_ super;
public:
					object_scene			( game_world& w );
	virtual			~object_scene			( );
	virtual void	load					( configs::binary_config_value const& t );

	inline event_manager* get_event_manager	( )	{ return &m_event_manager; }

			void	start					( );
			void	stop					( bool unloading );
			void	tick					( );

private:
	typedef vector<object_scene_job*>	jobs_list;
	jobs_list							m_jobs;

	scenes_list							m_sub_scenes;
	logic_action_list					m_actions;

	configs::binary_config_value		m_data;
	pcstr								m_name;
	event_manager						m_event_manager;
}; // class object_scene

class object_scene_job : private boost::noncopyable 
{
public:
					object_scene_job		( object_scene* owner, configs::binary_config_value const& data );
					~object_scene_job		( );

object_behaviour*	get_behaviour			( pcstr name ) const;
			void	start					( );
			void	stop					( );
			void	tick					( );
	game_world&		get_game_world			( );
			void	switch_to_behaviour		( object_behaviour* b );
	inline	pcstr	name					( ) const { return m_name; }	
	event_manager*	get_event_manager		( ) const;
	inline	game_object_ptr_	job_resource( ) const { return m_job_resource; }
private:
	object_scene*						m_owner;
	
	typedef vector<object_behaviour*>	behaviours_list;
	behaviours_list						m_behaviours;
	
	configs::binary_config_value const&	m_data;
	object_behaviour*					m_active_behaviour;
	game_object_ptr_					m_job_resource;
	pcstr								m_name;
}; // class object_scene_job


class object_timer :public game_object_, public object_controlled
{
	typedef game_object_ super;
public:
					object_timer			( game_world& w ):super(w){};
	virtual			~object_timer			( ){};
}; // class object_timer

} // namespace stalker2

#endif // #ifndef OBJECT_SCENE_H_INCLUDED
