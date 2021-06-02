////////////////////////////////////////////////////////////////////////////
//	Created		: 30.03.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resources_manager.h"
#include "resources_thread_local_data.h"
#include "resources_device_manager.h"
#include "resources_allocators.h"
#include "managed_node.h"

namespace xray {
namespace resources {

void   resources_manager::push_to_device_manager (query_result & query)
{
	vfs::vfs_iterator const fat_it		=	query.get_fat_it();
	R_ASSERT								(!fat_it.is_end());

	device_manager * const manager		=	query.find_capable_device_manager();
	manager->push_query						(&query);

	wakeup_resources_thread					();
}

void   allocate_functionality::prepare_raw_resource_for_managed_or_unmanaged_cook (query_result * query, reallocating_bool reallocating)
{
	vfs::vfs_iterator const fat_it		=	query->get_fat_it();

	allocation_result_enum allocation_result	=	allocation_result_success;

	if ( query->creation_data_from_user() )
	{
		// will use creation data instead of raw resource
		query->prepare_final_resource			();
	}
	else if ( fat_it.is_end() )
	{
		// will generate in final resource
		query->prepare_final_resource			();
	}
	else if ( fat_it.data_node()->is_inlined() && !fat_it.is_compressed() )
	{
		// will use inline data instead of raw resource
		query->on_load_operation_end					();
	}
	else if ( fat_it.data_node()->is_inlined() && fat_it.is_compressed() )
	{
		allocation_result							=	query->allocate_raw_managed_resource_if_needed	();
		if ( allocation_result == allocation_result_success )
			query->on_load_operation_end				();
	}
	else if ( fat_it.is_compressed() )
	{		
		allocation_result				=	query->allocate_compressed_resource_if_needed	();
		if ( allocation_result == allocation_result_success )
			g_resources_manager->on_allocated_raw_resource	(query);
	}
	else
	{
		if ( reallocating == reallocating_false )
		{
			// when first time, let device manager to allocate it later
			g_resources_manager->on_allocated_raw_resource	(query);
		}
		else
		{
			allocation_result			=	query->allocate_raw_managed_resource_if_needed	();
			if ( allocation_result == allocation_result_success )
				g_resources_manager->on_allocated_raw_resource	(query);
		}
	}

	if ( allocation_result == allocation_result_failed )
	{
		query->end_query_might_destroy_this	();
		return;
	}
}

void   allocate_functionality::prepare_raw_resource_for_inplace_managed_cook (query_result * query, reallocating_bool reallocating)
{
	vfs::vfs_iterator const fat_it		=	query->get_fat_it();

	allocation_result_enum allocation_result	=	allocation_result_success;

	if ( query->creation_data_from_user() )
	{
		allocation_result				=	query->allocate_raw_managed_resource_if_needed	();
		if ( allocation_result == allocation_result_success )
		{
			bool const copied_data		=	query->copy_creation_data_to_resource_if_needed	();
			R_ASSERT_U						(copied_data);
			query->send_to_create_resource	();
		}
	}
	else if ( fat_it.is_end() )
	{
		allocation_result				=	query->allocate_raw_managed_resource_if_needed	();
		if ( allocation_result == allocation_result_success )
			query->send_to_create_resource	();		
	}
	else if ( fat_it.data_node()->is_inlined() && !fat_it.is_compressed() )
	{
		allocation_result				=	query->allocate_raw_managed_resource_if_needed	();
		if ( allocation_result == allocation_result_success )
		{
			bool const copied_data		=	query->copy_inline_data_to_resource_if_needed	();
			R_ASSERT_U						(copied_data);
			query->send_to_create_resource	();
		}
	}
	else if ( fat_it.data_node()->is_inlined() && fat_it.is_compressed() )
	{
		allocation_result				=	query->allocate_raw_managed_resource_if_needed	();
		if ( allocation_result == allocation_result_success )
			query->on_load_operation_end	();
	}
	else if ( fat_it.is_compressed() )
	{
		allocation_result				=	query->allocate_compressed_resource_if_needed	();
		if ( allocation_result == allocation_result_success )
			g_resources_manager->on_allocated_raw_resource	(query);
	}
	else
	{
		if ( reallocating == reallocating_false )
		{
			// when first time, let device manager to allocate it later
			g_resources_manager->on_allocated_raw_resource	(query);
		}
		else
		{
			allocation_result			=	query->allocate_raw_managed_resource_if_needed	();
			if ( allocation_result == allocation_result_success )
				g_resources_manager->on_allocated_raw_resource	(query);
		}
	}

	if ( allocation_result == allocation_result_failed )
	{
		query->end_query_might_destroy_this	();
		return;
	}
}

void   allocate_functionality::prepare_raw_resource_for_inplace_unmanaged_cook (query_result * query)
{
	vfs::vfs_iterator const fat_it		=	query->get_fat_it();

	allocation_result_enum allocation_result	=	allocation_result_success;

	if ( query->need_create_resource_inplace_in_creation_or_inline_data() )
	{
		query->bind_unmanaged_resource_buffer_to_creation_or_inline_data();
		query->send_to_create_resource		();
		return;
	}
	else if ( !fat_it.is_end() && query->is_compressed() && !fat_it.data_node()->is_inlined() )
	{
		allocation_result				=	query->allocate_compressed_resource_if_needed	();
	}

	if ( allocation_result == allocation_result_failed )
	{
		query->end_query_might_destroy_this	();
		return;
	}
	else if ( allocation_result == allocation_result_reallocate_scheduled )
		return;

	if ( query->raw_unmanaged_buffer() )
	{
		continue_prepare_raw_resource_for_inplace_unmanaged_cook	(query);
		return;
	}
	
	u32 const allocate_thread_id					=	query->allocate_thread_id();
	if ( allocate_thread_id != threading::current_thread_id() )
	{
		thread_local_data * const local_data			=	g_resources_manager->get_thread_local_data(allocate_thread_id, true);
		local_data->to_allocate_raw_resource.push_back		(query);
		g_resources_manager->wakeup_thread_by_id_if_needed	(allocate_thread_id);
		return;
	}
	
	allocation_result							=	query->allocate_raw_unmanaged_resource_if_needed	();
	if ( allocation_result == allocation_result_success )
	{
		continue_prepare_raw_resource_for_inplace_unmanaged_cook	(query);
	}
	else if ( allocation_result == allocation_result_failed )
	{
		query->end_query_might_destroy_this	();
		return;
	}
}

void   allocate_functionality::continue_prepare_raw_resource_for_inplace_unmanaged_cook (query_result * query)
{
	vfs::vfs_iterator const fat_it		=	query->get_fat_it();

	if ( query->creation_data_from_user() )
	{
		bool const copied_data			=	query->copy_creation_data_to_resource_if_needed	();
		R_ASSERT_U							(copied_data);
		query->send_to_create_resource		();
	}
	else if ( fat_it.is_end() )
	{
		query->send_to_create_resource		();		
	}
	else if ( fat_it.data_node()->is_inlined() && !fat_it.is_compressed() )
	{
		bool const copied_data			=	query->copy_inline_data_to_resource_if_needed	();
		R_ASSERT_U							(copied_data);
		query->send_to_create_resource		();
	}
	else if ( fat_it.data_node()->is_inlined() && fat_it.is_compressed() )
	{
		query->on_load_operation_end		();
	}
	else if ( fat_it.is_compressed() )
	{
		g_resources_manager->on_allocated_raw_resource	(query);
	}
	else
	{
		g_resources_manager->on_allocated_raw_resource	(query);
	}
}

void   allocate_functionality::prepare_raw_resource (query_result * query, reallocating_bool reallocating)
{
	class_id_enum const class_id				=	query->get_class_id();
	if ( cook_base::find_managed_cook(class_id) || 
		 cook_base::find_unmanaged_cook(class_id) )
	{
		prepare_raw_resource_for_managed_or_unmanaged_cook		(query, reallocating);
	}
	else if ( cook_base::find_inplace_managed_cook(class_id) ||
			 !cook_base::find_cook(class_id) )
	{
		prepare_raw_resource_for_inplace_managed_cook			(query, reallocating);
	}
	else if ( cook_base::find_inplace_unmanaged_cook(class_id) )
	{
		prepare_raw_resource_for_inplace_unmanaged_cook			(query);
	}
	else
		NOT_IMPLEMENTED();
}

void   allocate_functionality::prepare_final_resource (query_result * const query)
{
	class_id_enum const class_id			=	query->get_class_id();
	allocation_result_enum  allocation_result	
											=	allocation_result_success;

	if ( cook_base::find_managed_cook(class_id) )
	{
		if ( threading::current_thread_id() != g_resources_manager->resources_thread_id() )
		{
			m_queries_to_allocate_managed_resource.push_back	(query);		
			g_resources_manager->wakeup_resources_thread		();
			return;
		}

		allocation_result				=	query->allocate_final_managed_resource_if_needed();
		if ( allocation_result == allocation_result_success )
			query->on_allocated_final_resource	();
	}
	else if ( unmanaged_cook * const cook = cook_base::find_unmanaged_cook(class_id) )
	{
		if ( cook->has_flags(cook_base::flag_create_allocates_destroy_deallocates) )
		{
			// no allocation needed, create will allocate
			query->on_allocated_final_resource	();
			return;
		}

		u32 const allocate_thread_id		=	query->allocate_thread_id();
		if ( allocate_thread_id != threading::current_thread_id() )
		{
			thread_local_data * const tls		=	g_resources_manager->get_thread_local_data(allocate_thread_id, true);
			tls->to_allocate_resource.push_back		(query);
			g_resources_manager->wakeup_thread_by_id_if_needed	(allocate_thread_id);
			return;
		}

		allocation_result				=	query->allocate_final_unmanaged_resource_if_needed();
		if ( allocation_result == allocation_result_success )
			query->on_allocated_final_resource		();
	}
	else
		NOT_IMPLEMENTED								();

	if ( allocation_result == allocation_result_failed )
		query->end_query_might_destroy_this	();
}

void   allocate_functionality::tick (bool finalizing_thread)
{
	if ( threading::current_thread_id() == g_resources_manager->resources_thread_id() )
	{
		allocate_final_resources				(m_queries_to_allocate_managed_resource, finalizing_thread);
	}
	else
	{
		thread_local_data * const tls		=	g_resources_manager->get_thread_local_data(threading::current_thread_id(), false);
		R_ASSERT								(tls);
		
		allocate_final_resources				(tls->to_allocate_resource, finalizing_thread);
		allocate_raw_resources					(tls, finalizing_thread);
	}
}

template <class query_list>
void   allocate_functionality::allocate_final_resources (query_list & queries, bool finalizing_thread)
{
	query_result * it_query				=	queries.pop_all_and_clear(); 

	while ( it_query )
	{
		query_result * const next_query	=	queries.get_next_of_object(it_query);
		
		if ( finalizing_thread )
		{
			it_query->set_error_type		(query_result::error_type_canceled_by_finalization);
			it_query->end_query_might_destroy_this	();
		}
		else
		{
			prepare_final_resource			(it_query);
		} 

		it_query						=	next_query;
	}
}

void   allocate_functionality::allocate_raw_resources (thread_local_data * tls, bool finalizing_thread)
{
	bool set_event						=	false;

	query_result * it_query				=	tls->to_allocate_raw_resource.pop_all_and_clear();

	while ( it_query )
	{
		query_result* const next_query	=	tls->to_allocate_raw_resource.get_next_of_object(it_query);
		if ( finalizing_thread )
		{
			it_query->set_error_type		(query_result::error_type_canceled_by_finalization);
			it_query->end_query_might_destroy_this	();
		}
		else
			prepare_raw_resource			(it_query, reallocating_false);

		it_query						=	next_query;
		set_event						=	true;
	}

	if ( set_event )
		g_resources_manager->wakeup_resources_thread	();
}

void   resources_manager::dispatch_allocated_raw_resources ()
{
	query_result * it_query				=	m_queries_with_allocated_raw_resources.pop_all_and_clear();

	while ( it_query )
	{
		query_result * const next		=	m_queries_with_allocated_raw_resources.get_next_of_object(it_query);
		push_to_device_manager				(* it_query);
		it_query						=	next;
	}
}

pvoid   resources_manager::allocate_unmanaged_memory (u32 size, pcstr type_name)
{
	XRAY_UNREFERENCED_PARAMETER				( type_name );
	return									XRAY_MALLOC_IMPL(memory::g_resources_unmanaged_allocator, size, type_name);
}

managed_resource *   resources_manager::allocate_managed_resource (u32 size, class_id_enum class_id)
{
	memory::managed_node * const node	=	memory::g_resources_managed_allocator.allocate(size);
	if ( !node )
		return								NULL;
	managed_resource * const out_resource	=	RES_NEW(managed_resource) (size, class_id);
	node->set_owner							(out_resource);
	out_resource->set_managed_node			(node);
	out_resource->set_is_unmovable			(true);

#if DO_RESOURCE_LOGGING
	LOGI_INFO("resources:allocator",		"%s %s", "allocated", res->log_string().c_str());
#endif // #if DO_RESOURCE_LOGGING

	return									out_resource;
}

void   resources_manager::free_managed_resource (managed_resource * resource)
{
	class_id_enum const class_id		=	resource->get_class_id();
//	cook_base * const cook				=	find_cook(class_id);

	fixed_string512 request_name		=	
#ifdef MASTER_GOLD
		"";
#else
		resource->m_request_path.c_str();
#endif

	memory_usage_type const memory_usage	=	resource->memory_usage();
	query_result * const destruction_observer	=	resource->get_destruction_observer();

	// we save reference to sub_fat here, so resource will have valid m_fat_it even after 
	// it frees its sub_fat reference
	vfs_sub_fat_resource_ptr const sub_fat_holder	=	resource->get_sub_fat();
	resource->set_sub_fat_resource			(NULL);

	LOGI_INFO								("resources", "deleted %s", log_string(resource).c_str());
	memory::g_resources_managed_allocator.deallocate	(resource->grab_managed_node());
	RES_DELETE								(resource);

	after_resource_deleted					(NULL, false, destruction_observer, memory_usage, class_id, request_name.c_str());

#if DO_RESOURCE_LOGGING
	LOGI_INFO("resources:allocator",		"deallocated %s", res->log_string().c_str());
#endif // #if DO_RESOURCE_LOGGING
}

void   resources_manager::on_allocated_raw_resource	(query_result * query) 
{ 
	m_queries_with_allocated_raw_resources.push_back(query); 
	wakeup_resources_thread					();
}

} // namespace resources
} // namespace xray
