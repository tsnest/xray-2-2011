////////////////////////////////////////////////////////////////////////////
//	Created		: 28.12.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/resources_query_result.h>
#include "resources_manager.h"
#include <xray/managed_allocator.h>
#include "resources_device_manager.h"
#include "game_resman_test_utils.h"

namespace xray {
namespace resources {

bool   query_result::try_synchronous_cook_from_inline_data ()
{
	if ( m_fat_it.is_end() )
		return							false;

	if ( m_fat_it.is_compressed() )
		return							false;

	if ( !m_fat_it.data_node()->is_inlined() )
		return							false;

	if ( !cook_base::allow_sync_load_from_inline(m_class_id) )
		return							false;

	if ( cook_base::cooks_managed_resource(m_class_id) )
		return							false;

	if ( allocate_thread_id() != threading::current_thread_id() )
		return							false;

	if ( m_raw_unmanaged_buffer )
		copy_inline_data_to_resource_if_needed();

	if ( need_create_resource_inplace_in_creation_or_inline_data() )
		bind_unmanaged_resource_buffer_to_creation_or_inline_data();
	else
		allocate_final_unmanaged_resource_if_needed	();

	increase_query_end_guard			();

		do_create_resource_impl			();

	if ( end_query_might_destroy_this () )
		return							true;

	R_ASSERT							(m_create_resource_result != cook_base::result_out_of_memory &&
										 m_create_resource_result != cook_base::result_postponed);

	on_create_resource_end				();
	return								true;
}

void   query_result::copy_data_to_resource (const_buffer data)
{
	R_ASSERT							(creation_data_from_user() || m_fat_it.data_node()->is_inlined());
	if ( m_raw_unmanaged_buffer )
	{
		pbyte const dest_data		=	(pbyte)m_raw_unmanaged_buffer.c_ptr() + m_offset_to_file;
		memory::copy					(mutable_buffer(dest_data, data.size()), data);
	}
	else if ( m_compressed_resource || m_raw_managed_resource )
	{
		managed_resource_ptr	dest_resource	=	(!m_fat_it.is_end() && m_fat_it.is_compressed()) ? 
											 m_compressed_resource : m_raw_managed_resource;

		R_ASSERT							(dest_resource);
		pinned_ptr_mutable<u8> dest_buffer	(dest_resource);
		memory::copy					(mutable_buffer(dest_buffer) + m_offset_to_file, data);
	}
	else
		NOT_IMPLEMENTED();
}

bool   query_result::copy_inline_data_to_resource_if_needed ()
{
	R_ASSERT							(m_fat_it.data_node()->is_inlined());
	const_buffer						inline_data;
	bool const got_inline_data		=	m_fat_it.get_inline_data(& inline_data);
	R_ASSERT_U							(got_inline_data);

	copy_data_to_resource				(inline_data);
	return								true;
}

bool   query_result::copy_creation_data_to_resource_if_needed ()
{
	const_buffer creation_data		=	creation_data_from_user();
	R_ASSERT							(creation_data);
	copy_data_to_resource				(creation_data);
	return								true;
}

allocation_result_enum   query_result::allocate_compressed_resource_if_needed ()
{
	if ( m_compressed_resource )
		return								allocation_result_success;

	if ( m_fat_it.is_end() )
		return								allocation_result_failed;

	if ( !m_fat_it.is_compressed() )
		return								allocation_result_failed;

	if ( m_fat_it.data_node()->is_inlined() )
		return								allocation_result_failed;

	u32 const compressed_size			=	m_fat_it.get_compressed_file_size();
	m_compressed_resource				=	g_resources_manager->allocate_managed_resource(compressed_size, raw_data_class);

	if ( !m_compressed_resource )
	{
		set_out_of_memory					(managed_memory, compressed_size);
		m_out_of_memory_type			=	out_of_memory_on_allocate_raw_resource;
		set_error_type						(error_type_out_of_memory);

		if ( out_of_memory_callback callback = get_out_of_memory_callback() )
		{
			callback						(this);
			return							allocation_result_reallocate_scheduled;
		}

		return								allocation_result_failed;
	}

	if ( m_compressed_resource )
		m_compressed_resource->late_set_fat_it	(has_flag(flag_uses_physical_path) ? vfs::vfs_iterator::end() : m_fat_it);

	set_creation_source_for_resource		(m_compressed_resource);

	return									allocation_result_success;
}

allocation_result_enum   query_result::allocate_final_managed_resource_if_needed ()
{
	managed_cook * const cook			=	cook_base::find_managed_cook(m_class_id);
	R_ASSERT								(cook);

	const_buffer	raw_data			=	need_create_resource_if_no_file() ? const_buffer(0, 0) : pin_raw_buffer();
	u32 const managed_size				=	cook->calculate_resource_size(raw_data, !need_create_resource_if_no_file());
	
	if ( !need_create_resource_if_no_file() )
		unpin_raw_buffer					(raw_data);

	if ( !managed_size )
		return								allocation_result_success;

	m_managed_resource					=	g_resources_manager->allocate_managed_resource(managed_size, m_class_id);

	if ( !m_managed_resource )
	{
		set_out_of_memory					(managed_memory, managed_size);
		m_out_of_memory_type			=	out_of_memory_on_allocate_final_resource;
		set_error_type						(error_type_out_of_memory);

		if ( out_of_memory_callback callback = get_out_of_memory_callback() )
		{
			callback						(this);
			return							allocation_result_reallocate_scheduled;
		}

		return								allocation_result_failed;
	}

	m_managed_resource->late_set_fat_it		(has_flag(flag_uses_physical_path) ? vfs::vfs_iterator::end() : m_fat_it);
	set_creation_source_for_resource		(m_managed_resource);
	return									allocation_result_success;
}

allocation_result_enum   query_result::allocate_raw_managed_resource_if_needed ()
{
	R_ASSERT								(!m_raw_managed_resource);
	R_ASSERT								(!cook_base::find_inplace_unmanaged_cook(m_class_id));

	u32 raw_managed_size				=	0;	
	if ( cook_base::find_managed_cook(m_class_id) || 
		 cook_base::find_unmanaged_cook(m_class_id) || 
		 !cook_base::find_cook(m_class_id) )
	{
		if ( m_fat_it.is_end() )
			return							allocation_result_failed;
		raw_managed_size				=	m_fat_it.get_file_size();
	}
	else if ( inplace_managed_cook * const cook = cook_base::find_inplace_managed_cook(m_class_id) )
	{
		raw_managed_size				=	cook->calculate_resource_size(need_create_resource_if_no_file() ? 0 : get_raw_file_size(), 
																		  m_offset_to_file, 
																		  !need_create_resource_if_no_file());
	}
	else
		NOT_IMPLEMENTED();

	m_raw_managed_resource				=	g_resources_manager->allocate_managed_resource(raw_managed_size, raw_data_class); 
	if ( !m_raw_managed_resource )
	{
		set_out_of_memory					(managed_memory, raw_managed_size);
		m_out_of_memory_type			=	out_of_memory_on_allocate_raw_resource;
		set_error_type						(error_type_out_of_memory);

		if ( out_of_memory_callback callback = get_out_of_memory_callback() )
		{
			callback						(this);
			return							allocation_result_reallocate_scheduled;
		}
		else
			return							allocation_result_failed;
	}

	if ( m_raw_managed_resource )
		m_raw_managed_resource->late_set_fat_it	(has_flag(flag_uses_physical_path) ? vfs::vfs_iterator::end() : m_fat_it);

	set_creation_source_for_resource		(m_raw_managed_resource);

	LOGI_TRACE								("resources", "allocated raw %s", 
											 resources::log_string(m_raw_managed_resource.c_ptr()).c_str());

	return									allocation_result_success;
}

allocation_result_enum   query_result::allocate_final_unmanaged_resource_if_needed ()
{
	unmanaged_cook * const cook			=	cook_base::find_unmanaged_cook(m_class_id);
	if ( !cook )
		return								allocation_result_failed;

	const_buffer const raw_data			=	need_create_resource_if_no_file() ? const_buffer(0, 0) : pin_raw_buffer();

	unset_flag								(flag_zero_allocation_was_set);
	m_unmanaged_buffer					=	cook->allocate_resource(* this, raw_data, !need_create_resource_if_no_file());
	if ( !m_unmanaged_buffer && !has_flag(flag_zero_allocation_was_set) )
	{
		R_ASSERT							(m_out_of_memory.type && m_out_of_memory.size,
											"in case when allocation fails, cook::allocate_resource must call query_result_for_cook::set_out_of_memory or query_result_for_cook::set_zero_allocation");
		m_out_of_memory_type			=	out_of_memory_on_allocate_final_resource;
		set_error_type						(error_type_out_of_memory);

		if ( out_of_memory_callback callback = get_out_of_memory_callback() )
		{
			callback						(this);
			return							allocation_result_reallocate_scheduled;
		}

		return								allocation_result_failed;
	}

	if ( !need_create_resource_if_no_file() )
		unpin_raw_buffer					(raw_data);	

	return									allocation_result_success;
}

bool   query_result::need_create_resource_inplace_in_creation_or_inline_data ()
{
	if ( !has_uncompressed_inline_data() && !creation_data_from_user() )
		return								false;

	inplace_unmanaged_cook * const cook	=	cook_base::find_inplace_unmanaged_cook(m_class_id);
	if ( !cook )
		return								false;

	if ( has_uncompressed_inline_data() )
		return								!!cook->get_create_resource_inplace_in_inline_fat_delegate();

	return									!!cook->get_create_resource_inplace_in_creation_data_delegate();
}

void   query_result::bind_unmanaged_resource_buffer_to_creation_or_inline_data ()
{
	if ( has_uncompressed_inline_data() )
	{
		const_buffer						inline_data;
		m_fat_it.get_inline_data			(& inline_data);
		m_raw_unmanaged_buffer			=	cast_away_const(inline_data);
	}
	else if ( creation_data_from_user() )
	{
		m_raw_unmanaged_buffer			=	cast_away_const(m_creation_data_from_user);	
	}
	else
		NOT_IMPLEMENTED();
}

allocation_result_enum   query_result::allocate_raw_unmanaged_resource_if_needed ()
{
	R_ASSERT								(!m_raw_unmanaged_buffer);

	inplace_unmanaged_cook * const cook	=	cook_base::find_inplace_unmanaged_cook(m_class_id);
	if ( !cook )
		return								allocation_result_failed;

	if ( allocate_thread_id() != threading::current_thread_id() )
		return								allocation_result_failed;

	if ( need_create_resource_inplace_in_creation_or_inline_data() )
		return								allocation_result_failed;

	unset_flag								(flag_zero_allocation_was_set);
	if ( need_create_resource_if_no_file() )
	{
		m_raw_unmanaged_buffer			=	cook->allocate_resource(*this, 0, m_offset_to_file, false);	
	}
	else
	{
		u32 const raw_file_size			=	get_raw_file_size();
		m_offset_to_file				=	0;
		m_raw_unmanaged_buffer			=	cook->allocate_resource(*this, raw_file_size, m_offset_to_file, true);
	
		R_ASSERT							(m_raw_unmanaged_buffer);
	}

	if ( !m_raw_unmanaged_buffer && !has_flag(flag_zero_allocation_was_set) )
	{
		R_ASSERT							(m_out_of_memory.type && m_out_of_memory.size,
											"in case when allocation fails, cook::allocate_resource must call query_result_for_cook::set_out_of_memory or query_result_for_cook::set_zero_allocation");
		m_out_of_memory_type			=	out_of_memory_on_allocate_raw_resource;
		set_error_type						(error_type_out_of_memory);

		if ( out_of_memory_callback callback = get_out_of_memory_callback() )
		{
			callback						(this);
			return							allocation_result_reallocate_scheduled;
		}
		else
			return							allocation_result_failed;
	}

	R_ASSERT								(m_offset_to_file || !m_raw_unmanaged_buffer,
											 "cook::allocate_raw_unmanaged should set offs_to_raw_file to a non-zero value");
	return									allocation_result_success;
}

struct helper_unmanaged_resource : public unmanaged_resource 
{
public:
};

void   query_result::free_unmanaged_buffer ()
{
	R_ASSERT								(!m_raw_unmanaged_buffer || !m_unmanaged_buffer);
	mutable_buffer const unmanaged_buffer	=	m_raw_unmanaged_buffer ? m_raw_unmanaged_buffer : m_unmanaged_buffer;
	if ( !unmanaged_buffer )
		return;

	R_ASSERT								(unmanaged_buffer.size() >= sizeof(unmanaged_resource));
	unmanaged_resource_ptr	ptr			=	new ( unmanaged_buffer.c_ptr() ) helper_unmanaged_resource();

	set_deleter_object						(ptr.c_ptr());
	ptr->late_set_fat_it					(get_fat_it_zero_if_physical_path_it());
	ptr->set_creation_source				(resource_base::creation_source_deallocate_buffer_helper, 
											 m_request_path, 
											 resources::memory_usage_type(nocache_memory, unmanaged_buffer.size()));

	m_raw_unmanaged_buffer.reset			();
	m_unmanaged_buffer.reset				();
}

void   query_result::set_deleter_object (unmanaged_resource * resource)
{
	cook_base *	const cook				=	cook_base::find_cook(m_class_id);
	u32 deallocate_thread_id			=	cook->allocate_thread_id();
	
	if ( deallocate_thread_id == cook_base::use_any_thread_id )
		deallocate_thread_id			=	m_user_thread_id;
	
	resource->set_deleter_object			(cook, deallocate_thread_id);
}

void   query_result::prepare_final_resource ()
{
	if ( cook_base::find_managed_cook(m_class_id) || cook_base::find_unmanaged_cook(m_class_id)  )
	{
		g_resources_manager->allocate()->prepare_final_resource (this);
		return;
	}

	on_allocated_final_resource			();
}

void   query_result::on_allocated_final_resource ()
{
	send_to_create_resource				();		
}

resource_base::creation_source_enum   query_result::creation_source_for_resource ()
{
	resource_base::creation_source_enum	  creation_source	=	creation_source_unset;
	if ( creation_data_from_user() )
	{
		creation_source					=	resource_base::creation_source_user_data;
	}
	else if ( cook_base::find_translate_query_cook(m_class_id) )
	{
		creation_source					=	creation_source_translate_query;
	}
	else
	{
		R_ASSERT							(has_flag(flag_processed_request_path) || has_flag(flag_translated_query));
		if ( has_flag(flag_uses_physical_path) )
			creation_source				=	resource_base::creation_source_physical_path;
		else
			creation_source				=	resource_base::creation_source_fat;
	}
	ASSERT									(creation_source != creation_source_unset);

	return									creation_source;
}

void   query_result::set_creation_source_for_resource (managed_resource_ptr resource)
{
	resource->set_creation_source			(creation_source_for_resource(), m_request_path);
}

void   query_result::set_creation_source_for_resource (unmanaged_resource_ptr resource)
{
	resource->set_creation_source			(creation_source_for_resource(), m_request_path, resource->memory_usage());
}

void   query_result_for_cook::save_generated_resource (save_generated_data * data)
{
	R_ASSERT								(!m_save_generated_data, "Calling save_generated_resource twice??");
	m_save_generated_data				=	data;
}

threading::thread_id_type   query_result::allocate_thread_id () const
{
	cook_base * const cook				=	cook_base::find_cook(m_class_id);
	R_ASSERT								(cook);

	u32 allocate_thread_id				=	cook->allocate_thread_id();
	if ( allocate_thread_id == cook_base::use_any_thread_id )
		allocate_thread_id				=	m_user_thread_id;

	return									allocate_thread_id;
}

threading::thread_id_type   query_result::translate_thread_id () const
{
	cook_base * const cook				=	cook_base::find_cook(m_class_id);
	R_ASSERT								(cook);

	u32 translate_thread_id				=	cook->translate_thread_id();
	if ( translate_thread_id == cook_base::use_any_thread_id )
		translate_thread_id				=	m_user_thread_id;

	return									translate_thread_id;
}

bool   query_result::retry_action_that_caused_out_of_memory	()
{
	set_error_type							(error_type_unset, false);
	++m_out_of_memory_reallocations_count;
	m_out_of_memory						=	memory_usage_type();
	out_of_memory_type_enum const out_of_memory_type	=	m_out_of_memory_type;
	m_out_of_memory_type				=	out_of_memory_type_unset;

	bool result							=	true;

	threading::interlocked_increment		(m_on_created_resource_guard);

	if ( out_of_memory_type == out_of_memory_on_translate_query )
	{
		unset_flag							(flag_translated_query);
		translate_query_if_needed			();
	}
	else if ( out_of_memory_type == out_of_memory_on_create_resource )
	{
		do_create_resource					(& result);
	}	
	else if ( out_of_memory_type == out_of_memory_on_allocate_raw_resource )
	{
		g_resources_manager->allocate()->prepare_raw_resource(this, reallocating_true);
	}
	else if ( out_of_memory_type == out_of_memory_on_allocate_final_resource )
	{
		prepare_final_resource				();
	}
	else 
		NOT_IMPLEMENTED();

	if ( out_of_memory_type != out_of_memory_on_create_resource )
	{
		result							=	(m_out_of_memory_type == out_of_memory_type_unset);
		if ( !result ) 
			R_ASSERT						(m_out_of_memory.type);
	}

	LOGI_INFO								("grm", "%s : reallocation: %s", 
											 resources::log_string(this).c_str(), 
											 result ? "success" : "failed");

	if ( result )
		unset_flag							(flag_in_grm_out_of_memory_queue);

	try_push_created_resource_to_manager_might_destroy_this	();

	return									result;
}

bool   query_result::ready_to_retry_action_that_caused_out_of_memory ()
{
	return									m_ready_to_retry_action_that_caused_out_of_memory && 
											!m_observed_resource_destructions_left;
}

} // namespace resources
} // namespace xray

