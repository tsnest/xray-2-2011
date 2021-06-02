#include "pch.h"


#include <xray/intrusive_spsc_queue.h>
#include <xray/intrusive_mpsc_queue.h>
#include <xray/fs/asynchronous_device_interface.h>
#include <xray/fs/windows_hdd_file_system.h>
#include <xray/fs/portable_path_string.h>
#include <xray/fs/native_path_string.h>
#include <xray/fs/physical_path_iterator.h>
#include <xray/threading_reader_writer_lock.h>
#include <xray/fs/path_string_utils.h>

#include <xray/testing.h>
#include "test.h"
#include <xray/core_test_suite.h>

#include <xray/os_include.h>

#if !XRAY_PLATFORM_PS3

XRAY_DECLARE_LINKAGE_ID(vfs_test);

namespace xray {

using namespace vfs;
using namespace fs_new;

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

//REGISTER_TEST_CLASS(vfs_test, core_test_suite);

void   vfs_test::test					(core_test_suite * suite)
{
	m_suite								=	suite;
	fs_new::windows_hdd_file_system			device;
	XRAY_CONSTRUCT_REFERENCE				(m_device, fs_new::asynchronous_device_interface) 
												(& device, fs_new::watcher_enabled_true);

	threading::spawn						(boost::bind(device_thread, m_device.c_ptr()), "hdd", "hdd", 0, 0);

    	logging::push_filter						("vfs", logging::warning, & memory::g_mt_allocator);
    	logging::push_filter						("fs", logging::warning, & memory::g_mt_allocator);
 
  	//test_mount_unmount						();
   	test_inside_mount						();
  	test_find								();
	test_patch								();

//	test_watcher							(); 

	threading::interlocked_exchange			(s_device_thread_exit, true); 
	m_device->wakeup_thread					();
	while ( !s_device_thread_exited ) ;
	m_device->finalize_thread_usage			();

	m_vfs.user_finalize						();
}

} // namespace xray

#endif // #if !XRAY_PLATFORM_PS3