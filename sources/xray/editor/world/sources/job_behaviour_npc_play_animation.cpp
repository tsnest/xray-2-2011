////////////////////////////////////////////////////////////////////////////
//	Created		: 21.10.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "job_behaviour_npc_play_animation.h"
#include "object_job.h"
#include "project_items.h"
#include "select_library_class_form.h"
#include "level_editor.h"

using namespace xray::editor::wpf_controls;

namespace xray {
namespace editor {

job_behaviour_npc_play_animation::job_behaviour_npc_play_animation( object_job^ parent_job )
	:job_behaviour_base(parent_job)
{
	selected_animation			= "";
	m_selected_animation_type	= "";
	m_animation_behaviour_config = NEW(configs::lua_config_ptr)();
	
	query_result_delegate* rq = XRAY_NEW_IMPL(g_allocator, query_result_delegate)(gcnew query_result_delegate::Delegate(this, &job_behaviour_npc_play_animation::on_behaviour_config_loaded), g_allocator);
	xray::resources::query_resource(
		"resources/brain_units/behaviours/play_animation.behaviour",
		xray::resources::lua_config_class,
		boost::bind(&query_result_delegate::callback, rq, _1),
		g_allocator
		);
}

void job_behaviour_npc_play_animation::load			( configs::lua_config_value const& config )
{
	job_behaviour_base::load(config);

	selected_animation			= gcnew System::String( config["selected_animation"] );
	m_selected_animation_type	= gcnew System::String( config["selected_animation_type"] );
}

void job_behaviour_npc_play_animation::save( configs::lua_config_value& config )
{
	job_behaviour_base::save(config);

	config["selected_animation"]		= unmanaged_string( selected_animation ).c_str( );
	config["selected_animation_type"]	= unmanaged_string( m_selected_animation_type ).c_str( );
	config["behaviour_config"].assign_lua_value( ( *m_animation_behaviour_config )->get_root().copy( ) );
	
	configs::lua_config_value goal_config = config["behaviour_config"]["goals"][0]["filter_sets"][0]["parameter0_filter"][0]["filenames"][0];
	
	goal_config["name"] = unmanaged_string( selected_animation ).c_str( );

	if ( m_selected_animation_type == "single" )
		goal_config["type"] = resources::single_animation_class;
	else if ( m_selected_animation_type == "collections" )
		goal_config["type"] = resources::animation_collection_class;
	else
		UNREACHABLE_CODE();

}

void job_behaviour_npc_play_animation::get_properties	( wpf_property_container^ to_container )
{
 	wpf_controls::property_descriptor^ prop_descriptor = gcnew wpf_controls::property_descriptor( this, "selected_animation" );
 	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));	
	prop_descriptor->dynamic_attributes->add( gcnew property_editors::attributes::external_editor_attribute(gcnew property_editors::attributes::external_editor_event_handler( this, &job_behaviour_npc_play_animation::on_select_animation ), false) );
 	to_container->properties->add(prop_descriptor);
 
	job_behaviour_base::get_properties( to_container );
}

void job_behaviour_npc_play_animation::on_select_animation	( wpf_controls::property_editors::property^ , Object^ )
{

	System::String^ selected = nullptr;
	if ( editor_base::resource_chooser::choose( "animations_list", m_selected_animation_type + "/" + selected_animation, "", selected, true ) )
	{
		int index = selected->IndexOf("/");
		selected_animation = selected->Substring( index + 1, selected->Length - index - 1 );
		m_selected_animation_type = selected->Substring(0, index);
 	}
}

void job_behaviour_npc_play_animation::on_behaviour_config_loaded( resources::queries_result& data )
{
	R_ASSERT(data.is_successful());
	(*m_animation_behaviour_config) = static_cast_checked<configs::lua_config*>(data[0].get_unmanaged_resource().c_ptr());

}

System::Collections::Generic::List<System::String^>^  job_behaviour_npc_play_animation::get_available_events_list( )
{
	System::Collections::Generic::List<System::String^>^ ret_lst = job_behaviour_base::get_available_events_list();

	ret_lst->Insert( 0, "animation_end" );

	return ret_lst;
}

} // namespace editor
} // namespace xray
