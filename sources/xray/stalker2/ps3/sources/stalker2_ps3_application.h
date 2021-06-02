////////////////////////////////////////////////////////////////////////////
//	Created 	: 26.08.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef STALKER2_PS3_APPLICATION_H_INCLUDED
#define STALKER2_PS3_APPLICATION_H_INCLUDED

#include <xray/stalker2/game_module_proxy.h>

namespace stalker2 {

class application {
public:
			void		initialize		( pcstr const command_line );
			void		execute			( );
			void		finalize		( );
	inline	u32			get_exit_code	( ) const { return m_exit_code; }

private:
	game_module_proxy	m_game_proxy;
	u32					m_exit_code;
}; // class application

} // namespace stalker2

#endif // #ifndef STALKER2_PS3_APPLICATION_H_INCLUDED