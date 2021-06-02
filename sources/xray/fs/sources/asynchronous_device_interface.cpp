////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs/asynchronous_device_interface.h>

namespace xray {
namespace fs_new {

asynchronous_device_interface::asynchronous_device_interface (synchronous_device_interface *	async_interface)
	:	m_device(async_interface->m_device), m_device_mode(device_mode_synchronous), 
		m_processing_queries(false), m_synchronous_thread_id(u32(-1))
{
}

asynchronous_device_interface::asynchronous_device_interface (device_file_system_interface *	device, 
															  watcher_enabled_bool				watcher_enabled)
	:	m_device(device, watcher_enabled), m_device_mode(device_mode_asynchronous), 
		m_processing_queries(false), m_synchronous_thread_id(u32(-1))
{
}

void   asynchronous_device_interface::get_synchronous_access (synchronous_device_interface *	in_out_synchronous_interface, 
															  memory::base_allocator *			allocator)
{
	synchronize_device_query * synchronize_query	=	NULL;

	if ( m_device_mode == device_mode_asynchronous && m_synchronous_thread_id != threading::current_thread_id() )
	{
		synchronize_query				=	XRAY_NEW_WITH_CHECK_IMPL(allocator, synchronize_query, synchronize_device_query)
												(this, threading::current_thread_id(), allocator);
		if ( synchronize_query )
			push_high_priority_query		(synchronize_query);
		else
			in_out_synchronous_interface->set_out_of_memory	();
	}

	in_out_synchronous_interface->initialize_private	(m_device, synchronize_query);
}

void   asynchronous_device_interface::push_query (query_type * query)
{
	m_queries.producer_push_to_process		(query);
	m_wakeup_event.set						(true);
}

void   asynchronous_device_interface::push_high_priority_query (query_type * query) 
{
	m_high_priority_queries.producer_push_to_process	(query);
	m_wakeup_event.set						(true);
}

void   asynchronous_device_interface::tick (bool const call_from_dedicated_thread)
{
	process_queries							();
	if ( call_from_dedicated_thread )
		m_wakeup_event.wait					(10);
}

void   asynchronous_device_interface::process_queries ()
{
	process_queries							(& m_high_priority_queries);
	process_queries							(& m_queries);
}

void   asynchronous_device_interface::dispatch_callbacks ()
{
	dispatch_callbacks						(& m_high_priority_queries);
	dispatch_callbacks						(& m_queries);
}

void   asynchronous_device_interface::dispatch_callbacks (queries_channel * queries)
{
	while ( query_type * query = queries->producer_pop_processed() )
		query->callback_if_needed			();
}

void   asynchronous_device_interface::finalize ()
{
	if ( m_device_mode == device_mode_synchronous )
		return;
	
	process_queries							();
	m_queries.consumer_finalize				();
	m_high_priority_queries.consumer_finalize	();
}

void   asynchronous_device_interface::finalize_thread_usage	()
{
	if ( m_device_mode == device_mode_synchronous )
		return;

	dispatch_callbacks						();
	m_queries.producer_finalize				();
	m_high_priority_queries.producer_finalize	();
}

void   asynchronous_device_interface::process_queries	(queries_channel * queries)
{
	threading::interlocked_exchange			(m_processing_queries, true);

	bool const processing_normal_queries =	(queries == & m_queries);
	while ( query_type * query = queries->consumer_pop_to_process() )
	{
		// process high priorities first
		if ( processing_normal_queries )
			process_queries					(& m_high_priority_queries);

		query->execute						();
		if ( query->event_to_fire_after_execute() )
			query->event_to_fire_after_execute()->set	(true);

		queries->consumer_push_processed	(query);
	}

	threading::interlocked_exchange			(m_processing_queries, false);
}

void   asynchronous_device_interface::set_synchronous_thread_id	(threading::thread_id_type const thread_id) 
{ 
	if ( thread_id != u32(-1) )
		R_ASSERT_CMP						(m_synchronous_thread_id, ==, u32(-1)); 
	else
		R_ASSERT							(m_synchronous_thread_id != u32(-1));

	m_synchronous_thread_id				=	thread_id; 
}

} // namespace fs_new
} // namespace xray