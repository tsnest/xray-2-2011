////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include <xray/resources.h>
#include <xray/testing_macro.h>
#include <xray/resources_cook_classes.h>
#include <xray/core_test_suite.h>

#include "game_resman_test_resource_with_quality_cook.h"
#include "game_resman_test_resource_cook.h"
#include "game_resman_test_utils.h"
#include "game_resman_test_allocator.h"
#include "game_resman.h"

XRAY_DECLARE_LINKAGE_ID(game_resources_manager_test)

namespace xray {
namespace resources {

math::random32							s_random(0);

void	flush_game_resources_manager	();
u64		current_increase_quality_tick	(); // for test

class resource_holder : public positional_unmanaged_resource
{
public:
	enum	{ max_held_resources = 200 };
public:
			resource_holder() : m_resources_count(0) {}

	void   remove_random_resource ()
	{
		ASSERT								(m_resources_count);
		u32 const resource_index		=	get_random_child_index();

		unmanaged_resource * resource	=	m_resources[resource_index].c_ptr();
		log_test_resource					(log_test_resource_no_user_reference, 
											 static_cast_checked<test_resource *>(resource));
		m_resources[resource_index].set_zero();
		
		--m_resources_count;
	}

	void   add_resource (test_resource_ptr resource)
	{
		R_ASSERT_CMP_U						(m_resources_count, <, (u32)max_held_resources);
		u32 index						=	0;
		for ( index=0; index<array_size(m_resources); ++index )
			if ( m_resources[index].c_ptr() == 0 )
				break;

		m_resources[index].initialize_and_set_parent	(this, resource.c_ptr());
		++m_resources_count;
	}

	test_resource_ptr   get_random_child ()
	{
		return								static_cast_resource_ptr<test_resource_ptr>
											(m_resources[get_random_child_index()]);
	}

	bool				is_full			() const { return m_resources_count == max_held_resources; }
	bool				has_resources	() const { return m_resources_count != 0; }
	void				clear			() 
	{ 
		m_resources_count				=	0; 
		for ( u32 index=0; index<array_size(m_resources); ++index )
			m_resources[index].set_zero		();
	}

private:
	u32   get_random_child_index ()
	{
		u32 skip_count					=	s_random.random(m_resources_count);
		u32 index						=	0;
		for ( index=0; index<array_size(m_resources); ++index )
			if ( m_resources[index].c_ptr() != 0 )
			{
				if ( !skip_count )
					break;
				--skip_count;
			}

		return								index;
	}

private:
	u32									m_resources_count;
	child_unmanaged_resource_ptr		m_resources[max_held_resources];
};

class game_resources_manager_tester
{
public:
	game_resources_manager_tester() 
	{
		s_random.seed						(1234);
		m_holder_ptr					=	& m_holder;
		m_holder.set_no_delete				();
	}

	void   on_mount_completed			(bool result) { TEST_ASSERT(result); }

	void   resources_callback			(xray::resources::queries_result & queries)
	{
		m_last_callback_result			=	queries.is_successful();
		if ( !m_last_callback_result )
			return;
		TEST_CURE_ASSERT_CMP				(queries.size(), ==, 1, return);
		TEST_CURE_ASSERT					(queries[0].is_successful(), return);
		test_resource_ptr resource		=	static_cast_resource_ptr<test_resource_ptr>(queries[0].get_unmanaged_resource());
		TEST_CURE_ASSERT					(resource, return);
		m_holder.add_resource				(resource);
		m_last							=	resource.c_ptr();
	}

	void   add_as_child (test_resource_ptr new_resource)
	{
		if ( s_random.random(4) )
			return;							// no childing

		u32 const parents_count			=	1 + s_random.random(3);
		u32 const max_tries				=	parents_count * 10;

		for ( u32 i=0, tries=0; i<parents_count && tries<max_tries; ++i, ++tries )
		{
			test_resource_ptr	parent	=	m_holder.get_random_child();
			bool const is_child_of_parent		=	parent->has_child(new_resource);
			if ( parent == new_resource || is_child_of_parent )
			{
				--i;
				continue;
			}

			parent->add_child				(new_resource);
			LOGI_INFO						("grm", "%s is now child of %s", new_resource->name(), parent->name());
		}
	}

	void   try_create_new_resource (u32 const resource_int_name, u32 const allocation_size)
	{
		pstr unused_buffer				=	NULL;
		STR_DUPLICATEA						(unused_buffer, "dont read/write this stack data");

		const_buffer						creation_buffer	(unused_buffer, allocation_size);
		fixed_string<32>					index_name;
		index_name.assignf					("%d", resource_int_name);
		bool const create_resource_with_quality	=	!s_random.random(2);
		class_id_enum const class_id			=	create_resource_with_quality ? 
													test_resource_class : s_class_ids[s_random.random(array_size(s_class_ids))];
		creation_request request			( index_name.c_str(), creation_buffer, class_id );
		
		if ( create_resource_with_quality )
		{
			math::float4x4					matrix;
			matrix.identity					();
			math::float4x4 const *			matrix_pointers[] = { & matrix };
			autoselect_quality_bool autoselect	=	autoselect_quality_true;
			query_resource_params	params	(NULL, 
											 &request, 1, 
											 boost::bind(&game_resources_manager_tester::resources_callback, this, _1), 
											 & memory::g_mt_allocator, 
											 NULL,
											 matrix_pointers);
			params.autoselect_quality	=	& autoselect;
			query_resources_and_wait		(params);
		}
		else
		{
			query_create_resources_and_wait	(&request, 
											 1,
											 boost::bind(&game_resources_manager_tester::resources_callback, this, _1), 
											 & memory::g_mt_allocator);
		}
	}

	void   do_test ()
	{
		for ( u32 mega_iteration=0; mega_iteration<1; ++mega_iteration )
		{
			LOGI_INFO						("grm", "random seed index %d", mega_iteration);
			s_random.seed					((1 + mega_iteration) * 123457);
		
		for ( u32 i=0; i<1 * resource_holder::max_held_resources * array_size(s_class_ids); ++i )
		{
			LOGI_INFO						("grm:test", "------------------------iteration %d------------------------", i);
			if ( (!s_random.random(3) || m_holder.is_full()) && m_holder.has_resources() )
			{
				m_holder.remove_random_resource		();
				continue;
			}

			u32 const allocation_size	=	min_resource_size + s_random.random(max_resource_size - min_resource_size);

			try_create_new_resource			(i, allocation_size);

			if ( m_last_callback_result )
				add_as_child				(m_last);
			else
				LOGI_INFO					("grm", "failed to allocate even after freeing all grm kept resources: %d", i);
		
			u64 const increase_quality_tick	=	current_increase_quality_tick();
			while ( current_increase_quality_tick() == increase_quality_tick )
			{
				if ( threading::g_debug_single_thread )
					resources::tick			();
			}
		}

		}
	
		release_and_dispatch_all_resources	();
		++m_iteration;
	}

	void release_and_dispatch_all_resources ()
	{
		m_holder.clear						();
		schedule_release_all_resources		();
		while ( is_release_all_resources_scheduled() )
		{
			resources::dispatch_callbacks	();
			if ( threading::g_debug_single_thread )
				resources::tick				();
		}
  			
 		resources::wait_and_dispatch_callbacks	(true);
	}

	void test							(core_test_suite * suite)
	{
		XRAY_UNREFERENCED_PARAMETERS					(suite);

		m_iteration						=	0;

		if ( debug::is_debugger_present() )
		{
			logging::push_filter			( "core:resources:allocator", logging::silent, & memory::g_mt_allocator );
			logging::push_filter			( "core:resources:test", logging::trace, & memory::g_mt_allocator );
			logging::push_filter			( "core:resources:device_manager", logging::trace, & memory::g_mt_allocator );
			logging::push_filter			( "core:resources", logging::trace, & memory::g_mt_allocator );
			logging::push_filter			( "core:resources", logging::warning, & memory::g_mt_allocator );
			logging::push_filter			( "core:grm", logging::silent, & memory::g_mt_allocator );
			logging::push_filter			( "core:grm:test", logging::trace, & memory::g_mt_allocator );
			
			
			logging::push_filter			( "core:grm", logging::trace, & memory::g_mt_allocator );
		}

		initialize_cooks					();
			do_test							();
		finalize_cooks						();

		for ( u32 i=0; i<test_allocators_count; ++i )
			s_test_allocators[i].finalize	();

		if ( debug::is_debugger_present() )
		{
			for ( u32 pop_count=0; pop_count<7; ++pop_count )
				logging::pop_filter			();
		}
	}

private:
	void unregister_cook (cook_base * cook)
	{
		u32 const time_for_cleanup		=	1000;

		timing::timer						timer;
		timer.start							();
		while ( cook->cook_users_count() && (timer.get_elapsed_msec() < time_for_cleanup || debug::is_debugger_present()) )
		{
			resources::dispatch_callbacks	();
			if ( threading::g_debug_single_thread )
				resources::tick				();
		}

		CURE_ASSERT							(timer.get_elapsed_msec() < time_for_cleanup || debug::is_debugger_present(), return, "cooker has failed to delete all resources/buffers, though it was given %d ms of time", time_for_cleanup);

		resources::unregister_cook			(cook->get_class_id());
	}

	void initialize_cooks ()
	{
		m_cooks.push_back					(MT_NEW(test_resource_cook)
											(test_resource_class1, 0, false, cook_base::use_current_thread_id, 
											 cook_base::use_current_thread_id));
		m_cooks.push_back					(MT_NEW(test_resource_cook)
											(test_resource_class2, 1, false, cook_base::use_current_thread_id, 
											cook_base::use_resource_manager_thread_id));
		m_cooks.push_back					(MT_NEW(test_resource_cook)
											(test_resource_class3, 2, true, cook_base::use_current_thread_id, 0));

		for ( u32 i=0; i<m_cooks.size(); ++i )
			register_cook					(m_cooks[i]);

		register_cook						(& m_resource_with_quality_cook);
	}

	void finalize_cooks ()
	{
		unregister_cook						(& m_resource_with_quality_cook);

		for ( u32 i=0; i<m_cooks.size(); ++i )
			unregister_cook					(m_cooks[i]);

		wait_and_dispatch_callbacks			(true);

		for ( u32 i=0; i<m_cooks.size(); ++i )
			MT_DELETE						(m_cooks[i]);
	}

private:
	fixed_vector<cook_base *, 3>			m_cooks;
	test_resource_with_quality_cook			m_resource_with_quality_cook;
	resource_holder							m_holder;
	unmanaged_resource_ptr					m_holder_ptr;
	test_resource *							m_last;
	
	u32										m_iteration;
	bool									m_last_callback_result;
};

//REGISTER_TEST_CLASS							(game_resources_manager_tester, core_test_suite);

} // namespace xray
} // namespace resources
