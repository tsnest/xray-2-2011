////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "tool_scripting.h"
#include "logic_event_property_holder.h"
#include "logic_event_properties_container.h"
#include "job_behaviour_wrapper.h"
#include "job_behaviour_base.h"
#include "job_behaviour_camera_track.h"
#include "job_behaviour_camera_from_object.h"
#include "job_behaviour_camera_director_default.h"
#include "job_behaviour_event_generator.h"
#include "job_behaviour_light_anim.h"
#include "job_behaviour_light_static.h"
#include "job_behaviour_proximity_trigger_default.h"
#include "job_behaviour_composite_default.h"
#include "job_behaviour_values_storage_default.h"
#include "job_behaviour_npc_play_animation.h"
#include "job_behaviour_npc_patrol.h"


using namespace xray::editor::wpf_controls;

namespace xray {
namespace editor {

job_behaviour_wrapper::job_behaviour_wrapper( object_job^ parent_job, bool on_loading )
{

	m_parent_job = parent_job;	
	m_properties_container = gcnew wpf_property_container();

	 if (!on_loading)
		behaviour_type = tool_scripting::behaviour_types_by_object_types[m_parent_job->job_type][0];

	property_descriptor^ descr = gcnew property_descriptor(this, "behaviour_type");
 	
 	ArrayList^ values = gcnew ArrayList();
 	for each (String^ val in tool_scripting::behaviour_types_by_object_types[m_parent_job->job_type])
 		values->Add(val);
 	
 	descr->dynamic_attributes->add(gcnew property_editors::attributes::combo_box_items_attribute(values));
	
 	m_properties_container->inner_value = descr;

}

void	job_behaviour_wrapper::behaviour_type::set( System::String^ value )
{
	if ( behaviour_type == value )
		return;
	
	configs::lua_config_value previous_behaviour_saved_events_container = configs::create_lua_config( )->get_root( );

	job_behaviour_base^ previous_behaviour = m_current_behaviour;
	m_current_behaviour = create_behaviour( value , m_parent_job );
	m_current_behaviour->initialize();

	if ( previous_behaviour != nullptr )
	{

		Generic::List<logic_event_property_holder^>^ events_to_remove_list = gcnew Generic::List<logic_event_property_holder^>();
		
		Generic::List<System::String^>^ available_events_list = m_current_behaviour->get_available_events_list();

		for each( logic_event_property_holder^ holder in previous_behaviour->events_container->m_events_list )
		{	
			if ( !available_events_list->Contains( holder->get_property_type( ) ) ){
				holder->remove_switch_action();
				events_to_remove_list->Add( holder );				
			}
		}

		for each( logic_event_property_holder^ holder in events_to_remove_list )
		{	
			previous_behaviour->events_container->remove_property(holder);
		}

		previous_behaviour->events_container->save(previous_behaviour_saved_events_container);	
		
		for each( logic_event_property_holder^ holder in previous_behaviour->events_container->m_events_list )
		{	
			holder->remove_switch_action();
		}

		m_parent_job->behaviours->Remove( previous_behaviour );	
	}

	m_properties_container->properties->clear();

	if ( hypergraph_node != nullptr )
	{
		m_current_behaviour->hypergraph_node = hypergraph_node;			
	}

	m_current_behaviour->get_properties( m_properties_container );
	m_parent_job->behaviours->Add( m_current_behaviour );
	
	if ( previous_behaviour != nullptr ){
		m_current_behaviour->events_container->load(previous_behaviour_saved_events_container);
		m_current_behaviour->events_container->property_container->refresh_properties();
		m_parent_job->return_queried_behaviours();
		m_current_behaviour->events_container->property_container->update_properties();
	}
}

 System::String^	job_behaviour_wrapper::behaviour_type::get( )
 {
	 if (m_current_behaviour == nullptr)
		 return "";
	 return m_current_behaviour->behaviour_type;
 }

 job_behaviour_base^ job_behaviour_wrapper::create_behaviour(System::String^ type, object_job^ parent_job )
 {
	job_behaviour_base^ behaviour = nullptr;

	if ( type == "camera_track" )
	{
		behaviour = gcnew job_behaviour_camera_track( parent_job );
	}
	else if ( type == "camera_from_object" )
	{
		behaviour = gcnew job_behaviour_camera_from_object( parent_job );
	}
	else if ( type == "active_camera" )
	{
		behaviour = gcnew job_behaviour_camera_director_default( parent_job );
	}
	else if ( type == "camera_free_fly" )
	{
		behaviour = gcnew job_behaviour_camera_free_fly( parent_job );
	}
	else if ( type == "default_timer" ){
		behaviour = gcnew job_behaviour_event_generator( parent_job );
	}
	else if ( type == "event_generator" )
	{
		behaviour = gcnew job_behaviour_event_generator( parent_job );
	}
	else if ( type == "light_anim" )
	{
		behaviour = gcnew job_behaviour_light_anim( parent_job );
	}
	else if ( type == "light_static" )
	{
		behaviour = gcnew job_behaviour_light_static( parent_job );
	}
	else if ( type == "proximity_trigger_default" )
	{
		behaviour = gcnew job_behaviour_proximity_trigger_default( parent_job );
	}
	else if ( type == "composite_default" )
	{
		behaviour = gcnew job_behaviour_composite_default( parent_job );
	}
	else if ( type == "values_storage_default" )
	{
		behaviour = gcnew job_behaviour_values_storage_default( parent_job );
	}
	else if ( type == "npc_play_animation" )
	{
		behaviour = gcnew job_behaviour_npc_play_animation( parent_job );
	}
	else if ( type == "npc_patrol" )
	{
		behaviour = gcnew job_behaviour_npc_patrol( parent_job );
	}

	behaviour->behaviour_type	= type;
	return						behaviour;

 }



} // namespace editor
} // namespace xray
