////////////////////////////////////////////////////////////////////////////
//	Created		: 16.04.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef STRING_HOLDER_H_INCLUDED
#define STRING_HOLDER_H_INCLUDED

using namespace System;

namespace xray {
namespace editor {

ref class string_holder {
public:
	string_holder(String^ name, configs::lua_config_value const& config){
		m_name = name;
		m_config = MT_NEW(configs::lua_config_value) (config);
	}
	
	!string_holder()
	{
		MT_DELETE	( m_config );
	}

	~string_holder()
	{
		this->!string_holder();
	}

	


private:
	String^ m_name;
	configs::lua_config_value* m_config;

public:
	void set(String^ value){
		(*m_config)[(pcstr)unmanaged_string(m_name).c_str()] = (pcstr)unmanaged_string(safe_cast<String^>(value)).c_str();
	}
	String^ get(){
		return gcnew String((*m_config)[(pcstr)unmanaged_string(m_name).c_str()]);
	}

}; // class string_holder

} // namespace editor
} // namespace xray

#endif // #ifndef STRING_HOLDER_H_INCLUDED