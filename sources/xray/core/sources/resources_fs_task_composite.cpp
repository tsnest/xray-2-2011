////////////////////////////////////////////////////////////////////////////
//	Created		: 08.09.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resources_fs_task_composite.h"
#include "resources_manager.h"

namespace xray {
namespace resources {

fs_task_composite::fs_task_composite	(memory::base_allocator * allocator)
		: fs_task(type_mount_composite, allocator)
{
	LOGI_INFO("resources", "adding mount composite task");
	g_resources_manager->change_count_of_pending_mount_operations	(+1);
}

void   fs_task_composite::add_child		(fs_task * const child) 
{ 
	m_children.push_back					(child); 
}

void   fs_task_composite::execute_may_destroy_this ()
{
	// no need to do here
}

} // namespace resources
} // namespace xray