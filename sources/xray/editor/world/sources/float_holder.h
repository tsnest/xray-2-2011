////////////////////////////////////////////////////////////////////////////
//	Created		: 16.04.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef FLOAT_HOLDER_H_INCLUDED
#define FLOAT_HOLDER_H_INCLUDED

using namespace System;

namespace xray {
namespace editor {

	ref class float_holder {
	public:
		float_holder(String^ name, configs::lua_config_value const& config){
			m_name = name;
			m_config = MT_NEW(configs::lua_config_value) (config);
		}

		!float_holder()
		{
			MT_DELETE	( m_config );
		}

		~float_holder()
		{
			this->!float_holder();
		}

	private:
		String^ m_name;
		configs::lua_config_value* m_config;

	public:
		void set(float value){
			(*m_config)[(pcstr)unmanaged_string(m_name).c_str()] = value;
		}
		float get(){
			return (float)(*m_config)[(pcstr)unmanaged_string(m_name).c_str()];
		}

	}; // class float_holder



} // namespace editor
} // namespace xray

#endif // #ifndef FLOAT_HOLDER_H_INCLUDED