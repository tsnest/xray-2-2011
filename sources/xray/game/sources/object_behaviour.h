////////////////////////////////////////////////////////////////////////////
//	Created		: 10.05.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_BEHAVIOUR_H_INCLUDED
#define OBJECT_BEHAVIOUR_H_INCLUDED

#include "logic_fwd_defines.h"

namespace stalker2 {

class object_controlled;
class object_scene_job;

class object_behaviour : private boost::noncopyable 
{

public:
	
						object_behaviour		( object_scene_job* owner, configs::binary_config_value const& data, pcstr name );
	virtual				~object_behaviour		( ) {}
	virtual void		attach_to_object		( object_controlled* o );
	virtual void		detach_from_object		( object_controlled* o );
	virtual void		start					( );
	virtual void		tick					( );
	fixed_string<128>	full_name				( ) const;
	inline	object_scene_job& get_parent_job	( ) const { return *m_owner; }
	inline	pcstr		name					( ) const {return m_name; }

protected:
	object_scene_job*					m_owner;
	object_controlled*					m_controlled_object;
	configs::binary_config_value const&	m_data;
	pcstr								m_name;
	logic_action_list					m_actions;
private:
	float						m_length;
	bool						m_cycled;
	struct timer_item{
		inline bool operator < ( timer_item const& other ) const { return m_time < other.m_time; }
		inline bool operator < ( float tm ) const { return m_time < tm; }
		float			m_time;
		shared_string	m_event;
	};
	typedef vector<timer_item>	events_list;
	events_list					m_time_events_list;
	timing::timer				m_timer;
	float						m_prev_time;
	bool						m_timing_enabled;
}; // class object_behaviour

class object_controlled : public event_propagator
{

public:
					object_controlled			( );
	virtual			~object_controlled			( ) { }
	virtual void	set_behaviour				( object_behaviour* b );
	virtual void	detach_current_behaviour	( );
	object_behaviour* get_current_behaviour		( ) const						{ return m_current_behaviour; }

protected:
	object_behaviour*	m_current_behaviour;
};

object_behaviour* create_behaviour( object_scene_job* owner, configs::binary_config_value const& t, pcstr name );

} // namespace xray

#endif // #ifndef OBJECT_BEHAVIOUR_H_INCLUDED