////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ENGINE_API_H_INCLUDED
#define XRAY_ENGINE_API_H_INCLUDED



#ifndef XRAY_ENGINE_API
#	ifdef XRAY_STATIC_LIBRARIES
#		define XRAY_ENGINE_API
#	else // #ifdef XRAY_STATIC_LIBRARIES
#		ifdef XRAY_ENGINE_LIBRARY_BUILDING
#			define XRAY_ENGINE_API				XRAY_DLL_EXPORT
#		else // #ifdef XRAY_ENGINE_LIBRARY_BUILDING
#			ifndef XRAY_ENGINE_BUILDING
#				define XRAY_ENGINE_API			XRAY_DLL_IMPORT
#			else // #ifndef XRAY_ENGINE_BUILDING
#				define XRAY_ENGINE_API			XRAY_DLL_EXPORT
#			endif // #ifndef XRAY_ENGINE_BUILDING
#		endif // #ifdef XRAY_ENGINE_LIBRARY_BUILDING
#	endif // #ifdef XRAY_STATIC_LIBRARIES
#endif // #ifndef XRAY_ENGINE_API

namespace xray {

namespace engine_user {
	struct module_proxy;
} // namespace engine_user

namespace engine {

XRAY_ENGINE_API	void	preinitialize					(
							engine_user::module_proxy& proxy,
							pcstr command_line,
							pcstr application,
							pcstr build_date
						);
XRAY_ENGINE_API	void	initialize						( );
XRAY_ENGINE_API	void	execute							( );
XRAY_ENGINE_API	int		get_exit_code					( );
XRAY_ENGINE_API	void	finalize						( );
XRAY_ENGINE_API	bool	command_line_editor				( );
XRAY_ENGINE_API	bool	command_line_no_splash_screen	( );

} // namespace engine
} // namespace xray

#endif // #ifndef XRAY_ENGINE_API_H_INCLUDED