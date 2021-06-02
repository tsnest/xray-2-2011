//-------------------------------------------------------------------------
// This file serves as dependency tree between components of the subsystem 
//-------------------------------------------------------------------------

namespace xray {
namespace vfs {

virtual_file_system (i)
{
	virtual_file_system
		query_notification_operation
		
	finding
		virtual_file_system
		vfs_iterator

	watcher
		folder_watcher
			mount_history
		mount_history
		
	vfs_mount_ptr
		vfs_mount
			virtual_file_system
		
} // virtual_file_system
		
		
} // namespace vfs
} // namespace xray

namespace xray {
namespace fs_new {

synchronous_device_interface (i)
	device_file_system_interface
	asynchronous_device_interface
	
	device_file_system_proxy
		device_file_system_watcher_proxy
			device_file_system_proxy_base
				device_file_system_interface

		device_file_system_no_watcher_proxy
			device_file_system_proxy_base
				
windows_hdd_file_system (i)
	device_file_system_interface

asynchronous_device_interface (i)
	synchronous_device_interface
	device_file_system_interface


} // namespace fs_new
} // namespace xray


todos:
1. use spsc queues in watcher
2. implement hot unmount of mount root
3. implement find of several iterators

































