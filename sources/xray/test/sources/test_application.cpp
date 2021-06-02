#include "pch.h"
#include "test_application.h"

#include "xray/buffer_string.h"
#include "xray/engine/engine.h"
#include <xray/core/sources/fs_file_system.h>

#include <xray/core/core.h>
#include <xray/intrusive_list.h>

#include <stdio.h>

#include <xray/os_preinclude.h>
#undef NOUSER
#undef NOMSG
#undef NOWINMESSAGES
#undef NOCTLMGR
#include <xray/os_include.h>
#include <xray/compressor_ppmd.h>
using namespace xray;

#include <xray/tasks_system.h>
#include <xray/tasks_thread_tls.h>

// #include <xray/memory_buffer.h>
// #include <xray/tasks_allocator.h>
// #include <xray/tasks_type.h>
// #include <xray/tasks_task.h>
// #include <xray/tasks_thread_pool.h>

#include <xray/tasks_aware_functions.h>

#define USE_CHECKED_TYPES

//	#include "checked_bool.h" test
//	#include "checked_numbers.h"

#include <xray/uninitialized_reference.h>
#include <xray/core/simple_engine.h>

xray::memory::doug_lea_allocator_type		g_test_allocator;

using namespace xray::resources;
using namespace xray::fs;

static 	xray::core::simple_engine			core_engine;

void   application::initialize (u32 , pstr const* )
{
	xray::core::preinitialize					(& core_engine, 
												 core::create_log, 
												 GetCommandLine(), 
												 command_line::contains_application_true,
												 "test");
	g_test_allocator.user_current_thread_id		();
	g_test_allocator.do_register				(768 * 1024 * 1024,	"test allocator");
	xray::memory::allocate_region				();
	xray::core::initialize						("test", core::perform_debug_initialization);
}

void   application::finalize ()
{
	xray::core::finalize						();
}

static tasks::task_type *	s_my_task_types []	=	{	tasks::create_new_task_type("task 1", 0),
														tasks::create_new_task_type("task 2", 0),
														tasks::create_new_task_type("task 3", 0),
														tasks::create_new_task_type("task 4", 0),	};

void   task_B ();

typedef vector<float>	numbers_type;


void   task_A (numbers_type * in_container)
{
	XRAY_UNREFERENCED_PARAMETER					( in_container );
	//static u32 executed_count				=	0;
	//tasks::log									(tls, "%d>started A", tls->hardware_thread);
	//tasks::spawn_task							(& task_B, s_my_task_types[1]);
	//tasks::wait_for_all_children				();
	//threading::yield							(1);
	//tasks::log									(tls, "%d>ended A", tls->hardware_thread);

//    	tasks::thread_tls * const tls			=	tasks::current_task_thread_tls();
//    	tasks::log									(tls, "%d>A 0x%08x", tls->hardware_thread, in_container);

 //	numbers_type & container				=	* in_container;
 //
	//u32 const half_numbers_count			=	container.size() / 2;
	//u32 const buffer_size					=	half_numbers_count*sizeof(numbers_type::value_type);
	//memory::copy								( &*container.begin() + half_numbers_count, buffer_size, &*container.begin(), buffer_size );

//	for ( u32 i=0; i<half_numbers_count; ++i )
//	{
//		if ( i % 2 )
//			container[0]					=	container[1];
//		else
//			container[1]					=	container[2];
//// 		if ( i % 2 )
//// 			container[i]					=	container[half_numbers_count + i];
//// 		else
//// 			container[half_numbers_count + i]	=	container[i];
//	}
}

void   task_B ()
{
	static u32 executed_count				=	0;

	tasks::thread_tls * const tls			=	tasks::current_task_thread_tls();

	threading::yield							(5);	
	tasks::log									(tls, "%d>executed B", tls->hardware_thread);
}

command_line::key	s_task_thread_count_key		("threads_count", "tc", "test", "");
command_line::key	s_task_size_key				("task_size", "ts", "test", "(kb)");
command_line::key	s_task_factor				("count_factor", "cf", "test", "(kb)");

void spawn_task( bool are_different, int const count, numbers_type **aN )
{
	tasks::task_type * types[4];
	for ( u32 i=0; i<4; ++i )
		types[i]						=	are_different ? s_my_task_types[i] : s_my_task_types[0];

	for ( u32 i=0, n=count/4; i<n; ++i )
	{
		u32 const start_index			=	rand() % 4;
		tasks::spawn_task					(boost::bind(&task_A, aN[(start_index+0)]), 
											 types[(start_index+0)]);
		tasks::spawn_task					(boost::bind(&task_A, aN[(start_index+1) % 4]), 
											 types[(start_index+1) % 4]);
		tasks::spawn_task					(boost::bind(&task_A, aN[(start_index+2) % 4]), 
											 types[(start_index+2) % 4]);
		tasks::spawn_task					(boost::bind(&task_A, aN[(start_index+3) % 4]), 
											 types[(start_index+3) % 4]);
	}

	tasks::wait_for_all_children			( );
}

void   test (u32 const task_thread_count, u32 const task_size)
{
	XRAY_UNREFERENCED_PARAMETER					( task_thread_count );

	LOG_INFO									(logging::settings(NULL, logging::settings::flags_log_to_console | logging::settings::flags_log_only_user_string | logging::settings::flags_log_only_user_string), 
												 "using task size : %d kb", (u32)task_size);

	u32 const 		number_count			= 	task_size * 1024;
	numbers_type	a0							(number_count, 1.f);
	numbers_type	a1							(number_count, 2.f);
	numbers_type	a2							(number_count, 3.f);
	numbers_type	a3							(number_count, 4.f);
	numbers_type *  aN[]					=	{ & a0, & a1, & a2, & a3 };

	float count_factor						=	1.f;
	s_task_factor.is_set_as_number				(& count_factor);

//	u32 const		tasks_count				=	150000; 
	u32 const		tasks_count				=	500000; 
//	u32 const		tasks_count				=	4; 
//	u32 const		tasks_count				=	task_size >= 32 ? u32(count_factor * 1024 * 100 / task_size) : 100000;
	u32 const		tasks_count_div_4		=	tasks_count / 4;
// 	LOG_INFO									(logging::settings(NULL, logging::settings::flags_log_to_console | logging::settings::flags_log_only_user_string | logging::settings::flags_log_only_user_string), 
// 												 "tasks count: %d", tasks_count);

 	timing::timer	timer;
 	float			sequential_grouped		=	1;

// 
// 
//  	timer.start									();
//  	
//  	for ( u32 j=0; j<4; ++j ) 
//  	{
//  		numbers_type* const current			=	aN[j];
//  		for ( u32 i=0; i<tasks_count_div_4; ++i )
//  			task_A								(current);
//  	}
//  
//  	sequential_grouped						=	timer.get_elapsed_sec()  / task_thread_count;
//  
//  	timer.start									( );
//  	for ( u32 i=0; i<tasks_count_div_4; ++i )
//  	{
//  		for ( u32 j=0; j<4; ++j ) 
//  			task_A								( aN[j] );
//  	}
//  
//  	float const sequential_shuffled			=	timer.get_elapsed_sec()  / task_thread_count;
//  
//  	LOG_INFO									(logging::settings(NULL, logging::settings::flags_log_to_console | logging::settings::flags_log_only_user_string | logging::settings::flags_log_only_user_string), 
//  												 "%06d %04d kb tasks>sequential shuffled: %6.2f, %06d per 1 ms", 
//  												 tasks_count, task_size, sequential_shuffled, u32((0.001f * tasks_count) / sequential_shuffled));
//  	LOG_INFO									(logging::settings(NULL, logging::settings::flags_log_to_console | logging::settings::flags_log_only_user_string | logging::settings::flags_log_only_user_string), 
//  												 "%06d %04d kb tasks>sequential grouped:  %6.2f, %06d per 1 ms", 
//  												 tasks_count, task_size, sequential_grouped, u32((0.001f * tasks_count) / sequential_grouped));
//  

   	for ( u32 i=0; i<100; ++i )
    		tasks::spawn_task						(boost::bind(&task_A, aN[i%4]), s_my_task_types[i%4]);
 
 	tasks::wait_for_all_children				();

	threading::yield							(100);

	u32 const num_iterations				=	12;
	float shuffled_total					=	0;
	float grouped_total						=	0;
	float shuffled_average					=	0;
	float grouped_average					=	0;
	float shuffled_percent					=	0;
	float grouped_percent					=	0;

#if 1
	for ( u32 test_iteration=0; test_iteration<num_iterations*2; ++test_iteration )
//	for ( u32 test_iteration=0; test_iteration<2; ++test_iteration )
	{
		srand									(0);

		timer.start								();
//		tasks::pause_all_task_threads			();
		tasks::task_type * types[4];
		for ( u32 i=0; i<4; ++i )
			types[i]						=	(test_iteration % 2) ? s_my_task_types[0] : s_my_task_types[i];

		for ( u32 i=0; i<tasks_count_div_4; ++i )
		{
//  			tasks::spawn_task					(boost::bind(&task_A, aN[0]), types[0]);
 
			u32 const start_index			=	rand() % 4;
			tasks::spawn_task					(boost::bind(&task_A, aN[(start_index+0)]), 
												 types[(start_index+0)]);
			tasks::spawn_task					(boost::bind(&task_A, aN[(start_index+1) % 4]), 
												 types[(start_index+1) % 4]);
			tasks::spawn_task					(boost::bind(&task_A, aN[(start_index+2) % 4]), 
												 types[(start_index+2) % 4]);
			tasks::spawn_task					(boost::bind(&task_A, aN[(start_index+3) % 4]), 
												 types[(start_index+3) % 4]);
		}

//		threading::yield						(100);
//		timer.start								();
//		tasks::resume_all_task_threads			();

		tasks::wait_for_all_children			();
		float const elapsed_sec				=	timer.get_elapsed_sec();

		threading::yield						(20);
		//tasks::collect_garbage					();
 		//tasks::check_all_free					();
		//R_ASSERT								(tasks::zero_tasks());

// 		LOG_INFO								(logging::settings(NULL, logging::settings::flags_log_to_console | logging::settings::flags_log_only_user_string | logging::settings::flags_log_only_user_string), 
//  											 "parallel execution time (%s): %6.2f", 
// 												 (test_iteration % 2) ? "shuffled" : "grouped", elapsed_sec);

		if ( test_iteration % 2 )
			shuffled_total					+=	 elapsed_sec;
		else
			grouped_total					+=	 elapsed_sec;
	}

	shuffled_average						=	shuffled_total / num_iterations;
	grouped_average							=	grouped_total / num_iterations;
	shuffled_percent						=	100 * shuffled_average / sequential_grouped;
	grouped_percent							=	100 * grouped_average / sequential_grouped;

	LOG_INFO									(logging::settings(NULL, logging::settings::flags_log_to_console | logging::settings::flags_log_only_user_string | logging::settings::flags_log_only_user_string), 
												 "%06d %04d kb tasks>parallel shuffled:   %6.2f, %06d per 1 ms, %2.f%%", tasks_count, task_size, shuffled_average, u32((0.001f * tasks_count) / shuffled_average), shuffled_percent);
	LOG_INFO									(logging::settings(NULL, logging::settings::flags_log_to_console | logging::settings::flags_log_only_user_string | logging::settings::flags_log_only_user_string), 
												 "%06d %04d kb tasks>parallel grouped:    %6.2f, %06d per 1 ms, %2.f%%", tasks_count, task_size, grouped_average, u32((0.001f * tasks_count) / grouped_average), grouped_percent);

#endif // #if 0

 	for ( u32 i=0; i<100; ++i )
    		tasks::spawn_task						(boost::bind(&task_A, aN[i%4]), s_my_task_types[i%4]);
  	tasks::wait_for_all_children				();

	shuffled_total							=	 0;
	grouped_total							=	 0;

#if 1
	for ( u32 test_iteration=0; test_iteration<num_iterations*2; ++test_iteration )
	{
		srand									( 0 );

		timer.start								( );
		tasks::spawn_task						( boost::bind(&spawn_task, !!(test_iteration % 2), tasks_count_div_4, &aN[0] ), s_my_task_types[0] );
		tasks::spawn_task						( boost::bind(&spawn_task, !!(test_iteration % 2), tasks_count_div_4, &aN[0] ), s_my_task_types[0] );
		tasks::spawn_task						( boost::bind(&spawn_task, !!(test_iteration % 2), tasks_count_div_4, &aN[0] ), s_my_task_types[0] );
		tasks::spawn_task						( boost::bind(&spawn_task, !!(test_iteration % 2), tasks_count_div_4, &aN[0] ), s_my_task_types[0] );
		tasks::wait_for_all_children			( )	;
		float const elapsed_sec				=	timer.get_elapsed_sec();

		//threading::yield						(10);
// 		tasks::collect_garbage					();
//  		tasks::check_all_free					();
// 		R_ASSERT								(tasks::zero_tasks());

		if ( test_iteration % 2 )
			shuffled_total					+=	 elapsed_sec;
		else
			grouped_total					+=	 elapsed_sec;
	}

	u32 tasks_freed							=	0;
	timer.start									( );
	tasks::collect_garbage						(& tasks_freed);
	float const elapsed_sec					=	timer.get_elapsed_sec();
	shuffled_total							+=	 elapsed_sec;

	LOG_INFO									(logging::settings(NULL, logging::settings::flags_log_to_console | logging::settings::flags_log_only_user_string | logging::settings::flags_log_only_user_string), 
												 "tasks freed: %d", tasks_freed);


	shuffled_average						=	shuffled_total / num_iterations;
	grouped_average							=	grouped_total / num_iterations;
	shuffled_percent						=	100 * shuffled_average / sequential_grouped;
	grouped_percent							=	100 * grouped_average / sequential_grouped;

	LOG_INFO									(logging::settings(NULL, logging::settings::flags_log_to_console | logging::settings::flags_log_only_user_string | logging::settings::flags_log_only_user_string), 
												"%06d %04d kb tasks>parallel shuffled:   %6.2f, %06d per 1 ms, %2.f%%", tasks_count, task_size, shuffled_average, u32((0.001f * tasks_count) / (shuffled_average ? shuffled_average : 1.f)), shuffled_percent);
	LOG_INFO									(logging::settings(NULL, logging::settings::flags_log_to_console | logging::settings::flags_log_only_user_string | logging::settings::flags_log_only_user_string), 
												"%06d %04d kb tasks>parallel grouped:    %6.2f, %06d per 1 ms, %2.f%%", tasks_count, task_size, grouped_average, u32((0.001f * tasks_count) / (grouped_average ? grouped_average : 1.f)), grouped_percent);
#endif // #if 0

	// 
//	for ( u32 i=0; i<10; ++i )
//	{
//		tasks::log								(NULL, "%d>main work", threading::current_thread_affinity());
//		threading::yield						(7);
//	}
//	u32 const execution_time				=	tasks::s_thread_pool.time_elapsed();
//	LOG_INFO									(logging::settings(NULL, logging::settings::flags_log_to_console | logging::settings::flags_log_only_user_string | logging::settings::flags_log_only_user_string), 
// 												 "    execution time: %d\n",
// 												 execution_time);
}

void   application::execute ()
{
// 	SetPriorityClass							(GetCurrentProcess(),	HIGH_PRIORITY_CLASS);
// 																		REALTIME_PRIORITY_CLASS);

	//logging::push_filter							("test:tasks", logging::silent);

	float task_thread_count					=	2;
	bool const default_thread_count			=	!s_task_thread_count_key.is_set_as_number(& task_thread_count);
	LOG_INFO									(logging::settings(NULL, logging::settings::flags_log_to_console | logging::settings::flags_log_only_user_string | logging::settings::flags_log_only_user_string), 
												 "using%s task thread count: %d", 
												 default_thread_count ? " default" : "", (u32)task_thread_count);

	float task_size_in_kb					=	1024;
	s_task_size_key.is_set_as_number			(& task_size_in_kb);

//	tasks::set_logging							(false);

	threading::set_current_thread_affinity		(0);

//	u32 const test_task_sizes[]				=	{ 16, 32, 64, 128, 256, 512, 1024, 2048, 4096 };
	
	//u32 const test_task_sizes[]				=	{ 0, 1, 2, 4, 8, 16, 32, 64, };// 128, 256, 512, 1024, 2048, 4096 };
	u32 const test_task_sizes[]				=	{ 0 };
	for ( u32 i=0; i<array_size(test_task_sizes); ++i )
		test									((u32)task_thread_count, test_task_sizes[i]);

	//test										((u32)task_thread_count, (u32)task_size_in_kb);
	
	m_exit_code								=	0;
}
