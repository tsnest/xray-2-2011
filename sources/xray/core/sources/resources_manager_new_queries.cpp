////////////////////////////////////////////////////////////////////////////
//	Created		: 14.06.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resources_manager.h"
#include "resources_vfs_associations.h"

namespace xray {
namespace resources {

void   resources_manager::add_to_generate_if_no_file_queue (query_result & query)
{
	m_generate_if_no_file_queue.push_back	(& query);
	R_ASSERT								(!query.has_flag(query_result::flag_is_referer));
	R_ASSERT								(!query.has_flag(query_result::flag_in_generating_because_no_file_queue));
	query.set_flag							(query_result::flag_in_generating_because_no_file_queue);
}

void   resources_manager::remove_from_generate_if_no_file_queue	(query_result & query)
{
	R_ASSERT								(!query.has_flag(query_result::flag_is_referer));
	R_ASSERT								(query.has_flag(query_result::flag_in_generating_because_no_file_queue));
	query.unset_flag						(query_result::flag_in_generating_because_no_file_queue);
	m_generate_if_no_file_queue.erase		(& query);
}

static command_line::key    s_skip_file_not_found	("skip_file_not_found", "", "fs", "doesn't debug break if file is not found");

void   resources_manager::init_query_with_no_fat_it (query_result & query)
{
#ifdef DEBUG
	vfs::vfs_iterator const fat_it		=	query.get_fat_it();
	ASSERT									(fat_it.is_end());
#endif // #ifdef DEBUG
	 
	if ( cook_base::does_create_resource_if_no_file(query.get_class_id()) && 
		 query.get_create_resource_result() != cook_base::result_requery )
	{
		generate_if_no_file_queue::mutex_raii	raii	(m_generate_if_no_file_queue.policy());
		if ( cook_base::reuse_type(query.get_class_id()) != cook_base::reuse_false )
		{
			query_result * active_query	=	m_generate_if_no_file_queue.front();
			while ( active_query )
			{
				if ( strings::equal(query.get_requested_path(), active_query->get_requested_path()) )
				{
					LOGI_INFO				("resources:manager", "no fat_it query %s added as refering to [quid %d]", 
											 resources::log_string(& query).c_str(),
											 active_query->uid());
					active_query->add_referrer	(& query, false);
					return;
				}
				active_query			=	m_generate_if_no_file_queue.get_next_of_object(active_query);
			}
		}

		LOGI_INFO							("resources:manager", "no fat_it query '%s' added for generate_if_no_file cook", resources::log_string(& query).c_str());
		query.set_error_type				(query_result::error_type_file_not_found);
		add_to_generate_if_no_file_queue	(query);
		allocate()->prepare_raw_resource	(& query, reallocating_false);
		return;
	}

	bool const assert_on_not_found		=	query.get_owner_queries() && 
											query.get_owner_queries()->assert_on_fail();
	if ( assert_on_not_found )
	{
		LOGI_WARNING						("resources", "cannot find file: '%s' (class id: %d)", query.get_requested_path(), query.get_class_id());
		//if ( debug::is_debugger_present() && !s_skip_file_not_found )
		//	DEBUG_BREAK						();
	}

	query.set_error_type					(query_result::error_type_file_not_found);
	query.end_query_might_destroy_this	();
}

void   on_hot_mounted					(vfs::mount_result result)
{
	// or maybe no need in this assert?
	R_ASSERT								(result.result == vfs::result_success);
}

void   resources_manager::init_new_query (query_result & query)
{
	if ( query.is_translate_query() )
	{
		if ( query.get_create_resource_result() == cook_base::result_requery )
		{
			fs_new::virtual_path_string	const virtual_path	=	query.get_requested_path();
			vfs::query_hot_mount_and_wait	(m_vfs, virtual_path, NULL, helper_allocator());
		}

		push_to_translate_query				(& query);
		return;
	}

	bool const is_query_create			=	query.creation_data_from_user();

	query_result::consider_with_name_registry_result_enum const result	=	
			query.consider_with_name_registry(is_query_create ? 
											  query_result::only_try_to_get_associated_resource_false :
											  query_result::only_try_to_get_associated_resource_true);

	if ( result == query_result::consider_with_name_registry_result_error || 
		 result == query_result::consider_with_name_registry_result_got_associated_resource )
	{
		query.end_query_might_destroy_this	();
		return;
	}
	else if ( result == query_result::consider_with_name_registry_result_added_as_referer )
		return;

	if ( query.creation_data_from_user() )
	{
		allocate()->prepare_raw_resource	(& query, reallocating_false);
		return;
	}

	query.process_request_path				(false);
}

void   resources_manager::continue_init_new_query (query_result & query)
{
	vfs::vfs_iterator const fat_it		=	query.get_fat_it();
	if ( fat_it.is_end() )
	{
		init_query_with_no_fat_it			(query);
		return;
	}

	fs_new::native_path_string const file_path	=	fat_it.get_physical_path();

	cook_base::reuse_enum const reuse_type	=	cook_base::reuse_type(query.get_class_id());

	const_buffer							inline_data;
	if ( unmanaged_resource_ptr const cached_unmanaged = get_associated_unmanaged_resource_ptr(fat_it) )
	{
		R_ASSERT							(reuse_type == cook_base::reuse_true);

		query.set_unmanaged_resource		(cached_unmanaged, cached_unmanaged->memory_usage());
		query.set_flag						(query_result::flag_reused_resource);
		query.end_query_might_destroy_this	();
	}
	else if ( managed_resource_ptr const cached_resource = get_associated_managed_resource_ptr(fat_it) )
	{
		if ( reuse_type == cook_base::reuse_raw )
			R_ASSERT						(cached_resource->needs_cook());
		else if ( reuse_type == cook_base::reuse_true )
			R_ASSERT						(!cached_resource->needs_cook() && cook_base::cooks_managed_resource(query.get_class_id()));
		else
			NOT_IMPLEMENTED();

		if ( cached_resource->needs_cook() )
		{
			query.m_raw_managed_resource	=	cached_resource;
			query.set_flag					(query_result::flag_reused_raw_resource);
			R_ASSERT						(cached_resource->get_size());
			query.on_file_operation_end		();
		}
		else
		{
			query.set_managed_resource		(cached_resource);
			query.set_flag					(query_result::flag_reused_resource);
			query.end_query_might_destroy_this	();
		}
	}
	else if ( query_result * const active_query = get_associated_query_result(fat_it) )
	{
		active_query->add_referrer			(& query);
	}
	else
	{
		R_ASSERT 							(!is_associated(fat_it) || !query.is_load_type());
		
		if ( query.is_load_type() && !fat_it.is_end() )
		{
			if ( cook_base::reuse_type(query.get_class_id()) != cook_base::reuse_false )
				set_associated				(fat_it, & query);
		}

		allocate()->prepare_raw_resource	(& query, reallocating_false);
	}
}

void   resources_manager::init_new_queries (query_result* const queries_with_unlocked_fat_it)
{
	query_result *	new_query			=	queries_with_unlocked_fat_it;

	while ( new_query )
	{
		query_result *	const next_query	=	new_query->m_next_to_init;
		
		class_id_enum const class_id		=	new_query->get_class_id();

		if ( class_id != raw_data_class && 
			 class_id != raw_data_class_no_reuse && 
			 !find_cook(class_id) )
		{
			if ( !cooks_are_registering() )
			{
				new_query->set_error_type	(query_result::error_type_cook_not_registered);
				new_query->end_query_might_destroy_this	();
			}
			else
			{
				m_new_queries_waiting_for_cook_register.push_back	(new_query);
			}						
		}
		else
		{
			init_new_query				(*new_query);
		}

		new_query						=	next_query;
	}
}

void   add_autoselect_quality_query (query_result * query);

void   resources_manager::init_new_autoselect_quality_query	(query_result * query)
{
	add_autoselect_quality_query			(query);
}

void   resources_manager::init_new_autoselect_quality_queries ()
{
	for ( query_result *	it_query	=	m_new_autoselect_quality_queries.pop_all_and_clear();
							it_query; )
	{
		query_result * const it_next	=	m_new_autoselect_quality_queries.get_next_of_object(it_query);
		init_new_autoselect_quality_query	(it_query);
		it_query						=	it_next;
	}
}

void   resources_manager::push_new_query (query_result * query)
{
	if ( query->is_autoselect_quality_query() && !query->has_flag(query_result::flag_selected_quality_level) )
	{
		if ( threading::current_thread_id() == (u32)m_resources_thread_id )
			init_new_autoselect_quality_query				(query);
		else
		{
			m_new_autoselect_quality_queries.push_back		(query);
			wakeup_resources_thread							();
		}
	}
	else if (	query->is_helper_query_for_mount() || query->get_parent_query() != NULL )
	{
		m_new_inorder_queries.push_back					(query);
	}
	else
	{
		if ( query->has_flag(query_result::flag_locked_fat_iterator) )
			m_new_queries_with_locked_fat_it.push_back		(query);
		else
			m_new_queries_with_unlocked_fat_it.push_back	(query);
	}
}

} // namespace resources
} // namespace xray
