////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/core/core.h>		// for error_mode
#include "debug_platform.h"
#include <xray/debug.h>
#include "debug.h"
#include <sys/dbg.h>			// for floating point exceptions

#pragma comment( lib, "lv2dbg_stub" )

namespace xray {
namespace core {
namespace debug {
	extern xray::command_line::key	g_fpe_disabled;
} // namespace debug 
} // namespace core
} // namespace xray

void xray::core::debug::platform::change_storage_access_handler	(
		error_mode error_mode, 
		void (*storage_access_handler) ( int )
	)
{
	NOT_IMPLEMENTED	( );
}

namespace xray {
namespace core {
namespace debug {

static bool s_debug_preinitialized		=	false;

void preinitialize						( )
{
	if ( s_debug_preinitialized )
		return;

	s_debug_preinitialized				= true;
}

} // namespace debug
} // namespace core
} // namespace xray

#ifdef DEBUG
void xray::debug::enable_fpe			( bool value )
{
	if ( value )
		sys_dbg_enable_floating_point_enabled_exception	( threading::current_thread_id(), 0, 0, 0 );
	else
		sys_dbg_disable_floating_point_enabled_exception( threading::current_thread_id(), 0, 0, 0 );
}
#endif // #ifdef DEBUG

void xray::core::debug::on_thread_spawn	( )
{
#if defined(DEBUG)
	if ( !g_fpe_disabled )
		xray::debug::enable_fpe		( true );
#endif // #if defined(DEBUG)
}