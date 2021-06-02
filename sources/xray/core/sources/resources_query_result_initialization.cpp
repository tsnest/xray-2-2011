////////////////////////////////////////////////////////////////////////////
//	Created		: 24.11.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/resources_query_result.h>
#include "resources_manager.h"

namespace xray {
namespace resources {

query_result_for_user::query_result_for_user () : 
						resource_base				(resource_flag_is_query_result, raw_data_class),
						m_request_path				(NULL), 
						m_error_type				(error_type_unset), 
						m_create_resource_result	(cook_base::result_undefined), 
						m_unmanaged_resource		(NULL),
						m_requery_path				(NULL),
						m_save_generated_data		(false)

{
}

query_result_for_cook::query_result_for_cook (queries_result * parent) :
						m_request_path_max_size			(array_size(m_request_path_default_storage)),
						m_user_allocator				(NULL),
						m_user_data						(NULL),
						m_parent						(parent),
						m_transform						(NULL),
						m_out_of_memory_type			(out_of_memory_type_unset),
						m_out_of_memory_retries_count	(0),
						m_out_of_memory_reallocations_count(0),
						m_pending_tasks_count			(0),
						m_out_of_memory_sub_queries		(0)
{
	m_request_path							=	& m_request_path_default_storage[0];
	m_request_path_default_storage[0]		=	NULL;
}

query_result::query_result (u16						const flags,
							queries_result*			const parent,
							memory::base_allocator* const allocator,
							u32						const user_thread_id,
							float					const target_satisfaction,
							bool					const disable_cache,
							u32						const quality_index,
							query_type_enum			const query_type,
							autoselect_quality_bool const autoselect_quality)
	:	query_result_for_cook				(parent),
		m_flags								(flags),
		m_final_resource_size				(0),
		m_data_to_save_generator			(NULL),
		m_next_in_device_manager			(NULL),
		m_next_in_generate_if_no_file_queue	(NULL),
		m_prev_in_generate_if_no_file_queue	(NULL),
		m_next_out_of_memory				(NULL),
		m_user_thread_id					(user_thread_id),
		m_offset_to_file					(0),
		m_loaded_bytes						(0),
		m_observed_resource_destructions_left(0),
		m_ready_to_retry_action_that_caused_out_of_memory(true),
		m_on_created_resource_guard			(1),
		m_resource_for_grm_cache			(NULL),
		m_query_end_guard					(1),
		m_quality_index						(quality_index)		
{
	if ( disable_cache )
		set_flag							(flag_disable_cache);
	if ( query_type == query_type_helper_for_mount )
		set_flag							(flag_is_helper_query_for_mount);
	if ( autoselect_quality == autoselect_quality_true )
		set_flag							(flag_is_autoselect_query);

	m_user_allocator					=	allocator;
	m_next_referer						=	this; // connect intrusive links

	set_target_satisfaction					(target_satisfaction);

	g_resources_manager->debug()->register_pending_query	(this);

	set_flag								(flag_in_pending_list);
}

void   query_result_for_cook::clear_user_data ()
{
	if ( m_user_data )
		m_user_data->~user_data_variant		();
	m_user_data							=	NULL;
}

query_result_for_cook::~query_result_for_cook ()
{
	clear_user_data							();
}

} // namespace resources
} // namespace xray