////////////////////////////////////////////////////////////////////////////
//	Created		: 29.06.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef FS_WATCHER_SUBSCRIBER_H_INCLUDED
#define FS_WATCHER_SUBSCRIBER_H_INCLUDED

#if XRAY_FS_NEW_WATCHER_ENABLED

#include <xray/intrusive_list.h>

namespace xray {
namespace fs_new {

typedef	intrusive_list< fs_notification, 
						fs_notification *, 
						& fs_notification::next >	fs_notification_list;

struct fs_watcher_subscriber
{
	fs_watcher_subscriber				(threading::thread_id_type			thread_id, 
										 native_path_string const &			path, 
										 fs_notification_callback const &	callback);

	u32									id;
	threading::thread_id_type			thread_id;

	fs_notification_list				notifications;
	fs_notification_callback			callback;
	native_path_string					path;

	fs_watcher_subscriber *				global_next;
	fs_watcher_subscriber *				per_thread_next;
};

typedef intrusive_list< fs_watcher_subscriber, fs_watcher_subscriber *, & fs_watcher_subscriber::global_next >	
	fs_watcher_subscriber_global_list;

typedef intrusive_list< fs_watcher_subscriber, fs_watcher_subscriber *, & fs_watcher_subscriber::per_thread_next >	
	fs_watcher_subscriber_per_thread_list;

} // namespace fs_new
} // namespace xray

#endif // #if XRAY_FS_NEW_WATCHER_ENABLED

#endif // #ifndef FS_WATCHER_SUBSCRIBER_H_INCLUDED