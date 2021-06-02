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

void   query_result::requery			()
{
	prepare_requery							();
	g_resources_manager->push_new_query		(this);
}

void   query_result::requery_on_out_of_memory	()
{
	LOGI_INFO								("grm", "requerying out of memory query: %s", log_string().c_str());
	prepare_requery							();
	R_ASSERT_CMP							(m_query_end_guard, ==, 0);
	threading::interlocked_exchange			(m_query_end_guard, 1);

	g_resources_manager->push_new_query		(this);
}

void   query_result::prepare_requery	()
{
	set_error_type							(error_type_unset, false);
	R_ASSERT_CMP							(m_on_created_resource_guard, ==, 0);
	threading::interlocked_exchange			(m_on_created_resource_guard, 1);
	unset_flag								(flag_processed_request_path | flag_translated_query);
	m_out_of_memory_sub_queries			=	0;
}

void   query_result_for_cook::set_requery_path (pcstr path_to_requery)
{
	if ( strings::equal(m_request_path, path_to_requery) )
		return;

	R_ASSERT								(!m_requery_path, "Omg! More than one requery? Contact Lain!");
	m_requery_path						=	strings::duplicate(memory::g_mt_allocator, path_to_requery);
}

} // namespace resources
} // namespace xray