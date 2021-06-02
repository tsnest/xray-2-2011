////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_graph_node.h"
#include "particle_graph_document.h"
#include "particle_system_config_wrapper.h"
#include "particle_data_source_property_holder.h"
#include "particle_data_sources_panel.h"
#include "particle_data_source_base.h"


using namespace System::Windows::Forms;
using xray::editor::wpf_controls::float_curve;

namespace xray {
namespace particle_editor {
	
	//extern xray::configs::lua_config_value create_changes_config_from_string_path(String^ path, xray::configs::lua_config_value cfg);

	particle_data_source_property_holder::particle_data_source_property_holder(xray::editor::wpf_controls::property_container^ container,configs::lua_config_value const& config, particle_graph_node^ node, String^ property_name,String^ property_description ,String^ property_category, String^ data_source_type, String^ property_path)
	{
		m_parent_node			= node;
		m_property_name			= property_name;
		m_property_description	= property_description;
		m_property_category		= property_category;
		m_selected_source		= "";
		m_data_source_input		= "";
		m_data_source_type		= data_source_type;
		m_property_path			= property_path;
		m_container				= container;
		m_config				= CRT_NEW(xray::configs::lua_config_value)(config);	
	}

	String^ particle_data_source_property_holder::get_input( )
	{
		return m_data_source_input;
	}

	void particle_data_source_property_holder::set_input( String^ value )
	{
		if (m_data_source_input == value)
			return; 
		m_data_source_input = value;
		( *m_config )["Input"] = unmanaged_string(m_data_source_input).c_str();
		
		configs::lua_config_value changes_config = configs::create_lua_config()->get_root();

		changes_config["Input"] = unmanaged_string(m_data_source_input).c_str();

		send_changes_to_render( changes_config );
	}

	void	particle_data_source_property_holder::set_data_source_impl(String^ name)
	{
		bool need_to_send_changes = true /*m_selected_source != ""*/;
		if (m_selected_source == name)
 			return; 
		
		particle_data_source_base^ previous_data_source = m_parent_node->parent_document->data_sources->get_data_source( m_data_source_type, m_selected_source );
		if ( !!previous_data_source )
			previous_data_source->data_source_data_changed -= gcnew on_data_source_data_changed( this, &particle_data_source_property_holder::send_changes_to_render );
		
		m_selected_source = name;
		
		( *m_config ).clear();

		( *m_config )["Input"] = unmanaged_string(m_data_source_input).c_str();

		( *m_config )[ "selected_source" ] = (pcstr)unmanaged_string( m_selected_source ).c_str( );

		particle_data_source_base^ data_source	= m_parent_node->parent_document->data_sources->get_data_source( m_data_source_type, m_selected_source );
		data_source->data_source_data_changed	+= gcnew on_data_source_data_changed( this, &particle_data_source_property_holder::send_changes_to_render );

		m_parent_node->parent_editor->data_sources_panel->add_data_source_to_panel( data_source, m_data_source_type );

		if (m_container->properties->contains( m_property_category + "/" + "Data" ) )
		{
			editor::wpf_controls::property_descriptor^ desc = m_container->properties[ m_property_category + "/" + "Data" ];
			desc->set_source( gcnew editor::wpf_controls::property_property_value( data_source, "data" )  );
		}
		else 
		{
			m_container->properties->add_from_obj( data_source, "data" );
		}
		
		//m_container->properties->add_from_obj( data_source, "data" );
 		
		configs::lua_config_value selected_doc_config = *m_parent_node->parent_editor->particle_configs["particles/" + m_parent_node->parent_document->source_path]->c_ptr();

 		((*m_config)).add_super_table(selected_doc_config , unmanaged_string("data_sources." + m_data_source_type + "." + m_selected_source).c_str());
		m_parent_node->parent_editor->properties_panel->show_properties( m_parent_node->properties->container );

		if (need_to_send_changes)
			send_changes_to_render( ( m_parent_node->parent_document->data_sources->data_sources_config[ unmanaged_string( m_data_source_type ).c_str( ) ][ unmanaged_string( m_selected_source ).c_str( ) ] ).copy( ) );


//		m_parent_node->parent_editor->properties_panel->property_grid_control->update();
	}

configs::lua_config_value create_changes_config_from_string_path(String^ path, configs::lua_config_value config){
	if (path == "")
		return config;

	String^ field  = path->Substring(0, path->IndexOf("/"));
	configs::lua_config_value new_config = config[unmanaged_string(field).c_str()].create_table();
	String^ new_path = path->Remove(0, path->IndexOf("/") + 1);
	return create_changes_config_from_string_path(new_path, new_config);
}
	

void particle_data_source_property_holder::send_changes_to_render( configs::lua_config_value data_changes_config )
{
	//data_changes_config.save_as("d:/changes");

	//data_changes_config.save_as("d:/data");
	
	configs::lua_config_value* changes_config = CRT_NEW( configs::lua_config_value ) ( configs::create_lua_config()->get_root() );
	
	create_changes_config_from_string_path( m_property_path , *changes_config ).assign_lua_value( data_changes_config ) ;

	m_parent_node->parent_document->particle_system_property_changed( m_parent_node, *changes_config );
}

} // namespace particle_editor
} // namespace xray