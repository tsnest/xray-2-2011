////////////////////////////////////////////////////////////////////////////
//	Created		: 11.11.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if XRAY_FS_NEW_WATCHER_ENABLED

#include "resources_manager_watcher.h"
#include "resources_manager.h"

namespace xray {
namespace resources {

watcher::watcher						(vfs::virtual_file_system * vfs)
	:	m_vfs_watcher	(threading::g_debug_single_thread ? 
							fs_new::watcher_type_synchronous :
							fs_new::watcher_type_asynchronous,
						 & vfs->mount_history, 0),
		m_vfs			(vfs)
{
	m_vfs_watcher.set_callback				(boost::bind(& watcher::on_watcher_notification, this, _1));
	m_vfs->watcher						=	& m_vfs_watcher;
}

void   watcher::on_vfs_changed			(vfs::mount_result result, vfs::vfs_notification * notification)
{
	R_ASSERT								(result.result == vfs::result_success);

	m_vfs_watcher.notify_subscribers_if_needed	(* notification);
	
	DEBUG_FREE								(notification);
}

void   watcher::on_watcher_notification	(vfs::vfs_notification const & notification)
{
	vfs::vfs_notification * const copy	=	notification.create(& debug::g_mt_allocator, notification);
	m_notifications.push_back				(copy);
	g_resources_manager->wakeup_resources_thread	();
}

void   watcher::dispatch_notifications	()
{
	vfs::vfs_notification * notification	=	m_notifications.pop_all_and_clear();
	while ( notification )
	{
		vfs::vfs_notification * const next	=	m_notifications.get_next_of_object(notification);

		m_vfs->on_watcher_notification		(* notification, 
											 boost::bind(& watcher::on_vfs_changed, this, _1, notification), 
											 vfs::lock_operation_lock);

		notification					=	next;
	}
}

void	watcher::dispatch_watcher_callbacks	()
{
	m_vfs_watcher.dispatch_callbacks		();
}
		
void	watcher::watcher_thread_tick	()
{
	m_vfs_watcher.tick_async			();
}

void   watcher_thread_tick				()
{
	g_resources_manager->get_watcher()->watcher_thread_tick	();
}

} // namespace resources
} // namespace xray

#endif
