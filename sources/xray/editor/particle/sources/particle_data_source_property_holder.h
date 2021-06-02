////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_FLOAT_CURVE_HOLDER_H_INCLUDED
#define PARTICLE_FLOAT_CURVE_HOLDER_H_INCLUDED

using namespace System;

namespace xray {
namespace particle_editor {	

	ref class particle_graph_node;

	public ref class particle_data_source_property_holder : xray::editor::wpf_controls::i_property_value {
	public:
		particle_data_source_property_holder(xray::editor::wpf_controls::property_container^ container,configs::lua_config_value const& config, particle_graph_node^ node, String^ property_name,String^ property_description ,String^ property_category, String^ data_source_type, String^ property_path);

	private:
		!particle_data_source_property_holder(){
			delete m_container;
			CRT_DELETE( m_config );
		}
		~particle_data_source_property_holder(){
			this->!particle_data_source_property_holder();
		}

	private:
		particle_graph_node^					m_parent_node;
		editor::wpf_controls::property_container^	m_container;
		configs::lua_config_value*				m_config;
		String^									m_property_name;
		String^									m_property_description;
		String^									m_property_category;
		String^									m_selected_source;
		String^									m_data_source_type;
		String^									m_property_path;
		String^									m_data_source_input;
		
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
		void					set_input( String^ value );
		String^					get_input( );
	private:
		void					set_data_source_impl(String^ name);
		
		void					send_changes_to_render( configs::lua_config_value data_changes_config );

	};// class particle_curve_data_source_holder


} // namespace particle_editor
} // namespace xray

#endif // #ifndef PARTICLE_FLOAT_CURVE_HOLDER_H_INCLUDED