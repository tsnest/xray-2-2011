////////////////////////////////////////////////////////////////////////////
//	Created		: 14.06.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resources_manager.h"

namespace xray {
namespace resources {

void   resources_manager::out_of_memory_functionality::push_out_of_memory_query_impl (query_result * query)
{
	m_out_of_memory_queue.push_back					(query);
	g_resources_manager->wakeup_resources_thread	();
}

void   resources_manager::out_of_memory_functionality::tick ()
{
	while ( query_result * const query = m_out_of_memory_queue.pop_front() )
	{

	}
}


} // namespace resources
} // namespace xray

