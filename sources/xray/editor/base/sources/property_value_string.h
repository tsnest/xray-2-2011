////////////////////////////////////////////////////////////////////////////
//	Created		: 02.09.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_STRING_PROPERTY_HOLDER_H_INCLUDED
#define PARTICLE_STRING_PROPERTY_HOLDER_H_INCLUDED

#include "property_value_base.h"

using namespace System;

namespace xray {
	namespace editor_base {
		public ref class property_value_string : xray::editor::wpf_controls::i_property_value, property_value_base {
		public:

			property_value_string( String^ name, configs::lua_config_value const& config, String^ property_parent_path ){
				m_name			= name;
				m_property_path = property_parent_path + m_name + "/";
				m_config		= CRT_NEW(configs::lua_config_value) (config);				
			}

			!property_value_string()
			{ 
				CRT_DELETE( m_config );				
			}

			~property_value_string()
			{
				this->!property_value_string();
			}
		private:
			String^						m_name;
			String^						m_property_path;
			configs::lua_config_value*	m_config;			

		public:
			virtual void			set_value		(System::Object^ value){

				(*m_config)[unmanaged_string(m_name).c_str()] = unmanaged_string(safe_cast<String^>(value)).c_str();
				
				on_property_value_changed( m_property_path, (*m_config)[unmanaged_string(m_name).c_str()] );
			
			}
			virtual System::Object	^get_value		(){
				return gcnew String((*m_config)[unmanaged_string(m_name).c_str()]);
			}

			virtual property System::Type^			value_type
			{
				System::Type^ get() {return System::String::typeid;}
			}

			virtual property on_property_value_changed^ on_property_value_changed;
		}; // class property_value_bool
	} // namespace editor_base
} // namespace xray

#endif // #ifndef PARTICLE_STRING_PROPERTY_HOLDER_H_INCLUDED