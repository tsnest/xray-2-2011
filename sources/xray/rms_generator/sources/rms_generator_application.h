////////////////////////////////////////////////////////////////////////////
//	Created 	: 26.08.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef FS_PACK_APPLICATION_H_INCLUDED
#define FS_PACK_APPLICATION_H_INCLUDED

#include <xray/resources_queries_result.h>
#include <xray/resources_query_result.h>

namespace rms_generator {

class application 
{
public:
			void	initialize		( u32 argc, pstr const * argv );
			void	execute			( );
			void	finalize		( );
	inline	u32		get_exit_code	( ) const { return m_exit_code; }
private:
			void	on_config_loaded( xray::resources::queries_result& queries );
private:
	HWND			m_splash_screen;
	pcstr const*	m_argv;
	u32				m_argc;

private:
	u32				m_exit_code;
}; // class application


} // namespace rms_generator

#endif // #ifndef FS_PACK_APPLICATION_H_INCLUDED