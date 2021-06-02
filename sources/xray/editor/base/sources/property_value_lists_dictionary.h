////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_OBJECT_LIST_HOLDER_H_INCLUDED
#define PARTICLE_OBJECT_LIST_HOLDER_H_INCLUDED

#include "property_value_base.h"

using namespace System;

namespace xray {

namespace editor_base {	

	public ref class property_value_lists_dictionary : xray::editor::wpf_controls::i_property_value, property_value_base {
	public: 
		property_value_lists_dictionary( properties_holder^ holder, editor::wpf_controls::property_container_base^ container, configs::lua_config_value const& properties_config, configs::lua_config_value const& defaults_config, String^ prop_name, String^ prop_desc, String^ prop_category, String^ property_path)
			:m_property_name(prop_name),
			m_prop_desc(prop_desc),
			m_prop_category(prop_category),
			m_container(container),
			m_properties_holder(holder),
			m_property_path(property_path)
		{
			m_properties_config		= CRT_NEW(configs::lua_config_value)(properties_config);
			m_defaults_config		= CRT_NEW(configs::lua_config_value)(defaults_config);
			m_property_path			= property_path;
	};
		!property_value_lists_dictionary()
		{
			CRT_DELETE(m_properties_config);
			CRT_DELETE(m_defaults_config);			
		}
		~property_value_lists_dictionary()
		{
			this->!property_value_lists_dictionary();
		}


	private:
		editor::wpf_controls::property_container_base^	m_container;
		properties_holder^							m_properties_holder;
		configs::lua_config_value*					m_properties_config;
		configs::lua_config_value*					m_defaults_config;
		String^										m_property_path;
		System::String^								m_selected_value;
		System::String^								m_property_name;
		System::String^								m_prop_desc;
		System::String^								m_prop_category;

	public:
		virtual void			set_value		(System::Object^ value);
		virtual System::Object	^get_value		(){return m_selected_value;}

		virtual property System::Type^			value_type
		{
			System::Type^ get() {return System::String::typeid;}
		}

		virtual property on_property_value_changed^ on_property_value_changed;

	};// class property_value_lists_dictionary


} // namespace editor_base
} // namespace xray


#endif // #ifndef PARTICLE_OBJECT_LIST_HOLDER_H_INCLUDED