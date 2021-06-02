////////////////////////////////////////////////////////////////////////////
//	Created 	: 23.01.2009
//	Author		: Dmitriy Iassenev
//  Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "timing.h"
#include <xray/platform_extensions_win.h>
#include <xray/os_include.h>

#pragma warning(push)
#pragma warning(disable: 4201)
#include <mmsystem.h>
#pragma warning(pop)

#pragma comment(lib, "winmm.lib")

u64	 xray::timing::g_qpc_per_second				= 0;
bool xray::timing::g_cpu_supports_time_stamp	= false;

//extern "C" static bool __stdcall cpu_supports_time_stamp( )
//{
//	bool tsc_feature			= false;
//	int cpu_info[4]				= { -1 };
//	__cpuid						( cpu_info, 0x80000001 );
//	int feature_info			= cpu_info[3];
//	
//	// check tsc feature bit (bit 27) in CPUInfo[3]
//	if (feature_info & 0x4000000)
//		tsc_feature				= true;
//	
//	return						tsc_feature;
//}

void xray::timing::initialize					( )
{
	timeBeginPeriod				( 1 );
	
	bool const result			= platform::unload_delay_loaded_library( "winmm.dll" );
	XRAY_UNREFERENCED_PARAMETER	( result );
	R_ASSERT					( result );
	
	QueryPerformanceFrequency	( ( PLARGE_INTEGER )&g_qpc_per_second );
	
	g_cpu_supports_time_stamp	= false;//cpu_supports_time_stamp( );
}

u64 xray::timing::query_performance_counter	( )
{
	u64							result;
	QueryPerformanceCounter		( ( PLARGE_INTEGER )&result );
	return						(result);
}