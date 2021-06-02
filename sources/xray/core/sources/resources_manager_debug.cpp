////////////////////////////////////////////////////////////////////////////
//	Created		: 15.12.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resources_manager.h"

namespace xray {
namespace resources {

void   resources_manager::fill_stats	(strings::text_tree_item & stats)
{
	m_debug.fill_stats						(stats);
	stats.new_child							("fs tasks", m_fs_tasks.size());
	stats.new_child							("fs subtasks", m_fs_sub_tasks.size());
	stats.new_child							("to_create", m_resources_to_create.size());

	strings::text_tree_item & threads_item	=	* stats.new_child("threads");

	m_thread_local_data_lock.lock_read		();

	for ( thread_local_data_tree::const_iterator	it		=	m_thread_local_data.begin(),
													it_end	=	m_thread_local_data.end();
													it		!=	it_end;
													++it )
	{
		thread_local_data const & tls	=	* it;

		strings::text_tree_item & thread_item	=	* threads_item.new_childf(
			tls.thread_name.c_str(),
			"ready q(%d), ready fs(%d), alloc(%d), create(%d), translate(%d)",
			tls.finished_queries.size(),
			tls.ready_fs_tasks.size(),
			tls.to_allocate_resource.size() + tls.to_allocate_raw_resource.size(),
			tls.to_create_resource.size(),
			tls.to_translate_query.size()
		);
	}

	m_thread_local_data_lock.unlock_read	();
}

#if DEBUG_RESOURCES
void   debug_state::fill_stats			(strings::text_tree_item & stats)
{
	stats.new_child							("unmanaged resources", m_unmanaged_resources.size());

	stats.new_child							("pending queries", m_pending_query_list.size());
	stats.new_child							("vfs lock queries", m_vfs_lock_query_list.size());
	stats.new_child							("delay delete unmanaged", m_global_delay_delete_unmanaged_list.size());
	stats.new_child							("delay deallocate unmanaged", m_global_delay_deallocate_unmanaged_buffer_list.size());
}
#endif // #if DEBUG_RESOURCES

} // namespace resources
} // namespace xray