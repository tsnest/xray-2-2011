#include "pch.h"
#include "test_application.h"

#include <xray/core/core.h>
#include <xray/intrusive_list.h>
#include <stdio.h>

#include <xray/os_preinclude.h>
#undef NOUSER
#undef NOMSG
#undef NOWINMESSAGES
#undef NOCTLMGR
#include <xray/os_include.h>
using namespace xray;

#include <xray/core/simple_engine.h>
#include <xray/uninitialized_reference.h>

#include <xray/intrusive_spsc_queue.h>
#include <xray/intrusive_mpsc_queue.h>
#include <xray/fs_asynchronous_device_interface.h>
#include <xray/fs_windows_hdd_file_system.h>
#include <xray/fs_portable_path_string.h>
#include <xray/fs_native_path_string.h>
#include <xray/fs_physical_path_iterator.h>
#include <xray/threading_reader_writer_lock.h>
#include <xray/fs_path_string_utils.h>
#include "../../core/sources/vfs_virtual_file_system.h"

xray::memory::doug_lea_allocator_type		g_test_allocator;

using namespace xray::resources;
using namespace xray;

static xray::core::simple_engine			core_engine;

threading::atomic32_type	s_device_thread_exit	=	false;
threading::atomic32_type	s_device_thread_exited	=	false;

void	device_thread					(fs_new::asynchronous_device_interface * device_interface)
{
	while ( !s_device_thread_exit )
	{
		device_interface->tick				(true);
		threading::yield					();
	}

	device_interface->finalize				();
	threading::interlocked_exchange			(s_device_thread_exited, true);
}

void   application::execute				()
{
	vfs::virtual_file_system				vfs;
	fs_new::windows_hdd_file_system			device;
	fs_new::asynchronous_device_interface	device_interface	(&device, fs_new::watcher_enabled_true);

	threading::spawn						(boost::bind(device_thread, & device_interface), "hdd", "hdd", 0, 0);

// 	SetPriorityClass( GetCurrentProcess(), REALTIME_PRIORITY_CLASS );
// 	SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL );
// 
// 	timing::timer	t;
// 	t.start();
// 	u32 elapsed = t.get_elapsed_ms();
//  
//  logging::push_filter						("test:detail", logging::silent);
//  logging::push_filter						("core:fs_new", logging::silent);

	threading::interlocked_exchange			(s_device_thread_exit, true); 
	device_interface.wakeup_thread			();
	while ( !s_device_thread_exited ) ;

	device_interface.finalize_thread_usage	();

	vfs.user_finalize						();

	m_exit_code							=	0;
}

void   application::initialize			(u32 , pstr const* )
{
	core::preinitialize						(& core_engine, 
											 core::create_log, 
											 GetCommandLine(), 
											 command_line::contains_application_true,
											 "test",
											 __DATE__);

	g_test_allocator.user_current_thread_id	();

	g_test_allocator.do_register			(128 * 1024 * 1024,	"test allocator");
	memory::allocate_region					();
	core::initialize						("../../resources/sources/", "test", core::perform_debug_initialization);
}

void   application::finalize ()
{
	core::finalize							();
}
