////////////////////////////////////////////////////////////////////////////
//	Created		: 16.04.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BOOL_HOLDER_H_INCLUDED
#define BOOL_HOLDER_H_INCLUDED

using namespace System;

namespace xray {
namespace editor {

	ref class bool_holder {
	public:
		bool_holder(String^ name, configs::lua_config_value const& config){
			m_name = name;
			m_config = MT_NEW(configs::lua_config_value) (config);
		}

		!bool_holder()
		{
			MT_DELETE( m_config );
		}

		~bool_holder()
		{
			this->!bool_holder();
		}
	private:
		String^ m_name;
		configs::lua_config_value* m_config;

	public:
		void set(bool value){
			(*m_config)[(pcstr)unmanaged_string(m_name).c_str()] = value;
		}
		bool get(){
			return (*m_config)[(pcstr)unmanaged_string(m_name).c_str()];
		}

	}; // class bool_holder

} // namespace editor
} // namespace xray

#endif // #ifndef BOOL_HOLDER_H_INCLUDED