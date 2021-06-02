////////////////////////////////////////////////////////////////////////////
//	Created		: 19.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PCH_H_INCLUDED
#define PCH_H_INCLUDED

#define XRAY_ENGINE_BUILDING
#define XRAY_DISABLE_NEW_OPERATOR	0
#define XRAY_MEMORY_OVERRIDE_OPERATORS_H_INCLUDED

#define XRAY_LOG_MODULE_INITIATOR	"texture_compressor_test"


#include <xray/os_preinclude.h>
#undef NOKERNEL
#undef NOD3D
#undef NOTEXTMETRIC
#undef NOGDI
#undef NOUSER
#undef NOMSG
#include <xray/os_include.h>
#include <freeimage/freeimage.h>

#include <xray/extensions.h>
#include "texture_compressor_test_memory.h"

// nvDXTcompress call delete[] before new[]!s
static bool b_new_called = false;

inline pvoid __cdecl operator new		( size_t size )
{
	return	MALLOC( size, "pure new" );
}

inline pvoid __cdecl operator new[]		( size_t size )
{
	b_new_called = true;
	return	MALLOC( size, "pure new" );
}

inline void __cdecl operator delete		( pvoid pointer )
{
	FREE	( pointer );
}

inline void __cdecl operator delete[]	( pvoid pointer ) throw ( )
{
	if (b_new_called)
		FREE	( pointer );
}

#endif // #ifndef PCH_H_INCLUDED