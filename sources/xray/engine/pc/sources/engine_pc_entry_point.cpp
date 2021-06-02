////////////////////////////////////////////////////////////////////////////
//	Created		: 17.09.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_STATIC_LIBRARIES
#	define XRAY_ENGINE_BUILDING
#endif // #ifndef XRAY_STATIC_LIBRARIES

#define	XRAY_LOG_MODULE_INITIATOR	"engine_pc"

#include <xray/macro_extensions.h>

#include <xray/engine/library_dependencies.h>

#define XRAY_ENGINE_LIBRARY_BUILDING
#include <xray/engine/library_linkage.h>

#ifndef XRAY_STATIC_LIBRARIES

#include <xray/type_extensions.h>
#include <boost/noncopyable.hpp>
#include <xray/detail_noncopyable.h>
#include <xray/memory_base_allocator.h>
#include <stdio.h>
#include <xray/os_extensions.h>
#include <xray/os_include.h>
#include <xray/engine/api.h>
#include <xray/debug/extensions.h>

#include "linkage_resolver.h"


BOOL APIENTRY DllMain							( HANDLE hModule, unsigned int ul_reason_for_call, LPVOID lpReserved )
{
	XRAY_UNREFERENCED_PARAMETERS( hModule, lpReserved );

	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH: {
			break;
		}
		case DLL_PROCESS_DETACH: {
			break;
		}
	}
	return						( TRUE );
}

#endif // #ifndef XRAY_STATIC_LIBRARIES