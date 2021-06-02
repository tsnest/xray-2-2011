////////////////////////////////////////////////////////////////////////////
//	Created		: 26.08.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_SYSTEM_CONFIG_WRAPPER_H_INCLUDED
#define PARTICLE_SYSTEM_CONFIG_WRAPPER_H_INCLUDED

namespace xray {
	namespace particle_editor {
	

public ref class particle_system_config_wrapper: IDisposable {
public:
	particle_system_config_wrapper(configs::lua_config_ptr config)
	{
		m_config = CRT_NEW(configs::lua_config_ptr)(config);
		m_is_disposed = false;
	}
	!particle_system_config_wrapper(){
 		if(!m_is_disposed)
 			this->~particle_system_config_wrapper();
	}
	~particle_system_config_wrapper(){
 		CRT_DELETE(m_config);
 		m_is_disposed			= true;
	}

private:
	configs::lua_config_ptr* m_config;
	bool					m_is_disposed;
public:
	configs::lua_config_ptr		c_ptr	(){return *m_config;};

}; // class particle_system_config_wrapper


} // namespace particle_editor
} // namespace xray

#endif // #ifndef PARTICLE_SYSTEM_CONFIG_WRAPPER_H_INCLUDED