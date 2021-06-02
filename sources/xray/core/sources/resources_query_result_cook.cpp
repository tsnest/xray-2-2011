////////////////////////////////////////////////////////////////////////////
//	Created		: 28.12.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/resources_query_result.h>
#include "resources_manager.h"
#include "game_resman_test_utils.h"

namespace xray {
namespace resources {

void   query_result::send_to_create_resource ()
{
	bool const is_inline_uncompressed	=	!m_fat_it.is_end() && m_fat_it.data_node()->is_inlined() && !m_fat_it.is_compressed();
	R_ASSERT_U							((is_successful() && (m_raw_managed_resource || m_raw_unmanaged_buffer || is_inline_uncompressed)) || 
										 cook_base::does_create_resource_if_no_file(m_class_id) || 
										 creation_data_from_user());

	if ( cook_base::does_create_resource(m_class_id) )
	{
		g_resources_manager->add_resource_to_create	(this);
		return;
	}

	// NO PROCESSING WAY

	m_managed_resource				=	m_raw_managed_resource;
	LOGI_INFO("resources",				"created %s", 
										 resources::log_string(m_managed_resource.c_ptr()).c_str(),
										 m_managed_resource.c_ptr(), 
										 uid());

	set_create_resource_result			(cook_base::result_success, query_result_for_user::error_type_unset);
	m_final_resource_size			=	m_managed_resource->get_size();
	on_create_resource_end				();
}

void   query_result::set_create_resource_result (cook_base::result_enum const	result,
												 error_type_enum const			error_type)
{
	if ( result == cook_base::result_postponed )
	{
		if ( !has_flag(flag_finished_create_resource) )
			m_create_resource_result			=	result;
		else
			R_ASSERT								(m_create_resource_result != cook_base::result_undefined);
	}
	else
	{
		R_ASSERT									(!has_flag(flag_finished_create_resource) ||
													  has_flag(flag_is_referer),
													 "finish_query was already called!");

		m_error_type							=	error_type;
		m_create_resource_result				=	result;
	}
}

void   query_result::do_managed_create_resource (managed_cook * cook)
{
	set_is_unmovable_if_needed						(m_managed_resource.c_ptr(), true);

	if ( need_create_resource_if_no_file() )
	{
		managed_cook::create_resource_if_no_file_delegate_type delegate	=	cook->get_create_resource_if_no_file_delegate();
		R_ASSERT									(delegate);

		delegate									(* this, m_managed_resource);
	}
	else
	{
		R_ASSERT									(!m_offset_to_file);
		const_buffer const raw_data				=	pin_raw_file();
		cook->create_resource						(* this, raw_data, m_managed_resource);
		unpin_raw_file								(raw_data);
	}

	set_is_unmovable_if_needed						(m_managed_resource.c_ptr(), false);
}

void   query_result::do_inplace_managed_create_resource (inplace_managed_cook * cook)
{
	set_is_unmovable_if_needed						(m_raw_managed_resource.c_ptr(), true);

	if ( need_create_resource_if_no_file() )
	{
		inplace_managed_cook::create_resource_if_no_file_delegate_type delegate	=	cook->get_create_resource_if_no_file_delegate();
		R_ASSERT									(delegate);
		delegate									(* this, m_raw_managed_resource, m_final_resource_size);
	}
	else
	{
		cook->create_resource						(* this, m_raw_managed_resource, get_raw_file_size(), m_final_resource_size);
	}

	m_managed_resource							=	m_raw_managed_resource;

	set_is_unmovable_if_needed						(m_raw_managed_resource.c_ptr(), false);
}

void   query_result::do_unmanaged_create_resource (unmanaged_cook * cook)
{
	const_buffer raw_data	;
	if ( need_create_resource_if_no_file() )
	{
		unmanaged_cook::create_resource_if_no_file_delegate_type delegate	=	cook->get_create_resource_if_no_file_delegate();
		R_ASSERT									(delegate);
		delegate									(* this, m_unmanaged_buffer);
	}
	else
	{
		raw_data								=	pin_raw_buffer();
		cook->create_resource						(* this, raw_data, m_unmanaged_buffer);
		unpin_raw_buffer							(raw_data);
	}

	if ( m_create_resource_result != cook_base::result_postponed && 
		 m_create_resource_result != cook_base::result_requery && 
		 m_create_resource_result != cook_base::result_out_of_memory && 
		 m_create_resource_result != cook_base::result_error && 
		 !has_flag(flag_zero_unmanaged_resource_was_set) )
	{
		CURE_ASSERT									(m_unmanaged_resource, return, "cook::create_resource should have called query.set_unmanaged_resource");
		set_deleter_object_if_needed				();
	}
	else
		R_ASSERT									(!m_unmanaged_resource, "m_create_resource_result = %d raw_data.size = %d req path = %s", m_create_resource_result, raw_data.size(), get_requested_path());
}

void   query_result::do_inplace_unmanaged_create_resource (inplace_unmanaged_cook * cook)
{
	bool const do_inplace_create_resource	=	need_create_resource_inplace_in_creation_or_inline_data();

	if ( do_inplace_create_resource )
	{
		inplace_unmanaged_cook::create_resource_inplace_delegate_type delegate	=	has_uncompressed_inline_data() ?
			cook->get_create_resource_inplace_in_inline_fat_delegate() : cook->get_create_resource_inplace_in_creation_data_delegate();

		R_ASSERT								(delegate);
		delegate								(* this, m_raw_unmanaged_buffer);

		R_ASSERT								(cook->reuse_type() == cook_base::reuse_true || has_flag(flag_zero_unmanaged_resource_was_set));
	}
	else if ( need_create_resource_if_no_file() )
	{
		inplace_unmanaged_cook::create_resource_if_no_file_delegate_type delegate	=	cook->get_create_resource_if_no_file_delegate();
		R_ASSERT								(delegate);
		delegate								(* this, m_raw_unmanaged_buffer);
	}
	else
	{
		cook->create_resource					(* this, m_raw_unmanaged_buffer);
	}

	if ( m_create_resource_result != cook_base::result_postponed && 
		 m_create_resource_result != cook_base::result_requery && 
		!has_flag(flag_zero_unmanaged_resource_was_set) )
	{
		CURE_ASSERT						(m_unmanaged_resource, return, "cook::create_resource should have called query.set_unmanaged_resource or query.set_zero_unmanaged_resource");
		
		if ( do_inplace_create_resource && has_uncompressed_inline_data() )
			m_unmanaged_resource->set_as_inlined_in_fat	();
		else
		{
			if ( !m_unmanaged_resource->has_deleter_object() )
				set_deleter_object		(m_unmanaged_resource.c_ptr());
		}

		if ( m_unmanaged_resource->creation_source() == resource_base::creation_source_unset )
		{
			m_unmanaged_resource->late_set_fat_it		(get_fat_it_zero_if_physical_path_it());
			set_creation_source_for_resource			(m_unmanaged_resource);
		}
	}
	else
		R_ASSERT						(!m_unmanaged_resource, "m_create_resource_result = %d", m_create_resource_result);
}

void   query_result::do_create_resource_impl ()
{
	if ( m_fat_it.is_end() )
	{
		R_ASSERT						(cook_base::does_create_resource_if_no_file(m_class_id) ||
										 creation_data_from_user() ||
										 has_uncompressed_inline_data());
	}

	unset_flag									(flag_finished_create_resource);
	set_flag									(flag_in_create_resource);			

	if ( managed_cook * managed_cook = cook_base::find_managed_cook(m_class_id) )
	{
		do_managed_create_resource				(managed_cook);
	}
	else if ( inplace_managed_cook * inplace_managed_cook = cook_base::find_inplace_managed_cook(m_class_id) )
	{
		do_inplace_managed_create_resource		(inplace_managed_cook);		
	}
	else if ( unmanaged_cook * unmanaged_cook = cook_base::find_unmanaged_cook(m_class_id) )
	{
		do_unmanaged_create_resource			(unmanaged_cook);		
	}
	else if ( inplace_unmanaged_cook * inplace_unmanaged_cook = cook_base::find_inplace_unmanaged_cook(m_class_id) )
	{
		do_inplace_unmanaged_create_resource	(inplace_unmanaged_cook);
	}
	else
	{
		set_create_resource_result				(cook_base::result_success, query_result_for_user::error_type_unset);
	}

	R_ASSERT									(m_create_resource_result != cook_base::result_undefined,
												 "cooker should have called finish_query!");

	unset_flag									(flag_in_create_resource);
}

void   query_result::do_create_resource (bool * out_finished_create)
{
	increase_query_end_guard					();

	do_create_resource_impl						();
	if ( out_finished_create )
		* out_finished_create			=	get_error_type() == error_type_unset;

	if ( get_save_generated_data() )
	{
		R_ASSERT	(m_create_resource_result == cook_base::result_success,
					 "when save_generated_resource should only be called along with finish_query(result_success)");
		unset_flag	(flag_finished_create_resource); // unsetting, because we're waiting async save operation that will finish us

		u32 const ref_count				=	decrease_query_end_guard();
		R_ASSERT_U							(ref_count != 0);
		g_resources_manager->push_generated_resource_to_save	(this);
		return;
	}
	else if ( m_create_resource_result != cook_base::result_postponed &&
			  m_create_resource_result != cook_base::result_out_of_memory )
	{
		do_create_resource_end_part				();
		if ( end_query_might_destroy_this () )
			CURE_ASSERT						(false, return);

		try_push_created_resource_to_manager_might_destroy_this	();
		return;
	}

	end_query_might_destroy_this			();
}

u32   query_result::decrease_query_end_guard ()
{
	return									threading::interlocked_decrement(m_query_end_guard);
}

void   query_result::increase_query_end_guard ()
{
	threading::interlocked_increment		(m_query_end_guard);
}

bool   query_result::end_query_might_destroy_this ()
{
	R_ASSERT								(m_query_end_guard != 0);
	if ( threading::interlocked_decrement(m_query_end_guard) == 0 )
	{
		end_query_might_destroy_this_impl	();
		return								true;
	}

	return									false;
}

void   query_result::do_create_resource_end_part ()
{
	R_ASSERT								(!m_raw_managed_resource || !m_raw_managed_resource->is_unmovable());
	R_ASSERT								(!m_managed_resource || !m_managed_resource->is_unmovable());

	if ( m_create_resource_result == cook_base::result_requery )
	{
		R_ASSERT							(!m_raw_unmanaged_buffer && !m_unmanaged_buffer);
		m_raw_managed_resource			=	NULL;
	}

	if ( m_create_resource_result == cook_base::result_error )
	{
	}
	else if ( m_create_resource_result == cook_base::result_requery )
	{
		set_error_type						(error_type_unset, false);
		LOGI_INFO							("resources", "requerying resource '%s' [quid %d]", 
											 m_raw_managed_resource ? m_raw_managed_resource->log_string().c_str() : get_requested_path(),
											 uid());
	}
	else
	{
		if ( has_flag(flag_zero_unmanaged_resource_was_set) )
		{
			LOGI_INFO						("resources",	"no resource was cooked by cook, request path was: '%s' [quid %d]", 
											 get_requested_path(),
											 uid());
		}
		else
		{
			LOGI_INFO						("resources",	"%s %s [quid %d]", 
											 need_create_resource_if_no_file() ? "generated" : "cooked",
											 resources::log_string(m_managed_resource ? 
												(resource_base *)m_managed_resource.c_ptr() : m_unmanaged_resource.c_ptr()
																   ).c_str(),
											 uid());
		}
	}
}

template <class Resource>
void   query_result::propogate_sub_fats_to_resource (Resource & resource)
{
	if ( !resource )
		return;
	if ( resource->get_fat_it() != m_fat_it )
		return;

	resource->set_sub_fat_resource			(m_sub_fat.c_ptr());
}

void   query_result::propogate_sub_fats_to_resources ()
{
	propogate_sub_fats_to_resource			(m_managed_resource);
	propogate_sub_fats_to_resource			(m_raw_managed_resource);
	propogate_sub_fats_to_resource			(m_compressed_resource);
	propogate_sub_fats_to_resource			(m_unmanaged_resource);
}

void   query_result::on_create_resource_end ()
{
	if ( get_error_type() == error_type_unset && cook_base::does_create_resource(m_class_id) )
	{
		if ( m_managed_resource )
		{
			if ( cook_base::cooks_inplace(m_class_id) )
			{
				R_ASSERT					(m_final_resource_size || !m_managed_resource->get_size());
				m_managed_resource->resize_down	(m_final_resource_size);
			}
		}
		else if ( m_unmanaged_resource || has_flag(flag_zero_unmanaged_resource_was_set) )
		{

		}
		else
			R_ASSERT						(get_save_generated_data());
	}
	
	if ( get_error_type() == error_type_unset )
		associate_created_resource_with_fat_or_name_registry	();

	propogate_sub_fats_to_resources			();

	if ( has_flag(flag_in_generating_because_no_file_queue) )
		g_resources_manager->remove_from_generate_if_no_file_queue	(* this);

	R_ASSERT								(!m_compressed_resource || !m_compressed_resource->is_unmovable());
	R_ASSERT								(!m_raw_managed_resource || !m_raw_managed_resource->is_unmovable());

	if ( !has_flag(flag_is_referer) )
	{
		query_result * cur				=	m_next_referer;
		while ( cur != this )
		{
			query_result * next			=	cur->m_next_referer;
			cur->on_refered_query_ended		(this);
			cur							=	next;
		}
	}

	if ( get_error_type() != error_type_unset )
		m_raw_managed_resource			=	NULL;

	end_query_might_destroy_this			();
}

void   query_result::finish_normal_query (cook_base::result_enum const create_resource_result)
{
	// setting flag_finished_create_resource must be after set_create_resource_result
	if ( create_resource_result != cook_base::result_postponed )
		set_flag							(flag_finished_create_resource);

	set_deleter_object_if_needed			();

	if ( has_flag(flag_in_create_resource) )
		return;
	
	if ( get_save_generated_data() )
	{
		R_ASSERT	(m_create_resource_result == cook_base::result_success,
			"when save_generated_resource should only be called along with finish_query(result_success)");
		unset_flag	(flag_finished_create_resource); // unsetting, because we're waiting async save operation that will finish us

		g_resources_manager->push_generated_resource_to_save	(this);
		return;
	}

	do_create_resource_end_part				();
	try_push_created_resource_to_manager_might_destroy_this		();
}

void   query_result::set_deleter_object_if_needed (bool * out_is_new_resource)
{
	if ( out_is_new_resource )
		* out_is_new_resource			=	false;

	if ( m_unmanaged_resource )
	{
		if ( m_unmanaged_resource->creation_source() == resource_base::creation_source_unset )
		{
			if ( out_is_new_resource )
				* out_is_new_resource	=	true;
			set_deleter_object				(m_unmanaged_resource.c_ptr());
			m_unmanaged_resource->late_set_fat_it	(get_fat_it_zero_if_physical_path_it());
			set_creation_source_for_resource	(m_unmanaged_resource);
		}
		else
			R_ASSERT						(m_unmanaged_resource->has_deleter_object());
	}
	else if ( m_managed_resource )
	{
		if ( m_managed_resource->creation_source() == resource_base::creation_source_unset )
		{
			if ( out_is_new_resource )
				* out_is_new_resource	=	true;
			m_managed_resource->late_set_fat_it		(get_fat_it_zero_if_physical_path_it());
			set_creation_source_for_resource		(m_managed_resource);
		}
	}
}

void   query_result::finish_translated_query (cook_base::result_enum result)
{
	bool const is_requery_result		=	(result == cook_base::result_requery);
	R_ASSERT								(result == cook_base::result_success || 
											 result == cook_base::result_error ||
											 result == cook_base::result_out_of_memory ||
											 is_requery_result);

	if ( result == cook_base::result_success )
		R_ASSERT							(m_unmanaged_resource || m_managed_resource);

	bool is_new_resource				=	false;
	set_deleter_object_if_needed			(& is_new_resource);

	if ( get_save_generated_data() )
	{
		R_ASSERT							(m_create_resource_result == cook_base::result_success,
											"when save_generated_resource should only be called along with finish_query(result_success)");
		unset_flag							(flag_finished_create_resource); // unsetting, because we're waiting async save operation that will finish us

		g_resources_manager->push_generated_resource_to_save	(this);
		return;
	}

	if ( has_flag(flag_zero_unmanaged_resource_was_set) )
	{
		LOGI_INFO							("resources", "no resource was cooked by cook: %s", log_string().c_str());
	}
	else if ( !is_requery_result && result == cook_base::result_success )
	{
		R_ASSERT							(m_managed_resource || m_unmanaged_resource);
		if ( is_new_resource )
		{
			LOGI_INFO						("resources",	"cooked %s [quid %d] (cook id: %d)", 
											 resources::log_string(m_managed_resource ? 
																   (resource_base *)m_managed_resource.c_ptr() : m_unmanaged_resource.c_ptr()).c_str(),
											 uid(),
											 m_class_id
											);
		}
	}

	try_push_created_resource_to_manager_might_destroy_this	();
}

} // namespace resources
} // namespace xray
