////////////////////////////////////////////////////////////////////////////
//	Created		: 13.05.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef LOGIC_EVENT_ACTION_SWITCH_H_INCLUDED
#define LOGIC_EVENT_ACTION_SWITCH_H_INCLUDED

#include "logic_event_action_base.h"

namespace xray {
namespace editor {

ref class job_behaviour_base;
ref class logic_event_property_holder;

ref class logic_event_action_switch: logic_event_action_base {
public:
	logic_event_action_switch( logic_event_property_holder^ parent_holder, job_behaviour_base^	behaviour_switch_to )
	{
		action_type				= "switch_to_behaviour";
		m_parent_holder			= parent_holder;
		m_behaviour_switch_to	= behaviour_switch_to;
	}
	logic_event_action_switch( logic_event_property_holder^ parent_holder )
	{
		action_type				= "switch_to_behaviour";	
		m_parent_holder			= parent_holder;
	}

	virtual property System::String^	action_type;	
	virtual void						save( configs::lua_config_value config );
	virtual void						load( configs::lua_config_value const& config );

	void								on_loaded_behaviour_switch_to( job_behaviour_base^ behaviour );

private:

	job_behaviour_base^					m_behaviour_switch_to;	
	logic_event_property_holder^		m_parent_holder;	
public:
	property job_behaviour_base^		behaviour_switch_to 
	{
		job_behaviour_base^				get( ){ return m_behaviour_switch_to; }
	}

}; // class logic_event_action_switch

} // namespace editor
} // namespace xray

#endif // #ifndef LOGIC_EVENT_ACTION_SWITCH_H_INCLUDED