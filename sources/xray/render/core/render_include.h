////////////////////////////////////////////////////////////////////////////
//	Created		: 01.06.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_RENDER_INCLUDE_H_INCLUDED
#define XRAY_RENDER_CORE_RENDER_INCLUDE_H_INCLUDED

#include <xray/os_preinclude.h>
#undef NOUSER
#undef NOMSG
#undef NOGDI
#undef NOTEXTMETRIC
#include <xray/os_include.h>

#if XRAY_PLATFORM_WINDOWS
#	if !USE_DX10
#		include <d3d11.h>
#	else // #if !USE_DX10 
#		include <d3d10.h>
#	endif // #if !USE_DX10 
#	include <dxdiag.h>
#	include <xray/render/core/d3d_defines.h>
#elif XRAY_PLATFORM_XBOX_360 // #if XRAY_PLATFORM_WINDOWS
#elif XRAY_PLATFORM_PS3 // #elif XRAY_PLATFORM_XBOX_360
#else // #elif XRAY_PLATFORM_PS3
#endif // #if XRAY_PLATFORM_WINDOWS

#endif // #ifndef XRAY_RENDER_CORE_RENDER_INCLUDE_H_INCLUDED