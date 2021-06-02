////////////////////////////////////////////////////////////////////////////
//	Created		: 02.09.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_FLOAT_PROPERTY_HOLDER_H_INCLUDED
#define PARTICLE_FLOAT_PROPERTY_HOLDER_H_INCLUDED

#include "property_value_base.h"

using namespace System;

namespace xray {
	namespace editor_base {
		
		ref class property_value_float : xray::editor::wpf_controls::i_property_value, property_value_base {
		public:

			property_value_float( String^ name, configs::lua_config_value const& config, String^ property_parent_path ){
				m_name = name;
				m_property_path = property_parent_path + m_name + "/";
				m_config = CRT_NEW(configs::lua_config_value) (config);
			}

			!property_value_float()
			{ 
				CRT_DELETE( m_config );
			}

			~property_value_float()
			{
				this->!property_value_float();
			}
		private:
			String^						m_name;
			configs::lua_config_value*	m_config;
			String^						m_property_path;

		public:
			virtual void			set_value		(System::Object^ value){

				(*m_config)[unmanaged_string(m_name).c_str()] = (float)value;

				on_property_value_changed( m_property_path, (*m_config)[unmanaged_string(m_name).c_str()] );
			
			}
			virtual System::Object	^get_value		(){
 				return (float)(*m_config)[(pcstr)unmanaged_string(m_name).c_str()];
			}

			virtual property System::Type^			value_type
			{
				System::Type^ get() {return float::typeid;}
			}

			virtual property on_property_value_changed^ on_property_value_changed;

		}; // class property_value_bool
	} // namespace editor_base
} // namespace xray

#endif // #ifndef PARTICLE_FLOAT_PROPERTY_HOLDER_H_INCLUDED