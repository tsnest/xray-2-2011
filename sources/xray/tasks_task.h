////////////////////////////////////////////////////////////////////////////
//	Created		: 11.11.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_TASKS_TASK_H_INCLUDED
#define XRAY_TASKS_TASK_H_INCLUDED

#include <xray/intrusive_list.h>
#include <xray/tasks_system.h>

//#define USE_MPMC_QUEUE_FOR_TYPE
//#define USE_MPMC_QUEUE_FOR_FULL

namespace xray {
namespace tasks {

void   deallocate_task (task * const task);

class task_type;
class task;
struct task_base
{
	task * m_next_task_in_child_queue;
};

class task : public task_base, public core::noncopyable
{
public:
	static u32 const references_held_by_system_count	=	4;

	typedef task_function								function;

public:
							task						();
							task						(function const &	function, 
														 task_type *		type, 
														 u64				priority, 
														 task *				parent);

	void					execute						();

private:
	void					on_child_task_added 		(task * child);
	void					on_child_task_ended 		();

	task *					grab_next_child				();

	void					set_event_for_wait_children	(threading::event * event);
	threading::event *		event_for_wait_children		() const { return m_event_wait_for_children; }

	void					unlink_from_children		();
	void					unlink_from_child			(task * child); 

	bool					no_children_to_execute		() const { return !m_child_counter; }
	inline bool				try_grab					()
	{
		return										threading::interlocked_compare_exchange
													(m_state, state_grabbed, state_allocated) == state_allocated;
	}

	void					decrement_reference_count_and_deallocate_when_zero	()
	{
		long const resulting_value				=	threading::interlocked_decrement(m_reference_counter);
		if ( !resulting_value )
		{
			deallocate_task							(this);
			return;
		}
	}

private:
	enum state_enum			{	state_deallocated,
								state_allocated,
								state_grabbed,
								state_executed,
								state_captured,		};
private:
	task *						m_next_task_in_allocator;
	task *						m_next_task_in_type_queue;
	task *						m_next_task_in_full_queue;
	intrusive_list<task_base, task *, & task_base::m_next_task_in_child_queue, threading::single_threading_policy>	m_children;
	threading::atomic32_type	m_child_counter;
	threading::event *			m_event_wait_for_children;

	function					m_function;
	u64							m_ordinal;
	task_type *					m_type;
	task *						m_parent;
	threading::atomic32_type	m_state;
	threading::atomic32_type	m_reference_counter;

	friend class				task_manager;
	friend class				task_allocator;
	friend class				task_type;
	friend class				thread_pool;
};

} // namespace tasks
} // namespace xray
#endif // #ifndef TASKS_TASK_H_INCLUDED