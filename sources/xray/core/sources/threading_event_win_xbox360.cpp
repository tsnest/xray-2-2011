////////////////////////////////////////////////////////////////////////////
//	Created 	: 09.04.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/threading_event.h>
#include <xray/os_include.h>

namespace xray { 
namespace threading { 

XRAY_CORE_API u32 const wait_time_infinite	= INFINITE;

event_tasks_unaware::event_tasks_unaware			( bool const initial_state )
{
	COMPILE_ASSERT		( sizeof(m_event) == sizeof(HANDLE), please_change_buffer_size );
	(HANDLE&)m_event	= CreateEvent( NULL, FALSE, initial_state, NULL );
	R_ASSERT			( (HANDLE&)m_event );
}

event_tasks_unaware::~event_tasks_unaware			( )
{
	CloseHandle			( (HANDLE&)m_event );
}

void event_tasks_unaware::set			( bool const value )
{
	if ( value )
		SetEvent		( (HANDLE&)m_event );
	else
		ResetEvent		( (HANDLE&)m_event );
}

event_tasks_unaware::wait_result_enum event_tasks_unaware::wait	( u32 const max_wait_time_ms )
{
	DWORD const result	= WaitForSingleObject( (HANDLE&)m_event, max_wait_time_ms );
	if ( result == WAIT_OBJECT_0 )
		return			event_tasks_unaware::wait_result_signaled;

	if ( result == WAIT_TIMEOUT )
		return			event_tasks_unaware::wait_result_still_nonsignaled;

	if ( result == WAIT_FAILED )
		return			event_tasks_unaware::wait_result_failed;

	UNREACHABLE_CODE	( return event_tasks_unaware::wait_result_failed );
}

pvoid event_tasks_unaware::get_handle	( ) const
{
	return				(HANDLE&)m_event;
}

} // namespace threading
} // namespace xray