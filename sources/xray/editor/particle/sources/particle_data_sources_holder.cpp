////////////////////////////////////////////////////////////////////////////
//	Created		: 27.05.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_data_sources_holder.h"
#include "particle_graph_document.h"
#include "particle_editor.h"
#include "particle_data_sources_panel.h"
#include "particle_data_source_base.h"
#include "particle_data_source_float.h"
#include "particle_data_source_float3.h"
#include "particle_data_source_color.h"
#include "particle_data_source_color_matrix.h"

namespace xray {
namespace particle_editor {

void particle_data_sources_holder::in_constructor(){
	m_data_sources_types_dictionary = gcnew data_sources_lists_by_types_dictionary();

	for each ( Generic::KeyValuePair<String^, String^>^ pair in m_parent_document->parent_editor->available_data_source_types )
	{
		m_data_sources_types_dictionary->Add( pair->Key , gcnew data_sources_list( ) );
	}
}


particle_data_sources_holder::~particle_data_sources_holder( )
{

}

void particle_data_sources_holder::add_data_source( System::String^ data_source_type, particle_data_source_base^ data_source ){
	bool particle_data_source_type_exists = m_parent_document->parent_editor->available_data_source_types->ContainsKey( data_source_type );
	
	R_ASSERT( particle_data_source_type_exists );
	
	m_data_sources_types_dictionary[ data_source_type ]->Add( data_source );	
}

void particle_data_sources_holder::create_data_source_from_config(String^ data_source_type, 
																  String^ data_source_name,
																  configs::lua_config_value config)
{
	
	particle_data_source_base^ new_data_source = nullptr;

	configs::lua_config_value data_source_initialize_config = config;

	if (!(*m_data_sources_config)[unmanaged_string(data_source_type).c_str()].value_exists(unmanaged_string(data_source_name).c_str())){
		(*m_data_sources_config)[unmanaged_string(data_source_type).c_str()][unmanaged_string(data_source_name).c_str()].assign_lua_value(config.copy());
		
		data_source_initialize_config = 
			(*m_data_sources_config)[unmanaged_string(data_source_type).c_str()][unmanaged_string(data_source_name).c_str()];
	}

	if ( data_source_type == "float" )
	{			
		new_data_source = gcnew particle_data_source_float( data_source_name , data_source_initialize_config[ "source" ] );
	}
	else if ( data_source_type == "color" )
	{
		new_data_source = gcnew particle_data_source_color( data_source_name , data_source_initialize_config[ "source" ] );
	}
	else if ( data_source_type == "float3" )
	{
		new_data_source = gcnew particle_data_source_float3( data_source_name , data_source_initialize_config );
	}
	else if ( data_source_type == "color_matrix" )
	{
		new_data_source = gcnew particle_data_source_color_matrix( data_source_name , data_source_initialize_config[ "source" ] );
	}
	
	add_data_source( data_source_type, new_data_source );
	
	
}

void particle_data_sources_holder::deserialize_from_config( configs::lua_config_value config )
{
	m_data_sources_config = CRT_NEW( configs::lua_config_value )( config );

	configs::lua_config_value::iterator begin		= config.begin();
	configs::lua_config_value::iterator end			= config.end();

	for(;begin != end;++begin){
		configs::lua_config_value current_type_config	= *begin ;
		String^	current_type_name				= gcnew String( begin.key( ) );

		configs::lua_config_value::iterator current_type_begin		= current_type_config.begin();
		configs::lua_config_value::iterator current_type_end		= current_type_config.end();
		for( ; current_type_begin != current_type_end; ++current_type_begin ) 
		{
			create_data_source_from_config( current_type_name, gcnew String( current_type_begin.key( ) ), *current_type_begin );
		}
	}

}

particle_data_source_base^ particle_data_sources_holder::get_data_source( String^ data_source_type, String^ data_source_name )
{
	for each( particle_data_source_base^ data_source in m_data_sources_types_dictionary[ data_source_type ] )
	{
		if ( data_source->name == data_source_name )
			return data_source;
	}
	
	return nullptr;
}

Generic::IEnumerable< System::String^ >^	particle_data_sources_holder::get_data_sources_names_by_type( String^ data_source_type )
{
	Generic::List< String^ >^ ret_list = gcnew Generic::List< String^ >(); 
	for each ( particle_data_source_base^ data_source in m_data_sources_types_dictionary[ data_source_type ] )
	{
		ret_list->Add( data_source->name );
	}
	return ret_list;
}

System::String^ particle_data_sources_holder::generate_data_source_name( String^ data_source_type )
{
	Generic::List<String^>^ names_list = safe_cast<Generic::List<String^>^>(get_data_sources_names_by_type(data_source_type));
	
	int index = 0;

	for(;names_list->Contains(data_source_type+index);index++){}

	return data_source_type + index;
}

} // namespace particle_editor
} // namespace xray
