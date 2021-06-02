////////////////////////////////////////////////////////////////////////////
//	Created		: 24.06.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resources_manager.h"
#include "resources_thread_local_data.h"

namespace xray {
namespace resources {

void   resources_manager::push_delayed_delete_managed_resource (managed_resource * res)
{
	ASSERT_CMP								(threading::current_thread_id(), !=, static_cast<u32>( m_resources_thread_id ));
	m_delayed_delete_managed_resources.push_back	(res);
	wakeup_resources_thread					();
}

void   resources_manager::push_delayed_delete_unmanaged_resource (unmanaged_resource * dying_resource)
{
	debug()->register_delay_delete_unmanaged	(dying_resource);	

	threading::thread_id_type const destruction_thread_id	=	dying_resource->destruction_thread_id();
	ASSERT									(threading::current_thread_id() != destruction_thread_id);

	cook_base * const cook				=	find_cook( dying_resource->get_class_id() );
	threading::thread_id_type const deallocation_thread_id	=	cook->allocate_thread_id();

	dying_resource->set_delay_delete		();
	thread_local_data *	const thread_data	=	get_thread_local_data(destruction_thread_id, true);

	threading::interlocked_increment		(m_delay_delete_unmanaged_resources_count);

	if ( cook->does_deallocate() && destruction_thread_id != deallocation_thread_id )
	{
		thread_local_data *	const deallocation_thread_data	=	get_thread_local_data(deallocation_thread_id, true);
		threading::interlocked_increment	(deallocation_thread_data->resources_to_deallocate_after_destroy_in_other_thread_count);
	}

	thread_data->delayed_delete_unmanaged_resources.push_back	(dying_resource);

	wakeup_thread_by_id_if_needed			(destruction_thread_id);
}

void   resources_manager::after_resource_deleted (cook_base * cook, bool was_delay_delete, query_result * const destruction_observer, memory_usage_type const & memory_usage, class_id_enum class_id, pcstr request_name)
{
	if ( was_delay_delete )
	{
		ASSERT_CMP							(m_delay_delete_unmanaged_resources_count, >, 0);
		threading::interlocked_decrement	(m_delay_delete_unmanaged_resources_count);
	}

	if ( cook )
		cook->change_cook_users_count		(-1);

	if ( destruction_observer )
		destruction_observer->on_observed_resource_destroyed	(request_name);

	resource_freed_callback const callback	=	get_resource_freed_callback();
 	if ( callback )
 		callback							(destruction_observer, memory_usage, class_id);
}

void   resources_manager::delete_unmanaged_resource (unmanaged_resource * dying_resource)
{
	cook_base * const cook				=	find_cook( dying_resource->get_class_id() );
	ASSERT									(cook);
	R_ASSERT								(dying_resource->destruction_thread_id() == threading::current_thread_id() 
											 || cook->destroy_in_any_thread());

	pvoid const buffer_ptr				=	dynamic_cast<pvoid>(dying_resource);
	memory_usage_type const memory_usage	=	dying_resource->memory_usage();
	bool const is_delay_delete			=	dying_resource->is_delay_delete();
	resource_base::creation_source_enum	creation_source	=	dying_resource->creation_source();
	bool const is_helper_deallocate_resource	=	(dying_resource->creation_source() == resource_base::creation_source_deallocate_buffer_helper);
	query_result * const destruction_observer	=	dying_resource->get_destruction_observer();

	fs_new::virtual_path_string const request_path	=	dying_resource->request_path();
	
	if ( is_delay_delete )
		debug()->unregister_delay_delete_unmanaged	(dying_resource);

	if ( is_helper_deallocate_resource )
	{
		dying_resource->~unmanaged_resource	();
	}
	else
	{
		cook->call_destroy_resource			(dying_resource);
		if ( !cook->does_deallocate() )
		{
			after_resource_deleted			(cook, is_delay_delete, destruction_observer, 
											 memory_usage, cook->get_class_id(), request_path.c_str());
			return;
		}
	}

	u32 const deallocate_thread_id	=	cook->allocate_thread_id();

	bool const is_delay_deallocate	=	(threading::current_thread_id() != deallocate_thread_id);
	unmanaged_resource_buffer * buffer	=	new (buffer_ptr) unmanaged_resource_buffer
		(creation_source, request_path.c_str(), memory_usage, cook->get_class_id(),  destruction_observer, is_delay_delete, is_delay_deallocate);
	if ( threading::current_thread_id() == deallocate_thread_id )
	{
		deallocate_unmanaged_resource	(buffer);
	}
	else
	{
		debug()->register_delay_deallocate_unmanaged_buffer	(buffer);

		thread_local_data *	const thread_data	=	get_thread_local_data	(deallocate_thread_id, true);

		thread_data->delayed_deallocate_unmanaged_resources.push_back	(buffer);

		if ( is_delay_delete )
		{
			R_ASSERT_CMP	(thread_data->resources_to_deallocate_after_destroy_in_other_thread_count, >, 0);
			threading::interlocked_decrement	(thread_data->resources_to_deallocate_after_destroy_in_other_thread_count);
		}

		wakeup_thread_by_id_if_needed		(deallocate_thread_id);
	}
}

void   resources_manager::deallocate_unmanaged_resource (unmanaged_resource_buffer * resource_buffer)
{
	bool const is_delay_deallocate		=	resource_buffer->is_delay_deallocate();
	bool const is_delay_delete			=	resource_buffer->is_delay_delete();

	fs_new::virtual_path_string	const request_path	=	resource_buffer->request_name();

	if ( is_delay_deallocate )
		debug()->unregister_delay_deallocate_unmanaged_buffer	(resource_buffer);

#ifndef MASTER_GOLD
	LOGI_INFO								("resources", "deallocating '%s'", request_path.c_str());
#endif

	memory_usage_type const memory_usage	=	resource_buffer->get_memory_usage();
	class_id_enum const class_id			=	resource_buffer->get_class_id();
	query_result * const destruction_observer	=	resource_buffer->get_destruction_observer();

	cook_base * const cook				=	find_cook(resource_buffer->get_class_id());
	ASSERT									(cook);
	ASSERT									(!cook->cast_translate_query_cook());

	resource_buffer->~unmanaged_resource_buffer	();
	cook->deallocate_resource				(resource_buffer);

	after_resource_deleted					(cook, is_delay_delete, destruction_observer, memory_usage, class_id, request_path.c_str());
}

void   resources_manager::delete_delayed_unmanaged_resources ()
{
	thread_local_data *	thread_data		=	get_thread_local_data(threading::current_thread_id(), false);
	if ( !thread_data )
		return;

	if ( thread_data->delayed_delete_unmanaged_resources.empty() )
		return;

	threading::interlocked_exchange			(thread_data->deleting_resources, 1);

	unmanaged_resource * dying_resource	=	thread_data->delayed_delete_unmanaged_resources.pop_all_and_clear();
	while ( dying_resource )
	{
		unmanaged_resource * const next	=	thread_data->delayed_delete_unmanaged_resources.get_next_of_object(dying_resource);
		delete_unmanaged_resource			(dying_resource);
		dying_resource					=	next;
	}

	threading::interlocked_exchange			(thread_data->deleting_resources, 0);
}

void   resources_manager::deallocate_delayed_unmanaged_resources ()
{
	thread_local_data *	thread_data		=	get_thread_local_data(threading::current_thread_id(), false);
	if ( !thread_data )
		return;

	threading::interlocked_exchange			(thread_data->deallocating_resources, 1);

	unmanaged_resource_buffer * buffer_to_deallocate	=	thread_data->delayed_deallocate_unmanaged_resources.pop_all_and_clear();
	while ( buffer_to_deallocate )
	{
		unmanaged_resource_buffer * const next	=	thread_data->delayed_deallocate_unmanaged_resources.get_next_of_object(buffer_to_deallocate);
		deallocate_unmanaged_resource		(buffer_to_deallocate);
		buffer_to_deallocate			=	next;
	}

	threading::interlocked_exchange			(thread_data->deallocating_resources, 0);
}

void   resources_manager::delete_delayed_managed_resources ()
{
	managed_resource * it_dying_resource =	m_delayed_delete_managed_resources.pop_all_and_clear();
	while ( it_dying_resource )
	{
		managed_resource * const next_it =	m_delayed_delete_managed_resources.get_next_of_object(it_dying_resource);
		free_managed_resource				(it_dying_resource);
		it_dying_resource				=	next_it;
	}
}

} // namespace resources
} // namespace xray