////////////////////////////////////////////////////////////////////////////
//	Created		: 25.11.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TASKS_AWARE_EVENT_H_INCLUDED
#define TASKS_AWARE_EVENT_H_INCLUDED

#include <xray/threading_event.h>

namespace xray {
namespace tasks {

class event : private threading::event
{
public:
	typedef	threading::event::wait_result_enum		wait_result_enum;
public:
						event	(bool initial_state = false) : threading::event(initial_state) {}
	void				set		(bool value) { threading::event::set(value); }
	wait_result_enum	wait	(u32 max_wait_time_ms);
};

} // namespace tasks
} // namespace xray

#endif // #ifndef TASKS_AWARE_EVENT_H_INCLUDED