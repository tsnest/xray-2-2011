////////////////////////////////////////////////////////////////////////////
//	Created		: 19.11.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "tasks_thread_pool.h"

namespace xray {
namespace tasks {

//-----------------------------------------------------------------------------------
// thread_pool
//-----------------------------------------------------------------------------------

void   thread_pool::on_task_thread_started (thread_tls * const tls)
{
	threading::tls_set_value					(m_thread_tls_key, (pvoid)tls);
	if ( threading::interlocked_increment(m_num_task_threads_started) == (long)m_task_thread_tls.size() )
		m_all_task_threads_started.set			(true);
}

void   thread_pool::on_task_thread_exited (thread_tls * const tls)
{
	XRAY_UNREFERENCED_PARAMETER					(tls);
	threading::mutex_tasks_unaware_raii raii	(m_thread_exiting_mutex);

	if ( (u32)threading::interlocked_increment(m_num_task_threads_exited) == m_task_thread_tls.size() )
		m_all_task_threads_destroyed.set		(true);
}

void   thread_pool::deactivate_task_thread (thread_tls * const tls)
{
	R_ASSERT									(tls->state == thread_tls::state_active);
	log											(tls, "%d>deactivated(%d)", 
												 tls->hardware_thread, m_core_thread_count[tls->hardware_thread]);

	threading::interlocked_exchange				(tls->state, thread_tls::state_inactive);
	threading::interlocked_decrement			(m_core_thread_count[tls->hardware_thread]);
	threading::interlocked_decrement			(m_active_task_thread_count);
}

// can return false in case when some parallel thread resumed this thread faster
bool   thread_pool::try_activate_task_thread (thread_tls & tls, u32 const use_hardware_thread)
{
	threading::atomic32_value_type const previous_state				=	threading::interlocked_compare_exchange
												(tls.state, thread_tls::state_active, thread_tls::state_inactive);
	
	if ( previous_state != thread_tls::state_inactive )
		return									false;
	
	threading::interlocked_increment			(m_active_task_thread_count);

	tls.hardware_thread						=	use_hardware_thread;
	threading::atomic32_value_type const resulting_core_thread_count	=	threading::interlocked_increment
												(m_core_thread_count[tls.hardware_thread]);
	log											(& tls, "%d>activated(%d)", use_hardware_thread, resulting_core_thread_count);
	tls.event_should_work.set					(true);
	return										true;
}

bool   thread_pool::current_thread_core_is_oversubscribed () const
{
	u32 const hardware_thread				=	threading::current_thread_affinity();
	if ( m_core_thread_count[hardware_thread] <= 1 )
		return									false;

	u32 const active_thread_count			=	calculate_active_task_thread_count();
	if ( active_thread_count <= m_min_permanent_working_threads )
		return									false;

	return										true;
}

bool   thread_pool::deactivate_if_oversubscribed (thread_tls * const tls)
{
	if ( !current_thread_core_is_oversubscribed() )
		return									false;

	threading::atomic32_value_type const resulting_value				=	threading::interlocked_decrement
												(m_core_thread_count[tls->hardware_thread]);

	if ( resulting_value == 0 )
	{
		// some other thread just deactivated
		threading::interlocked_increment		(m_core_thread_count[tls->hardware_thread]);
		return									false;
	}

	log											(tls, "%d>deactivated(%d)", 
												 tls->hardware_thread, resulting_value + 1);
	threading::interlocked_exchange				(tls->state, thread_tls::state_inactive);
	threading::interlocked_decrement			(m_active_task_thread_count);
	return										true;
}

bool   zero_tasks ();

void   thread_pool::on_current_thread_locks ()
{
	u32 const hardware_thread				=	threading::current_thread_affinity();
	R_ASSERT									(hardware_thread != u32(-1), "only threads that called set_current_thread_affinity can call this function:)");
	threading::atomic32_value_type const resulting_value				=	threading::interlocked_decrement
												(m_core_thread_count[hardware_thread]);

	thread_tls * const tls					=	get_thread_tls();
	log											(tls->is_task_thread() ? tls : NULL, 
												 "%d>locked(%d)", hardware_thread, resulting_value+1);
	if ( tls->is_task_thread() )
	{
		threading::interlocked_exchange			(tls->state, thread_tls::state_locked);
		threading::interlocked_decrement		(m_active_task_thread_count);
	}

	if ( !resulting_value && !zero_tasks() )
	{
		for ( thread_tls_container::iterator	it	=	m_task_thread_tls.begin();
												it	!=	m_task_thread_tls.end();
											  ++it )
		{
			thread_tls & thread_tls				=	* it;

			if ( thread_tls.state == thread_tls::state_inactive )
			{
				if ( try_activate_task_thread(thread_tls, hardware_thread) )
					break;
			}				
		}
	}
}

void   thread_pool::on_current_thread_unlocks ()
{
	u32 const hardware_thread				=	threading::current_thread_affinity();
	R_ASSERT									(hardware_thread != u32(-1), "only threads that called set_current_thread_affinity can call this function:)");
	threading::atomic32_value_type const new_core_thread_count		=	threading::interlocked_increment
												(m_core_thread_count[hardware_thread]);

	thread_tls * const tls					=	get_thread_tls();
	if ( tls->is_task_thread() )
	{
		R_ASSERT								(tls->state == thread_tls::state_locked);
		threading::interlocked_exchange			(tls->state, thread_tls::state_active);
		threading::interlocked_increment		(m_active_task_thread_count);
		log										(tls, "%d>unlocked(%d)", hardware_thread, new_core_thread_count);
	}
	else
		log										(NULL, "%d>unlocked(%d)", hardware_thread, new_core_thread_count);
}

void   thread_pool::on_new_task ()
{
	u32 core_with_no_threads				=	u32(-1);
	u32 core_with_minimum_threads_index		=	u32(-1);
	long core_with_minimum_threads_value	=	-1;
	core_thread_count_container::iterator	begin	=	m_core_thread_count.begin();
	core_thread_count_container::iterator	end		=	m_core_thread_count.end();
	for ( core_thread_count_container::iterator	it	=	begin;
												it	!=	end;
											  ++it )
	{
		long const thread_count				=	* it;
		if ( core_with_minimum_threads_index == u32(-1) || thread_count < core_with_minimum_threads_value )
		{
			core_with_minimum_threads_index		=	u32(it - begin);
			core_with_minimum_threads_value		=	*it;
		}

		if ( !thread_count )
		{
			core_with_no_threads			=	u32(it - begin);
			break;
		}
	}

	bool all_permament_threads_active		=	true;
	if ( core_with_no_threads == u32(-1) ) // optimization
		all_permament_threads_active		=	calculate_active_task_thread_count() >= m_min_permanent_working_threads;

 	if ( core_with_no_threads != u32(-1) || !all_permament_threads_active )
 	{
		for ( thread_tls_container::iterator	it	=	m_task_thread_tls.begin();
												it	!=	m_task_thread_tls.end();
											  ++it )
		{
			thread_tls & tls				=	* it;
			if ( tls.state == thread_tls::state_inactive )
			{
 				if ( try_activate_task_thread(tls, core_with_minimum_threads_index) )
 					return;
			}
		}
 	}
}

thread_tls *	current_task_thread_tls			();
void			deallocate_task					(task * const);

execute_children_result_enum   thread_pool::execute_children (task * const parent)
{
	while ( identity(true) )
	{
		tasks::task * const current_task	=	parent->grab_next_child();
		if ( !current_task )
			return								execute_children_result_success;

		current_task->execute					();
		
		if ( current_thread_core_is_oversubscribed() )
			return								execute_children_result_thread_should_sleep;
	}

	UNREACHABLE_CODE(return execute_children_result_success);
}

void   thread_pool::wait_for_all_children ()
{
	thread_tls * const tls					=	get_thread_tls();
	task * current_task						=	tls->current_task;
	R_ASSERT									(current_task);
	wait_for_task_list							(current_task);

	if ( tls->thread_type == thread_tls::type_user_thread )
	{
		current_task->unlink_from_children		();
		tls->current_task					=	& tls->user_thread_root_task;
	}
}

void   thread_pool::wait_for_task_list (task * parent)
{
	thread_tls * const tls					=	get_thread_tls();
	if ( execute_while_wait_for_children() == execute_while_wait_for_children_true )
	{
		log										(tls, "%d>exec children", tls->hardware_thread);
		if ( execute_children(parent) == execute_children_result_success &&
			 parent->no_children_to_execute() )
		{
			return;
		}
	}

	log											(tls, "%d>wait4children", tls->hardware_thread);
	
	parent->set_event_for_wait_children			(& tls->event_wait_for_children);
	if ( !parent->no_children_to_execute() )
		parent->event_for_wait_children()->wait	(threading::wait_time_infinite);

	R_ASSERT									(parent->no_children_to_execute());
	parent->set_event_for_wait_children			(NULL);
}

void   thread_pool::fill_stats			(strings::text_tree_item &		stats, 
										 thread_tls_container &			tls_array,
										 thread_tls::type_enum			filter,
										 u32 *							task_threads_per_core)
{
	for ( thread_tls_container::const_iterator	it		=	tls_array.begin(),
												it_end	=	tls_array.end();
												it		!=	it_end;
												++it )
	{
		thread_tls const & tls			=	* it;
		if ( !tls.thread_name.length() )
			continue;

		//if ( tls.thread_type != filter )
		//	continue;

		u64 const ms_after_last_task	=	tls.last_task_end_tick ? 
											((ticks_elapsed() - tls.last_task_end_tick) * 1000) / timing::g_qpc_per_second 
											: 0;

		bool const executing			=	(tls.current_task && tls.current_task != & tls.user_thread_root_task) 
											|| (ms_after_last_task < 100);
		if ( executing )
			++task_threads_per_core[tls.hardware_thread];
		
		fixed_string<256>					state_string;
		if ( tls.state == thread_tls::state_locked )
			state_string				=	"SLEEP";
		else if ( tls.state == thread_tls::state_inactive )
			state_string				=	"IDLE ";
		else if ( executing )
			state_string				=	"TASK ";
		else
		{
			if ( tls.thread_type == thread_tls::type_task_thread )
				state_string			=	"TASK ";
			else
				state_string			=	"USER ";
		}

		stats.new_childf(
			tls.thread_name.c_str(),
			"state(%s), hardware core(%d), executed(%d)",
			state_string.c_str(),
			tls.hardware_thread,
			tls.executed_tasks_count
		);
	}
}

void   thread_pool::fill_stats			(strings::text_tree_item & stats)
{
	u32 * const task_threads_per_core	=	(u32 *)ALLOCA(sizeof(u32) * m_core_thread_count.size());
	for ( u32 i=0; i<m_core_thread_count.size(); ++i )
		task_threads_per_core[i]		=	0;	

	//fill_stats								(* stats.new_child("task threads"), 
	//										 m_task_thread_tls, 
	//										 thread_tls::type_task_thread, 
	//										 task_threads_per_core);

	fill_stats								(* stats.new_child("threads"), 
											 m_user_thread_tls, 
											 thread_tls::type_user_thread, 
											 task_threads_per_core);

	strings::text_tree_item & cores_item	=	* stats.new_child("CORES");
	
	for ( u32 i=0; i<m_core_thread_count.size(); ++i )
	{
		u32 const threads_count			=	m_core_thread_count[i];
		cores_item.new_childf				(fixed_string512::createf("CORE %d", i).c_str(), 
											 "(%d user + %d task)", 
											 threads_count - task_threads_per_core[i], 
											 task_threads_per_core[i]);
	}
}

//-----------------------------------------------------------------------------------
// thread_pool thread procedure
//-----------------------------------------------------------------------------------

void   thread_tls::thread_proc_impl ()
{
	bool deactivated						=	true;
	R_ASSERT									(state == state_inactive);
	pool->on_task_thread_started				(this);

	while ( identity(true) )
	{
		if ( deactivated )
		{
			event_should_work.wait					(threading::wait_time_infinite);
			threading::set_current_thread_affinity	(hardware_thread);
			deactivated						=	false;
		}

		R_ASSERT								(state == state_active);

		if ( pool->paused() )
		{
			pool->on_task_thread_paused			();
			event_pause_ended.wait				(threading::wait_time_infinite);
			pool->on_task_thread_resumed		();
		}

		tasks::task * current_task			=	grab_next_task();
		if ( !current_task )
		{
			if ( pool->destroying() )
				break;

			deactivated						=	true;
			pool->deactivate_task_thread		(this);

			current_task					=	grab_next_task();
			if ( !current_task )
				continue;
			
			deactivated						=	false;
			pool->try_activate_task_thread			(*this, hardware_thread);
			event_should_work.wait					(threading::wait_time_infinite);
			threading::set_current_thread_affinity	(hardware_thread);
		}

		current_task->execute					();
		threading::interlocked_exchange			(last_task_end_tick, pool->ticks_elapsed());
		threading::interlocked_increment		(executed_tasks_count);

   		if ( !pool->destroying() )
   			deactivated						=	pool->deactivate_if_oversubscribed(this);
	}

	pool->deactivate_task_thread				(this);
}

void   thread_tls::thread_proc ()
{
	event_start_thread_work.wait				(threading::wait_time_infinite);
	
	thread_proc_impl							();

	pool->on_task_thread_exited					(this);
}

} // namespace tasks
} // namespace xray