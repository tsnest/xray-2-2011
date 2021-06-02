////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_COLOR_CURVE_HOLDER_H_INCLUDED
#define PARTICLE_COLOR_CURVE_HOLDER_H_INCLUDED

using namespace System;

namespace xray {
namespace editor_base { class property_holder; }
namespace particle_editor {	

	ref class particle_graph_node;

	public ref class particle_color_curve_holder: xray::editor::wpf_controls::i_property_value {
	public:
		particle_color_curve_holder(xray::editor::wpf_controls::property_container^ container,configs::lua_config_value const& config, particle_graph_node^ node, String^ property_name,String^ property_description ,String^ property_category, u32 def_value, String^ property_path);
	private:
		!particle_color_curve_holder(){
			delete m_container;
			CRT_DELETE( m_config );
			CRT_DELETE( m_changes_config );
			CRT_DELETE( m_changes_config_value );
		}
		~particle_color_curve_holder(){
			this->!particle_color_curve_holder();
		}
	private:
		particle_graph_node^					m_parent_node;
		editor::wpf_controls::property_container^	m_container;
		configs::lua_config_value*				m_config;
		configs::lua_config_value*				m_changes_config;
		configs::lua_config_value*				m_changes_config_value;
		String^									m_property_name;
		String^									m_property_description;
		String^									m_property_category;
		String^									m_selected_source;
		const u32&								m_default_value;
		u32										m_last_color_value;
	public:

		virtual property System::Type^			value_type
		{
			System::Type^ get() {return System::String::typeid;}
		}

	public:
		virtual System::Object	^get_value		(){return m_selected_source;}		
		virtual void			set_value		(System::Object^ value){
																			set_data_source_impl(safe_cast<String^>(value));
																			m_parent_node->parent_editor->reset_transform();
																		}
		void					synchronize_config(Object^ sender, EventArgs^ e);
		
	private:
		void					set_data_source_impl(String^ name);


	};// class particle_color_curve_holder


} // namespace particle_editor
} // namespace xray

#endif // #ifndef PARTICLE_COLOR_CURVE_HOLDER_H_INCLUDED