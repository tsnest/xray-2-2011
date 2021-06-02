////////////////////////////////////////////////////////////////////////////
//	Created		: 29.06.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if XRAY_FS_NEW_WATCHER_ENABLED

#include <xray/intrusive_double_linked_list.h>
#include <xray/fs/watcher.h>
#include <xray/type_predicates.h>

namespace xray {
namespace fs_new {

fs_watcher::fs_watcher					(watcher_type_enum const type, fs_notification_callback const & callback) 
	:	m_type							(type), 
		m_callback						(callback) 
{
}

fs_watcher::~fs_watcher						()
{
	delete_predicate						delete_predicate(& debug::g_mt_allocator);
	m_folder_watchers.for_each				(delete_predicate);
	m_subscribers.for_each					(delete_predicate);
}

void   fs_watcher::tick_sync			()
{
	R_ASSERT								(m_type == watcher_type_synchronous);
	tick_impl								(0);
}

void   fs_watcher::tick_async			(u32 max_millis_to_sleep)
{
	R_ASSERT								(m_type == watcher_type_asynchronous);
	tick_impl								(max_millis_to_sleep);
}

void   fs_watcher::tick_impl			(u32 max_millis_to_sleep)
{
	struct try_recieve_changes_predicate
	{
		try_recieve_changes_predicate	(watcher_type_enum type) : type(type) {}
		void operator ()				(folder_watcher * const folder_watcher)
		{
			if ( type == watcher_type_asynchronous )
				folder_watcher->try_receive_changes_async	();
			else
				folder_watcher->try_receive_changes_sync	();
		}

		watcher_type_enum type;
	};

	if ( m_type == watcher_type_asynchronous )
		m_event_folder_recieved_changes.wait	(max_millis_to_sleep);
	
	mutex::mutex_raii	raii				(get_mutex());
	try_recieve_changes_predicate			try_recieve_changes_predicate(m_type);
	m_folder_watchers.for_each				(try_recieve_changes_predicate);
}

void   fs_watcher::wakeup				()
{
	m_event_folder_recieved_changes.set		(true);
}

struct find_watcher_predicate
{
	enum find_enum						{ find_exact, find_as_subfolder };

	find_watcher_predicate				(find_enum find, native_path_string const & path) 
										: find(find), path(path), found_watcher(NULL) {}

	void operator ()					(folder_watcher * const folder_watcher)
	{
		if ( find == find_exact )
		{
			if ( path == folder_watcher->path() )
				found_watcher			=	folder_watcher;
		}
		else
		{
			native_path_string watcher_path_with_slash	=	folder_watcher->path();
			if ( watcher_path_with_slash )
				watcher_path_with_slash	+=	native_path_string::separator;
						
			if ( path.find(watcher_path_with_slash.c_str()) == 0 || path == folder_watcher->path() )
			{
				R_ASSERT					(!found_watcher);
				found_watcher			=	folder_watcher;
			}
		}
	}

	find_enum							find;
	folder_watcher *					found_watcher;
	native_path_string					path;
}; // find_watcher_predicate

void   fs_watcher::watch_physical_directory	(native_path_string const & path)
{
	threading::mutex_raii	raii			(get_mutex());
	find_watcher_predicate					find_watcher(find_watcher_predicate::find_as_subfolder, path);
	m_folder_watchers.for_each				(find_watcher);

	if ( find_watcher.found_watcher )
		return; // already watching

	folder_watcher * const new_watcher	=	DEBUG_NEW(folder_watcher)
											(path, m_callback, & m_event_folder_recieved_changes);
	
	m_folder_watchers.push_back				(new_watcher);

	m_event_folder_recieved_changes.set		(true);
}

void   fs_watcher::quit_watching_physical_directory (native_path_string const & path)
{
	threading::mutex_raii	raii			(get_mutex());
	find_watcher_predicate					find_watcher(find_watcher_predicate::find_exact, path);
	m_folder_watchers.for_each				(find_watcher);

	if ( !find_watcher.found_watcher )
		return;

	CURE_ASSERT								(find_watcher.found_watcher, return, "not watching directory: %s", path.c_str());
	m_folder_watchers.erase					(find_watcher.found_watcher);

	DEBUG_DELETE							(find_watcher.found_watcher);	
}

} // namespace fs_new
} // namespace xray

#endif // #if XRAY_FS_NEW_WATCHER_ENABLED

