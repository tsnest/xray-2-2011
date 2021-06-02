////////////////////////////////////////////////////////////////////////////
//	Created 	: 23.01.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/timing_functions.h>
#include "timing.h"
#include <sys/sys_time.h>
#include <sys/time_util.h>

u64	 xray::timing::g_qpc_per_second				= 0; // should be 79 800 000
bool xray::timing::g_cpu_supports_time_stamp	= false;

void xray::timing::initialize				( )
{
	g_qpc_per_second	= sys_time_get_timebase_frequency( );
}

u64 xray::timing::query_performance_counter	( )
{
	// mftb instruction has a problem because of a 4 cycle lag between
	// getting the lower 32 bit and higher ones. here we suppose that 
	// sys_time_get_system_time solves this problem like
	// QueryPerformanceCounter does it on Xbox360
//	return				__mftb( );
	register u64		now;
	SYS_TIMEBASE_GET	( now );
	return				now;
}