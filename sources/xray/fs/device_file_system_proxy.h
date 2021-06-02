////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef FS_DEVICE_FILE_SYSTEM_H_INCLUDED
#define FS_DEVICE_FILE_SYSTEM_H_INCLUDED

#include <xray/fs/platform_configuration.h>

#if XRAY_FS_NEW_WATCHER_ENABLED
	#include <xray/fs/device_file_system_watcher.h>
#else
	#include <xray/fs/device_file_system_no_watcher.h>
#endif // #if XRAY_FS_NEW_WATCHER_ENABLED

namespace xray {
namespace fs_new {

enum	device_mode_enum				{ device_mode_synchronous, device_mode_asynchronous };

#if XRAY_FS_NEW_WATCHER_ENABLED
	typedef	device_file_system_watcher_proxy		device_file_system_proxy;
#else
	typedef	device_file_system_no_watcher_proxy		device_file_system_proxy;
#endif // #if XRAY_FS_NEW_WATCHER_ENABLED

} // namespace fs_new
} // namespace xray


#endif // #ifndef FS_DEVICE_FILE_SYSTEM_H_INCLUDED