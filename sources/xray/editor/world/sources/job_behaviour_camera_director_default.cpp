////////////////////////////////////////////////////////////////////////////
//	Created		: 23.05.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "job_behaviour_camera_director_default.h"
#include "object_job.h"
#include "object_scene.h"
#include "tool_scripting.h"
#include "level_editor.h"
#include "project.h"

using namespace xray::editor::wpf_controls;

namespace xray {
namespace editor {

job_behaviour_camera_director_default::job_behaviour_camera_director_default( object_job^ parent_job ):job_behaviour_base(parent_job)
{
	transition				= "immediate";
}

void job_behaviour_camera_director_default::load			( configs::lua_config_value const& config )
{
	job_behaviour_base::load(config);

	System::String^ active_camera = gcnew System::String(config["active_camera"]);
	if ( active_camera != "")
		parent_job->tool->get_level_editor()->get_project()->query_project_item( active_camera, gcnew xray::editor::queried_object_loaded(this, &job_behaviour_camera_director_default::selected_camera_loaded) );		
	
	if (config.value_exists("transition"))
		transition = gcnew System::String(config["transition"]);
}
void job_behaviour_camera_director_default::save			( configs::lua_config_value& config )
{
	job_behaviour_base::save(config);

	config["active_camera"]	= unmanaged_string(active_camera).c_str();	
	config["transition"]	= unmanaged_string(transition).c_str();	
}

void job_behaviour_camera_director_default::get_properties	( wpf_property_container^ to_container )
{
	property_descriptor^ prop_descriptor = gcnew property_descriptor( this, "active_camera" );	
	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ) );	
	prop_descriptor->dynamic_attributes->add( gcnew property_editors::attributes::external_editor_attribute(gcnew property_editors::attributes::external_editor_event_handler( this, &job_behaviour_camera_director_default::on_select_camera), false) );
	to_container->properties->add( prop_descriptor );

	prop_descriptor = gcnew property_descriptor( this, "transition" );	
	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ) );
	prop_descriptor->dynamic_attributes->add( gcnew property_editors::attributes::combo_box_items_attribute( gcnew cli::array<String^>(3){"immediate","fade","mix"} ) );
	to_container->properties->add( prop_descriptor );



	job_behaviour_base::get_properties( to_container );
}

void job_behaviour_camera_director_default::on_select_camera	( wpf_controls::property_editors::property^ , Object^ )
{
	System::String^ selected;
	bool result = editor_base::resource_chooser::choose( "project_item", nullptr, nullptr, selected, true );
	if ( result )
	{
		m_active_camera = parent_job->tool->get_level_editor()->get_project()->find_by_path(selected, true);
	}

}

System::Collections::Generic::List<System::String^>^  job_behaviour_camera_director_default::get_available_events_list( )
{
	Collections::Generic::List<System::String^>^ ret_lst = job_behaviour_base::get_available_events_list();

	return ret_lst;
}


void job_behaviour_camera_director_default::selected_camera_loaded	( project_item_base^ track )
{
	m_active_camera = track;
}

} // namespace editor
} // namespace xray