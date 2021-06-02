////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/resources_query_result.h>
#include "resources_helper.h"
#include "resources_manager.h"
#include "resources_vfs_associations.h"

namespace xray {
namespace resources {

bool   query_result::check_fat_for_resource_reusage ()
{
	if ( cook_base::reuse_type(m_class_id) != cook_base::reuse_true )
		return								false;

	if ( m_fat_it.is_end() )
		return								false;

	resource_base * associated			=	NULL;

	if ( resources::unmanaged_resource_ptr const cached_unmanaged_resource = get_associated_unmanaged_resource_ptr(m_fat_it) )
	{
		m_unmanaged_resource			=	cached_unmanaged_resource;
		associated						=	cached_unmanaged_resource.c_ptr();
	}
	else if ( resources::managed_resource_ptr const cached_resource = get_associated_managed_resource_ptr(m_fat_it) )
	{
		if ( !cached_resource->needs_cook() )
		{
			m_managed_resource			=	cached_resource;
			associated					=	cached_resource.c_ptr();
		}
	}

	if ( associated )
	{
		R_ASSERT_CMP						(m_class_id, ==, associated->get_class_id(), 
											 "Omg! Associated resource '%s' is of a different class",
											 associated->log_string().c_str());

		set_flag							(flag_reused_resource);
		end_query_might_destroy_this		();
	}

	return									associated != NULL;
}

void   query_result::associate_created_resource_with_fat_or_name_registry ()
{
	R_ASSERT									( get_error_type() == error_type_unset);

	resources::resource_base * resource_base	=	NULL;
	if ( !m_fat_it.is_end() )
	{
		cook_base * const cook			=	g_resources_manager->find_cook(m_class_id);

		if ( !cook )
		{
			if ( m_class_id != raw_data_class_no_reuse )
			{
				R_ASSERT					(m_managed_resource);
				resource_base			=	m_managed_resource.c_ptr();
				set_associated				(m_fat_it, m_managed_resource.c_ptr());
			}
		}
		else if ( cook->cooks_managed_resource() && cook->reuse_type() == cook_base::reuse_true && m_managed_resource )
		{
			resource_base				=	m_managed_resource.c_ptr();
			set_associated					(m_fat_it, m_managed_resource.c_ptr());
		}
		else if ( cook->cooks_unmanaged_resource() && cook->reuse_type() == cook_base::reuse_raw && m_raw_managed_resource )
		{
			if ( has_flag(flag_reused_raw_resource) )
				resource_base			=	m_raw_managed_resource.c_ptr();

			m_raw_managed_resource->set_need_cook	();
			set_associated					(m_fat_it, m_raw_managed_resource.c_ptr());
		}
		else if ( cook->cooks_unmanaged_resource() && cook->reuse_type() == cook_base::reuse_true && m_unmanaged_resource )
		{
			resource_base				=	m_unmanaged_resource.c_ptr();
			set_associated					(m_fat_it, m_unmanaged_resource.c_ptr());
		}
	}
	else if ( (is_translate_query() || creation_data_from_user()) && 
		  cook_base::reuse_type(m_class_id) == cook_base::reuse_true &&
		  strings::length(m_request_path) > 0 )
	{
		R_ASSERT							(m_name_registry_entry.name	== m_request_path);

		name_registry_entry * new_entry	=	NULL;
		if ( get_error_type() == error_type_unset )
		{
			u32 const name_registry_entry_size	=	sizeof(name_registry_entry) + strings::length(m_request_path) + 1;
			mutable_buffer allocation				(RES_ALLOC(char, name_registry_entry_size), name_registry_entry_size);
			new_entry					=	new (allocation.c_ptr()) name_registry_entry;
			allocation					+=	sizeof(name_registry_entry);
			new_entry->name				=	(pcstr)allocation.c_ptr();
			new_entry->class_id			=	m_class_id;
			strings::copy					((pstr)allocation.c_ptr(), allocation.size(), m_request_path);
			
			if ( m_managed_resource )
			{
				new_entry->associated	=	m_managed_resource.c_ptr();
				resource_base			=	m_managed_resource.c_ptr();
				m_managed_resource->set_name_registry_entry		(new_entry);
			}
			else if ( m_unmanaged_resource )
			{
				new_entry->associated	=	m_unmanaged_resource.c_ptr();
				resource_base			=	m_unmanaged_resource.c_ptr();
				m_unmanaged_resource->set_name_registry_entry	(new_entry);
			}
			else
				NOT_IMPLEMENTED				();
		}

		resources_manager::name_registry_type & name_registry	=	g_resources_manager->name_registry();
		threading::mutex_raii				raii(g_resources_manager->name_registry_mutex());

		resources_manager::name_registry_type::iterator const it =	name_registry.find(& m_name_registry_entry);
		name_registry_entry const * const entry	=	* it;
		R_ASSERT_U							(entry == & m_name_registry_entry);
		name_registry.erase					(it);
		unset_flag							(flag_name_registry_entry_in_queue);
		if ( get_error_type() == error_type_unset )
			name_registry.insert			(new_entry);
		else
			R_ASSERT						(!new_entry);
	}

	query_result * query				=	this;
	bool disable_cache					=	false;
	while ( query )
	{
		if ( query->has_flag(flag_disable_cache) )
		{
			disable_cache				=	true;
			break;
		}

		if ( !query->m_parent )
			break;

		query_result_for_cook * const query_for_cook	=	query->m_parent->get_parent_query();
		query							=	static_cast_checked<query_result *>(query_for_cook);
	}

	cook_base * const cook				=	g_resources_manager->find_cook(m_class_id);
	if ( resource_base && 
		 (!cook || cook->cache_by_game_resources_manager()) &&
		 g_resources_manager->get_query_finished_callback() &&
		 !disable_cache &&
		 !resource_base->cast_base_of_intrusive_base()->was_pinned_for_query_finished_callback() )
	{
		m_resource_for_grm_cache		=	resource_base;
		m_resource_for_grm_cache->cast_base_of_intrusive_base()->pin_reference_count_for_query_finished_callback	();
	}
}

query_result::consider_with_name_registry_result_enum   query_result::consider_with_name_registry (only_try_to_get_associated_resource_bool  only_try_to_get_associated_resource)
{
	if ( cook_base::reuse_type(m_class_id) != cook_base::reuse_true || !strings::length(m_request_path) )
		return								consider_with_name_registry_result_no_action;

	pstr cur_path						=	m_request_path;
	u32 const temp_size					=	(strings::length(cur_path) + 1)*sizeof(char);
	pstr const temp_path				=	static_cast<pstr>( ALLOCA( temp_size ) );
	while ( cur_path )
	{
		pstr const separator_pos		=	strchr(cur_path, request::path_separator);
		if ( separator_pos )
			*separator_pos				=	NULL;

		if ( *cur_path != physical_path_char )
		{
			consider_with_name_registry_result_enum const out_result	=	
				consider_with_name_registry_impl(cur_path, only_try_to_get_associated_resource);
			if ( out_result == consider_with_name_registry_result_no_action )
			{
				;
			}
			else 
			{
				if ( out_result != consider_with_name_registry_result_error && cur_path != m_request_path )
				{
					strings::copy			( temp_path, temp_size, cur_path );
					strings::copy			( m_request_path, temp_size, temp_path );
				}

				return						out_result;
			}
		}

		if ( separator_pos )
		{
			*separator_pos				=	request::path_separator;
			cur_path					=	separator_pos + 1;
		}
		else
			break;
	}

	return									consider_with_name_registry_result_no_action;
}

query_result::consider_with_name_registry_result_enum   query_result::consider_with_name_registry_impl (pcstr name, only_try_to_get_associated_resource_bool only_try_to_get_associated_resource)
{
	bool const only_get_associated_resource	=	(only_try_to_get_associated_resource == only_try_to_get_associated_resource_true);

	resources_manager::name_registry_type & name_registry	=	g_resources_manager->name_registry();

	m_name_registry_entry.name			=	name;
	m_name_registry_entry.associated	=	this;
	m_name_registry_entry.class_id		=	get_class_id();
	
#pragma message(XRAY_TODO("consider using hashmap with per-hash interlocked synchronization if profiler shows too much waiting for name_registry_mutex"))

	threading::mutex_raii	raii			(g_resources_manager->name_registry_mutex());
	resources_manager::name_registry_type::iterator it =	name_registry.find(& m_name_registry_entry);
	name_registry_entry * const entry	=	* it;
	if ( !entry && !only_get_associated_resource )
	{
		R_ASSERT							(is_translate_query() || creation_data_from_user());
		name_registry.insert				(& m_name_registry_entry);
		set_flag							(flag_name_registry_entry_in_queue);
		return								consider_with_name_registry_result_added_self_as_host_for_referers;
	}

	if ( entry )
	{
		R_ASSERT							(entry->associated);
		query_result * active_query		=	NULL;
		if ( (active_query = entry->associated->cast_query_result()) != 0 )
		{
			//if ( only_get_associated_resource )
			//	return						consider_with_name_registry_result_no_action;

			if ( active_query == this )
				return						consider_with_name_registry_result_no_action;

			//R_ASSERT						(is_translate_query() || creation_data_from_user());

			CURE_ASSERT						(active_query->get_class_id() == m_class_id, 
											 return consider_with_name_registry_result_error,
											 "active_query associated with path '%s' has different class id: '%d', self class id: '%d'", 
											 get_requested_path(), 
											 active_query->get_class_id(), 
											 m_class_id);

			active_query->add_referrer		(this);
			return							consider_with_name_registry_result_added_as_referer;
		}
		else if ( unmanaged_resource * const unmanaged_resource = entry->associated->cast_unmanaged_resource() )
		{
			set_unmanaged_resource			(unmanaged_resource, unmanaged_resource->memory_usage());
			set_flag						(flag_reused_resource);
		}
		else if ( managed_resource * const managed_resource = entry->associated->cast_managed_resource() )
		{
			set_managed_resource			(managed_resource);
			set_flag						(flag_reused_resource);
		}
		else
		{
			set_error_type					(error_type_name_registry_error);
			NOT_IMPLEMENTED					(return consider_with_name_registry_result_error);
		}

		if ( has_flag(flag_reused_resource) )
		{
			R_ASSERT_CMP					((u32)m_class_id, ==, (u32)entry->class_id, 
											 "Omg! Associated resource '%s' is of a different class", name);
		}

		return								consider_with_name_registry_result_got_associated_resource;
	}
	
	return									consider_with_name_registry_result_no_action;
}



} // namespace resources
} // namespace xray
