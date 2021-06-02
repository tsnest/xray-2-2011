////////////////////////////////////////////////////////////////////////////
//	Created		: 11.11.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_MANAGER_WATCHER_H_INCLUDED
#define RESOURCES_MANAGER_WATCHER_H_INCLUDED

#if XRAY_FS_NEW_WATCHER_ENABLED

#include <xray/threading_active_object.h>
#include <xray/vfs/watcher.h>
#include <xray/vfs/virtual_file_system.h>

namespace xray {
namespace resources {

class watcher
{
public:
			watcher								(vfs::virtual_file_system * vfs);

	void	dispatch_notifications				();
	void	dispatch_watcher_callbacks			();
	void	watcher_thread_tick					();

private:
	void    on_watcher_notification				(vfs::vfs_notification const & notification);
	void	on_vfs_changed						(vfs::mount_result result, vfs::vfs_notification * notification);

private:
	vfs::virtual_file_system *					m_vfs;
	vfs::vfs_watcher							m_vfs_watcher;
	vfs::vfs_notification_list					m_notifications;
};

} // namespace resources
} // namespace xray

#endif

#endif // #ifndef RESOURCES_MANAGER_WATCHER_H_INCLUDED