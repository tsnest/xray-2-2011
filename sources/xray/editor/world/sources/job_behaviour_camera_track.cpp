////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "job_behaviour_camera_track.h"
#include "object_job.h"
#include "object_scene.h"
#include "tool_scripting.h"
#include "level_editor.h"
#include "project.h"

using namespace xray::editor::wpf_controls;

namespace xray {
namespace editor {

job_behaviour_camera_track::job_behaviour_camera_track( object_job^ parent_job )
:job_behaviour_base(parent_job)
{
	cyclic					= false;
}

void job_behaviour_camera_track::load			( configs::lua_config_value const& config )
{
	job_behaviour_base::load(config);

	fov_x		=	config["fov_x"];
	fov_y		=	config["fov_y"];	
	far_plane	= 	config["far_plane"];
	near_plane	= 	config["near_plane"];

	if (config.value_exists("cyclic"))
		cyclic = (bool)config["cyclic"];

	System::String^ selected_track = gcnew System::String(config["camera_track"]);
	if ( selected_track != "")
		parent_job->tool->get_level_editor()->get_project()->query_project_item( selected_track, gcnew xray::editor::queried_object_loaded(this, &job_behaviour_camera_track::selected_track_loaded) );		
}
void job_behaviour_camera_track::save			( configs::lua_config_value& config )
{
	job_behaviour_base::save(config);

	config["fov_x"]			= fov_x;
	config["fov_y"]			= fov_y;
	config["far_plane"]		= far_plane;
	config["near_plane"]	= near_plane;
	config["cyclic"]		= cyclic;
	config["camera_track"]	= unmanaged_string(camera_track).c_str();	
}

void job_behaviour_camera_track::get_properties	( wpf_property_container^ to_container )
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

	prop_descriptor = gcnew property_descriptor( this, "cyclic" );
	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));	
	to_container->properties->add(prop_descriptor);

	prop_descriptor = gcnew property_descriptor( this, "camera_track" );	
	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ) );	
	prop_descriptor->dynamic_attributes->add( gcnew property_editors::attributes::external_editor_attribute(gcnew property_editors::attributes::external_editor_event_handler( this, &job_behaviour_camera_track::on_select_camera_track), false) );
	to_container->properties->add( prop_descriptor );	

	job_behaviour_base::get_properties( to_container );
}

void job_behaviour_camera_track::on_select_camera_track	( wpf_controls::property_editors::property^ , Object^ )
{
	System::String^ selected;
    bool result = editor_base::resource_chooser::choose( "project_item", nullptr, nullptr, selected, true );
	if ( result )
	{
		m_current_camera_track = parent_job->tool->get_level_editor()->get_project()->find_by_path(selected, true);
	}
	
}

System::Collections::Generic::List<System::String^>^  job_behaviour_camera_track::get_available_events_list( )
{
	System::Collections::Generic::List<System::String^>^ ret_lst = job_behaviour_base::get_available_events_list();

	ret_lst->Insert( 0, "animation_end" );
	
	return ret_lst;
}


void job_behaviour_camera_track::selected_track_loaded	( project_item_base^ track )
{
	m_current_camera_track = track;
}

} // namespace editor
} // namespace xray
