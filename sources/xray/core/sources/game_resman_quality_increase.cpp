////////////////////////////////////////////////////////////////////////////
//	Created		: 03.08.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "game_resman_quality_increase.h"
#include "resources_manager.h"
#include "game_resman_test_utils.h"
#include <xray/testing.h>

namespace xray {
namespace resources {

static	float const		retry_to_increase_quality_period_sec	=	2;
float			quality_increase_functionality::s_elapsed_sec_from_start	=	0;
timing::timer	quality_increase_functionality::s_tick_timer;
bool			quality_increase_functionality::s_started_tick_timer	=	false;

quality_increase_functionality::quality_increase_functionality (game_resources_manager_data & data) : m_data(data)
{
	if ( !s_started_tick_timer )
	{
		s_started_tick_timer			=	true;
		s_tick_timer.start					();
	}
}

void   quality_increase_functionality::erase_from_increase_quality_tree (resource_base * resource)
{
	ASSERT									(resource->has_flags(resource_flag_in_grm_increase_quality_tree));
	ASSERT									(resource->is_resource_with_quality());
	m_data.increase_quality_tree.erase		(* resource);
	resource->unset_flags					(resource_flag_in_grm_increase_quality_tree);
}

void   quality_increase_functionality::insert_to_increase_quality_tree (resource_base * resource)
{
	ASSERT									(resource->is_resource_with_quality());
	ASSERT									(!resource->has_flags(resource_flag_in_grm_increase_quality_tree));
	resource->set_flags						(resource_flag_in_grm_increase_quality_tree);
	m_data.increase_quality_tree.insert		(* resource);
}

void   quality_increase_functionality::update_current_satisfaction_for_resource (resource_base * resource)
{
	bool const in_increase_quality_tree	=	resource->has_flags(resource_flag_in_grm_increase_quality_tree);
	if ( resource->is_resource_with_quality() )
	{
		if ( in_increase_quality_tree )
			erase_from_increase_quality_tree	(resource);
	}

	resource->update_satisfaction			(m_data.current_increase_quality_tick);
	
	if ( resource->is_resource_with_quality() )
	{
		resource->set_target_quality		(resource->current_quality_level(), resource->current_satisfaction());
		if ( in_increase_quality_tree )
			insert_to_increase_quality_tree		(resource);
	}
}

void   quality_increase_functionality::update_current_satisfaction_for_memory_type (memory_type * memory_type_resources)
{
	for ( resource_base *	it_resource	=	memory_type_resources->resources.front();
							it_resource;
							it_resource	=	memory_type_resources->resources.get_next_of_object(it_resource) )
	{
		if ( it_resource->is_resource_with_quality() )
		if ( it_resource->target_quality_level() != it_resource->current_quality_level() )
		{
			if ( !it_resource->is_increasing_quality() )
			{
				// failed to increase quality, cooldown with this resource for some time
				update_current_satisfaction_for_resource			(it_resource);
				it_resource->set_last_fail_of_increasing_quality	(s_elapsed_sec_from_start);
			}
			continue;
		}

		update_current_satisfaction_for_resource	(it_resource);
	}
}

void   quality_increase_functionality::update_current_satisfaction ()
{
	for ( memory_type * it_memory		=	m_data.memory_types.front();
						it_memory;
						it_memory		=	m_data.memory_types.get_next_of_object(it_memory) )
	{
		update_current_satisfaction_for_memory_type	(it_memory);
	}
}

void   quality_increase_functionality::select_resource_to_increase_quality (resource_base * const			resource, 
																			increase_quality_queue * const	out_increase_queue)
{
	u32 const	old_target_level		=	resource->target_quality_level();
	u32 const	new_target_level		=	old_target_level == u32(-1) ? 
											resource->quality_levels_count() - 1 : old_target_level - 1;

	float const new_target_satisfaction	=	resource->satisfaction(new_target_level);

	// resource can be 0-refed and satisfaction blazingly become > min_satisfaction_of_unreferenced_resource()
	//float const old_target_satisfaction	=	resource->target_satisfaction();
	//R_ASSERT_CMP_U						(new_target_satisfaction, >=, old_target_satisfaction);

	erase_from_increase_quality_tree		(resource);
	resource->set_target_quality			(new_target_level, new_target_satisfaction);
	insert_to_increase_quality_tree			(resource);

	if ( !resource->has_flags(resource_flag_in_grm_increase_quality_queue) )
	{
		resource->set_flags					(resource_flag_in_grm_increase_quality_queue);
		out_increase_queue->push_back		(resource);
	}
}

resource_base *   quality_increase_functionality::find_next_to_select_to_increase_quality ()
{
	for ( target_satisfaction_tree_type::iterator	it		=	m_data.increase_quality_tree.begin(),
													end_it	=	m_data.increase_quality_tree.end();
													it		!=	end_it;
													++it )
	{
		resource_base * const resource	=	& * it;
		if ( resource->current_quality_level() == 0 )
			continue;

		bool const in_queue				=	resource->has_flags(resource_flag_in_grm_increase_quality_queue);
		bool const increasing			=	resource->current_quality_level() != resource->target_quality_level();
		bool const increasing_from_last_tick	=	!in_queue && increasing;
		
		if ( increasing_from_last_tick )
			continue;

		if ( s_elapsed_sec_from_start < resource->last_fail_of_increasing_quality() + retry_to_increase_quality_period_sec )
			continue;

		if ( resource->target_quality_level() == 0 )
			continue;

		if ( resource->target_satisfaction() >= min_satisfaction_of_unreferenced_resource() )
			return							false;
		
		return								resource;
	}

	return									NULL;
}

void   quality_increase_functionality::select_to_increase_quality (increase_quality_queue * out_increase_queue)
{
	u32 const max_to_schedule_per_tick	=	15;
	u32 currently_scheduled				=	0;

	while ( resource_base * const resource = find_next_to_select_to_increase_quality() )
	{
		bool const already_in_queue		=	resource->has_flags(resource_flag_in_grm_increase_quality_queue);
		select_resource_to_increase_quality	(resource, out_increase_queue);
		if ( !already_in_queue )
			++currently_scheduled;

		if ( currently_scheduled == max_to_schedule_per_tick )
			break;
	}
}

void   quality_increase_functionality::schedule_to_increase_quality (increase_quality_queue & in_out_increase_queue)
{
	while ( !in_out_increase_queue.empty() )
	{
		resource_base * const resource	=	in_out_increase_queue.pop_front();
		ASSERT								(resource->has_flags(resource_flag_in_grm_increase_quality_queue));

		resource->unset_flags				(resource_flag_in_grm_increase_quality_queue);

		query_result * const query		=	resource->cast_query_result();
		LOGI_DEBUG							("grm", "%s %s scheduling increase quality %d->%d (target satisfaction: %f)", 
											query ? "query" : "resource", 
											log_string(resource).c_str(), 
											resource->current_quality_level(),
											resource->target_quality_level(),
											resource->target_satisfaction());

		resource->increase_quality_to_target	(query);
	}
}

void   quality_increase_functionality::log_increase_quality_queue ()
{
	fixed_string8192						log_string;
	for ( target_satisfaction_tree_type::iterator	it		=	m_data.increase_quality_tree.begin(),
													end_it	=	m_data.increase_quality_tree.end();
													it		!=	end_it;
													++it )
	{
		resource_base * const resource	=	& * it;
		log_string.appendf("%s(", resources::log_string(resource).c_str());
		if ( resource->current_quality_level() == resource->target_quality_level() )
			log_string.appendf("%0.2f) ", resource->target_satisfaction());
		else
			log_string.appendf("%0.2f->%0.2f) ", resource->current_satisfaction(), resource->target_satisfaction());
	}
	LOGI_DEBUG									("grm", "increase quality queue: %s", log_string.c_str());
}

void   quality_increase_functionality::tick ()
{
	R_ASSERT_CMP							(threading::current_thread_id(), ==, g_resources_manager->resources_thread_id());

	if ( s_tick_timer.get_elapsed_msec() < tick_period && !testing::run_tests_command_line() )
		return;

	s_elapsed_sec_from_start			=	m_data.increase_quality_timer.get_elapsed_sec();
	update_current_satisfaction				();

	increase_quality_queue					increase_queue;
	select_to_increase_quality				(& increase_queue);

	if ( !increase_queue.empty() )
		log_increase_quality_queue			();

	schedule_to_increase_quality			(increase_queue);

	++m_data.current_increase_quality_tick;
	s_tick_timer.start						();
}

} // namespace xray
} // namespace resources

