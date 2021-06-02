////////////////////////////////////////////////////////////////////////////
//	Created 	: 09.04.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/threading_event.h>
#include <sys/synchronization.h>

namespace xray { 
namespace threading { 

XRAY_CORE_API u32 const wait_time_infinite	= u32(-1);

event::event			( bool const initial_state )
{
	COMPILE_ASSERT		( sizeof(m_event) == sizeof(sys_event_flag_t), please_change_buffer_size );

	sys_event_flag_attribute_t	attribute;
	sys_event_flag_attribute_initialize(attribute);
	attribute.attr_protocol	= SYS_SYNC_PRIORITY;
	attribute.attr_pshared = SYS_SYNC_NOT_PROCESS_SHARED;
	attribute.type		= SYS_SYNC_WAITER_MULTIPLE;
	attribute.name[0]	= 0;

	int const error		= sys_event_flag_create( (sys_event_flag_t*)&m_event, &attribute, initial_state ? 0 : 1 );
	if ( error != CELL_OK ) {
		if ( error == EINVAL )
			LOG_ERROR	( "an invalid value has been specified in the attribute structure");
		else if ( error == ENOMEM )
			LOG_ERROR	( "not enough memory to create new flag event");
		else if ( error == EAGAIN )
			LOG_ERROR	( "not enough ids to create new flag event");
		else if ( error == EFAULT )
			LOG_ERROR	( "the specified address is invalid");
		else
			UNREACHABLE_CODE( );
	}

	R_ASSERT_CMP		( error, ==, CELL_OK );

	set					( initial_state );
}

event::~event			( )
{
	int const error		= sys_event_flag_destroy( (sys_event_flag_t&)m_event );

	if ( error != CELL_OK ) {
		if ( error == ESRCH )
			LOG_ERROR	( "mutex has been destroyed already");
		else if ( error == EBUSY )
			LOG_ERROR	( "event is busy");
		else
			UNREACHABLE_CODE( );
	}

	R_ASSERT_CMP		( error, ==, CELL_OK );
}

void event::set			( bool const value )
{
	int error;

	if ( value )
		error			= sys_event_flag_set( (sys_event_flag_t&)m_event, 1 );
	else
		error			= sys_event_flag_clear( (sys_event_flag_t&)m_event, 0 );

	R_ASSERT_CMP		( error, ==, CELL_OK );
}

event::wait_result_enum event::wait	( u32 const max_wait_time_ms )
{
	u64 result;
	int const error		=
		sys_event_flag_wait(
			(sys_event_flag_t&)m_event,
			1,
			SYS_EVENT_FLAG_WAIT_AND | SYS_EVENT_FLAG_WAIT_CLEAR,
			&result,
			(max_wait_time_ms == wait_time_infinite) ? SYS_NO_TIMEOUT : u64(max_wait_time_ms)*1000
		);

	if ( error == CELL_OK ) {
		R_ASSERT_CMP	( result, ==, 1 );
		return			event::wait_result_signaled;
	}

	if ( error == ETIMEDOUT )
		return			event::wait_result_still_nonsignaled;

	if ( error == ECANCELED )
		return			event::wait_result_failed;

	if ( error == EINVAL )
		LOG_ERROR		( "condition for event flag is invalid" );
	else if ( error == ESRCH )
		LOG_ERROR		( "flag event has been already destroyed" );
	else if ( error == EPERM )
		LOG_ERROR		( "multiple threads performed a wait (should not be returned since we pass SYS_SYNC_WAITER_MULTIPLE on creation)" );
	else if ( error == EFAULT )
		LOG_ERROR		( "invalid address" );
	else
		NODEFAULT		( return event::wait_result_failed );

	UNREACHABLE_CODE	( return event::wait_result_failed );
}

pvoid event::get_handle	( ) const
{
	return				(sys_event_flag_t*)&m_event;
}

} // namespace threading
} // namespace xray