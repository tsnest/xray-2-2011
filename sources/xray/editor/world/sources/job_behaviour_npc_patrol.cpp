////////////////////////////////////////////////////////////////////////////
//	Created		: 15.11.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "job_behaviour_npc_patrol.h"
#include "object_job.h"
#include "project_items.h"
#include "select_library_class_form.h"
#include "level_editor.h"
#include "project.h"
#include "object_patrol_graph.h"

using namespace xray::editor::wpf_controls;

namespace xray {
namespace editor {

job_behaviour_npc_patrol::job_behaviour_npc_patrol( object_job^ parent_job )
	:job_behaviour_base(parent_job)
{
	selected_patrol			= "";
	m_animation_behaviour_config = NEW(configs::lua_config_ptr)();

	query_result_delegate* rq = XRAY_NEW_IMPL(g_allocator, query_result_delegate)(gcnew query_result_delegate::Delegate(this, &job_behaviour_npc_patrol::on_behaviour_config_loaded), g_allocator);
	xray::resources::query_resource(
		"resources/brain_units/behaviours/patrol.behaviour",
		xray::resources::lua_config_class,
		boost::bind(&query_result_delegate::callback, rq, _1),
		g_allocator
		);
}

void job_behaviour_npc_patrol::load			( configs::lua_config_value const& config )
{
	job_behaviour_base::load(config);

	selected_patrol			= gcnew System::String( config["selected_patrol"] );
	
}

void job_behaviour_npc_patrol::save( configs::lua_config_value& config )
{
	job_behaviour_base::save(config);

	config["selected_patrol"]		= unmanaged_string( selected_patrol ).c_str( );

	project_item_base^ patrol_item = parent_job->get_project()->find_by_path( selected_patrol, true );

	config["behaviour_config"].assign_lua_value( ( *m_animation_behaviour_config )->get_root().copy( ) );

	config["patrol_graph_guid"] = unmanaged_string( patrol_item->m_guid.ToString( ) ).c_str();	
}

void job_behaviour_npc_patrol::get_properties	( wpf_property_container^ to_container )
{
	wpf_controls::property_descriptor^ prop_descriptor = gcnew wpf_controls::property_descriptor( this, "selected_patrol" );
	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));	
	prop_descriptor->dynamic_attributes->add( gcnew property_editors::attributes::external_editor_attribute(gcnew property_editors::attributes::external_editor_event_handler( this, &job_behaviour_npc_patrol::on_select_patrol ), false) );
	to_container->properties->add(prop_descriptor);

	job_behaviour_base::get_properties( to_container );
}

void job_behaviour_npc_patrol::on_select_patrol	( wpf_controls::property_editors::property^ , Object^ )
{

 	System::String^ selected = nullptr;
 	if ( editor_base::resource_chooser::choose( "project_item", selected_patrol, "xray.editor.object_patrol_graph", selected, true ) )
 	{
 		selected_patrol = selected; 		
 	}
}

void job_behaviour_npc_patrol::on_behaviour_config_loaded( resources::queries_result& data )
{
	R_ASSERT(data.is_successful());
	(*m_animation_behaviour_config) = static_cast_checked<configs::lua_config*>(data[0].get_unmanaged_resource().c_ptr());

}

System::Collections::Generic::List<System::String^>^  job_behaviour_npc_patrol::get_available_events_list( )
{
	System::Collections::Generic::List<System::String^>^ ret_lst = job_behaviour_base::get_available_events_list();

	//ret_lst->Insert( 0, "animation_end" );

	object_patrol_graph^ selected_patrol_graph = safe_cast< object_patrol_graph^ >( parent_job->get_project()->find_by_path( selected_patrol, true )->get_object( ) );

	if ( selected_patrol_graph )
		ret_lst->AddRange( selected_patrol_graph->get_signals_list( ) );

	return ret_lst;
}

} // namespace editor
} // namespace xray