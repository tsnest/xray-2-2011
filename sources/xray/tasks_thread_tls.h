////////////////////////////////////////////////////////////////////////////
//	Created		: 25.11.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TASKS_THREAD_TLS_H_INCLUDED
#define TASKS_THREAD_TLS_H_INCLUDED

#include <xray/tasks_task.h>

namespace xray {
namespace tasks {

class thread_pool;

class thread_tls : private boost::noncopyable
{
public:
	enum type_enum				{ type_task_thread, type_user_thread };
	enum state_enum				{ state_active, state_inactive, state_locked };

public:
	thread_tls					() : thread_type(type_task_thread), pool(NULL), thread_index(0),
									 thread_id(0), hardware_thread(0), current_task(NULL), executed_tasks_count(0),
									 last_task_type(NULL), state(state_inactive), in_grab_next_task(0) {}
	bool						is_task_thread () const { return thread_type == type_task_thread; }
	bool						is_user_thread () const { return thread_type == type_user_thread; }

	void						thread_proc_impl();
	void						thread_proc		();

public:
	XRAY_MAX_CACHE_LINE_PAD;
	threading::event_tasks_unaware	event_should_work;
	threading::atomic32_type	state;
	XRAY_MAX_CACHE_LINE_PAD;
	task						user_thread_root_task;
	threading::event_tasks_unaware	event_start_thread_work;
	threading::event_tasks_unaware	event_pause_ended;
	threading::event				event_wait_for_children;
	thread_pool *				pool;
	task *						current_task;
	task_type *					last_task_type;
	u32							thread_index;
	threading::thread_id_type	thread_id;
	u32							hardware_thread;
	type_enum					thread_type;
	threading::atomic32_type	in_grab_next_task;
	fixed_string<32>			thread_name;
	threading::atomic64_type	last_task_end_tick;
	threading::atomic32_type	executed_tasks_count;
};

XRAY_CORE_API	thread_tls *	current_task_thread_tls		();

// log through the tasking system logging wrapper
XRAY_CORE_API	void			log							(thread_tls * tls, pcstr format, ...);

} // namespace xray
} // namespace tasks

#endif // #ifndef TASKS_THREAD_TLS_H_INCLUDED