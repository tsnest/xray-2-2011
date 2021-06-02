////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURES_CALLBACKS_H_INCLUDED
#define RESOURES_CALLBACKS_H_INCLUDED

#include "resources_helper.h"
#include <xray/intrusive_list.h>

namespace xray {
namespace resources {

//----------------------------------------------------------
// thread local callbacks
//----------------------------------------------------------

class	fs_task;
class	queries_result;

class	thread_local_data
{
public:
	intrusive_list<queries_result, queries_result, &queries_result::m_next_ready>					finished_queries;
	intrusive_list<fs_task,	fs_task_base, & fs_task_base::next>										ready_fs_tasks;
	intrusive_list<query_result, query_result, &query_result::m_next_to_create_resource>			to_create_resource;
	intrusive_list<query_result, query_result, &query_result::m_next_to_allocate_raw_resource>		to_allocate_raw_resource;
	intrusive_list<query_result, query_result, &query_result::m_next_to_allocate_resource>			to_allocate_resource;
	intrusive_list<query_result, query_result, &query_result::m_next_to_translate_query>			to_translate_query;
	intrusive_list<raw_file_buffer_helper, raw_file_buffer_helper, &raw_file_buffer_helper::next>	to_free_user_buffers;

	bool																							in_transaction;
	intrusive_list<query_result, query_result, &query_result::m_next_to_init>						to_init_by_transaction;

	intrusive_list<unmanaged_resource, unmanaged_resource,&unmanaged_resource::m_next_delay_delete>	delayed_delete_unmanaged_resources;

	thread_local_data (memory::base_allocator * allocator) : m_allocator(allocator), in_transaction(false) { }

	void	delete_this ()
	{ 
		thread_local_data * this_ptr	=	this;
		XRAY_DELETE_IMPL					(m_allocator, this_ptr);
	}

private:
	memory::base_allocator *																		m_allocator;
};

} // namespace resources 
} // namespace xray 

#endif // #ifndef RESOURES_CALLBACKS_H_INCLUDED
