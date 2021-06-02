////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2011
//	Author		: 
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/threading_active_object.h>

namespace xray {
namespace threading {

active_object::active_object (pcstr thread_name, pcstr thread_log_name, bool sleep_after_tick) 
	:	m_thread_id				(u32(-1)), 
		m_thread_started		(false), 
		m_thread_must_exit		(false), 
		m_thread_exited			(false), 
		m_sleep_after_tick		(sleep_after_tick)
{
	if ( threading::g_debug_single_thread )
		return;

	threading::thread_id_type const 
	device_thread_id					=	threading::spawn	(boost::bind(& active_object::thread_proc_main, this), 
																 thread_name, 
																 thread_log_name, 
																 0,
																 0,
																 threading::tasks_aware);

	threading::interlocked_exchange			(m_thread_id, device_thread_id);
	while ( !m_thread_started )
		threading::yield					(0);
}

void   active_object::tick ()
{
	if ( threading::g_debug_single_thread )
		tick_impl									();
}

active_object::~active_object ()
{
	if ( !threading::g_debug_single_thread )
	{
		threading::interlocked_exchange				(m_thread_must_exit, true);
		m_wakeup_event.set							(true);
		while ( !m_thread_exited )
			threading::yield						(0);
	}
}

void   active_object::wakeup_thread	()
{
	m_wakeup_event.set								(true);
}

void   active_object::thread_proc_main ()
{
	threading::interlocked_exchange					(m_thread_started, true);
	while ( !m_thread_must_exit )
	{
		if ( m_sleep_after_tick )
			m_wakeup_event.wait						(threading::wait_time_infinite);

		if ( m_thread_must_exit )
			break;
		
		tick_impl									();
	}

	threading::interlocked_exchange					(m_thread_exited, true);
}

} // namespace threading
} // namespace xray