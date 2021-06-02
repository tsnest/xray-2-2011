////////////////////////////////////////////////////////////////////////////
//	Created		: 12.04.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_graph_node.h"
#include "particle_time_parameters_holder.h"
#include "particle_data_source_property_holder.h"
#include "particle_graph_document.h"
#include "particle_data_sources_panel.h"

using namespace System;
using namespace xray::editor::wpf_controls;

namespace xray {
namespace particle_editor {


particle_node_properties::particle_node_properties(particle_graph_node^ owner): 
	xray::editor_base::properties_holder( (*owner->node_config)["properties"], "library/particle_configs/particle_source.library" )
{
	m_particle_node					= owner;
	m_time_parameters_holder		= nullptr;
}

void	particle_node_properties::load_properties	( configs::lua_config_value* config, System::String^ properties_path )
{

	if (!m_particle_node->is_on_graph)
 		return;

	properties_holder::load_properties( config, properties_path );

 	if (!m_particle_node->is_root_node())
 	{
 		container->properties->add_from_obj(m_particle_node, "node_text");
 		container->properties->add_from_obj(m_particle_node, "enabled");		
 		container->properties->add_from_obj(m_particle_node, "id");
  	}
 
 	if ( !m_particle_node->is_root_node()&&m_particle_node->is_on_graph&&(m_particle_node->is_emitter_node() || m_particle_node->is_action_node())){
 
 		//float length = parent_node->is_root_node() ? 400.f : parent_node->m_time_parameters_holder->get_length_time_impl();
 
 		float length = 1.f;
 		if (!(*config).value_exists("Delay"))
 			(*config)["Delay"]		= 0.0f;
 		if (!(*config).value_exists("Duration"))
 			(*config)["Duration"]	= length;
 
 		m_time_parameters_holder = gcnew particle_time_parameters_holder(m_particle_node, *config);
 		
 		container->properties->add_from_obj(m_time_parameters_holder, "Delay");
 		container->properties->add_from_obj(m_time_parameters_holder, "Duration");
 	}	
}

bool	particle_node_properties::load_property	(	wpf_property_container_base^	 container,
													configs::lua_config_value*		 object_properties_config,
													configs::lua_config_value*		 property_descriptor_value,
													configs::lua_config_value const& property_default_value,
													System::String^ const			 property_path )
{
	if ( properties_holder::load_property	( container, object_properties_config, property_descriptor_value, property_default_value, property_path) )
		return true;
	
	String^ prop_type		= gcnew String((*property_descriptor_value)["type"]);		
	String^ prop_name		= gcnew String((*property_descriptor_value)["name"]);
	String^ prop_desc		= gcnew String((*property_descriptor_value)["description"]);
	String^ prop_category	= gcnew String((*property_descriptor_value)["category"]);
	bool prop_readonly		= (bool)((*property_descriptor_value)["readonly"]);

	if (prop_type == "data_source"){

		configs::lua_config_value data_source_holder_config		= (*object_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()];

		System::String^ data_source_type = gcnew System::String( (*property_descriptor_value)[ "data_source_type" ]);
		
		wpf_property_container^ data_source_property_container = gcnew wpf_property_container();

		String^ selected_data_source =	data_source_holder_config.value_exists( "selected_source" )
									? gcnew String( data_source_holder_config[ "selected_source" ] ) : "" ;

		particle_data_source_property_holder^ data_source_holder = gcnew particle_data_source_property_holder( data_source_property_container, data_source_holder_config ,m_particle_node, prop_name, prop_desc, "", data_source_type, property_path + prop_name + "/");

		property_descriptor^ prop_descriptor = data_source_property_container->properties->add( "DataSource", prop_category, prop_desc, selected_data_source, data_source_holder );
		prop_descriptor->is_read_only = prop_readonly;
		
		R_ASSERT(((*property_descriptor_value).value_exists("input_values_list")));

		configs::lua_config_value values_table = (*property_descriptor_value)["input_values_list"];

		System::Collections::Generic::List<String^>^ input_values_list = gcnew System::Collections::Generic::List<String^>();

		configs::lua_config_value::iterator values_begin		= values_table.begin();
		configs::lua_config_value::iterator values_end			= values_table.end();

		for(;values_begin != values_end;++values_begin){
			input_values_list->Add(gcnew String(*values_begin));
		}	
		
		String^ selected_data_source_input =	data_source_holder_config.value_exists( "Input" )
			? gcnew String( data_source_holder_config[ "Input" ] ) : input_values_list[0] ;

		data_source_holder->set_input( selected_data_source_input );

		if (selected_data_source == ""){
			System::String^ data_source_name = m_particle_node->parent_document->data_sources->generate_data_source_name( data_source_type );
			m_particle_node->parent_document->data_sources->create_data_source_from_config(data_source_type, data_source_name, data_source_holder_config);			
			selected_data_source = data_source_name;
		}
		data_source_holder->set_value( selected_data_source );

		property_descriptor^ descr = container->properties->add_container(prop_name, prop_category,prop_desc, data_source_property_container);
		descr->is_read_only = prop_readonly;
		descr->dynamic_attributes->add(gcnew editor::wpf_controls::property_editors::attributes::data_source_attribute( 
			data_source_type, 
			gcnew Func<System::String^, System::Collections::Generic::IEnumerable<System::String^>^>( m_particle_node->parent_document->data_sources, &particle_data_sources_holder::get_data_sources_names_by_type ), 
			input_values_list, 
			gcnew Action<String^>( data_source_holder, &particle_data_source_property_holder::set_input ),
			gcnew Func<String^>( data_source_holder, &particle_data_source_property_holder::get_input )
		));
	}
	else
	{
		return false;
	}
	return true;
}

void particle_node_properties::process_outgoing_changes( configs::lua_config_value* changes_config)
{
	//changes_config->save_as("d:/changes");
	m_particle_node->parent_document->particle_system_property_changed( m_particle_node, *changes_config );
}

} // namespace particle_editor
} // namespace xray

