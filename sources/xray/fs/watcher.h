////////////////////////////////////////////////////////////////////////////
//	Created		: 06.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef FS_WATCHER_H_INCLUDED
#define FS_WATCHER_H_INCLUDED

#if XRAY_FS_NEW_WATCHER_ENABLED

#include <xray/fs/watcher_notification.h>
#include <xray/fs/folder_watcher.h>
#include <xray/fs/watcher_subscriber.h>
#include <xray/intrusive_double_linked_list.h>

namespace xray {
namespace fs_new {

class fs_watcher
{
public:
	typedef	threading::mutex				mutex;
public:
			fs_watcher						(watcher_type_enum type, fs_notification_callback const & callback);

	void	tick_async						(u32 max_millis_to_sleep = 100);
	void	tick_sync						();

	u32		subscribe						(native_path_string const & path, fs_notification_callback const &);
	void	unsubscribe						(u32 subscribe_id);

	void	notify_subscribers_if_needed	(fs_notification const & info);

	void	watch_physical_directory			(native_path_string const & path);
	void	quit_watching_physical_directory	(native_path_string const & path);

	void	dispatch_callbacks				();
	void	set_callback					(fs_notification_callback const & callback) { m_callback = callback; }
	void	wakeup							();

			~fs_watcher						();

	mutex &	get_mutex						() { return m_mutex; }

private:
	void	tick_impl						(u32 max_millis_to_sleep);

private:
	typedef intrusive_double_linked_list<	folder_watcher, 
											folder_watcher *, 
											& folder_watcher::m_prev_watcher, 
											& folder_watcher::m_next_watcher, 
											threading::mutex	>	folder_watchers;

	threading::mutex					m_mutex;
	folder_watchers						m_folder_watchers;
	fs_notification_callback			m_callback;

	threading::event					m_event_folder_recieved_changes;
	fs_watcher_subscriber_global_list	m_subscribers;
	watcher_type_enum					m_type;
};

} // namespace fs_new
} // namespace xray

#endif // #if XRAY_FS_NEW_WATCHER_ENABLED

#endif // #ifndef FS_WATCHER_H_INCLUDED