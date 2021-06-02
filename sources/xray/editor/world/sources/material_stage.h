////////////////////////////////////////////////////////////////////////////
//	Created		: 20.10.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MATERIAL_STAGE_H_INCLUDED
#define MATERIAL_STAGE_H_INCLUDED

using namespace System::Collections::Generic;

namespace xray {
namespace editor {

ref class material;
ref class material_effect;

template<typename TConfigType>
ref class material_property;
ref class material_property_base;
		
ref class material_stage
{
	typedef xray::editor::wpf_controls::property_container		wpf_property_container;
	typedef xray::editor::wpf_controls::property_descriptor		property_descriptor;
	typedef xray::editor::wpf_controls::i_property_container	i_property_container;
	
public:
				material_stage		(	material^ parent, 
										System::String^ name, 
										const configs::lua_config_value& config_value );
	virtual		~material_stage		( );
	wpf_property_container^			get_property_container( bool material_editor_mode );
	void		check_integrity		( );
	void							load						( );
	material^						m_parent;
	material_effect^				m_effect;
	property System::String^		name;
	
private:

	void							properties_loaded		( resources::queries_result& result );
	void							effect_loaded			( );
	void							fail_loading			( System::String^ error_message );
	void							texture_selector_launch	( wpf_controls::property_editors::property^ prop, Object^ filter );

	configs::lua_config_value*		m_config;
	List<material_property_base^>	m_properties;
	bool							m_effect_loaded;
	bool							m_properties_loaded;
	query_result_delegate*			m_load_query;

}; // class material_stage

} // namespace editor
} // namespace xray

#endif // #ifndef MATERIAL_STAGE_H_INCLUDED