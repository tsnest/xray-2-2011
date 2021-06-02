////////////////////////////////////////////////////////////////////////////
//	Created 	: 03.02.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/resources_fs_task.h>
#include <xray/resources_fs.h>
#include <xray/vfs/virtual_file_system.h>
#include "resources_manager.h"
#include "resources_fs_task_iterator.h"

namespace xray {
namespace resources {

fs_task::fs_task						(type_enum const type, 
										 memory::base_allocator * const allocator,
										 query_result_for_cook * const parent_query)
	:	m_type			(type), 
		m_allocator		(allocator),
		m_thread_id		(threading::current_thread_id()),
		m_result		(false),
		m_parent_query	(parent_query),
		m_next			(NULL)
{
}

bool   fs_task::is_mount_task			() const
{
	 return									(m_type > type_mount_operations_start) 
														&& 
											(m_type < type_mount_operations_end);
}

void   fs_task::on_task_ready_may_destroy_this	()
{
	u32 const thread_id					=	m_thread_id;
	thread_local_data * const local_data	=	g_resources_manager->get_thread_local_data(thread_id, true);
	local_data->ready_fs_tasks.push_back	(this);
	g_resources_manager->wakeup_thread_by_id_if_needed	(thread_id);
}

} // namespace resources
} // namespace xray