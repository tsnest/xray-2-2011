////////////////////////////////////////////////////////////////////////////
//	Module 		: platform.cpp
//	Created 	: 27.08.2006
//  Modified 	: 27.08.2006
//	Author		: Dmitriy Iassenev
//	Description : platform class
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <cs/core/platform.h>
#include <cs/core/os_include.h>		// QueryPerformanceFrequency

#if CS_PLATFORM_WINDOWS
#	pragma warning(push)
#	pragma warning(disable: 4201)
#	include <mmsystem.h>
#	pragma warning(pop)
#	pragma comment(lib, "winmm.lib")
#endif // #if CS_PLATFORM_WINDOWS

#if CS_PLATFORM_PS3
#	include <sys/sys_time.h>
#endif // #if CS_PLATFORM_PS3


using cs::core::platform;

platform						cs::core::g_platform;

platform::platform				( )
{
#if CS_PLATFORM_WINDOWS
	timeBeginPeriod				( 1 );
#endif // #if CS_PLATFORM_WINDOWS

#if CS_PLATFORM_WINDOWS | CS_PLATFORM_XBOX_360
	QueryPerformanceFrequency	( (PLARGE_INTEGER)&qpc_per_second );
#elif CS_PLATFORM_PS3 // #if CS_PLATFORM_WINDOWS | CS_PLATFORM_XBOX_360
	qpc_per_second				= sys_time_get_timebase_frequency( );
#else // #elif CS_PLATFORM_PS3
#	error get high performance counter frequency here
#endif // #if CS_PLATFORM_WINDOWS | CS_PLATFORM_XBOX_360
}

u64 platform::get_QPC			( )
{
#if CS_PLATFORM_WINDOWS | CS_PLATFORM_XBOX_360
	u64 result;
	QueryPerformanceCounter		( (PLARGE_INTEGER)&result );
	return						result;
#elif CS_PLATFORM_PS3 // #if CS_PLATFORM_WINDOWS | CS_PLATFORM_XBOX_360
	return						sys_time_get_system_time( );
#else // #elif CS_PLATFORM_PS3
#	error get high performance counter value here
#endif // #if CS_PLATFORM_WINDOWS | CS_PLATFORM_XBOX_360
}