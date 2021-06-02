////////////////////////////////////////////////////////////////////////////
//	Created 	: 23.01.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "timing.h"
#include <xray/timing_functions.h>
#include <xray/os_include.h>
#include <float.h>

u64	 xray::timing::g_qpc_per_second;
bool xray::timing::g_cpu_supports_time_stamp	= false;

void xray::timing::initialize				( )
{
	QueryPerformanceFrequency	( ( PLARGE_INTEGER )&g_qpc_per_second );
	errno_t const error			= _controlfp_s( 0, _PC_24, MCW_PC );
	XRAY_UNREFERENCED_PARAMETER	( error );
	R_ASSERT					( !error );
}

u64 xray::timing::query_performance_counter	( )
{
	u64							result;
	QueryPerformanceCounter		( ( PLARGE_INTEGER )&result );
	return						(result);
}