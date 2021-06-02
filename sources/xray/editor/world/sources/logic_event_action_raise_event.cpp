////////////////////////////////////////////////////////////////////////////
//	Created		: 13.05.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "logic_event_action_raise_event.h"
#include "logic_event.h"
#include "logic_event_property_holder.h"
#include "job_behaviour_base.h"
#include "object_logic.h"
#include "object_scene.h"
#include "project_items.h"

namespace xray {
namespace editor {


void logic_event_action_raise_event::save( configs::lua_config_value config )
{
	config["action_type"] = unmanaged_string(action_type).c_str();
	config["event_parent_scene"] =  unmanaged_string(m_parent_logic_object->parent_scene->owner_project_item->get_full_name()).c_str();
	config["raised_event"] = unmanaged_string(m_raised_event->logic_event_view->event_name).c_str();
}

void logic_event_action_raise_event::load( configs::lua_config_value const& config )
{
	action_type = gcnew System::String( config["action_type"] );
	System::String^ loaded_event_name = gcnew System::String( config["raised_event"] );
	m_raised_event = m_parent_logic_object->parent_scene->get_event_by_name( loaded_event_name );
}

System::String^					logic_event_action_raise_event::raised_event_name::get()
{ 
	if (m_raised_event == nullptr)
		return "";
	return m_raised_event->logic_event_view->event_name; 
}

void logic_event_action_raise_event::on_job_parent_changed( object_scene^ /*scene*/ )
{
	
}



} // namespace editor
} // namespace xray
