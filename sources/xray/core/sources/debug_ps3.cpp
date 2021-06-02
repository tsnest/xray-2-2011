////////////////////////////////////////////////////////////////////////////
//	Created		: 14.05.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/os_include.h>
#include "debug_platform.h"
#include "debug.h"

#pragma comment( lib, "c")			// for _Sce_assert
#pragma comment( lib, "cs")			// for _Sce_assert
#pragma comment( lib, "c_stub")		// for _Sce_assert
#pragma comment( lib, "stdc++")	// for _Sce_assert

bool xray::debug::is_debugger_present( )
{
#ifdef DEBUG
	return						( !!IS_DEBUGGER_PRESENT() );
#else // #ifdef DEBUG
	return						( false );
#endif // #ifdef DEBUG
}

pcstr xray::core::debug::platform::get_graphics_api_error_description	( int const error_code )
{
	NOT_IMPLEMENTED				( return "" );
}

void xray::core::debug::notify_xbox_debugger ( pcstr message )
{
	XRAY_UNREFERENCED_PARAMETER	( message );
}