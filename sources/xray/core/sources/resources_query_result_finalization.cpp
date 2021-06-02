////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/resources_query_result.h>
#include "resources_manager.h"
#include "resources_vfs_associations.h"

static xray::command_line::key s_assert_on_cook_failure( "assert_on_cook_failure", "", "debug", "", "" );

namespace xray {
namespace resources {

void   query_result::on_refered_query_ended (query_result * refered_query)
{
	set_error_type							(refered_query->get_error_type());

	m_raw_managed_resource				=	refered_query->m_raw_managed_resource;
	m_raw_unmanaged_buffer				=	refered_query->m_raw_unmanaged_buffer;
	m_managed_resource					=	refered_query->m_managed_resource;
	m_unmanaged_resource				=	refered_query->m_unmanaged_resource;
	set_create_resource_result				(refered_query->get_create_resource_result(), refered_query->get_error_type());
	set_flag								(flag_refers_to_raw_file);
	clear_reference							();

	if ( refered_query->get_error_type() != error_type_unset )
	{
		end_query_might_destroy_this	();
		return;
	}
	
	if ( cook_base::reuse_type(m_class_id) == cook_base::reuse_raw )
	{
		R_ASSERT							(cook_base::does_create_resource(m_class_id));
		m_unmanaged_resource			=	NULL;
		m_managed_resource				=	NULL;
		
		if ( cook_base::cooks_inplace(m_class_id) )
		{ 
			m_raw_managed_resource		=	NULL; // this are not real raw resources
			m_raw_unmanaged_buffer		=	mutable_buffer(NULL, 0); // this are not real raw resources
		}

		if ( !m_raw_managed_resource && !m_raw_unmanaged_buffer )
		{
			finish_query					(cook_base::result_requery);
			return;
		}

		R_ASSERT							(m_raw_unmanaged_buffer || m_raw_managed_resource);
		set_flag							(flag_reused_raw_resource);
		prepare_final_resource				();
		return;
	}
	else
		set_flag							(flag_reused_resource);

	end_query_might_destroy_this			();
}

bool   query_result::try_push_created_resource_to_manager_might_destroy_this ()
{
	if ( threading::interlocked_decrement(m_on_created_resource_guard) == 0 )
	{
		g_resources_manager->on_created_resource	(this);
		return								true;
	}

	return									false;
}

void   query_result::end_query_might_destroy_this_impl ()
{	
	if ( m_out_of_memory_type != out_of_memory_type_unset || get_out_of_memory_sub_queries() )
	{
		query_result_for_cook * const parent	=	get_parent_query();
		if ( parent )
		{
			parent->on_sub_query_out_of_memory	();
		}
		else
		{
			// top query out of memory?
			u32 const max_retries_on_out_of_memory	=	5;
			if ( m_out_of_memory_retries_count < max_retries_on_out_of_memory )
			{
				++m_out_of_memory_retries_count;
				requery_on_out_of_memory	();
				return;
			}
			else
				LOGI_INFO					("grm", "failed out of memory query: '%s'", log_string().c_str());
		}
	}

	if ( m_out_of_memory_retries_count )
		LOGI_INFO							("grm", "successfully requeried out of memory query: '%s'", log_string().c_str());

	R_ASSERT								(!has_flag(flag_finished));
	if ( is_successful() && has_flag(flag_reused_resource) )
	{
		LOGI_INFO							("resources",	"reused resource: %s", 
											 resources::log_string(m_managed_resource ? 
											   (resource_base *)m_managed_resource.c_ptr() : m_unmanaged_resource.c_ptr()).c_str());
	}

	g_resources_manager->debug()->unregister_pending_query	(this);	

	if  ( is_load_type() )
		g_resources_manager->decrement_uncooked_queries_count();

	unset_flag								(flag_in_pending_list);

	if ( !m_fat_it.is_end() && get_associated_query_result(m_fat_it) )
		set_associated						(m_fat_it, NULL);

	unlock_fat_it							();

	if ( has_flag(flag_in_generating_because_no_file_queue) )
		g_resources_manager->remove_from_generate_if_no_file_queue	(* this);

	set_flag								(flag_finished);

	if ( has_flag(flag_name_registry_entry_in_queue) )
	{
		resources_manager::name_registry_type & name_registry	=	g_resources_manager->name_registry();
		threading::mutex_raii	raii		(g_resources_manager->name_registry_mutex());
		resources_manager::name_registry_type::iterator it		=	name_registry.find(& m_name_registry_entry);
		
		R_ASSERT							(it != name_registry.end());
		name_registry_entry * const entry	=	* it;
		R_ASSERT_U							(entry == & m_name_registry_entry);
		R_ASSERT							(entry->associated == this);
		name_registry.erase					(it);

		unset_flag							(flag_name_registry_entry_in_queue);
	}

	if ( m_parent )
		m_parent->on_child_query_end		(this, (get_error_type() == error_type_unset) && 
												   (m_create_resource_result != cook_base::result_error));
}

void   query_result::on_save_operation_end ()
{
	if ( is_save_type() )
	{
		R_ASSERT									(m_data_to_save_generator);
		m_data_to_save_generator->set_error_type	(get_error_type(), false);
		m_data_to_save_generator->late_set_fat_it	(get_fat_it());
		m_data_to_save_generator->finish_query		(m_data_to_save_generator->get_create_resource_result());
	}

	end_query_might_destroy_this			();
	query_result *	this_ptr			=	this;
	RES_DELETE								(this_ptr);
}

void   query_result::on_load_operation_end (query_result * const	refered)
{
	XRAY_UNREFERENCED_PARAMETER				(refered);
	ASSERT									(has_flag(flag_is_referer) == (refered != NULL));

	if ( get_error_type() != error_type_unset )
	{
		if ( cook_base::does_create_resource_if_no_file(m_class_id) )
		{
			prepare_final_resource			();
			return;
		}

		end_query_might_destroy_this		();
		return;
	}

	if ( !check_file_crc() )
	{
		set_error_type						(error_type_hash_not_equal_to_db_hash);
		end_query_might_destroy_this		();
		return;
	}

	if ( !m_fat_it.is_replicated() )
	{
		fs_new::native_path_string const path	=	m_fat_it.get_physical_path();
		if ( resources_manager::need_replication(path.c_str()) )
			replicate						();
	}

	if ( m_fat_it.is_compressed() )
	{
		g_resources_manager->add_resource_to_decompress	(this);
		return;
	}

	on_decompressing_end					();
}

void   query_result::set_is_unmovable_if_needed (managed_resource * resource, bool is_unmovable)
{
	if ( !resource )
		return;
	
	if ( resource->is_unmovable() == is_unmovable )
		return;

	resource->set_is_unmovable				(is_unmovable);
}
  
void   query_result::on_file_operation_end (query_result * const refered)
{
	set_is_unmovable_if_needed				(m_raw_managed_resource.c_ptr(), false);
	set_is_unmovable_if_needed				(m_managed_resource.c_ptr(), false);
	set_is_unmovable_if_needed				(m_compressed_resource.c_ptr(), false);

	if ( is_load_type() )
	{
		on_load_operation_end				(refered);
	}
	else
	{
		R_ASSERT							(!refered);
		on_save_operation_end				();
	}
}

void   query_result::on_decompressing_end ()
{
	if ( get_error_type() != error_type_unset )
	{
		end_query_might_destroy_this		();
		return;
	}

	prepare_final_resource					();
}

void   query_result_for_cook::finish_query (error_type_enum error_code, assert_on_fail_bool assert_on_cook_failure)
{
	finish_query_impl					(
		(error_code == error_type_unset) ? cook_base::result_success : cook_base::result_error, 
		assert_on_cook_failure, 
		error_code
	);
}

void   query_result_for_cook::finish_query (cook_base::result_enum result, assert_on_fail_bool const assert_on_cook_failure)
{
	finish_query_impl					(
		result, 
		assert_on_cook_failure, 
		result == cook_base::result_error ? error_type_cook_failed : error_type_unset
	);
}

void   query_result_for_cook::finish_query_impl (cook_base::result_enum		result, 
												 assert_on_fail_bool const	assert_on_cook_failure,
												 error_type_enum			error_code)
{
	if ( result == cook_base::result_error )
	{
		if ( m_parent->assert_on_fail() && assert_on_cook_failure )
			LOGI_WARNING					("resources", "cook of %s failed", log_string().c_str());

		if ( assert_on_cook_failure && s_assert_on_cook_failure.is_set() )
			DEBUG_BREAK						();
	}

	query_result * const this_ptr		=	static_cast_checked<query_result *>(this);

	if ( result == cook_base::result_out_of_memory )
	{
		R_ASSERT							(m_out_of_memory.type && m_out_of_memory.size, 
			"if you should call set_out_of_memory before finish_query(result_out_of_memory)");

		this_ptr->m_out_of_memory_type	=	this_ptr->is_translate_query() ? 
											out_of_memory_on_translate_query : out_of_memory_on_create_resource;

		set_error_type						( error_type_out_of_memory );

		if ( out_of_memory_callback callback = get_out_of_memory_callback() )
		{
			this_ptr->set_create_resource_result	(result, error_code);
			callback						(this_ptr);
			return;
		}
		else
			result									=	cook_base::result_error;
	}

	this_ptr->set_create_resource_result	(result, error_code);

	if ( this_ptr->is_translate_query() )
		this_ptr->finish_translated_query	(result);
	else
		this_ptr->finish_normal_query		(result);
}

query_result_for_user::~query_result_for_user ()
{
	if ( m_requery_path )
		MT_FREE								(m_requery_path);
}

query_result::~query_result	()
{
	if ( is_fs_iterator_query() && m_result_iterator )
	{
		g_resources_manager->debug()->unregister_vfs_lock_query	(this);
	}

	R_ASSERT								(!has_flag(flag_in_pending_list));
	clear_reference							();

#ifndef MASTER_GOLD
	if ( m_save_generated_data )
	{
		m_save_generated_data->delete_this	();
		m_save_generated_data			=	NULL;
	}
#endif

	if ( has_flag(flag_uses_physical_path) )
	{
		vfs::destroy_temp_physical_node		(m_fat_it.get_node());

		m_fat_it						=	vfs::vfs_iterator::end();
		unset_flag							(flag_uses_physical_path);
	}

	if ( has_flag(flag_should_free_request_path) )
	{
		ASSERT								(m_user_allocator);
		XRAY_FREE_IMPL						(* m_user_allocator, m_request_path);
		unset_flag							(flag_should_free_request_path);
	}
}

void   query_result::clear_reference ()
{
	if ( !has_flag(flag_is_referer) )
	{
		R_ASSERT						(m_next_referer == this);
		return;
	}

	// last links to first, which has no flag_is_referer flag
	query_result* 	query			=	m_next_referer;
	while ( !query->has_flag(flag_is_referer) )
		query						=	query->m_next_referer;

	while ( query->m_next_referer != this )
		query						=	query->m_next_referer;

	query->m_next_referer			=	m_next_referer;
	R_ASSERT							(has_flag(flag_was_referer));
	unset_flag							(flag_is_referer);
	m_next_referer					=	this;
}

} // namespace resources
} // namespace xray
