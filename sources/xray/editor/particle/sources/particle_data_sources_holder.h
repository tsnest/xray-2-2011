////////////////////////////////////////////////////////////////////////////
//	Created		: 27.05.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_DATA_SOURCES_HOLDER_H_INCLUDED
#define PARTICLE_DATA_SOURCES_HOLDER_H_INCLUDED

using namespace System::Collections;

namespace xray {
namespace particle_editor {


interface class particle_data_source_base;
ref class particle_graph_document;

public ref class particle_data_sources_holder {

typedef System::Collections::Generic::List< particle_data_source_base^ >					data_sources_list;
typedef System::Collections::Generic::Dictionary< System::String^, data_sources_list^ >		data_sources_lists_by_types_dictionary;


public:
	particle_data_sources_holder(particle_graph_document^ parent_document):
	m_parent_document(parent_document)
	{
		in_constructor();		
	}
	

private:
	~particle_data_sources_holder();

#pragma region | InnerTypes |

#pragma endregion

#pragma region | Initialize |
private:
	void									in_constructor();

#pragma endregion

#pragma region | Fields |
private:
	data_sources_lists_by_types_dictionary^	m_data_sources_types_dictionary;
	particle_graph_document^				m_parent_document;
	configs::lua_config_value*				m_data_sources_config;

#pragma endregion

#pragma region | Methods |
public:
	void						add_data_source					( System::String^ data_source_type, particle_data_source_base^ data_source );
	particle_data_source_base^	get_data_source					( System::String^ data_source_type, System::String^ data_source_name );

	System::Collections::Generic::IEnumerable< System::String^ >^	get_data_sources_names_by_type( System::String^ data_source_type );
	
	void						deserialize_from_config			( configs::lua_config_value config );

	void						create_data_source_from_config	( System::String^ data_source_type, System::String^ data_source_name, configs::lua_config_value config);
	System::String^				generate_data_source_name		( System::String^ data_source_type );
	

#pragma endregion

#pragma region | Properties |
	
public:

	property data_sources_lists_by_types_dictionary^	data_sources_dictionary
	{
		data_sources_lists_by_types_dictionary^			get( ){ return m_data_sources_types_dictionary;}
	}

	property configs::lua_config_value data_sources_config
	{
		configs::lua_config_value		get( ) { return *m_data_sources_config; }
	}

#pragma endregion

#pragma region | Events |

#pragma endregion

}; // class particle_data_sources_holder

} // namespace particle_editor
} // namespace xray

#endif // #ifndef PARTICLE_DATA_SOURCES_HOLDER_H_INCLUDED