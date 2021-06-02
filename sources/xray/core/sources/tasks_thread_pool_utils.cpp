////////////////////////////////////////////////////////////////////////////
//	Created		: 25.11.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "tasks_thread_pool.h"

namespace xray {
namespace tasks {

//-----------------------------------------------------------------------------------
// spin count
//-----------------------------------------------------------------------------------

command_line::key static		s_spin_count_key	("spin_count", 
													 "", 
													 "task system", 
													 "number of trylocks before task system is notified about long lock");

u32 static const				default_spin_count	=	5;
static threading::mutex_tasks_unaware		s_mutex;

static threading::atomic32_type	s_spin_count	=	default_spin_count;

u32  spin_count_before_notify_task_system ()
{
	return										s_spin_count;
}

//-----------------------------------------------------------------------------------
// thread_pool
//-----------------------------------------------------------------------------------


thread_pool::thread_pool(u32 const								max_task_threads, 
						 u32 const								max_user_threads,
						 u32 const								min_permanent_working_threads,
						 execute_while_wait_for_children_enum	execute_while_wait_for_children,
						 do_logging_bool						do_logging) :
								m_min_permanent_working_threads(min_permanent_working_threads),
								m_execute_while_wait_for_children(execute_while_wait_for_children),
								m_initialized(false), 
								m_thread_tls_key(0),
								m_user_thread_index(0),
								m_num_task_threads_exited(0),
								m_destroying(false),
								m_paused(false),
								m_num_paused_threads(0),
								m_active_task_thread_count(0),
								m_do_logging(do_logging),
								m_core_thread_count(MT_ALLOC(long, threading::core_count()),threading::core_count()),
								m_task_thread_tls(MT_ALLOC(thread_tls, max_task_threads), max_task_threads),
								m_user_thread_tls(MT_ALLOC(thread_tls, max_user_threads), max_user_threads)
{
	u32 spin_count	=	0;
	if ( s_spin_count_key.is_set_as_number(& spin_count) )
		threading::interlocked_exchange			(s_spin_count, spin_count);

	R_ASSERT									(max_task_threads);

	m_task_thread_tls.resize					(max_task_threads);
	m_user_thread_tls.resize					(max_user_threads);

	m_thread_tls_key						=	threading::tls_create_key();
	m_threads_can_start_work.set				(false);

	for ( u32 i=0; i<threading::core_count(); ++i )
		m_core_thread_count.push_back			(0);

	m_num_task_threads_started					=	0;
}

void thread_pool::initialize					( )
{
	R_ASSERT									(!m_initialized);

	u32 const max_task_threads					= m_task_thread_tls.size( );
	for ( u32 i=0; i<max_task_threads; ++i )
	{
		fixed_string512							thread_name_for_debugger;
		thread_name_for_debugger.assignf		("task #%d", i + 1);
		fixed_string512							thread_name_for_logging;
		thread_name_for_logging.assignf			("task #%d", i + 1);

		tasks::thread_tls & tls				=	m_task_thread_tls[i];
		tls.pool							=	this;
		tls.thread_index					=	i;
		tls.hardware_thread					=	i % threading::core_count();
		tls.state							=	thread_tls::state_inactive;
		tls.thread_type						=	thread_tls::type_task_thread;
		tls.thread_name						=	thread_name_for_logging;

		tls.thread_id						=	threading::spawn(	boost::bind(&tasks::thread_tls::thread_proc, &tls),
																	thread_name_for_debugger.c_str(), 
																	thread_name_for_logging.c_str(), 
																	0, 
																	tls.hardware_thread,
																	threading::tasks_aware
																);
	}

	log_columns_header							();

	m_timer.start								();

	for ( u32 i=0; i<max_task_threads; ++i )
	{
		tasks::thread_tls & tls				=	m_task_thread_tls[i];
		tls.event_start_thread_work.set			(true);
	}

	m_all_task_threads_started.wait				(threading::wait_time_infinite);
	m_initialized							=	true;
}

void   thread_proc_impl (thread_tls * const tls);

thread_pool::~thread_pool ()
{
	threading::interlocked_exchange				(m_destroying, true);
	for ( u32 i=0; i<m_task_thread_tls.size(); ++i )
		try_activate_task_thread				(m_task_thread_tls[i], i % threading::core_count());

	m_all_task_threads_destroyed.wait			(threading::wait_time_infinite);

	core_thread_count_container::pointer core_thread_count_buffer	=	& * m_core_thread_count.begin();
	thread_tls_container::pointer user_thread_tls_buffer		=	& * m_user_thread_tls.begin();
	thread_tls_container::pointer task_thread_tls_buffer		=	& * m_task_thread_tls.begin();
	m_core_thread_count.resize					(0);
	m_user_thread_tls.resize					(0);
	m_task_thread_tls.resize					(0);
	MT_FREE										(core_thread_count_buffer);
	MT_FREE										(user_thread_tls_buffer);
	MT_FREE										(task_thread_tls_buffer);
}

void   thread_pool::log_columns_header ()
{
	if ( !m_do_logging )
		return;

	u32 const screen_width					=	80;
	u32 const num_columns					=	(u32)m_task_thread_tls.size() + 1;
	u32 const column_width					=	screen_width / num_columns;

	fixed_string512								output;
	for ( u32 column=0; column<num_columns; ++column )
	{
		fixed_string512	column_output;
		if ( column == 0 )
			column_output					+=	"user";
		else
			column_output.appendf				("task %d", column-1);

		u32 const initial_chars				=	column_output.length();
		for ( u32 i=initial_chars; i<column_width; ++i )
			column_output					+=	' ';
		output								+=	column_output;
	}

	LOGI_INFO									("tasks", logging::format_message, logging::log_to_console, "%s", output.c_str());
}

void   thread_pool::log (thread_tls * const tls, pcstr format, ...)
{
	if ( !m_do_logging )
		return;

	u32 const screen_width					=	80;
	u32 const num_columns					=	(u32)m_task_thread_tls.size() + 1;
	u32 const column_width					=	screen_width / num_columns;
	
	fixed_string512	output;
	u32 const column						=	tls && tls->is_task_thread() ? (tls->thread_index+1)  : 0;
	for ( u32 i=0; i<column*column_width; ++i )
		output								+=	' ';

	va_list										argptr;
	va_start 									(argptr, format);
	output.appendf_va_list						(format, argptr);
	va_end	 									(argptr);

	LOGI_INFO									("tasks", logging::format_message, logging::log_to_console,
												 "%s", output.c_str());
}

u32   thread_pool::calculate_active_task_thread_count () const
{
	return										m_active_task_thread_count;
}

thread_tls *   thread_pool::get_thread_tls ()
{
	thread_tls * out_result					=	(thread_tls *)threading::tls_get_value(m_thread_tls_key);
	R_ASSERT									(out_result);
	return										out_result;
}

void   thread_pool::register_current_thread_as_core_user ()
{
	u32 const hardware_thread				=	threading::current_thread_affinity();
	R_ASSERT									(hardware_thread != u32(-1), "only threads that called set_current_thread_affinity can call this function:)");
	threading::interlocked_increment			(m_core_thread_count[hardware_thread]);

	R_ASSERT									(!threading::tls_get_value(m_thread_tls_key));

	threading::mutex_tasks_unaware_raii	raii	(s_mutex);

	thread_tls & tls						=	m_user_thread_tls[m_user_thread_index];
	tls.thread_index						=	m_user_thread_index;
	tls.hardware_thread						=	hardware_thread;
	tls.thread_type							=	thread_tls::type_user_thread;
	tls.pool								=	this;
	tls.current_task						=	& tls.user_thread_root_task;
	tls.thread_name							=	threading::current_thread_logging_name();

	threading::tls_set_value					(m_thread_tls_key, (pvoid)& m_user_thread_tls[m_user_thread_index]);
	++m_user_thread_index;
}

void   thread_pool::on_task_thread_paused ()
{
	if ( threading::interlocked_increment(m_num_paused_threads) == (threading::atomic32_value_type )m_task_thread_tls.size() )
		m_all_task_threads_paused.set			(true);
}

void   thread_pool::on_task_thread_resumed ()
{
 	if ( threading::interlocked_decrement(m_num_paused_threads) == 0 )
 		m_all_task_threads_resumed.set			(true);
}

void   thread_pool::pause_all_task_threads ()
{
	R_ASSERT									(!m_paused);

	m_num_paused_threads					=	0;
	for ( thread_tls_container::iterator	it	=	m_task_thread_tls.begin();
											it	!=	m_task_thread_tls.end();
											++it )
	{
		thread_tls & tls					=	* it;
		tls.event_pause_ended.set				(false);
	}

	m_all_task_threads_paused.set				(false);
	threading::interlocked_exchange				(m_paused, true);
	
	for ( thread_tls_container::iterator	it	=	m_task_thread_tls.begin();
											it	!=	m_task_thread_tls.end();
											++it )
	{
		thread_tls & tls					=	* it;
		try_activate_task_thread				(tls, tls.hardware_thread);
	}

	m_all_task_threads_paused.wait				(threading::wait_time_infinite);
	R_ASSERT									(m_num_paused_threads == (threading::atomic32_value_type )m_task_thread_tls.size());
}

void   thread_pool::resume_all_task_threads ()
{
	R_ASSERT									(m_paused);
	R_ASSERT									(m_num_paused_threads == (threading::atomic32_value_type )m_task_thread_tls.size());
	m_all_task_threads_resumed.set				(false);
	threading::interlocked_exchange				(m_paused, false);
	for ( thread_tls_container::iterator	it	=	m_task_thread_tls.begin();
											it	!=	m_task_thread_tls.end();
											++it )
	{
		thread_tls & tls					=	* it;
		tls.event_pause_ended.set				(true);
	}

	m_all_task_threads_resumed.wait				(threading::wait_time_infinite);
	R_ASSERT									(m_num_paused_threads == 0);
}

void   thread_pool::wait_while_all_threads_end_grab_next_task ()
{
	for ( thread_tls_container::iterator	it	=	m_task_thread_tls.begin();
											it	!=	m_task_thread_tls.end();
											++it )
	{
		thread_tls & tls					=	* it;
		while ( tls.in_grab_next_task )			{ ; }
	}

	for ( thread_tls_container::iterator	it	=	m_user_thread_tls.begin();
											it	!=	m_user_thread_tls.end();
											++it )
	{
		thread_tls & tls					=	* it;
		while ( tls.in_grab_next_task )			{ ; }
	}
}

//-----------------------------------------------------------------------------------
// pool interface functions
//-----------------------------------------------------------------------------------

static uninitialized_reference<tasks::thread_pool>	s_thread_pool;

void   on_new_task ()
{
	s_thread_pool->on_new_task					();
}

void   on_current_thread_locks ()
{
	if ( s_thread_pool.initialized() )
		s_thread_pool->on_current_thread_locks		();
}

void   on_current_thread_unlocks ()
{
	if ( s_thread_pool.initialized() )
		s_thread_pool->on_current_thread_unlocks	();
}

void   register_current_thread_as_core_user	()
{
	s_thread_pool->register_current_thread_as_core_user	();
}

void   initialize (u32 const max_task_threads, 
				   u32 const max_user_threads, 
				   u32 const min_permanent_working_threads,
				   execute_while_wait_for_children_enum execute_while_wait_for_children,
				   do_logging_bool do_logging)
{
	XRAY_CONSTRUCT_REFERENCE					(s_thread_pool, thread_pool) (	max_task_threads, 
																				max_user_threads, 
																				min_permanent_working_threads, 
																				execute_while_wait_for_children,
																				do_logging	);
	s_thread_pool->initialize					( );
}

void   finalize ()
{
	XRAY_DESTROY_REFERENCE						(s_thread_pool);
}

thread_tls *   current_task_thread_tls ()
{
	return	s_thread_pool->get_thread_tls		();
}

void   log (thread_tls * tls, pcstr format, ...)
{
	fixed_string512								output;
	va_list										argptr;
	va_start 									(argptr, format);
	output.appendf_va_list						(format, argptr);
	va_end	 									(argptr);

	s_thread_pool->log							(tls, "%s", output.c_str());
}

bool   thread_can_execute_while_wait_for_children ()
{
	return										s_thread_pool->execute_while_wait_for_children();
}

void   wait_for_all_children ()
{
	s_thread_pool->wait_for_all_children		();
}

void   wait_for_task_list (task * const task_list)
{
	s_thread_pool->wait_for_task_list			(task_list);
}

void   pause_all_task_threads ()
{
	s_thread_pool->pause_all_task_threads		();
}

void   resume_all_task_threads ()
{
	s_thread_pool->resume_all_task_threads		();
}

void   set_logging (do_logging_bool const do_logging)
{
	s_thread_pool->set_logging					(do_logging);
}

void   wait_while_all_threads_end_grab_next_task ()
{
	s_thread_pool->wait_while_all_threads_end_grab_next_task	();
}

tasks::thread_pool *	get_thread_pool	()
{
	return	s_thread_pool.c_ptr();
}

} // namespace tasks
} // namespace xray
