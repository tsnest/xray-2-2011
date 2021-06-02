////////////////////////////////////////////////////////////////////////////
//	Created		: 29.06.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_WATCHER_H_INCLUDED
#define VFS_WATCHER_H_INCLUDED

#include <xray/fs/watcher.h>

#if XRAY_FS_NEW_WATCHER_ENABLED

#include <xray/vfs/watcher_notification.h>
#include <xray/vfs/watcher_subscriber.h>

namespace xray {
namespace vfs {

class vfs_watcher
{
public:
	typedef	fs_new::fs_watcher::mutex		mutex;
public:
			vfs_watcher						(fs_new::watcher_type_enum	type, 
											 mount_history_container *	mount_history, 
											 vfs_notification_callback	callback);

	u32		subscribe						(fs_new::virtual_path_string const & virtual_path, vfs_notification_callback const &);
	void	unsubscribe						(u32 subscriber_id);
	void	notify_subscribers_if_needed	(vfs_notification const & notification);

	void	tick_async						(u32 max_millis_to_sleep = 100) { m_fs_watcher.tick_async(max_millis_to_sleep); }
	void	tick_sync						() { m_fs_watcher.tick_sync(); }

	void	watch_physical_directory			(fs_new::native_path_string const & path) { m_fs_watcher.watch_physical_directory(path); }
	void	quit_watching_physical_directory	(fs_new::native_path_string const & path) { m_fs_watcher.quit_watching_physical_directory(path); }

	void	set_callback					(vfs_notification_callback const & callback) { m_callback = callback; }
	void	dispatch_callbacks				();
	void	wakeup							();
			~vfs_watcher					();

	mutex &	get_mutex						() { return m_fs_watcher.get_mutex(); }
private:
	void	on_fs_watcher_notification		(fs_new::fs_notification const & notification);

private:
	fs_new::fs_watcher						m_fs_watcher;
	mount_history_container *				m_mount_history;
	vfs_notification_callback				m_callback;
	vfs_watcher_subscriber_global_list		m_subscribers;
};

} // namespace vfs
} // namespace xray

#endif // #if XRAY_FS_NEW_WATCHER_ENABLED

#endif // #ifndef VFS_WATCHER_WIN_H_INCLUDED