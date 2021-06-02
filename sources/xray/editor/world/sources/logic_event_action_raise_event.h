////////////////////////////////////////////////////////////////////////////
//	Created		: 13.05.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef LOGIC_EVENT_ACTION_RAISE_EVENT_H_INCLUDED
#define LOGIC_EVENT_ACTION_RAISE_EVENT_H_INCLUDED

#include "logic_event_action_base.h"

namespace xray {
namespace editor {

ref class logic_event;
ref class object_scene;
ref class object_logic;

public ref class logic_event_action_raise_event: logic_event_action_base {
public:
	logic_event_action_raise_event(  object_logic^ parent, logic_event^	raised_event )
	{
		action_type				= "raise_event";
		m_parent_logic_object	= parent;
		m_raised_event			= raised_event;
	}
	logic_event_action_raise_event( object_logic^ parent )
	{
		action_type				= "raise_event";
		m_parent_logic_object	= parent;
	}
	virtual property System::String^	action_type;
	property System::String^			raised_event_name{
		System::String^					get();
	}
	property logic_event^				raised_event{
		logic_event^					get(){ return m_raised_event; }
	}
	virtual void						save( configs::lua_config_value config );
	virtual void						load( configs::lua_config_value const& config );

	void								on_job_parent_changed( object_scene^ scene );

private:

	logic_event^					m_raised_event;
	object_logic^					m_parent_logic_object;

}; // class logic_event_action_switch

} // namespace editor
} // namespace xray

#endif // #ifndef LOGIC_EVENT_ACTION_RAISE_EVENT_H_INCLUDED