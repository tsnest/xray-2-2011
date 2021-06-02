////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef LOGIC_EVENT_ACTION_START_SCENE_H_INCLUDED
#define LOGIC_EVENT_ACTION_START_SCENE_H_INCLUDED

#include "logic_event_action_base.h"

namespace xray {
namespace editor {

ref class job_behaviour_base;
ref class logic_scene_event_property_holder;

ref class logic_event_action_start_scene: logic_event_action_base {
public:
	logic_event_action_start_scene( logic_scene_event_property_holder^ parent_holder, object_scene^	scene_start_to )
	{
		action_type				= "start_scene";
		m_parent_holder			= parent_holder;
		m_scene_start_to		= scene_start_to;
	}
	logic_event_action_start_scene( logic_scene_event_property_holder^ parent_holder )
	{
		action_type				= "start_scene";	
		m_parent_holder			= parent_holder;
	}

	virtual property System::String^	action_type;	
	virtual void						save( configs::lua_config_value config );
	virtual void						load( configs::lua_config_value const& config );

	void								on_loaded_scene_start_to( project_item_base^ scene );

private:

	object_scene^						m_scene_start_to;
	logic_scene_event_property_holder^	m_parent_holder;	

public:
	property object_scene^				scene_start_to
	{
		object_scene^					get( ) { return m_scene_start_to; }
	}

}; // class logic_event_action_switch

} // namespace editor
} // namespace xray

#endif // #ifndef LOGIC_EVENT_ACTION_START_SCENE_H_INCLUDED