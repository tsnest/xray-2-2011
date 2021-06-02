////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "tasks_type.h"
#include "tasks_thread_pool.h"
#include <xray/tasks_thread_tls.h>
#include <xray/intrusive_queue.h>

namespace xray {
namespace tasks {

class task_type_cmp
{
public:
	bool operator () (task_type const & left, task_type const & right) const
	{
		return			left.min_tasks_ordinal() < right.min_tasks_ordinal();
	}
};

class task_manager 
{
public:
						task_manager						();

	void				spawn_task							(task::function const & function, 
															 task_type *			type, 
															 task *					parent);
	task *				grab_next_task						();
	
	task *				get_current_user_thread_root_task	();

	task_allocator *	get_task_allocator					() { return & m_task_allocator; }

	bool				zero_tasks							();

	void				check_all_free						() { m_task_allocator.check_all_free(); }

	void				deallocate_task						(task * const task) { m_task_allocator.deallocate(task); }

	void				collect_garbage						(u32 * out_tasks_freed_count = NULL);

	void				fill_stats							(strings::text_tree_item & stats);

private:
	struct collector
	{
		collector (u32 * const tasks_freed_count) : m_tasks_freed_count(tasks_freed_count) { ; }
		void operator () (task_type * const task_type)
		{
			while ( !task_type->m_tasks.empty() )
			{
				task * const task		=	task_type->m_tasks.front();
				if ( task->m_state != task::state_allocated )
				{
					task_type->m_tasks.pop_front();
					if ( m_tasks_freed_count && task->m_reference_counter == 1 )
						++ * m_tasks_freed_count;

					task->decrement_reference_count_and_deallocate_when_zero();
				}
				else
				{
					break;
				}
			}
		}
	
		u32 *	m_tasks_freed_count;
	};

private:
	pbyte				get_task_allocator_base				();

	typedef intrusive_queue<task, task, & task::m_next_task_in_full_queue>	tasks_full_queue;
	tasks_full_queue					m_pending_tasks;
//	char								m_cachel_line_pad[XRAY_MAX_CACHE_LINE_SIZE];
	
	threading::mutex_tasks_unaware		m_mutex_collect_garbage;
	threading::atomic64_type			m_task_ordinal;
	threading::atomic64_type			m_oldest_task_ordinal;

	enum {
		task_age_require_immediate_execution	=	50*1024,
		task_age_difference_does_not_matter		=	20,
	};

	task_allocator						m_task_allocator;
	XRAY_MAX_CACHE_LINE_PAD;
	u32									m_current_thread_task_tls_key;

//	threading::atomic32_type			m_grabbing_next_task_threads_count;
	threading::atomic32_type			m_collecting_garbage;

	friend class						task_queue;
};

static task_manager						s_task_manager;

task_manager::task_manager () 
{
	m_task_ordinal							=	0;
	m_oldest_task_ordinal					=	0;
	m_collecting_garbage					=	false;
//	m_grabbing_next_task_threads_count		=	0;
	m_current_thread_task_tls_key			=	threading::tls_create_key();
}

pbyte   task_manager::get_task_allocator_base ()
{
	return										m_task_allocator.get_base();
}

bool   task_manager::zero_tasks ()
{
	return										m_pending_tasks.empty();
}

task *   task_manager::get_current_user_thread_root_task ()
{
	thread_tls * const tls					=	current_task_thread_tls();
	ASSERT										(tls);
	return										& tls->user_thread_root_task;
}

task_type_list_container *		get_task_type_list							();
void							wait_while_all_threads_end_grab_next_task	();

void   task_manager::collect_garbage (u32 * const out_tasks_freed_count)
{
	XRAY_UNREFERENCED_PARAMETER					( out_tasks_freed_count );
	threading::mutex_tasks_unaware_raii	raii	(m_mutex_collect_garbage);

	threading::interlocked_exchange				(m_collecting_garbage, true);

	wait_while_all_threads_end_grab_next_task	();

	if ( out_tasks_freed_count )
		* out_tasks_freed_count				=	0;

	// make this somehow working
	for (;;)
	{
		task * const task					=	m_pending_tasks.front();
		if ( !task || task->m_state == task::state_allocated )
			break;

		m_pending_tasks.pop_front				();
		if ( out_tasks_freed_count && task->m_reference_counter == 1 )
			++ * out_tasks_freed_count;

		task->decrement_reference_count_and_deallocate_when_zero();
	}

	task_type_list_container * task_type_list	=	get_task_type_list();

	collector	collector(out_tasks_freed_count);
		
	if ( task_type_list )
		task_type_list->for_each				(collector);

	threading::interlocked_exchange				(m_collecting_garbage, false);
}

void   on_new_task ();

void   task_manager::spawn_task (task_function const &  function, 
								 task_type * const		type, 
								 task *					parent)
{
	XRAY_UNREFERENCED_PARAMETER					( type );
	XRAY_UNREFERENCED_PARAMETER					( function );

	task *  new_task						=	m_task_allocator.allocate();
	if ( !new_task )
	{
		collect_garbage							();
		new_task							=	m_task_allocator.allocate();
		R_ASSERT								(new_task, "tasks: out of memory" );
	}
	
	threading::atomic64_value_type saved_ordinal;	// beware uninitialized
	threading::atomic64_value_type next_ordinal;	// beware uninitialized
	do
	{
		saved_ordinal						=	m_task_ordinal;
		next_ordinal						=	saved_ordinal + 1;
	}
	while ( threading::interlocked_compare_exchange(m_task_ordinal, next_ordinal, saved_ordinal) 
			!= saved_ordinal ) ;

	thread_tls * const tls					=	current_task_thread_tls();
	if ( !parent )
	{
		parent								=	tls->current_task;
		ASSERT									(parent);
	}

	new (new_task)	task						(function, type, next_ordinal, parent);

	if ( parent )
		parent->on_child_task_added				(new_task);

	m_pending_tasks.push_back					(new_task);
	type->m_tasks.push_back						(new_task);

	on_new_task									();
}

task *   task_manager::grab_next_task ()
{
	while ( m_collecting_garbage ) { ; }		// spin while collecting 

	struct grabbing_next_task_thread_count_raii_helper
	{
		grabbing_next_task_thread_count_raii_helper	(threading::atomic32_type* const in_grab_next_task) :	m_in_grab_next_task(in_grab_next_task)
														{ threading::interlocked_exchange(* in_grab_next_task, true); }
	   ~grabbing_next_task_thread_count_raii_helper	()	{ threading::interlocked_exchange(* m_in_grab_next_task, false); }
		threading::atomic32_type *			m_in_grab_next_task;
	};

	thread_tls * const tls					=	current_task_thread_tls();
	grabbing_next_task_thread_count_raii_helper	raii_helper(& tls->in_grab_next_task);

	for (;;)
	{
		// pop front if task ordinal is not too far away from oldest task ordinal
		task * result_task					=	NULL;
		task_type * const last_task_type	=	tls->last_task_type;
		bool popped_from_type				=	false;
		if ( last_task_type )
		{
			if ( m_oldest_task_ordinal + task_age_require_immediate_execution > 
				 last_task_type->m_min_task_ordinal )
			{
				result_task					=	last_task_type->m_tasks.pop_front();
				popped_from_type			=	true;
			}
		}

		if ( !result_task )
			result_task						=	m_pending_tasks.pop_front();

		if ( !result_task )
			return								NULL;

		bool const grabbed					=	result_task->try_grab();
		result_task->decrement_reference_count_and_deallocate_when_zero	();

		if ( grabbed )
		{
			if ( !popped_from_type )
			{
				threading::atomic64_type const new_full_list_ordinal	=	result_task ? result_task->m_ordinal : 0;
				threading::interlocked_compare_exchange	
					(m_oldest_task_ordinal, new_full_list_ordinal, m_oldest_task_ordinal);
			}
			else
			{
				threading::atomic64_type const new_type_list_ordinal	=	result_task ? result_task->m_ordinal : 0;
				threading::interlocked_compare_exchange	
					(last_task_type->m_min_task_ordinal, new_type_list_ordinal, last_task_type->m_min_task_ordinal);
			}

			tls->current_task				=	result_task;
			tls->last_task_type				=	result_task->m_type;
			return								result_task;
		}
	}
}

tasks::thread_pool *	get_thread_pool();

void   task_manager::fill_stats			(strings::text_tree_item & stats)
{
	get_thread_pool()->fill_stats					(stats);
}

void   spawn_task (task::function const & function, task_type * type, task * parent)
{
	s_task_manager.spawn_task					(function, type, parent);
}

task *   grab_next_task ()
{
	return										s_task_manager.grab_next_task ();
}

bool   zero_tasks ()
{
	return										s_task_manager.zero_tasks();
}

void   check_all_free ()
{
#if 0
	s_task_manager.check_all_free				();
#endif // #ifndef USE_MPMC_QUEUE_FOR_TYPE
}

void   deallocate_task (task * const task)
{
	s_task_manager.deallocate_task				(task);
}

void   collect_garbage (u32 * const out_tasks_freed_count)
{
	s_task_manager.collect_garbage				(out_tasks_freed_count);
}

void	on_thread_spawn	( )
{
	register_current_thread_as_core_user	( );
}

void    fill_stats (strings::text_tree_item & stats)
{
	s_task_manager.fill_stats				(stats);
}

} // namespace tasks
} // namespace xray