////////////////////////////////////////////////////////////////////////////
//	Created		: 16.04.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef INTEGER_HOLDER_H_INCLUDED
#define INTEGER_HOLDER_H_INCLUDED

using namespace System;

namespace xray {
namespace editor {

	ref class integer_holder {
	public:
		integer_holder(String^ name, configs::lua_config_value const& config){
			m_name = name;
			m_config = MT_NEW(configs::lua_config_value) (config);
		}

		!integer_holder()
		{
			MT_DELETE	( m_config );
		}

		~integer_holder()
		{
			this->!integer_holder();
		}


	private:
		String^ m_name;
		configs::lua_config_value* m_config;

	public:
		void set(int value){
			(*m_config)[(pcstr)unmanaged_string(m_name).c_str()] = value;
		}
		int get(){
			return (int)(*m_config)[(pcstr)unmanaged_string(m_name).c_str()];
		}

	}; // class bool_holder

} // namespace editor
} // namespace xray

#endif // #ifndef INTEGER_HOLDER_H_INCLUDED