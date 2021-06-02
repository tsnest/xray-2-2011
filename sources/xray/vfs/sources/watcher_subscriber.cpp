////////////////////////////////////////////////////////////////////////////
//	Created		: 06.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if XRAY_FS_NEW_WATCHER_ENABLED

#include <xray/vfs/watcher.h>
#include <xray/threading_functions_tls.h>
#include <xray/type_predicates.h>

namespace xray {
namespace vfs {

#if XRAY_FS_NEW_WATCHER_ENABLED

using namespace fs_new;

static u32 const s_vfs_subscriber_tls_key	=	threading::tls_create_key();
static threading::atomic32_type	s_vfs_subscriber_id	=	0;

vfs_watcher_subscriber::vfs_watcher_subscriber	(threading::thread_id_type const		thread_id, 
												 fs_new::virtual_path_string const &	path, 
												 vfs_notification_callback const &		callback)
				:	id							(threading::interlocked_increment(s_vfs_subscriber_id)),
					thread_id					(thread_id), 
					callback					(callback), 
					path						(path), 
					global_next					(NULL), 
					per_thread_next				(NULL) 
{
}

u32   vfs_watcher::subscribe			(virtual_path_string const &			path, 
										 vfs_notification_callback const &		callback)
{
	R_ASSERT								(!path.ends_with(virtual_path_string::separator), "path should not end with separator!");
	vfs_watcher_subscriber * new_subscriber	=	DEBUG_NEW(vfs_watcher_subscriber)
													(threading::current_thread_id(), path, callback);
	m_subscribers.push_back					(new_subscriber);

	vfs_watcher_subscriber_per_thread_list *	per_thread_subscribers	=	
											threading::get_or_create_tls_value<vfs_watcher_subscriber_per_thread_list>
												(s_vfs_subscriber_tls_key, & debug::g_mt_allocator);

	per_thread_subscribers->push_back		(new_subscriber);
	return									new_subscriber->id;
}

void   vfs_watcher::unsubscribe				(u32 id)
{
	vfs_watcher_subscriber * erased		=	m_subscribers.remove_if
											(
												member_equal_to(& vfs_watcher_subscriber::id, id)
											);

	R_ASSERT								(erased);

	vfs_watcher_subscriber_per_thread_list *	per_thread_subscribers	=	
		(vfs_watcher_subscriber_per_thread_list*)threading::tls_get_value(s_vfs_subscriber_tls_key);

	per_thread_subscribers->erase			(erased);

	DEBUG_DELETE							(erased);
}

struct notify_subscriber : core::noncopyable
{
	notify_subscriber (vfs_notification const & notification, mount_history_container *	mount_history) 
		: notification(notification), mount_history(mount_history) {}

	void operator ()					(vfs_watcher_subscriber * subscriber) const
	{
		if ( notification.type == vfs_notification::type_renamed )
		{
			if ( subscriber->path.length() && !path_starts_with(notification.virtual_old_path, subscriber->path.c_str()) )
				return;
		}
		else
		{
			if ( subscriber->path.length() && !path_starts_with(notification.virtual_path, subscriber->path.c_str()) )
				return;
		}
		
		vfs_notification * const new_notification	=	vfs_notification::create(& debug::g_mt_allocator, notification);
		subscriber->notifications.push_back	(new_notification);
	}

	vfs_notification const &			notification;
	mount_history_container *			mount_history;
};

void   vfs_watcher::notify_subscribers_if_needed	(vfs_notification const & notification)
{
	m_subscribers.for_each					(notify_subscriber(notification, m_mount_history));
}

struct dispatch_notifications
{
	void operator ()					(vfs_watcher_subscriber * const subscriber) const
	{
		vfs_notification * it_notification	=	subscriber->notifications.pop_all_and_clear();
		while ( it_notification )
		{
			vfs_notification * const next	=	it_notification->next;

			subscriber->callback			(* it_notification);

			DEBUG_FREE						(it_notification);
			it_notification				=	next;
		}
	}
};

void	vfs_watcher::dispatch_callbacks	()
{
	vfs_watcher_subscriber_per_thread_list *	per_thread_subscribers	=	
		(vfs_watcher_subscriber_per_thread_list *)threading::tls_get_value(s_vfs_subscriber_tls_key);

	if ( !per_thread_subscribers )
		return;

	per_thread_subscribers->for_each		(dispatch_notifications());
}

void   vfs_watcher::wakeup				()
{
	m_fs_watcher.wakeup						();
}

#endif // #if XRAY_FS_NEW_WATCHER_ENABLED

} // namespace vfs
} // namespace xray

#endif // #if XRAY_FS_NEW_WATCHER_ENABLED