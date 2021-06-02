////////////////////////////////////////////////////////////////////////////
//	Created		: 02.09.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_FLOAT3_PROPERTY_HOLDER_H_INCLUDED
#define PARTICLE_FLOAT3_PROPERTY_HOLDER_H_INCLUDED

#include "property_value_base.h"

using namespace System;

namespace xray {
	namespace editor_base {
		ref class property_value_float3 : xray::editor::wpf_controls::i_property_value, property_value_base {
		public:

			property_value_float3( String^ name, configs::lua_config_value const& config, String^ property_parent_path ){
				m_name = name;
				m_property_path = property_parent_path + m_name + "/";
				m_config = CRT_NEW(configs::lua_config_value) (config);		
			}

			!property_value_float3()
			{ 
				CRT_DELETE( m_config );
			}

			~property_value_float3()
			{
				this->!property_value_float3();
			}
		private:
			String^						m_name;
			configs::lua_config_value*	m_config;
			String^						m_property_path;

		public:
			virtual void			set_value		(System::Object^ value){
				using xray::math::float3_pod;
				System::Windows::Media::Media3D::Vector3D vec = System::Windows::Media::Media3D::Vector3D(value);

				(*m_config)[unmanaged_string(m_name).c_str()] = float3((float3_pod::type)vec.X, (float3_pod::type)vec.Y, (float3_pod::type)vec.Z);

				on_property_value_changed( m_property_path, (*m_config)[unmanaged_string(m_name).c_str()] );
			
			}
			virtual System::Object	^get_value		(){
				float3 vec = (float3)(*m_config)[(pcstr)unmanaged_string(m_name).c_str()];

				return System::Windows::Media::Media3D::Vector3D(double(vec.x), double(vec.y), double(vec.z));
			}

			virtual property System::Type^			value_type
			{
				System::Type^ get() {return System::Windows::Media::Media3D::Vector3D::typeid;}
			}

			virtual property on_property_value_changed^ on_property_value_changed;

		}; // class property_value_bool
	} // namespace editor_base
} // namespace xray

#endif // #ifndef PARTICLE_FLOAT3_PROPERTY_HOLDER_H_INCLUDED