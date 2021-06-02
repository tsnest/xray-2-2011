////////////////////////////////////////////////////////////////////////////
//	Created		: 17.11.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/threading_event.h>

namespace xray {

namespace tasks {

u32		spin_count_before_notify_task_system	();
void	on_current_thread_locks					();
void	on_current_thread_unlocks				();

} // namespace tasks

namespace threading {

event::wait_result_enum   event::wait			( u32 const max_wait_time_ms )
{
	if ( !max_wait_time_ms )
		return							m_event.wait(max_wait_time_ms);

	for ( u32 i=0; i<tasks::spin_count_before_notify_task_system(); ++i )
		if ( m_event.wait(0) == event_tasks_unaware::wait_result_signaled )
			return						event_tasks_unaware::wait_result_signaled;

	tasks::on_current_thread_locks		();
	wait_result_enum const out_result	= m_event.wait(max_wait_time_ms);
	tasks::on_current_thread_unlocks	();

	return								out_result;
}

} // namespace threading
} // namespace xray