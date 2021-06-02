////////////////////////////////////////////////////////////////////////////
//	Created		: 27.10.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "platform.h"
#include <xtl.h>
#include <xbdm.h>
#include <dxerr9.h>
#include <xray/debug/debug.h>
#include <xray/core/engine.h>
#include <xray/debug/macros.h>
#include "utils.h"

#pragma comment( lib, "dxerr9.lib" )

pcstr xray::debug::platform::get_graphics_api_error_description	( int const error_code )
{
	return						DXGetErrorDescription9( error_code );
}

bool xray::debug::is_debugger_present( )
{
#ifdef DEBUG
	return						DmIsDebuggerPresent( ) == TRUE;
#else // #ifdef DEBUG
	return						false;
#endif // #ifdef DEBUG
}

#ifdef DEBUG
void xray::debug::notify_xbox_debugger ( pcstr in_message )
{
	DmSendNotificationString	(in_message);
}
#else // #ifdef DEBUG
void xray::debug::notify_xbox_debugger ( pcstr in_message )
{
	XRAY_UNREFERENCED_PARAMETER	(in_message);
}
#endif // #ifdef DEBUG

void xray::debug::debug_message_box (pcstr message)
{
	XRAY_UNREFERENCED_PARAMETER	(message);
}
