////////////////////////////////////////////////////////////////////////////
//	Created 	: 17.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_LIBRARY_LINKAGE_H_INCLUDED
#define XRAY_RENDER_ENGINE_LIBRARY_LINKAGE_H_INCLUDED

#include <xray/macro_library_name.h>

#if XRAY_PLATFORM_WINDOWS
#	pragma comment(lib, XRAY_LIBRARY_NAME( render_engine_dx11, lib ) )
#	pragma comment(lib, XRAY_LIBRARY_NAME( render_core_dx11, lib ) )
#elif XRAY_PLATFORM_XBOX_360 // #if XRAY_PLATFORM_WINDOWS
#	pragma comment(lib, XRAY_LIBRARY_NAME( render_engine_xbox360, lib ) )
#elif XRAY_PLATFORM_PS3 // #if XRAY_PLATFORM_WINDOWS
#	pragma comment(lib, XRAY_LIBRARY_NAME( render_engine_ps3, lib ) )
#else // #elif XRAY_PLATFORM_PS3
#	error define your platform here
#endif // #if XRAY_PLATFORM_WINDOWS

#endif // #ifndef XRAY_RENDER_ENGINE_LIBRARY_LINKAGE_H_INCLUDED