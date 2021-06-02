////////////////////////////////////////////////////////////////////////////
//	Created		: 17.10.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_FS_SIMPLE_SYNC_INTERFACE_H_INCLUDED
#define XRAY_FS_SIMPLE_SYNC_INTERFACE_H_INCLUDED

#include <xray/fs/windows_hdd_file_system.h>
#include <xray/fs/asynchronous_device_interface.h>

namespace xray {
namespace fs_new {

#if !XRAY_PLATFORM_PS3

class simple_synchronous_interface
{
public:
	simple_synchronous_interface			(watcher_enabled_bool);

	// order of member is important due to construction
private:
	windows_hdd_file_system					hdd_device;
	windows_hdd_file_system					dvd_device;
public:
	synchronous_device_interface			hdd_sync;
	synchronous_device_interface			dvd_sync;

	asynchronous_device_interface			hdd_async;
	asynchronous_device_interface			dvd_async;

}; // class simple_synchronous_interface

#endif // #if !XRAY_PLATFORM_PS3

} // namespace fs_new
} // namespace xray

#endif // #ifndef XRAY_FS_SIMPLE_SYNC_INTERFACE_H_INCLUDED