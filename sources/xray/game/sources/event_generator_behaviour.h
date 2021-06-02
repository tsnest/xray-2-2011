////////////////////////////////////////////////////////////////////////////
//	Created		: 31.05.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_GENERATOR_BEHAVIOUR_H_INCLUDED
#define EVENT_GENERATOR_BEHAVIOUR_H_INCLUDED

#include "object_behaviour.h"

namespace stalker2 {

class event_generator_behaviour  : public object_behaviour
{
	typedef object_behaviour super;
public:
						event_generator_behaviour	( object_scene_job* owner, configs::binary_config_value const& data, pcstr name );
	virtual void		attach_to_object			( object_controlled* o );
	virtual void		detach_from_object			( object_controlled* o );
	virtual void		tick						( );

private:
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
	object_controlled*			m_object;
	float						m_length;
	bool						m_cycled;
}; // class event_generator_behaviour

} // namespace stalker2

#endif // #ifndef EVENT_GENERATOR_BEHAVIOUR_H_INCLUDED