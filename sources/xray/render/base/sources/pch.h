////////////////////////////////////////////////////////////////////////////
//	Created 	: 28.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PCH_H_INCLUDED
#define PCH_H_INCLUDED

#define XRAY_RENDER_BASE_BUILDING

#ifndef XRAY_STATIC_LIBRARIES
#	define XRAY_ENGINE_BUILDING
#endif // #ifndef XRAY_STATIC_LIBRARIES

#define XRAY_LOG_MODULE_INITIATOR	"render_base"
#include <xray/extensions.h>

#if XRAY_PLATFORM_WINDOWS
#	include <xray/render/dx9/render_include.h>
#endif // #if XRAY_PLATFORM_WINDOWS

//Macros delete is declared in windows headers, so this is small hack to avoid problems
#undef DELETE
#include <xray/render/base/memory.h>

using xray::math::float2;
using xray::math::float3;
using xray::math::color;

#endif // #ifndef PCH_H_INCLUDED