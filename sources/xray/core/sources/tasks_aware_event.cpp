////////////////////////////////////////////////////////////////////////////
//	Created		: 25.11.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/tasks_aware_event.h>

namespace xray {
namespace tasks {

u32		spin_count_before_notify_task_system	();
void	on_current_thread_locks					();
void	on_current_thread_unlocks				();

event::wait_result_enum   event::wait (u32 const max_wait_time_ms)
{
	if ( !max_wait_time_ms )
		return									threading::event::wait(max_wait_time_ms);

	for ( u32 i=0; i<tasks::spin_count_before_notify_task_system(); ++i )
		if ( event::wait(0) == threading::event::wait_result_signaled )
			return								threading::event::wait_result_signaled;

	tasks::on_current_thread_locks				();
	threading::event::wait_result_enum const out_result	=	threading::event::wait(max_wait_time_ms);
	tasks::on_current_thread_unlocks			();

	return										out_result;
}

} // namespace tasks
} // namespace xray