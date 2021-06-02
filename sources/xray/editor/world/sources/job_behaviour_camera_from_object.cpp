////////////////////////////////////////////////////////////////////////////
//	Created		: 11.05.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "job_behaviour_camera_from_object.h"
#include "logic_event_properties_container.h"
#include "object_job.h"
#include "object_scene.h"
#include "tool_scripting.h"
#include "level_editor.h"
#include "project.h"

using namespace xray::editor::wpf_controls;

namespace xray {
namespace editor {

job_behaviour_camera_from_object::job_behaviour_camera_from_object( object_job^ parent_job )
:job_behaviour_base(parent_job)
{
}

void job_behaviour_camera_from_object::load			( configs::lua_config_value const& config )
{
	job_behaviour_base::load(config);

	fov_x		= (float)config["fov_x"];
	fov_y		= (float)config["fov_y"];
	far_plane	= (float)config["far_plane"];
	near_plane	= (float)config["near_plane"];

	m_current_object	= nullptr;
//	m_current_object = parent_job->tool->get_level_editor()->get_project()->find_by_path( gcnew System::String(config["from_object"] ) );
}
void job_behaviour_camera_from_object::save			( configs::lua_config_value& config )
{
	job_behaviour_base::save(config);

	config["fov_x"]			= fov_x;
	config["fov_y"]			= fov_y;
	config["far_plane"]		= far_plane;
	config["near_plane"]	= near_plane;
	config["from_object"]	= unmanaged_string(from_object).c_str();	
}

void job_behaviour_camera_from_object::get_properties	( wpf_property_container^ to_container )
{
	property_descriptor^ prop_descriptor = gcnew property_descriptor( this, "fov_x" );
	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));	
	to_container->properties->add(prop_descriptor);

	prop_descriptor = gcnew property_descriptor( this, "fov_y" );
	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));	
	to_container->properties->add(prop_descriptor);

	prop_descriptor = gcnew property_descriptor( this, "far_plane" );
	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));	
	to_container->properties->add(prop_descriptor);

	prop_descriptor = gcnew property_descriptor( this, "near_plane" );
	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));	
	to_container->properties->add(prop_descriptor);

	prop_descriptor = gcnew property_descriptor( this, "from_object" );	
	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ) );	
	prop_descriptor->dynamic_attributes->add( gcnew property_editors::attributes::external_editor_attribute(gcnew property_editors::attributes::external_editor_event_handler( this, &job_behaviour_camera_from_object::on_select_object), false) );
	to_container->properties->add( prop_descriptor );

	job_behaviour_base::get_properties( to_container );
}

void job_behaviour_camera_from_object::on_select_object	( wpf_controls::property_editors::property^ , Object^ )
{
	System::String^ selected;
	bool result = editor_base::resource_chooser::choose( "project_item", nullptr, nullptr, selected, true );
	if ( result )
	{
		m_current_object = parent_job->tool->get_level_editor()->get_project()->find_by_path(selected, true);
	}

}

System::Collections::Generic::List<System::String^>^  job_behaviour_camera_from_object::get_available_events_list( )
{
	return job_behaviour_base::get_available_events_list( );
}




} // namespace editor
} // namespace xray
