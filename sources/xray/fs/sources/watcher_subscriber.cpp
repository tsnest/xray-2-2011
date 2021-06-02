////////////////////////////////////////////////////////////////////////////
//	Created		: 06.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs/watcher.h>
#include <xray/threading_functions_tls.h>
#include <xray/type_predicates.h>

namespace xray {
namespace fs_new {

#if XRAY_FS_NEW_WATCHER_ENABLED

static u32 const s_fs_subscriber_tls_key	=	threading::tls_create_key();
static threading::atomic32_type	s_fs_subscriber_id	=	0;

fs_watcher_subscriber::fs_watcher_subscriber	(threading::thread_id_type const	thread_id, 
												 native_path_string const &			path, 
												 fs_notification_callback const &	callback)
				:	id							(threading::interlocked_increment(s_fs_subscriber_id)),
					thread_id					(thread_id), 
					callback					(callback), 
					path						(path), 
					global_next					(NULL), 
					per_thread_next				(NULL) 
{
}

u32   fs_watcher::subscribe				(native_path_string const &			path, 
										 fs_notification_callback const &	callback)
{
	fs_watcher_subscriber * new_subscriber	=	DEBUG_NEW(fs_watcher_subscriber)
													(threading::current_thread_id(), path, callback);
	m_subscribers.push_back					(new_subscriber);

	fs_watcher_subscriber_per_thread_list *	per_thread_subscribers	=	
											threading::get_or_create_tls_value<fs_watcher_subscriber_per_thread_list>
												(s_fs_subscriber_tls_key, & debug::g_mt_allocator);

	per_thread_subscribers->push_back		(new_subscriber);
	return									new_subscriber->id;
}

void   fs_watcher::unsubscribe				(u32 id)
{
	fs_watcher_subscriber * erased		=	m_subscribers.remove_if
											(
												member_equal_to(& fs_watcher_subscriber::id, id)
											);

	R_ASSERT								(erased);
	DEBUG_DELETE							(erased);
}

struct notify_subscriber : core::noncopyable
{
	notify_subscriber (fs_notification const & notification) : notification(notification) {}

	void operator ()					(fs_watcher_subscriber * subscriber) const
	{
		if ( notification.type == fs_notification::type_renamed )
		{
			if ( strstr(notification.old_path, subscriber->path.c_str()) != 0 )
				return;
		}
		else
		{
			if ( strstr(notification.path, subscriber->path.c_str()) != 0 )
				return;
		}
		
		fs_notification * const new_notification	=	fs_notification::create(& debug::g_mt_allocator, notification);
		subscriber->notifications.push_back	(new_notification);
	}

	fs_notification const &				notification;
};

void   fs_watcher::notify_subscribers_if_needed	(fs_notification const & notification)
{
	m_subscribers.for_each					(notify_subscriber(notification));
}

struct dispatch_notifications
{
	void operator ()					(fs_watcher_subscriber * const subscriber) const
	{
		fs_notification * it_notification	=	subscriber->notifications.pop_all_and_clear();
		while ( it_notification )
		{
			fs_notification * const next	=	it_notification->next;

			subscriber->callback			(* it_notification);

			DEBUG_FREE						(it_notification);
			it_notification				=	next;
		}
	}
};

void	fs_watcher::dispatch_callbacks	()
{
	fs_watcher_subscriber_per_thread_list *	per_thread_subscribers	
													=	
			(fs_watcher_subscriber_per_thread_list *)threading::tls_get_value(s_fs_subscriber_tls_key);

	if ( !per_thread_subscribers )
		return;

	per_thread_subscribers->for_each		(dispatch_notifications());
}

#endif // #if XRAY_FS_NEW_WATCHER_ENABLED

} // namespace fs_new
} // namespace xray