////////////////////////////////////////////////////////////////////////////
//	Created		: 19.11.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TASKS_THREAD_POOL_H_INCLUDED
#define TASKS_THREAD_POOL_H_INCLUDED

#include <xray/tasks_task.h>
#include "tasks_type.h"
#include <xray/tasks_thread_tls.h>
#include <xray/text_tree.h>

namespace xray {
namespace tasks {

enum execute_children_result_enum	{	execute_children_result_thread_should_sleep, 
										execute_children_result_success,	};

class thread_pool
{
public:
						thread_pool					(u32	max_task_threads, 
													 u32	max_user_threads,
													 u32	min_permanent_working_threads,
													 execute_while_wait_for_children_enum,
													 do_logging_bool do_logging);
			void		initialize					( );
					   ~thread_pool					();

	u32					thread_tls_key				() const { return m_thread_tls_key; }
	u64					ticks_elapsed				() const { return m_timer.get_elapsed_ticks(); }
	thread_tls *		get_thread_tls				();
	void				on_new_task					();

	void				set_logging					(do_logging_bool do_logging) { m_do_logging = do_logging; }
	
	void				on_task_thread_started		(thread_tls * tls);
	void				on_task_thread_exited		(thread_tls * tls);
	void				deactivate_task_thread		(thread_tls * tls);

	void				pause_all_task_threads		();
	void				resume_all_task_threads		();
	void				on_task_thread_paused		();
	void				on_task_thread_resumed		();

	bool				destroying					() const { return !!m_destroying; }
	bool				paused						() const { return !!m_paused; }

	// can return false in case when some parallel thread resumed this thread faster
	bool				try_activate_task_thread	(thread_tls & tls, u32 use_hardware_thread);
	bool				deactivate_if_oversubscribed(thread_tls * tls);
	void 				on_current_thread_locks		();
	void 				on_current_thread_unlocks	();
	void 				register_current_thread_as_core_user		();
	u32					calculate_active_task_thread_count			() const;
	bool				current_thread_core_is_oversubscribed		() const;

	threading::event_tasks_unaware &	get_current_thread_wait_for_children_event	();

	void				wait_for_all_children		();
	void				wait_for_task_list			(task * parent);
	execute_children_result_enum   execute_children (task * parent);

	void				log							(thread_tls * tls, pcstr format, ...);
	void				log_columns_header			();
	bool				execute_while_wait_for_children () const { return m_execute_while_wait_for_children == execute_while_wait_for_children_true; }

	void				wait_while_all_threads_end_grab_next_task	();

	typedef buffer_vector<thread_tls>		thread_tls_container;

	void				fill_stats					(strings::text_tree_item & stats);
	void				fill_stats					(strings::text_tree_item &		stats, 
													 thread_tls_container &			tls,
													 thread_tls::type_enum			filter,
													 u32 *							task_threads_per_core);

private:
	execute_while_wait_for_children_enum	m_execute_while_wait_for_children;
	threading::mutex_tasks_unaware			m_thread_exiting_mutex;
	threading::event_tasks_unaware			m_threads_can_start_work;
	threading::event_tasks_unaware			m_all_task_threads_paused;
	threading::event_tasks_unaware			m_all_task_threads_resumed;
	threading::event_tasks_unaware			m_all_task_threads_started;
	timing::timer							m_timer;

	threading::atomic32_type				m_num_task_threads_exited;
	threading::atomic32_type				m_num_task_threads_started;
	threading::event						m_all_task_threads_destroyed;
	threading::atomic32_type				m_active_task_thread_count;

	u32										m_thread_tls_key;
	u32										m_user_thread_index;
	thread_tls_container					m_task_thread_tls;
	thread_tls_container					m_user_thread_tls;

	typedef buffer_vector<threading::atomic32_type>	core_thread_count_container;
	core_thread_count_container				m_core_thread_count;
	u32										m_min_permanent_working_threads;
	threading::atomic32_type				m_destroying;
	threading::atomic32_type				m_paused;
	threading::atomic32_type				m_num_paused_threads;
	bool									m_initialized;
	do_logging_bool							m_do_logging;

}; // class thread_pool

} // namespace xray
} // namespace tasks

#endif // #ifndef TASKS_THREAD_POOL_H_INCLUDED