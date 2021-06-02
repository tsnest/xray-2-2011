////////////////////////////////////////////////////////////////////////////
//	Created 	: 25.09.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/threading_mutex.h>

#define	_WIN32_WINNT 0x0500				// for TryEnterCriticalSection
#include <xray/os_include.h>

using xray::threading::mutex_tasks_unaware;

mutex_tasks_unaware::mutex_tasks_unaware		()
{
	COMPILE_ASSERT				( sizeof( CRITICAL_SECTION ) <= sizeof( m_mutex ), please_change_buffer_size );
//	InitializeCriticalSection	( &( CRITICAL_SECTION& )m_mutex );
#if XRAY_PLATFORM_WINDOWS
	BOOL const result			= 
#endif // #if XRAY_PLATFORM_WINDOWS
		InitializeCriticalSectionAndSpinCount( &( CRITICAL_SECTION& )m_mutex, 10000 );

#if XRAY_PLATFORM_WINDOWS
	if ( !result )
		NODEFAULT				( );
#endif // #if XRAY_PLATFORM_WINDOWS
}

mutex_tasks_unaware::~mutex_tasks_unaware		()
{
	DeleteCriticalSection		( &( CRITICAL_SECTION& )m_mutex );
}

void mutex_tasks_unaware::lock	() const
{
//	debug::printf				(temp, "lock   0x%x[%d]\r\n", *(u32*)&m_mutex, GetCurrentThreadId());
	EnterCriticalSection		( &( CRITICAL_SECTION& )m_mutex );
}

void mutex_tasks_unaware::unlock	() const
{
//	debug::printf				(temp, "unlock 0x%x[%d]\r\n", *(u32*)&m_mutex, GetCurrentThreadId());
	LeaveCriticalSection		( &( CRITICAL_SECTION& )m_mutex );
}

bool mutex_tasks_unaware::try_lock	() const
{
	return						( !!TryEnterCriticalSection( &( CRITICAL_SECTION& )m_mutex ) );
}