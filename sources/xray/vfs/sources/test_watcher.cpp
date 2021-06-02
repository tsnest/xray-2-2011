////////////////////////////////////////////////////////////////////////////
//	Created		: 08.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if XRAY_FS_NEW_WATCHER_ENABLED

#include <xray/fs/asynchronous_device_interface.h>
#include <xray/fs/windows_hdd_file_system.h>
#include "test.h"
#include <xray/vfs/watcher.h>
#include "mount_archive.h"
#include <xray/vfs/virtual_file_system.h>
#include "find.h"
#include "branch_locks.h"

namespace xray {
namespace vfs {

using namespace fs_new;

threading::atomic32_type				s_watcher_thread_exit	=	0;
threading::atomic32_type				s_watcher_thread_exited	=	0;

void    watcher_thread					(vfs_watcher * watcher)
{
	while ( !s_watcher_thread_exit )
	{
		watcher->tick_async					(100);
	}

	threading::interlocked_exchange			(s_watcher_thread_exited, true);
}

void    on_vfs_changed					(mount_result				result, 
										 vfs_watcher *				watcher, 
										 vfs_notification *			notification)
{
	R_ASSERT								(result.result == result_success);

	watcher->notify_subscribers_if_needed	(* notification);
	
	DEBUG_FREE								(notification);
}

void    on_watcher_notification			(vfs_notification const &	notification, 
										 virtual_file_system *		vfs,
										 vfs_watcher *				watcher)
{
	vfs::vfs_notification * const copy	=	notification.create(& debug::g_mt_allocator, notification);

	vfs->on_watcher_notification			(notification, 
											 boost::bind(& on_vfs_changed, _1, watcher, copy), 
											 vfs::lock_operation_lock);
}

void   subscriber_callback				(vfs_notification const &	notification, 
										 virtual_file_system *		vfs,
										 bool *						quit_watcher_test)
{
	LOG_INFO								("subscriber callback called for: '%s'",
											 notification.virtual_path ? 
												notification.virtual_path : notification.virtual_old_path);

	log_vfs_root							(* vfs);
	if ( notification.physical_new_path && strstr(notification.physical_new_path, "quit") != 0 )
		* quit_watcher_test				=	true;
}

void   vfs_test::test_watcher			()
{
 	vfs_watcher	watcher						(watcher_type_asynchronous, & m_vfs.mount_history, NULL);
	watcher.set_callback					(boost::bind(& on_watcher_notification, _1, & m_vfs, & watcher));
	threading::spawn						(boost::bind(& watcher_thread, & watcher), "vfs_watcher", "vfs_watcher", 0, 0);

	bool	quit_watcher_test			=	false;
	watcher.subscribe						("", boost::bind(& subscriber_callback, _1, & m_vfs, & quit_watcher_test));

	native_path_string watcher_test_path	=	native_path_string::convert(m_suite->get_resources_path());
	char const separator				=	native_path_string::separator;
	watcher_test_path.appendf				("%ctests%cvfs%cwatcher", separator, separator, separator);

	query_mount_arguments	mount_args	=	query_mount_arguments::mount_physical_path	
												(& debug::g_mt_allocator, 
												 "", 
												 watcher_test_path,
												 "", 
												 m_device.c_ptr(),
												 NULL,
												 & mount_callback,
												 recursive_false);

	vfs_mount_ptr const mount_ptr		=	query_mount_and_wait(m_vfs, mount_args).mount;
	log_vfs_root							(m_vfs);  

	m_vfs.set_watcher						(& watcher);

	while ( !quit_watcher_test )
  	{
		watcher.dispatch_callbacks			();
  		m_device->dispatch_callbacks		();
  		m_vfs.dispatch_callbacks			();
  	}

	m_vfs.set_watcher						(NULL);

	threading::interlocked_exchange			(s_watcher_thread_exit, true); 
	while ( !s_watcher_thread_exited ) ;
}

} // namespace vfs
} // namespace xray

#endif // #if XRAY_FS_NEW_WATCHER_ENABLED