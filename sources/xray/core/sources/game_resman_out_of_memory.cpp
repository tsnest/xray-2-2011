////////////////////////////////////////////////////////////////////////////
//	Created		: 05.08.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "game_resman.h"
#include "game_resman_quality_decrease.h"
#include "game_resman_sorting.h"
#include "resources_manager.h"

namespace xray {
namespace resources {

bool   game_resources_manager::try_free_or_decrease_quality (query_result * query, memory_type * info)
{
	memory_usage_type const &			required_memory	=	query->get_out_of_memory();
	resources_to_free_collection		collection			(& required_memory, info, query);
	resource_freeing_functionality		resource_freeing	(m_data, collection);

	if ( resource_freeing.try_collect_to_free() )
	{
		resource_freeing.free_collected		();
		return								true;
	}

	return									false;
}

bool   game_resources_manager::try_reallocate_queue_front (memory_type * info)
{
	query_result * const query			=	info->queue.front();
	if ( !query->ready_to_retry_action_that_caused_out_of_memory() )
		return								false; // not all waited resources deleted

	return									query->retry_action_that_caused_out_of_memory();
}

bool   game_resources_manager::try_reallocate_queue	(memory_type * info)
{
	bool reallocated_some				=	false;
	while ( !info->queue.empty() )
	{
		query_result * const query		=	info->queue.front();
		query->increase_query_end_guard		();

		bool const reallocated			=	try_reallocate_queue_front(info);

		if ( reallocated )
			info->queue.pop_front			();

		query->end_query_might_destroy_this	();
		
		if ( !reallocated )
			break;

		reallocated_some				=	true;
	}

	return									reallocated_some;
}

void   game_resources_manager::on_resource_freed_callback (query_result * destruction_observer, memory_usage_type const & memory_usage, class_id_enum class_id)
{
	XRAY_UNREFERENCED_PARAMETERS			(class_id);

	memory_type * info					=	(memory_type *)memory_usage.type;

	if ( !info )
		return;

	if ( info->queue.empty() )
		return;

	memory_type::out_of_memory_queue::mutex_raii	raii(info->queue);
	
	bool const listen_deallocation		=	destruction_observer || (info->listen_type == memory_type::listen_all);
	if ( !listen_deallocation )
		return;

	if ( info->listen_type != memory_type::listen_all )
		info->listen_type				=	memory_type::listen_none;

	if ( try_reallocate_queue(info) )
	{
		info->listen_type				=	memory_type::listen_none;
		g_resources_manager->wakeup_resources_thread	();
	}
}

void   game_resources_manager::out_of_memory_callback (query_result * query)
{
	LOGI_INFO								("grm", "%s : out of memory (query satisfaction: %0.2f) [needed: %s]", 
											 log_string(query).c_str(),
											 query->target_satisfaction(),
											 query->get_out_of_memory().log_string().c_str());

	if ( query->has_flag(query_result::flag_in_grm_out_of_memory_queue) )
		return;

	memory_usage_type required_memory	=	query->get_out_of_memory();
	R_ASSERT								(required_memory.type != & nocache_memory);
		
	memory_type * const info			=	(memory_type *)required_memory.type;
	if ( !info->in_list )
	{
		m_data.memory_types.push_back		(info);
		info->in_list					=	true;
	}
	
	query->set_flag							(query_result::flag_in_grm_out_of_memory_queue);
	
	memory_type::out_of_memory_queue::mutex_raii	raii	(info->queue);
	info->queue.push_back					(query);

	info->listen_type					=	memory_type::listen_none;
	g_resources_manager->wakeup_resources_thread	();
}

void   game_resources_manager::tick_memory_type (memory_type * const info)
{
	u32 const sec_to_wait				=	10;
	if ( info->queue.empty() )
		return;

	if ( info->listen_type == memory_type::listen_freed )
	{
		// waiting scheduled freeing, will try reallocate on callback
		return;
	}

	memory_type::out_of_memory_queue::mutex_raii	raii	(info->queue);
	if ( info->queue.empty() )
		return;

	if ( info->listen_type == memory_type::listen_all && 
		 (info->listen_all_timer.get_elapsed_sec() < sec_to_wait) )
	{
		// timeout not yet passed
		return;
	}

	sorting_functionality	sorting			(m_data);
	sorting.sort_resources_if_needed		(info);

	query_result * query				=	info->queue.front();

	memory_type::listen_enum const saved_listen_type	=	info->listen_type;

	info->listen_type					=	memory_type::listen_freed;
	if ( try_free_or_decrease_quality (query, info) )
		return;

	info->listen_type					=	saved_listen_type;	
	
	LOGI_INFO								("grm", "couldn't free anything for %s", log_string(query).c_str());

	if ( info->listen_type == memory_type::listen_none )
	{
		info->listen_type				=	memory_type::listen_all;
		info->listen_all_timer.start		();
		return;
	}

	bool const can_reallocate			=	(info == & managed_memory) || (info == & unmanaged_memory);
	if ( can_reallocate && try_reallocate_queue(info) )
	{
		info->listen_type				=	memory_type::listen_none;
		g_resources_manager->wakeup_resources_thread	();
		return;
	}
	
	info->queue.pop_front					();
	LOGI_INFO								("grm", "%d sec passed and no deallocation - failing query %s", 
											 sec_to_wait, log_string(query).c_str());

	query->end_query_might_destroy_this		();
	
	if ( info->queue.empty() )
		info->listen_type				=	memory_type::listen_none;
	else
	{
		info->listen_type				=	memory_type::listen_all;
		info->listen_all_timer.start		();
	}
}

void   game_resources_manager::tick_memory_types ()
{
	for ( memory_type * it_memory		=	m_data.memory_types.front();
						it_memory;
						it_memory		=	m_data.memory_types.get_next_of_object(it_memory) )
	{
		tick_memory_type					(it_memory);
	}
}


} // namespace resources
} // namespace xray
