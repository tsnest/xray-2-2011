////////////////////////////////////////////////////////////////////////////
//	Created		: 27.10.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "debug_platform.h"
#include "debug.h"
#include <xray/os_include.h>
#include <dxerr9.h>
#include <xray/fixed_string.h>
#include <xray/core/engine.h>

#pragma comment( lib, "dxerr9.lib" )

pcstr xray::core::debug::platform::get_graphics_api_error_description	( int const error_code )
{
	return						DXGetErrorDescription9( error_code );
}

bool xray::debug::is_debugger_present( )
{
#ifdef DEBUG
	return						( !!IS_DEBUGGER_PRESENT() );
#else // #ifdef DEBUG
	return						( false );
#endif // #ifdef DEBUG
}

#ifdef DEBUG
void xray::core::debug::notify_xbox_debugger ( pcstr in_message )
{
	DmSendNotificationString	(in_message);
}
#else // #ifdef DEBUG
void xray::core::debug::notify_xbox_debugger ( pcstr in_message )
{
	XRAY_UNREFERENCED_PARAMETER	(in_message);
}
#endif // #ifdef DEBUG