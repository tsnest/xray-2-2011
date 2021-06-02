////////////////////////////////////////////////////////////////////////////
//	Created		: 15.12.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_watcher_internal.h"
#include "fs_macros.h"
#include "fs_folder_watcher_win.h"
#include "fs_file_system.h"
#include "fs_impl.h"
#include "resources_manager.h"
#include <xray/intrusive_double_linked_list.h>
#include <xray/resources.h>

namespace xray {
namespace fs {

static command_line::key					s_no_fs_watch("no_fs_watch", "", "file system", "disables file system changes watching");

bool is_watcher_enabled ()
{
	return									!s_no_fs_watch;
}

#if XRAY_FS_WATCHER_ENABLED

class watcher
{
public:
			watcher								(file_change_callback callback);

	void	thread_proc							();
	void	unsubscribe							(pcstr logical_path, file_change_callback);
	void	subscribe							(pcstr logical_path, file_change_callback);
	void	notify_subscribers_if_needed		(file_change_info & info);

	void	watch_physical_directory			(pcstr physical_directory);
	void	quit_watching_physical_directory	(pcstr physical_directory);

	threading::mutex &	mutex_folder_watcher	() { return m_mutex_folder_watcher; }
	void	wakeup_watcher_thread				() { m_event_folder_recieved_changes.set(1); }
	u32		watcher_thread_id					() const { return m_watcher_thread_id; }

			~watcher							();

private:
	typedef intrusive_double_linked_list<	folder_watcher, 
											folder_watcher *, 
											& folder_watcher::m_prev_watcher, 
											& folder_watcher::m_next_watcher, 
											threading::mutex	>	folder_watchers;

	threading::mutex							m_mutex_folder_watcher;
	folder_watchers								m_folder_watchers;
	file_change_callback						m_callback;

	threading::event							m_event_folder_recieved_changes;
	threading::atomic32_type					m_destroying;
	threading::atomic32_type					m_thread_proc_ended;
	watcher_subscriber_list						m_subscribers;
	u32											m_watcher_thread_id;
};

watcher::watcher (file_change_callback callback) : 
			m_destroying(false), m_thread_proc_ended(false), m_watcher_thread_id(0), m_callback(callback)
{
	m_watcher_thread_id			=	threading::spawn (
											boost::bind(&watcher::thread_proc, this), 
											"file system monitor", 
											"FS-monitor", 
											0,
											3 % threading::core_count(),
											threading::tasks_aware
										);
}

watcher::~watcher ()
{
	threading::interlocked_exchange		(m_destroying, true);
	m_event_folder_recieved_changes.set	(true);
	while ( !m_thread_proc_ended )		{ ; }

	struct delete_predicate
	{
		void operator ()(folder_watcher * folder_watcher)
		{
			DEBUG_DELETE				(folder_watcher);
		}
	};

	delete_predicate					delete_predicate;
	m_folder_watchers.for_each			(delete_predicate);

	struct subscriber_deleter
	{
		void operator ()(watcher_subscriber * watcher_subscriber)
		{
			DEBUG_DELETE				(watcher_subscriber);
		}
	}	deleter;
	m_subscribers.for_each		(deleter);
}

void   watcher::thread_proc ()
{
	struct try_recieve_changes_predicate
	{
		void operator () (folder_watcher * const folder_watcher)
		{
			folder_watcher->try_receive_changes	();
		}
	};

	for ( ;; )
	{
		m_event_folder_recieved_changes.wait	(threading::wait_time_infinite);
		if ( m_destroying )
			break;

	#pragma message (XRAY_TODO("fix it"))
	// resources::dispatch_callbacks			();

		threading::mutex_raii	raii			(m_mutex_folder_watcher);
		try_recieve_changes_predicate			try_recieve_changes_predicate;
		m_folder_watchers.for_each				(try_recieve_changes_predicate);
	}	

	resources::finalize_thread_usage	(false);

	threading::interlocked_exchange		(m_thread_proc_ended, true);
}

struct find_watcher_predicate
{
	enum find_enum	{ find_exact, find_as_subfolder };

	find_watcher_predicate	(find_enum find, pcstr physical_path) 
				: find(find), physical_path(physical_path), found_watcher(NULL) {}

	void operator () (folder_watcher * const folder_watcher)
	{
		if ( find == find_exact )
		{
			if ( physical_path == folder_watcher->path() )
				found_watcher		=	folder_watcher;
		}
		else
		{
			fs::path_string watcher_path_with_slash	=	folder_watcher->path();
			if ( watcher_path_with_slash.length() )
				watcher_path_with_slash				+=	'/';
						
			if ( physical_path.find(watcher_path_with_slash.c_str()) == 0 ||
				 physical_path == folder_watcher->path() )
			{
				found_watcher		=	folder_watcher;
			}
		}
	}

	find_enum							find;
	folder_watcher *					found_watcher;
	fixed_string512						physical_path;
}; // find_watcher_predicate

void   watcher::watch_physical_directory (pcstr physical_path)
{
	threading::mutex_raii	raii			(m_mutex_folder_watcher);
	find_watcher_predicate					find_watcher(find_watcher_predicate::find_as_subfolder, physical_path);
	m_folder_watchers.for_each				(find_watcher);

	if ( find_watcher.found_watcher )
		return; // already watching

	folder_watcher * const new_watcher	=	DEBUG_NEW(folder_watcher)
											(physical_path, 
											 m_callback, 
											 & m_event_folder_recieved_changes);
	
	m_folder_watchers.push_back				(new_watcher);

	m_event_folder_recieved_changes.set		(true);
}

void   watcher::quit_watching_physical_directory (pcstr physical_path)
{
	threading::mutex_raii	raii			(m_mutex_folder_watcher);
	find_watcher_predicate					find_watcher(find_watcher_predicate::find_exact, physical_path);
	m_folder_watchers.for_each				(find_watcher);

	if ( !find_watcher.found_watcher )
		return;

	CURE_ASSERT								(find_watcher.found_watcher, return, "not watching directory: %s", physical_path);
	m_folder_watchers.erase					(find_watcher.found_watcher);

	DEBUG_DELETE							(find_watcher.found_watcher);	
}

#ifndef MASTER_GOLD

void   watcher::subscribe (pcstr logical_path, file_change_callback callback)
{
	watcher_subscriber * subscriber		=	DEBUG_NEW(watcher_subscriber)(threading::current_thread_id(), 
																		  logical_path, 
																		  callback);
	m_subscribers.push_back				(subscriber);
}

struct erase_predicate
{
	erase_predicate	(pcstr logical_path, file_change_callback callback) : logical_path(logical_path), callback(callback) {}
	bool operator () (watcher_subscriber * subscriber)
	{
		if ( logical_path == subscriber->logical_path &&
			 threading::current_thread_id() == subscriber->thread_id &&
			 callback == subscriber->callback)
		{
			out_subscriber				=	subscriber;
			return							true;
		}

		return								false;
	}
	path_string								logical_path;
	file_change_callback					callback;
	watcher_subscriber *					out_subscriber;
};

void   watcher::unsubscribe (pcstr logical_path, file_change_callback callback)
{
	resources::g_resources_manager->remove_fs_watcher_notifications	(callback);

	erase_predicate		predicate			(logical_path, callback);
	bool const erased					=	m_subscribers.remove_if(predicate) != NULL;
	R_ASSERT								(erased);
	DEBUG_DELETE							(predicate.out_subscriber);
}

void   watcher::notify_subscribers_if_needed (file_change_info & info)
{
	watcher_subscriber_list::mutex_raii raii(m_subscribers);
	for ( watcher_subscriber *	it_subscriber	=	m_subscribers.front();
								it_subscriber;
								it_subscriber	=	m_subscribers.get_next_of_object(it_subscriber) )
	{
		if ( info.type == file_change_info::change_type_renamed )
		{
			if ( info.logical_renamed_old_file_path.find(it_subscriber->logical_path.c_str()) != 0 )
				continue;
		}
		else
		{
			if ( info.logical_file_path.find(it_subscriber->logical_path.c_str()) != 0 )
				continue;
		}

		resources::g_resources_manager->push_fs_watcher_notifications	
													(it_subscriber->thread_id, 
													 info, 
													 it_subscriber->callback);
	}
}

#endif // #ifndef MASTER_GOLD

static uninitialized_reference<watcher>		s_watcher;

threading::mutex &   mutex_folder_watcher ()
{
	return									s_watcher->mutex_folder_watcher();
}

bool   is_watcher_initialized ()
{
	return									s_watcher.initialized();
}

void   initialize_watcher ()
{
	if ( !is_watcher_enabled() )
		return;

	XRAY_CONSTRUCT_REFERENCE				(s_watcher, watcher)
											(file_change_callback(g_fat->impl(), & file_system_impl::on_disk_file_change));
}

void   finalize_watcher ()
{
	if ( !is_watcher_enabled() )
		return;

	XRAY_DESTROY_REFERENCE					(s_watcher);
}

void   watch_physical_directory (pcstr const physical_path)
{
	s_watcher->watch_physical_directory		(physical_path);
}

void   quit_watching_physical_directory (pcstr const physical_path)
{
	s_watcher->quit_watching_physical_directory	(physical_path);
}

void   wakeup_watcher_thread () 
{ 
	s_watcher->wakeup_watcher_thread		();
}

void   subscribe_watcher (pcstr logical_path, file_change_callback callback)
{
	s_watcher->subscribe					(logical_path, callback);
}

void   unsubscribe_watcher (pcstr logical_path, file_change_callback callback)
{
	s_watcher->unsubscribe					(logical_path, callback);
}

void   notify_subscribers (file_change_info & info)
{
	s_watcher->notify_subscribers_if_needed	(info);
}

u32   watcher_thread_id	() 
{
	return									s_watcher->watcher_thread_id(); 
}

struct skip_deassociation_node
{
	skip_deassociation_node	() : next(NULL), prev(NULL) {}

	skip_deassociation_node *	next;
	skip_deassociation_node *	prev;

#pragma warning (push)
#pragma warning (disable:4200)
	char						relative_path[];
#pragma warning (pop)
};

intrusive_double_linked_list<	skip_deassociation_node, skip_deassociation_node *,
								& skip_deassociation_node::prev, 
								& skip_deassociation_node::next, 
								threading::simple_lock	>	s_skip_deassociation_nodes;


static
void   skip_deassociation_notification (pcstr physical_path)
{
 	path_string const relative_path	=	convert_to_relative_path(physical_path);
	
	pvoid const buffer		=	DEBUG_MALLOC(sizeof(skip_deassociation_node) + relative_path.length() + 1,
											 "skip_deassociation_node");
	skip_deassociation_node * const new_node	=	new (buffer) skip_deassociation_node;
	strings::copy				(new_node->relative_path, relative_path.length() + 1, relative_path.c_str());

	s_skip_deassociation_nodes.push_back	(new_node);	
}

static void skip_deassociation_notifications_impl (pcstr physical_path, u32 notifications_to_skip)
{
	if ( !notifications_to_skip )
		return;

	ASSERT			( is_watcher_enabled() );

	path_string const relative_path	=	convert_to_relative_path(physical_path);
	path_string		logical_path;
	if ( !resources::convert_physical_to_logical_path(& logical_path, relative_path.c_str()) )
		return;

	for ( u32 i=0; i<notifications_to_skip; ++i )
		skip_deassociation_notification	(relative_path.c_str());	
}

void   skip_deassociation_notifications	(file_type* file, u32 const notifications_to_skip)
{
	if ( !is_watcher_enabled() )
		return;

	skip_deassociation_notifications_impl( file->file_name, notifications_to_skip );
}

void   skip_deassociation_notifications (pcstr physical_path, u32 notifications_to_skip)
{
	if ( !is_watcher_enabled() )
		return;

	skip_deassociation_notifications_impl( physical_path, notifications_to_skip );
}

bool   try_remove_deassociation_notification (pcstr physical_path)
{
	path_string const relative_path	=	convert_to_relative_path(physical_path);

	threading::simple_lock::mutex_raii	raii(s_skip_deassociation_nodes);
	for ( skip_deassociation_node *	it_node	=	s_skip_deassociation_nodes.back();
									it_node;
									it_node	=	s_skip_deassociation_nodes.get_prev_of_object(it_node) )
	{
		if ( relative_path != it_node->relative_path )
			continue;

		s_skip_deassociation_nodes.erase	(it_node);
		it_node->~skip_deassociation_node	();
		DEBUG_FREE							(it_node);
		return								true;		
	}

	return									false;
}

bool   try_remove_deassociation_notifications (pcstr physical_path, u32 notifications_to_skip)
{
	for ( u32 i=0; i<notifications_to_skip; ++i )
		if ( !try_remove_deassociation_notification(physical_path) )
			return							false;

	return									true;
}

#else // #if XRAY_FS_WATCHER_ENABLED

bool				is_watcher_initialized					() { return false; }
static threading::mutex		s_mutex_folder_watcher;
threading::mutex &  mutex_folder_watcher					() { return s_mutex_folder_watcher; }
void				initialize_watcher						() {}
void				finalize_watcher						() {}
void				watch_physical_directory				(pcstr) {}
void				quit_watching_physical_directory		(pcstr) {}
void				wakeup_watcher_thread					() {}
u32					watcher_thread_id						() { return 0; }
void				skip_deassociation_notifications		(file_type* , u32 const ) { }
void				skip_deassociation_notifications		(pcstr, u32) {}
bool				try_remove_deassociation_notifications	(pcstr, u32) { return true; }
bool				try_remove_deassociation_notification	(pcstr) { return true; }

#endif // #if XRAY_FS_WATCHER_ENABLED

} // namespace fs
} // namespace xray


