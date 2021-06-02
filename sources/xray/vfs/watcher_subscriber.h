////////////////////////////////////////////////////////////////////////////
//	Created		: 29.06.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_WATCHER_SUBSCRIBER_H_INCLUDED
#define VFS_WATCHER_SUBSCRIBER_H_INCLUDED

#if XRAY_FS_NEW_WATCHER_ENABLED

#include <xray/intrusive_list.h>
#include <xray/vfs/watcher_notification.h>

namespace xray	{
namespace vfs	{

typedef	intrusive_list< vfs_notification, 
						vfs_notification *, 
						& vfs_notification::next >	vfs_notification_list;

struct vfs_watcher_subscriber
{
	vfs_watcher_subscriber				(threading::thread_id_type				thread_id, 
										 fs_new::virtual_path_string const &	path, 
										 vfs_notification_callback const &		callback);

	u32									id;
	threading::thread_id_type			thread_id;

	vfs_notification_list				notifications;
	vfs_notification_callback			callback;
	fs_new::virtual_path_string			path;

	vfs_watcher_subscriber *			global_next;
	vfs_watcher_subscriber *			per_thread_next;
};

typedef intrusive_list< vfs_watcher_subscriber, vfs_watcher_subscriber *, & vfs_watcher_subscriber::global_next >	
	vfs_watcher_subscriber_global_list;

typedef intrusive_list< vfs_watcher_subscriber, vfs_watcher_subscriber *, & vfs_watcher_subscriber::per_thread_next >	
	vfs_watcher_subscriber_per_thread_list;

} // namespace vfs
} // namespace xray

#endif // #if XRAY_FS_NEW_WATCHER_ENABLED

#endif // #ifndef VFS_WATCHER_SUBSCRIBER_H_INCLUDED