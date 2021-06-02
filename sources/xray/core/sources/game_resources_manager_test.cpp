////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include <xray/resources.h>
#include <xray/resources_cook_classes.h>
#include "game_resources_manager_test_cook.h"
#include <xray/core_test_suite.h>
#include <xray/math_randoms_generator.h>
#include "game_resman.h"

DECLARE_LINKAGE_ID(game_resources_manager_test)

namespace xray {
namespace resources {

void   flush_game_resources_manager		();

class game_resources_manager_tester
{
public:
	void   on_mount_completed (bool result)
	{
		TEST_ASSERT					(result);
	}

	bool	m_last_callback_result;

	void   resources_callback (xray::resources::queries_result & queries)
	{
		m_last_callback_result		=	queries.is_successful();
		if ( m_last_callback_result )
		{
			TEST_CURE_ASSERT_CMP	(queries.size(), ==, 1, return);
			TEST_CURE_ASSERT		(queries[0].is_success(), return);
			test_resource_ptr resource	=	static_cast_resource_ptr<test_resource_ptr>(queries[0].get_unmanaged_resource());
			TEST_CURE_ASSERT		(resource, return);
			R_ASSERT				(m_resources.size() < m_resources.max_size());
			m_resources.push_back	(resource);
		}
	}

	static u32 const					full_allocator_size	=	1024 * 100;
	static u32 const					max_held_resources	=	100;
	fixed_vector<test_resource_ptr, max_held_resources>	m_resources;
	u32									m_iteration;

	bool   try_to_release_one_bigger_resource (math::random32 & random, u32 required_size)
	{
		LOGI_INFO							("grm", "trying to release one resource big enough");
		u32 const resource_start_index	=	random.random(m_resources.size());
		for ( u32 offset = 0; offset < m_resources.size(); ++offset )
		{
			u32 const resource_index	=	(resource_start_index + offset) % m_resources.size();
			test_resource_ptr resource_to_free	=	m_resources[resource_index];
			if ( resource_to_free->size() < required_size )
				continue;
			grm_log							(grm_log_no_user_reference, m_resources[resource_index].c_ptr());
			m_resources.erase				(m_resources.begin() + resource_index);
			return							true;
		}

		return								false;
	}

	void   work_with_specific_cooker	(cook_base * )
	{
		math::random32						random(m_iteration);

		pstr unused_buffer				=	NULL;
		STR_DUPLICATEA						(unused_buffer, "dont read/write this stack data");

		u32 const min_resource_size		=	sizeof(test_resource);
		u32 const max_resource_size		=	min_resource_size * 4;

		for ( u32 i=0; i<1000; ++i )
		{
			if ( (!random.random(3) || m_resources.size() == max_held_resources) && m_resources.size() )
			{
				u32 const resource_index	=	random.random(m_resources.size());
				grm_log							(grm_log_no_user_reference, m_resources[resource_index].c_ptr());
				m_resources.erase				(m_resources.begin() + resource_index);
				continue;
			}
			
			u32 const allocation_size	=	min_resource_size + random.random(max_resource_size - min_resource_size);

			const_buffer					creation_buffer	(unused_buffer, allocation_size);
			fixed_string<32>				index_name;
			index_name.assignf				("%d", i);
 			creation_request				creation_requests[]	=	{ { index_name.c_str(), creation_buffer, test_resource_class }, };
 			
			query_create_resources_and_wait	(creation_requests, 
											 1,
											 boost::bind(&game_resources_manager_tester::resources_callback, this, _1), 
											 & memory::g_mt_allocator);

			if ( !m_last_callback_result )
				LOGI_INFO				("grm", "failed to allocate even after freeing all grm kept resources: %d", allocation_size);
		}
		m_resources.clear					();
		
		release_all_resources			();
		while ( release_all_resources_scheduled() ) ;
  			
 		resources::wait_and_dispatch_callbacks	(true);
		++m_iteration;
	}

	void unregister_cook (cook_base * cook)
	{
		u32 const time_for_cleanup	=	1000;

		timing::timer				timer;
		timer.start					();
		while ( cook->cook_users_count() && (timer.get_elapsed_ms() < time_for_cleanup || debug::is_debugger_present()) )
			resources::dispatch_callbacks	();

		CURE_ASSERT					(timer.get_elapsed_ms() < time_for_cleanup || debug::is_debugger_present(), return, "cooker has failed to delete all resources/buffers, though it was given %d ms of time", time_for_cleanup);

		resources::unregister_cook	(cook->get_class_id());
	}

	void test (core_test_suite * suite)
	{
		XRAY_UNREFERENCED_PARAMETERS					(suite);

		s_test_allocator.allocator					=	MT_NEW(memory::doug_lea_allocator) (false, true);
		pvoid test_allocator_arena					=	MT_ALLOC(char, full_allocator_size);
		s_test_allocator.allocator->initialize			(test_allocator_arena, 
														full_allocator_size, "game res man test allocator");
		s_test_allocator.allocator->user_current_thread_id	();

		m_iteration				=	0;

		if ( debug::is_debugger_present() )
		{
			logging::push_rule			( "core:resources:allocator", logging::silent );
			logging::push_rule			( "core:resources:test", logging::trace );
			logging::push_rule			( "core:resources:device_manager", logging::trace );
			logging::push_rule			( "core:resources:detail", logging::trace );
		}

		for ( u32 allocate_in_current_thread=0; allocate_in_current_thread<2; ++allocate_in_current_thread )
		for ( u32 create_allocates_destroy_frees=0; create_allocates_destroy_frees<2; ++create_allocates_destroy_frees )
		{
			game_resources_manager_test_cook	cook
				(!!create_allocates_destroy_frees,
				 cook_base::use_current_thread_id, 
				 allocate_in_current_thread ? cook_base::use_current_thread_id : cook_base::use_resource_manager_thread_id);


			register_cook				(& cook);

			work_with_specific_cooker	(& cook);

			unregister_cook				(& cook);

		} while ( identity(0) );

		wait_and_dispatch_callbacks		(true);

		MT_DELETE						(s_test_allocator.allocator);
		MT_FREE							(test_allocator_arena);
	}
};

REGISTER_TEST_CLASS	(game_resources_manager_tester, core_test_suite);

} // namespace xray
} // namespace resources
