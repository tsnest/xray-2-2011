////////////////////////////////////////////////////////////////////////////
//	Created		: 18.05.2010
//	Author		: Armen Abroyan, Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PCH_H_INCLUDED
#define PCH_H_INCLUDED

#ifndef XRAY_STATIC_LIBRARIES
#	define XRAY_ENGINE_BUILDING
#endif // #ifndef XRAY_STATIC_LIBRARIES

//#define XRAY_DISABLE_NEW_OPERATOR	0

#define XRAY_MEMORY_OVERRIDE_OPERATORS_H_INCLUDED

#define XRAY_LOG_MODULE_INITIATOR	"shader_compiler"

#include <direct.h>

#define	WIN32_LEAN_AND_MEAN
#define	VC_EXTRALEAN				// useful only for MFC projects (http://support.microsoft.com/default.aspx?scid=kb;en-us;166474)


#include <xray/os_preinclude.h>

#undef NOMB
#undef NOMSG
#undef NOUSER
#undef NOTEXTMETRIC      			// typedef TEXTMETRIC and associated routines
#undef NOGDI



#include <xray/os_include.h>
#include <xray/extensions.h>

#include "shader_compiler_memory.h"


//inline pvoid __cdecl operator new		( size_t size )
//{
//	return	MALLOC( size, "pure new" );
//}
//
//inline pvoid __cdecl operator new[]		( size_t size )
//{
//	return	MALLOC( size, "pure new" );
//}
//
//inline void __cdecl operator delete		( pvoid pointer )
//{
//	FREE	( pointer );
//}
//
//inline void __cdecl operator delete[]	( pvoid pointer ) throw ( )
//{
//	FREE	( pointer );
//}

#include <stdio.h>
#include <stack>
#include <math.h>
#include <string>

#include <share.h>
#include <io.h>
#include <conio.h>
#include <fcntl.h>
#include <sys/stat.h>

#pragma warning(push)
#pragma warning(disable:4201)
#include <mmsystem.h>
#pragma warning(pop)

#pragma comment( lib, "winmm.lib" )

#pragma warning( push )
#pragma warning( disable : 4995 ) // warning C4995: 'sqrtf': name was marked as #pragma deprecated
#include <d3dx9shader.h>
#pragma warning( pop )
#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3dx9.lib" )

#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11Shader.h>
#pragma comment( lib, "d3dcompiler.lib" )
#pragma comment( lib, "dxguid.lib")
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dx11.lib" )

#include <d3d10.h>
#include <d3d10Shader.h>
#pragma comment( lib, "d3d10.lib" )
#pragma comment( lib, "d3dx10.lib" )


#endif // #ifndef PCH_H_INCLUDED