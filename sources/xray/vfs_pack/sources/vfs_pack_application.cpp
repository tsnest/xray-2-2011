#include "pch.h"
#include "vfs_pack_application.h"
#include "vfs_pack_memory.h"

#include <xray/buffer_string.h>
#include <xray/engine/api.h>
#include <xray/compressor_ppmd.h>

#include <stdio.h>

#include <xray/os_preinclude.h>
#undef NOUSER
#undef NOMSG
#undef NOWINMESSAGES
#undef NOCTLMGR
#include <xray/os_include.h>
#include <xray/core/core.h>
#include <xray/core/simple_engine.h>
#include <xray/fs/simple_synchronous_interface.h>

using namespace xray;
using namespace	xray::fs_new;
using namespace	xray::vfs;
using namespace	xray::resources;

namespace vfs_pack {

xray::memory::doug_lea_allocator_type	g_vfs_pack_allocator;

static 	xray::core::simple_engine		core_engine;

application::application				()
	:	m_debug_info						(debug_info_normal), 
		m_platform							(archive_platform_unset),
		m_log_flags							(logging::log_to_console)
{
	m_log_format.set						(logging::format_specifier_message);
}

void   application::initialize (u32 argc, pstr const* argv)
{
	m_argv								=	argv;
	m_argc								=	argc;

	xray::core::preinitialize				(&core_engine,
											 logging::create_log,
											 GetCommandLine(), 
											 command_line::contains_application_true,
											 "vfs_pack",
											 __DATE__);

	g_vfs_pack_allocator.do_register		(256*1024*1024,	"vfs_pack");
	g_vfs_pack_allocator.user_current_thread_id();

	xray::memory::allocate_region			();

	xray::core::initialize					("../../resources/sources/", "vfs_pack", core::perform_debug_initialization);
	
	XRAY_CONSTRUCT_REFERENCE				(m_fs_devices, xray::fs_new::simple_synchronous_interface) 
											(xray::fs_new::watcher_enabled_true);

	xray::core::initialize_resources		(m_fs_devices->hdd_async, m_fs_devices->dvd_async, resources::enable_fs_watcher_false);

	xray::resources::start_resources_threads	();
}

void   application::finalize ()
{
	xray::resources::finish_resources_threads	();
	xray::core::finalize_resources			();
	xray::core::finalize					();
	XRAY_DESTROY_REFERENCE					(m_fs_devices);
}

bool   application::parse_command_line	()
{
	fixed_string512	platform_as_string;
	target_platform.is_set_as_string		(& platform_as_string);

	m_platform							=	archive_platform_pc;
	if ( platform_as_string.find("pc") != platform_as_string.npos )
		m_platform						=	archive_platform_pc;
	else if ( platform_as_string.find("xbox360") != platform_as_string.npos )
		m_platform						=	archive_platform_xbox360;
	else if ( platform_as_string.find("ps3") != platform_as_string.npos )
		m_platform						=	archive_platform_ps3;
	else if ( pack_command )
	{
		printf								("you must set '%s' key to one of supported platforms : pc, xbox360, ps3\n", 
											 target_platform.full_name());
		return								false;
	}

	fixed_string512		debug_info_string;
	if ( debug_info_command.is_set_as_string(& debug_info_string) )
	{
		if ( debug_info_string == "detail" )
			m_debug_info				=	debug_info_detail;
		else if ( debug_info_string == "normal" )
			m_debug_info				=	debug_info_normal;
		else 
		{
			printf							("%s key can be set to either 'normal' or 'detail'\n", debug_info_command.full_name());
			return							false;
		}
	}

	if ( !pack_command && !unpack_command && !patch_command )
	{
		printf								("you must set '%', '%s' or '%s' key\n", 
											pack_command.full_name(), unpack_command.full_name(), patch_command.full_name());
		return								false;
	}

	return									true;
}

void   application::execute ()
{
	m_exit_code							=	0;
#ifndef XRAY_STATIC_LIBRARIES
	command_line::check_keys				();
#endif // #ifndef XRAY_STATIC_LIBRARIES
	
	if ( !parse_command_line() )
	{
		m_exit_code						=	1;
		command_line::show_help_and_exit	();
		return;
	}

	bool result							=	false;	
	if ( pack_command )
		result							=	pack();
	else if ( unpack_command )
		result							=	unpack();
	else if ( patch_command )
		result							=	patch();

	resources::wait_and_dispatch_callbacks	(true);
	
	m_exit_code							=	core_engine.get_exit_code();
	if ( !m_exit_code && !result )
		m_exit_code						=	1;
}

} // namespace vfs_pack
