////////////////////////////////////////////////////////////////////////////
//	Created		: 09.06.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PCH_H_INCLUDED
#define PCH_H_INCLUDED

#ifndef XRAY_STATIC_LIBRARIES
#	define XRAY_ENGINE_BUILDING
#endif // #ifndef XRAY_STATIC_LIBRARIES

#define XRAY_DISABLE_NEW_OPERATOR	0

#define XRAY_MEMORY_OVERRIDE_OPERATORS_H_INCLUDED

#define XRAY_LOG_MODULE_INITIATOR	"graphics_benchmark"

#define	WIN32_LEAN_AND_MEAN
#define	VC_EXTRALEAN				// useful only for MFC projects (http://support.microsoft.com/default.aspx?scid=kb;en-us;166474)

// sprintf, strcat
#define _CRT_SECURE_NO_WARNINGS

#include <xray/extensions.h>

using xray::math::float2;
using xray::math::float3;
using xray::math::color;

#include "graphics_benchmark_memory.h"

#include <xray/os_preinclude.h>



#undef NOMB
#undef NOUSER
#undef NOMSG
#undef NOWINSTYLES
#undef NOWINMESSAGES
#undef NOSYSMETRICS
#undef NOSHOWWINDOW
#undef NOWINOFFSETS
#undef NOTEXTMETRIC      			// typedef TEXTMETRIC and associated routines
#undef NOGDI

#define _WIN32_WINNT				0x0500
#include <xray/os_include.h>



#include <xray/render/core/render_include.h>
#include <xray/render/core/device.h>
#include <xray/render/core/effect_manager.h>
#include <xray/render/core/effect_compiler.h>
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/backend.h>
#include <xray/render/core/res_geometry.h>


inline pvoid __cdecl operator new		( size_t size )
{
	return	MALLOC( size, "pure new" );
}

inline pvoid __cdecl operator new[]		( size_t size )
{
	return	MALLOC( size, "pure new" );
}

inline void __cdecl operator delete		( pvoid pointer )
{
	FREE	( pointer );
}

inline void __cdecl operator delete[]	( pvoid pointer ) throw ( )
{
	FREE	( pointer );
}

#include <stdio.h>
#include <stack>
#include <math.h>
#include <string>


#	pragma comment( lib,"DXGI.lib")

// Turning on this ifdef may cause crash in startup: module is created incorrect.
//#if USE_DX10
#	pragma comment( lib,"D3D10.lib")
#	pragma comment( lib,"D3DX10.lib")
//#else
#	pragma comment( lib, "dxguid.lib")
#	pragma comment( lib,"D3D11.lib")
#	pragma comment( lib,"D3DX11.lib")
//#endif

#pragma comment( lib,"d3d9.lib") // For pix events
#pragma comment( lib,"d3dcompiler.lib") 

// For #include <D3D9Types.h> and #include <d3dx9mesh.h> only need to be removed.
#pragma comment( lib,"D3dx9.lib")


#if USE_HARDWARE_COUNTERS

// Includes for NVPerfSDK helper classes
#include "../nvidia/PerfSDK/NVPerfSDK.h"
#pragma comment (lib, "NVPerfSDK.lib")

//#include <trace.h>
//#include <tracedisplay.h>

// Includes for GPUPerfAPI helper classes
#include "../amd/GPUPerfAPI/GPUPerfAPI.h"
#pragma comment( lib,"GPUPerfAPIDX11.lib")


#endif


#endif // #ifndef PCH_H_INCLUDED