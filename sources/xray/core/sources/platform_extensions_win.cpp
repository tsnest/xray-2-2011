////////////////////////////////////////////////////////////////////////////
//	Created 	: 13.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/platform_extensions_win.h>
#include <xray/os_include.h>
#include "delayimp.h"

#pragma comment(lib, "delayimp")

bool xray::platform::unload_delay_loaded_library	( pcstr dll_name )
{
	return	( ( __FUnloadDelayLoadedDLL2(dll_name) != FALSE ) || true );
}