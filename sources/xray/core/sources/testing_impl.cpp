////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/intrusive_list.h>
#include <xray/fixed_string.h>
#include <xray/exit_codes.h>
#include <xray/testing.h>
#include <xray/debug/call_stack.h>

#include <xray/os_preinclude.h>
#undef NOUSER
#undef NOMSG
#undef NOWINMESSAGES
#undef NOCTLMGR
#include <xray/os_include.h>

#include <xray/core/engine.h>
#include <xray/logging/log_file.h>

namespace xray {
namespace logging {
	void	write_exit_code_file	( int );
} // namespace logging

namespace command_line	{
bool	key_is_set	( pcstr key_raw );
} // namespace command_line

namespace testing {

static char				s_run_tests_key_name[]	=	"run_tests";
static char				s_run_tests_and_exit_key_name[]	=	"run_tests_and_exit";

command_line::key		g_run_tests				(s_run_tests_key_name, "", "testing", "");
command_line::key		g_run_tests_and_exit	(s_run_tests_and_exit_key_name, "", "testing", "");

command_line::key		s_no_test_watch			("no_test_watch", "", "testing", "disables thread that watches for too long tests");

struct environment
{
	core::engine *						engine;
	threading::event					test_watcher_thread_must_exit;
	bool								test_watcher_thread_exited;
	bool								test_watcher_thread_started;
	u32									num_suites_total;
	threading::atomic32_type			num_suites_executed;
	u32									num_failed_tests;
	threading::atomic32_type			current_test_number;
	pcstr								current_test;
	pcstr								current_suite;
	assert_enum							awaited_exception;
	bool								caught_awaited_exception;
	threading::atomic32_type			is_testing;
	u32									exception_index;
	u32									num_top_callstack_frames_to_skip;

	environment() 					: 	engine(NULL),
										test_watcher_thread_started(false),
										test_watcher_thread_must_exit(false),
										test_watcher_thread_exited(false),
										num_failed_tests(0),
										current_test_number(0),
										num_top_callstack_frames_to_skip(0),
										is_testing(false), 
									  	exception_index(0),
									  	current_test("undefined"), 
									  	current_suite("undefined"), 
									  	awaited_exception(assert_untyped),
									  	caught_awaited_exception(false) {}

};

static environment						s_environment;

void   test_watcher_thread_proc ()
{
	static const u32 max_time_allowed_for_one_test	=	100*1000;
	static const u32 max_time_allowed_to_start_test	=	600*1000;

	s_environment.test_watcher_thread_started	=	true;
	while ( s_environment.num_suites_executed != (long)s_environment.num_suites_total )
	{
		long const current_test_number			=	s_environment.current_test_number;
		bool const in_test						=	!!s_environment.is_testing;


		s_environment.test_watcher_thread_must_exit.wait	( in_test ? max_time_allowed_for_one_test : 
															 			max_time_allowed_to_start_test);

		if ( s_environment.num_suites_executed == (long)s_environment.num_suites_total )
			break;

		bool terminate							=	true;
		if ( !in_test && !s_environment.is_testing && s_environment.current_test_number == current_test_number )
		{
			if ( !debug::is_debugger_present() )
				LOGI_ERROR							("test", "%.4f sec. passed and still next test is not executed", max_time_allowed_to_start_test / 1000.f);
		}
		else if ( in_test && s_environment.is_testing && s_environment.current_test_number == current_test_number )
		{
			if ( !debug::is_debugger_present() )
				LOGI_ERROR							("test", "%.4f sec. passed and test %s (suite %s) is still executing", 
													 max_time_allowed_to_start_test / 1000.f, 
													 s_environment.current_test, 
													 s_environment.current_suite);
		}
		else
			terminate							=	false;

		if ( terminate && !debug::is_debugger_present() )
		{
			R_ASSERT								(s_environment.engine);
			u32 const exit_code					=	s_environment.engine->get_exit_code() + 
													s_environment.num_failed_tests + 
													exit_code_tests_finished_by_test_watcher;
			xray::debug::terminate					(exit_code, "");
		}
	}

	s_environment.test_watcher_thread_exited	=	true;
}

bool   run_tests_command_line ()
{
	static u32 s_out_result						=	u32(-1);
	if ( s_out_result == u32(-1) )
	{
		if ( command_line::initialized() )
			s_out_result						=	g_run_tests || g_run_tests_and_exit;
		else
			s_out_result						=	command_line::key_is_set(s_run_tests_key_name) || 
													command_line::key_is_set(s_run_tests_and_exit_key_name);
	}

	return											!!s_out_result;
}

void   initialize (core::engine * const engine)
{
	s_environment.engine			=	engine;

	if ( run_tests_command_line() && !s_no_test_watch )
	{
		threading::spawn				( &test_watcher_thread_proc, 
										 "test watcher", 
										 "test-watcher",
										 0,
										 3 % threading::core_count(),
										 threading::tasks_aware);
	}
}

void   finalize ()
{
	if ( s_environment.test_watcher_thread_started )
	{
		s_environment.test_watcher_thread_must_exit.set(true);
		while ( !s_environment.test_watcher_thread_exited )
			threading::yield				(1);
	}
}

void   set_testing (bool is_testing)
{
	threading::interlocked_exchange		(s_environment.is_testing, is_testing);
}

bool   is_testing ()
{
	return								!!s_environment.is_testing;
}

u32   tests_failed_so_far ()
{
	return								s_environment.num_failed_tests;
}

void   run_protected_test (test_base*);

#if !XRAY_PLATFORM_PS3
static inline u32 execute_handler_filter( ... )
{
	return						EXCEPTION_EXECUTE_HANDLER;
}
#endif // #if !XRAY_PLATFORM_PS3

void   on_exception (assert_enum			assert_type, 
					 pcstr					description, 
					 _EXCEPTION_POINTERS*	exception_information,
					 bool					is_assertion)
{
	if ( s_environment.awaited_exception != assert_untyped &&
		 s_environment.awaited_exception == assert_type )
	{							
		s_environment.caught_awaited_exception	=	true;

#if !XRAY_PLATFORM_PS3
		RaiseException					(0xABCDEF, 0, 0, 0);
#endif // #if !XRAY_PLATFORM_PS3
		return;
	}																	

#if !XRAY_PLATFORM_PS3
	__try {
#endif // #if !XRAY_PLATFORM_PS3

	fixed_string8192 description_string	=	*description == '\n' ? (description + 1) : description;
	
	u32 const description_size		=	description_string.length();

	if ( description_size && description_string[description_size-1] != '\n' )
	{
		description_string			+=	'\n';
	}

	logging::helper	(__FILE__, __FUNCSIG__, __LINE__, "test", logging::error)
										("-------------------------------------------------------------\n"
										 "EXCEPTION #%d in test '%s', suite '%s'\n"
										 "-------------------------------------------------------------\n"
										 "%s",
										 s_environment.exception_index+1, 
										 s_environment.current_test, 
										 s_environment.current_suite, 
										 description_string.c_str());
	
	debug::dump_call_stack				("test", 
										 true, 
										 is_assertion ? 3 : 0, 
										 s_environment.num_top_callstack_frames_to_skip, 
										 exception_information);
#if !XRAY_PLATFORM_PS3
	} 
	__except ( execute_handler_filter( GetExceptionCode( ), GetExceptionInformation( ) ) ) {
		(void)0;
	}
#endif // #if !XRAY_PLATFORM_PS3

	++s_environment.exception_index;
}

void   run_protected_test_helper	(pvoid test)
{
	void*	stacktrace[64];
	debug::call_stack::get_stack_trace (stacktrace, array_size(stacktrace), 62, 0);

	s_environment.num_top_callstack_frames_to_skip	=	0;
	for ( u32 i=0; stacktrace[i] ; ++i ) ++s_environment.num_top_callstack_frames_to_skip;

	((test_base*)test)->test			();
}

void   run_protected_test (test_base* test)
{
	s_environment.awaited_exception	=	xray::assert_untyped;
	s_environment.exception_index	=	0;
	threading::interlocked_increment	(s_environment.current_test_number);

	debug::protected_call				(run_protected_test_helper, test);
}

namespace detail {

void   on_new_suite ()
{
	++s_environment.num_suites_total;
}

bool   run_tests_impl (test_base* test, pcstr suite_name)
{
	if ( !run_tests_command_line() )
		return							true;

	set_testing							(true);

	u32 num_failed_tests			=	0;
	u32 num_tests					=	0;
	timing::timer						timer;
	timer.start							();
	while ( test )
	{
		run_protected_test				(test);
		if ( s_environment.exception_index != 0 )
		{
			++num_failed_tests;
		}
		++num_tests;
		test						=	test->m_next_test;
	}

	logging::verbosity const message_type	=	num_failed_tests ? logging::error : logging::warning;
	fixed_string1024					result_string;

	if ( num_failed_tests )
	{
		result_string.appendf			("TEST SUITE '%s' : FAILED %d of %d tests", 
										 suite_name, 
										 num_failed_tests, 
										 num_tests);
	}
	else
	{
		result_string.appendf			("TEST SUITE '%s' : successfull %d tests", suite_name, num_tests);
	}

	logging::helper						(__FILE__, __FUNCSIG__, __LINE__, "test", message_type)
										(logging::format_message,
										 "------------------------------------------------------------------------------\n"
										 "%s (%d ms)\n"
										 "------------------------------------------------------------------------------",
										 result_string.c_str(), timer.get_elapsed_msec());

	s_environment.num_failed_tests	+=	num_failed_tests;
	threading::interlocked_increment	(s_environment.num_suites_executed);

	if ( s_environment.num_suites_executed == (long)s_environment.num_suites_total )
	{
		u32 new_exit_code			=	s_environment.num_failed_tests;
		if ( s_environment.engine )
			new_exit_code			+=	s_environment.engine->get_exit_code();

#if XRAY_PLATFORM_XBOX_360
		logging::write_exit_code_file	(new_exit_code);
#endif // #if XRAY_PLATFORM_XBOX_360

		if ( g_run_tests_and_exit )
		{
			ASSERT						(s_environment.engine);

			if ( s_environment.num_failed_tests != 0 && !debug::is_debugger_present() )
			{
				u32 const exit_code					=	s_environment.engine->get_exit_code() + 
														s_environment.num_failed_tests + 
														exit_code_tests_failed;
				xray::debug::terminate					(exit_code, "");
			}

			if ( s_environment.engine )
				s_environment.engine->exit	(new_exit_code);
		}
	}

	set_testing							(false);

	return								!num_failed_tests;
}

void   set_current_test (pcstr suite_name, pcstr test_name)
{
	s_environment.current_suite		=	suite_name;
	s_environment.current_test		=	test_name;
}

assert_enum   set_awaited_exception (assert_enum awaited_exception)
{
	assert_enum previous_awaited_exception	=	s_environment.awaited_exception;
		
	s_environment.awaited_exception			=	awaited_exception;
	s_environment.caught_awaited_exception	=	false;

	return							previous_awaited_exception;
}

void   check_awaited_exception (assert_enum previous_awaited_exception)
{
	s_environment.awaited_exception	=	previous_awaited_exception;

	if ( !s_environment.caught_awaited_exception )
	{
		on_exception				(assert_untyped, "test_throws failed, awaited exception was not caught\n", 0, true);
	}

	s_environment.caught_awaited_exception	=	false;
}

} // namespace detail

#if !XRAY_PLATFORM_PS3
COMPILE_ASSERT	(XRAY_EXCEPTION_EXECUTE_HANDLER == EXCEPTION_EXECUTE_HANDLER,
				 please_define_XRAY_EXCEPTION_EXECUTE_HANDLER_to_be_equal_EXCEPTION_EXECUTE_HANDLER);
#endif // #if !XRAY_PLATFORM_PS3

} // namespace testing
} // namespace xray