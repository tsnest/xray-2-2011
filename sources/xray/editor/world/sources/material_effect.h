////////////////////////////////////////////////////////////////////////////
//	Created		: 20.10.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MATERIAL_EFFECT_H_INCLUDED
#define MATERIAL_EFFECT_H_INCLUDED

using namespace System;
using namespace System::Collections::Generic;

namespace xray {
namespace editor {

ref class material_stage;
ref class material_property_base;

ref class material_effect
{
	typedef xray::editor::wpf_controls::property_container		wpf_property_container;
	typedef xray::editor::wpf_controls::property_descriptor		property_descriptor;

public:
								material_effect		( material_stage^ parent, const configs::lua_config_value& config_value );
	virtual						~material_effect	( );
	void						load						( System::Action^ cb );
	void						reload						( System::Action^ cb );
	wpf_property_container^		get_property_container		( bool material_editor_mode );
	void						set_base_texture			( System::String^ name );
	System::String^				get_base_texture			( );
	void						check_integrity				( );

private:
	void						iterate_effect_properties	( configs::lua_config_value config_root, bool& bchanged );
	void						iterate_effect_dependencies	( configs::lua_config_value config_root );
	void						fill_effect_property_grid	( wpf_property_container^ cont, configs::lua_config_value effect_cfg_root, bool material_editor_mode );
	void						fill_effect_property		( wpf_property_container^ cont, configs::lua_config_value property_cfg, bool material_editor_mode );
	
	configs::lua_config_value*				m_config;
	configs::lua_config_ptr*				m_effect_description_config;
	System::Action^							m_loaded_callback;
	Dictionary<System::String^, u32>		m_property_name_to_desc_idx;

public:
	material_stage^											m_parent_stage;
	Dictionary<System::String^, material_property_base^>	m_properties;
	property System::String^								name;

private:
	void						fail_loading				( System::String^ error_message );
	void						effect_properties_loaded	( resources::queries_result & result );
	void						effect_selector_launch		( wpf_controls::property_editors::property^ prop, System::Object^ filter );
	void						texture_selector_launch		( wpf_controls::property_editors::property^ prop, System::Object^ filter );
	void						diffuse_texture_value_chaged( material_property_base^ prop );
	void						effect_reloaded				( );
}; // ref class material_effect

} // namespace editor
} // namespace xray

#endif // #ifndef MATERIAL_EFFECT_H_INCLUDED