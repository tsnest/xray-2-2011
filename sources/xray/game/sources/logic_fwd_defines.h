////////////////////////////////////////////////////////////////////////////
//	Created		: 17.06.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef LOGIC_FWD_DEFINES_H_INCLUDED
#define LOGIC_FWD_DEFINES_H_INCLUDED

namespace stalker2 {

class object_behaviour;
class object_controlled;
class object_scene_job;
class event_manager;
class object_controlled;
class object_scene;

#define start_scene_event_name	"#StartScene"

class game_event
{
public:
	inline			game_event			( pcstr id ) : m_event_id( id ) { }
	inline			game_event			( shared_string const& id ) : m_event_id( id ) { }
	inline	bool	operator ==			( game_event const& other ) const	{ return m_event_id == other.m_event_id; }
	inline	bool	operator <			( game_event const& other ) const	{ return m_event_id <  other.m_event_id; }

	shared_string	m_event_id;
};

class event_propagator
{
};

class logic_action : private boost::noncopyable
{
public:
					logic_action		( event_manager* event_manager );
	virtual			~logic_action		( );
	virtual void	execute				( game_event const& e ) = 0;

	inline	u32		priority			( ) const { return m_priority; }

	static	void	destroy				( logic_action* action_to_destroy );
public:
	u32				m_reference_count;

protected:
	u32				m_priority;
	event_manager*	m_event_manager;
}; // class logic_action

typedef	intrusive_ptr<
	logic_action,
	logic_action,
	threading::single_threading_policy
> logic_action_ptr;

typedef vector< logic_action_ptr >		logic_action_list;


typedef	intrusive_ptr<
	object_scene,
	resources::unmanaged_intrusive_base,
	threading::single_threading_policy
> object_scene_ptr;

typedef vector<object_scene_ptr>	scenes_list;



} // namespace stalker2

#endif // #ifndef LOGIC_FWD_DEFINES_H_INCLUDED