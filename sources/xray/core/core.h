////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_CORE_CORE_H_INCLUDED
#define XRAY_CORE_CORE_H_INCLUDED

#include <xray/logging/log_file.h>
#include <xray/resources.h>
#include <xray/resources_resource.h>
#include <xray/resources_mount_ptr.h>
#include <xray/fs/asynchronous_device_interface.h>

namespace xray {
namespace core {

struct engine;

enum debug_initialization {
	delay_debug_initialization,
	perform_debug_initialization,
}; // enum bugtrap_usage

XRAY_CORE_API	void	preinitialize	(
							core::engine * engine,
							logging::log_file_usage log_file_usage,
							pcstr command_line,
							command_line::contains_application_bool,
							pcstr application,
							pcstr build_date
						);
XRAY_CORE_API	void	initialize	(
							pcstr lua_config_device_folder_to_save_to,
							pcstr debug_thread_id,
							debug_initialization debug_initialization
						);
XRAY_CORE_API	void	initialize_resources	(
							fs_new::asynchronous_device_interface &	hdd,
							fs_new::asynchronous_device_interface &	dvd,
							resources::enable_fs_watcher_bool		enable_fs_watcher
						);
XRAY_CORE_API	void	finalize_resources ( );
XRAY_CORE_API	void	finalize	( );
XRAY_CORE_API	void	run_tests	( );

} // namespace core
} // namespace xray

#endif // #ifndef XRAY_CORE_CORE_H_INCLUDED