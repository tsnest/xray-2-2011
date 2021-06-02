////////////////////////////////////////////////////////////////////////////
//	Created		: 05.05.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_MANAGER_H_INCLUDED
#define EVENT_MANAGER_H_INCLUDED

#include "logic_fwd_defines.h"

namespace stalker2 {

class event_manager {
public:
				event_manager				( );
	void		load						( configs::binary_config_value data );
	void		emit_event					( game_event const& e, event_propagator const* filter );
	
	void		subscribe_event_handler		(	game_event const& e, 
												event_propagator const* filter, 
												logic_action_ptr& action_to_execute, 
												pcstr description, 
												bool registered_event  );

	void		unsubscribe_event_handler	( logic_action* action );
	void		clear						( );
	void		dump_event_manager			( );
private:
	struct event_callback_item{
		logic_action_ptr			m_callback;
		event_propagator const*		m_filter;
		string256					m_description;
		bool operator == ( logic_action* action ) const {return m_callback==action;}
		bool operator < ( event_callback_item const& other) const;
	};
	typedef vector<event_callback_item>		handler_list;
	typedef map<game_event, handler_list>	events_registry_type;

	events_registry_type					m_events_registry;
}; // class event_manager

//extern event_manager* g_tmp_event_manager;

} // namespace stalker2

#endif // #ifndef EVENT_MANAGER_H_INCLUDED