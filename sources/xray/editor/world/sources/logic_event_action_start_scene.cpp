////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "logic_scene_event_property_holder.h"
#include "object_job.h"
#include "logic_event_action_start_scene.h"
#include "object_scene.h"
#include "project_items.h"
#include "tool_scripting.h"
#include "level_editor.h"
#include "project.h"


//using namespace xray::editor::wpf_controls;

namespace xray {
namespace editor {

	
void logic_event_action_start_scene::save( configs::lua_config_value config )
{
	config["action_type"] = unmanaged_string( action_type ).c_str();
	config["scene"] = unmanaged_string( m_scene_start_to->owner_project_item->get_full_name( ) ).c_str();
}

void logic_event_action_start_scene::load( configs::lua_config_value const& config )
{
	action_type = gcnew System::String( config["action_type"] );
	System::String^ loaded_scene_name = gcnew System::String(config["scene"]);
	m_parent_holder->parent_scene->tool->get_level_editor()->get_project()->query_project_item( loaded_scene_name, gcnew queried_object_loaded( this, &logic_event_action_start_scene::on_loaded_scene_start_to ));	
}

void logic_event_action_start_scene::on_loaded_scene_start_to( project_item_base^ scene  )
{
	m_scene_start_to = safe_cast<object_scene^>(scene->get_object());
}

} // namespace editor
} // namespace xray