////////////////////////////////////////////////////////////////////////////
//	Created 	: 22.01.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_TIMING_FUNCTIONS_H_INCLUDED
#define XRAY_TIMING_FUNCTIONS_H_INCLUDED

#if XRAY_PLATFORM_WINDOWS
#	include <intrin.h>
#	pragma intrinsic(__rdtsc)
#endif // #if XRAY_PLATFORM_WINDOWS

namespace xray {
namespace timing {

XRAY_CORE_API	extern u64	g_qpc_per_second;
XRAY_CORE_API	u64			query_performance_counter	( );

XRAY_CORE_API	extern bool	g_cpu_supports_time_stamp;

inline			u64			get_clocks					( )
{
#if XRAY_PLATFORM_WINDOWS
	return		( __rdtsc( ) );
#else // #if XRAY_PLATFORM_WINDOWS
	return		( query_performance_counter( ) );
#endif // #if XRAY_PLATFORM_WINDOWS
}

inline u64					get_QPC						( )
{
	if ( g_cpu_supports_time_stamp )
	{
		return		get_clocks( );
	}
	else
	{
		return		( query_performance_counter( ) );
	}
}

} // namespace timing
} // namespace xray

#endif // #ifndef XRAY_TIMING_FUNCTIONS_H_INCLUDED