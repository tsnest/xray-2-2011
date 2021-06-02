////////////////////////////////////////////////////////////////////////////
//	Created		: 29.12.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_PLATFORM_H_INCLUDED
#define XRAY_RENDER_ENGINE_PLATFORM_H_INCLUDED

#if XRAY_PLATFORM_WINDOWS
#	include <xray/render/engine/world_dx11.h>
#elif XRAY_PLATFORM_XBOX_360 // #if XRAY_PLATFORM_WINDOWS
#	include <xray/render/engine/world_xbox360.h>
#elif XRAY_PLATFORM_PS3 // #elif XRAY_PLATFORM_XBOX360
#	include <xray/render/engine/world_ps3.h>
#else // #elif XRAY_PLATFORM_PS3
#	error define your platform!
#endif // #if XRAY_PLATFORM_WINDOWS

#endif // #ifndef XRAY_RENDER_ENGINE_PLATFORM_H_INCLUDED