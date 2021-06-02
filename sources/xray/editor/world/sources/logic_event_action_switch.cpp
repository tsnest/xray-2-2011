////////////////////////////////////////////////////////////////////////////
//	Created		: 13.05.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "logic_event_property_holder.h"
#include "object_job.h"
#include "logic_event_action_switch.h"
#include "job_behaviour_base.h"

//using namespace xray::editor::wpf_controls;

namespace xray {
namespace editor {

	
void logic_event_action_switch::save( configs::lua_config_value config )
{
	config["action_type"] = unmanaged_string(action_type).c_str();
	config["switching_to_behaviour"] = unmanaged_string(m_behaviour_switch_to->behaviour_name).c_str();
}

void logic_event_action_switch::load( configs::lua_config_value const& config )
{
	action_type = gcnew System::String( config["action_type"] );
	System::String^ loaded_behaviour_name = gcnew System::String(config["switching_to_behaviour"]);
	m_parent_holder->parent_behaviour->parent_job->query_logic_behaviour( loaded_behaviour_name, gcnew queried_behaviour_loaded( this, &logic_event_action_switch::on_loaded_behaviour_switch_to ) );
}

void logic_event_action_switch::on_loaded_behaviour_switch_to( job_behaviour_base^ behaviour )
{
	//m_parent_holder->parent_behaviour->parent_job->logic_view->logic_hypergraph->create_link(m_parent_holder->own_property_descriptor, behaviour->hypergraph_node);
	m_behaviour_switch_to = behaviour;
}

} // namespace editor
} // namespace xray