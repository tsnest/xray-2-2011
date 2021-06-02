////////////////////////////////////////////////////////////////////////////
//	Created		: 18.05.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef COLOR_HOLDER_H_INCLUDED
#define COLOR_HOLDER_H_INCLUDED

using namespace System;

namespace xray {
	namespace editor {

		ref class color_holder {
		public:
			color_holder(String^ name, configs::lua_config_value const& config){
				m_name = name;
				m_config = MT_NEW(configs::lua_config_value) (config);
			}

			!color_holder()
			{
				MT_DELETE	( m_config );
			}

			~color_holder()
			{
				this->!color_holder();
			}
		private:
			String^ m_name;
			configs::lua_config_value* m_config;

		public:
			void set(color value){
				(*m_config)[(pcstr)unmanaged_string(m_name).c_str()] = Drawing::Color::FromArgb(255,int(255.f*value.r),int(255.f*value.g),int(255.f*value.b)).ToArgb();
			}
			color get(){
				return color(u32((*m_config)[(pcstr)unmanaged_string(m_name).c_str()]));
			}

		}; // class string_holder

	} // namespace editor
} // namespace xray

#endif // #ifndef COLOR_HOLDER_H_INCLUDED