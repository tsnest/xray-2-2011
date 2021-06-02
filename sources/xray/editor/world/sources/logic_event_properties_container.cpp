////////////////////////////////////////////////////////////////////////////
//	Created		: 14.04.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "logic_event_properties_container.h"
#include "logic_event_property_holder.h"
#include "job_behaviour_base.h"

using namespace xray::editor::wpf_controls;

namespace xray {
namespace editor {

logic_event_properties_container::logic_event_properties_container( job_behaviour_base^	parent_behaviour )
{
	m_container					= gcnew wpf_property_container_collection( "event{0}" );
	m_parent_behaviour			= parent_behaviour;	
	m_events_list				= gcnew Generic::List<logic_event_property_holder^>( );
}

bool logic_event_properties_container::add_property(){ 

	logic_event_property_holder^ property_value = gcnew logic_event_property_holder( m_parent_behaviour	);
	add_to_container( property_value );
	return true;
}

void logic_event_properties_container::add_to_container	( logic_event_property_holder^ holder )
{
	System::String^ event_name = "event" ;

	wpf_controls::property_descriptor^ prop_descriptor = gcnew wpf_controls::property_descriptor(event_name, "", "", "", holder);
	holder->own_property_descriptor = prop_descriptor;

	holder->own_property_descriptor->tag = holder;

	holder->refresh_attributes();

	prop_descriptor->is_read_only = false;

	if ( m_parent_behaviour->get_available_events_list()->Count > 0 && holder->get_property_type() == "" )
		holder->set_property_type(m_parent_behaviour->get_available_events_list()[0]);

	hypergraph::node_property_attribute^ node_attr = gcnew hypergraph::node_property_attribute( false, true );
	node_attr->type_id = "logic_behaviour";
	prop_descriptor->dynamic_attributes->add( node_attr );
	prop_descriptor->dynamic_attributes->add( gcnew property_editors::attributes::custom_property_attribute(
		gcnew System::Func<System::Collections::Generic::List<System::String^>^>(m_parent_behaviour, &job_behaviour_base::get_available_events_list) , 
		gcnew System::Func<System::String^>(holder, &logic_event_property_holder::get_property_type) , 
		gcnew System::Action<System::String^>(holder, &logic_event_property_holder::set_property_type)
		));

	m_container->properties->add( prop_descriptor );

	m_events_list->Add( holder );
}

bool logic_event_properties_container::remove_property(Object^ property_id){
	m_events_list[(System::Int32)property_id]->remove_switch_action();	
	safe_cast<property_container_collection^>(m_container)->properties->remove((System::Int32)property_id);	
	m_events_list->RemoveAt((System::Int32)property_id);

	return true;
}
void logic_event_properties_container::remove_property	( logic_event_property_holder^ holder )
{
	holder->remove_switch_action();
	safe_cast<property_container_collection^>(m_container)->properties->remove(m_events_list->IndexOf(holder));	
	m_events_list->Remove(holder);
}

bool logic_event_properties_container::move_property	( System::Int32 , System::String^  )
{
// 	property_container_collection::property_collection^ prop_collection = safe_cast<property_container_collection^>(m_container)->properties;
// 	property_descriptor^ descr = prop_collection[property_id];
// 
// 	System::String^ item_name_template = dynamic_cast<property_container_collection^>(m_container)->item_name_template;
// 
// 	System::Int32 prop_1 = property_id;
// 	System::Int32 prop_2;
// 
// 	if (order == "up" && property_id != 0)
// 	{
// 		prop_collection->Remove(descr);
// 		prop_collection->Insert(property_id - 1, descr);
// 		prop_2 = property_id - 1;
// 	}
// 	else if(order == "down" && property_id != prop_collection->Count - 1)
// 	{
// 		prop_collection->Remove(descr);
// 		prop_collection->Insert(property_id + 1, descr);
// 		prop_2 = property_id + 1;
// 	}
// 	else
// 		return false;
// 
// 	configs::lua_config_value lua_prop_value = (*m_event_properties_config)[unmanaged_string("event" + prop_1).c_str()];
// 	(*m_event_properties_config)[unmanaged_string( "event" + prop_1 ).c_str()].assign_lua_value(
// 		(*m_event_properties_config)[unmanaged_string("event" + prop_2).c_str()]
// 	);
// 	(*m_event_properties_config)[unmanaged_string( "event" + prop_2).c_str()].assign_lua_value(lua_prop_value);
// 
// 	//			m_particle_node->notify_render_on_property_changed(gcnew cli::array<String^>(1){m_property_path});
// 
// 	//			m_particle_node->parent_editor->reset_transform();
// 
// 	//on_property_value_changed( m_property_path, *m_node_properties_config);

	return true;
}

void logic_event_properties_container::load			( configs::lua_config_value const& config )
{
	configs::lua_config_iterator events_begin		= config.begin();
	configs::lua_config_iterator events_end			= config.end();

	for(; events_begin!=events_end; ++events_begin)
	{
		logic_event_property_holder^ property_value = gcnew logic_event_property_holder( m_parent_behaviour	);
		add_to_container( property_value );
		property_value->load(*events_begin);		
	}
}
void logic_event_properties_container::save			( configs::lua_config_value config )
{
	int index = 0;
	for each( logic_event_property_holder^ holder in m_events_list )
	{
		holder->save(config[unmanaged_string("event"+index).c_str()]);
		index++;
	}
}


} // namespace editor
} // namespace xray
