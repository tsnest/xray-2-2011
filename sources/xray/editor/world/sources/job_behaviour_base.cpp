////////////////////////////////////////////////////////////////////////////
//	Created		: 17.05.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "job_behaviour_base.h"
#include "object_scene.h"
#include "object_job.h"
#include "logic_event_properties_container.h"
#include "logic_event_property_holder.h"

using namespace xray::editor::wpf_controls;
using namespace System;

namespace xray {
namespace editor {

job_behaviour_base::job_behaviour_base( object_job^ parent_job )
{
	this->parent_job		= parent_job;
	m_events_container = gcnew logic_event_properties_container( this );
}

	
void job_behaviour_base::load			( configs::lua_config_value const& config )
{
	float2 loaded_position	= (float2)config["hypergraph_node"]["position"];
	logic_node_position		= System::Windows::Point( loaded_position.x, loaded_position.y );
	behaviour_type			= gcnew String(config["behaviour_type"]);
	if ( config.value_exists("cycled") )
		cycled					= config["cycled"];
	if ( config.value_exists("length") )
		length					= config["length"];
	m_events_container->load(config["events"]);

}
void job_behaviour_base::save			( configs::lua_config_value& config )
{
	config["hypergraph_node"]["position"] = float2( (float)logic_node_position.X , (float)logic_node_position.Y );
	
	config["behaviour_type"]	= unmanaged_string(behaviour_type).c_str();

	config["cycled"]			= cycled;
	config["length"]			= length;
	
	m_events_container->save(config["events"]);

	
	
	for each( logic_event_property_holder^ holder in m_events_container->m_events_list )
	{
		if ( !config.value_exists("timing") )
			config["timing"].create_table();

		configs::lua_config_value timing_config = config["timing"];
		int index = timing_config.size( );
		if( holder->get_property_type() == "on_time" )
		{
			timing_config[index]["time"] = holder->timer_value;
			timing_config[index]["value"] = unmanaged_string(holder->get_property_type() + holder->timer_value).c_str();
		}	
		if ( holder->get_property_type() == "on_time_end" )
		{			
			timing_config[index]["time"] = length;
			timing_config[index]["value"] = "on_time_end";		
		}
	}		

}

void job_behaviour_base::get_properties	( wpf_property_container^ to_container )
{
	wpf_property_container^ timer_container = gcnew wpf_property_container();

	property_descriptor^ prop_descriptor = gcnew property_descriptor( this, "cycled" );
	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));	
	timer_container->properties->add(prop_descriptor);

	prop_descriptor = gcnew property_descriptor( this, "length" );
	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));	
	timer_container->properties->add(prop_descriptor);

	prop_descriptor = gcnew property_descriptor("Timing", "","", timer_container);

	to_container->properties->add( prop_descriptor );	

	prop_descriptor = gcnew property_descriptor("Events", "","", m_events_container->property_container); 
		prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));
		prop_descriptor->dynamic_attributes->add(
				gcnew xray::editor::wpf_controls::property_editors::attributes::collection_attribute(
					gcnew System::Func<Boolean>(m_events_container, &logic_event_properties_container::add_property), 
					gcnew Func<Object^, Boolean>(m_events_container, &logic_event_properties_container::remove_property),
					gcnew Func<Int32, String^, Boolean>(m_events_container, &logic_event_properties_container::move_property)
					)
				);
	to_container->properties->add( prop_descriptor );	
}

System::Collections::Generic::List<System::String^>^ job_behaviour_base::get_available_events_list( )
{
	Collections::Generic::List<System::String^>^ ret_lst = gcnew Collections::Generic::List<System::String^>( );

	ret_lst->Insert( 0, "on_started" );
	ret_lst->Insert( 0, "on_time" );
	ret_lst->Insert( 0, "on_time_end" );	

	ret_lst->AddRange(parent_job->parent_scene->get_children_by_type("event"));	
	
	ret_lst->Remove("#StartScene");

	return ret_lst;
}



	} // namespace editor
} // namespace xray