////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/core/core.h>
#include <xray/logging/api.h>
#include <xray/debug/debug.h>
#include "resources_manager.h"
#include "game_resman.h"
#include "unmanaged_allocation_cook.h"
#include "strings_shared_manager.h"
#include "build_extensions.h"
#include <xray/core_entry_point.h>
#include "timing.h"
#include <xray/memory_base_allocator.h>
#include "memory.h"
#include <locale.h>			// for setlocale
#include <xray/compressor_ppmd.h>
#include <xray/core_test_suite.h>
#include <xray/core/engine.h>
#include <xray/fs/path_string_utils.h>
#include <xray/fs/synchronous_device_interface.h>
#include <xray/configs.h>
#include <xray/debug/log_callback.h>
#include <xray/console_command_processor.h>
#include <xray/console_command.h>

#if XRAY_PLATFORM_PS3
#	include <sys/paths.h>
#endif // #if XRAY_PLATFORM_PS3

#ifdef _MSC_VER
#	pragma warning( push )
#	pragma warning( disable : 4995 )
	pvoid function_pointers[] = {
		&malloc,
		&calloc,
		&realloc,
		&free,
		&_malloc_crt,
		&_calloc_crt,
		&_realloc_crt,
		&_recalloc_crt,
		&_msize
	};
#	pragma warning( pop )
#endif // #ifdef _MSC_VER

using xray::debug::error_mode;
using xray::debug::bugtrap_usage;

static bool	s_initialized				= false;
static xray::core::engine * s_engine	=	NULL;

xray::command_line::key	s_log_verbosity ("log_verbosity", "", "logging", "one of: [trace|debug|info|warning|error|silent]");
xray::command_line::key	s_write_errors_to_stderr ("write_errors_to_stderr", "", "logging", "");

xray::uninitialized_reference<xray::fs_new::synchronous_device_interface>	s_core_synchronous_device;

namespace xray {
namespace debug {
	void			notify_xbox_debugger( pcstr message );
	void			postinitialize		( );
} // namespace debug

namespace core {
	bool initialized	( );
} // namespace core

#ifndef	MASTER_GOLD
namespace core {
namespace configs {
	void initialize		( xray::core::engine& engine, pcstr lua_config_device_folder_to_save_to );
	void finalize		();
} // namespace configs
} // namespace core

#endif	// MASTER_GOLD

namespace threading {
	void preinitialize	( );
	void finalize		( );
	void initialize		( );
	void on_thread_spawn( tasks_awareness const task_awareness );
} // namespace threading

namespace memory {
extern doug_lea_allocator_type				g_log_allocator;
} // namespace memory

} // namespace xray

void log_callback						( pcstr initiator, 
										  bool	is_error_verbosity, 
										  bool	log_only_user_string, 
										  pcstr	message )
{
	using namespace xray;

	logging::log_flags_enum const log_flags	=	s_write_errors_to_stderr ? 
									logging::log_to_stderr : (logging::log_flags_enum)0;
	if ( log_only_user_string )
	{
		logging::helper				( __FILE__, __FUNCSIG__, __LINE__, initiator, 
			is_error_verbosity ? logging::error : logging::info)
			(logging::format_message, log_flags, "%s", message);
	}
	else
	{
		logging::helper				( __FILE__, __FUNCSIG__, __LINE__, initiator, 
			is_error_verbosity ? logging::error : logging::info)
			(log_flags, "%s", message);
	}

	logging::flush_log_file			( );
}

void xray::core::preinitialize		( core::engine *								engine, 
									  logging::log_file_usage const					log_file_usage,
									  pcstr const									command_line, 
									  command_line::contains_application_bool const	command_line_contains_application, 
									  pcstr	const									application,
									  pcstr	const									build_date
									)
{
	s_engine				= engine;
	R_ASSERT				( !s_initialized, "you cannot preinitialize core when it has been initialized already" );

	using namespace			debug;
	command_line::initialize( engine, command_line, command_line_contains_application );

	debug::initialize		( s_engine );

	set_application_name	( application );

	core::platform::preinitialize	( );
	threading::preinitialize( );

	XRAY_CONSTRUCT_REFERENCE(s_core_synchronous_device, fs_new::synchronous_device_interface)
								(get_core_device_file_system( ), fs_new::watcher_enabled_false);

	memory::preinitialize	( );
	build::preinitialize	( build_date );

	fs_new::device_file_system_proxy	device(get_core_device_file_system(), fs_new::watcher_enabled_true);
	logging::initialize		( device, log_file_usage );
	debug::set_log_callback	( & ::log_callback );
	
	logging::set_format		( logging::format_separator("{") + 
							  logging::format_thread_id + 
							  logging::format_time + 
							  logging::format_separator("} [") +
							  logging::format_initiator + 
							  logging::format_separator("] <") +
							  logging::format_verbosity + 
							  logging::format_separator(">   ") +
							  logging::format_message );
}

bool xray::core::initialized ( )
{
	return					s_initialized;
}

static void push_logging_rules ( )
{
	using namespace xray;
	logging::verbosity	verbosity	=	logging::trace;
	fixed_string512		verbosity_string;
	bool const log_verbosity_key_is_set	=	s_log_verbosity.is_set_as_string(& verbosity_string);
	if ( log_verbosity_key_is_set )
		verbosity			= logging::string_to_verbosity(verbosity_string.c_str());
	else if ( testing::run_tests_command_line() && !xray::debug::is_debugger_present() )
		verbosity			= logging::warning;

//	logging::verbosity const verbosity_for_resources	=	log_verbosity_key_is_set ? verbosity : logging::warning;
	logging::push_filter		( "", verbosity, & memory::g_mt_allocator );
//	logging::push_filter		( "core:fs", verbosity_for_resources, & memory::g_mt_allocator );
//	logging::push_filter		( "core:resources", verbosity_for_resources, & memory::g_mt_allocator );
//	logging::push_filter		( "core:resources:test", verbosity_for_resources, & memory::g_mt_allocator );
//	logging::push_filter		( "core:resources:device_manager", verbosity_for_resources, & memory::g_mt_allocator );

	fs_new::native_path_string	cfg_file_path;
	if ( fs_new::convert_to_absolute_path(& cfg_file_path, 
										  fs_new::native_path_string::convert("../../user_data/user.cfg"),
										  assert_on_fail_false) )
	{
		console_commands::execute_console_commands	( cfg_file_path, console_commands::execution_filter_early );
	}

}

void xray::core::initialize			(
		pcstr const lua_config_device_folder_to_save_to,
		pcstr const debug_thread_id,
		debug_initialization const debug_initialization
	)
{
	R_ASSERT				( !s_initialized, "you cannot initialize core when it has been initialized already" );

	if ( debug_initialization == perform_debug_initialization)
		debug::postinitialize	( );
	
	// for language-dependent strings	
	setlocale				( LC_CTYPE, "" );

	threading::set_thread_name	( debug_thread_id, debug_thread_id );
	
	threading::initialize	( );
	
	push_logging_rules		( );

	LOG_INFO				( "working directory: '%s'", fs_new::get_current_directory().c_str() );
	LOG_INFO				( "resources directory: '%s'", s_engine->get_resources_path() );

#ifdef XRAY_STATIC_LIBRARIES
	command_line::check_keys( );
	command_line::handle_help_key( );
#endif // #ifndef XRAY_STATIC_LIBRARIES

	timing::initialize		( );
	build::initialize		( s_engine );

	strings::initialize		( );


#if XRAY_PLATFORM_WINDOWS
	fs_new::native_path_string replication_folder_string = core::user_data_directory( );
	replication_folder_string.append_with_conversion( "/replication" );
	pcstr const replication_folder	= replication_folder_string.c_str();
#elif XRAY_PLATFORM_XBOX_360 // #if XRAY_PLATFORM_WINDOWS
	pcstr const replication_folder	= "cache:/replication";
#elif XRAY_PLATFORM_PS3 // #if XRAY_PLATFORM_WINDOWS
	pcstr const replication_folder	= SYS_DEV_HDD0 "/replication";
#else
#	error define your platform
#endif // #if XRAY_PLATFORM_WINDOWS

	XRAY_UNREFERENCED_PARAMETER				(replication_folder);

#ifndef MASTER_GOLD
	core::configs::initialize	( *s_engine, lua_config_device_folder_to_save_to );
#else // #ifndef MASTER_GOLD
	XRAY_UNREFERENCED_PARAMETER	( lua_config_device_folder_to_save_to );
#endif	// #ifndef MASTER_GOLD

	tasks::initialize		(	2 * threading::core_count(),	// tasks thread count
								64,								// user thread count
								threading::core_count(), //1,								// minimum active task thread count
								tasks::execute_while_wait_for_children_true, 
								tasks::do_logging_false
							);
	threading::set_current_thread_affinity	( 0 );
	threading::on_thread_spawn	( threading::tasks_aware );
}

void	xray::core::initialize_resources	(
							fs_new::asynchronous_device_interface &	hdd,
							fs_new::asynchronous_device_interface &	dvd,
							resources::enable_fs_watcher_bool		enable_fs_watcher
						)
{
	resources::initialize					( hdd, dvd, enable_fs_watcher );

	core_test_suite::singleton()->set_resources_path	( s_engine->get_resources_path() );

	resources::mount_mounts_path			( s_engine->get_mounts_path( ) );

	s_initialized						=	true;

	static resources::unmanaged_allocation_cook		s_unmanaged_allocation_cook;
	register_cook							( &s_unmanaged_allocation_cook );
}

void   xray::core::finalize_resources	( )
{
	resources::finalize		( );
}

void   xray::core::run_tests			( )
{
	testing::initialize			( s_engine );

	debug::notify_xbox_debugger ( "hello from core::initialize" );

	core_test_suite::singleton()->set_resources_path	( s_engine->get_resources_path() );
	core_test_suite::run_tests	( );
}

void xray::core::finalize			( )
{
	R_ASSERT				( s_initialized, "core library hasn't been initialized" );

	tasks::finalize			( );
	testing::finalize		( );

#ifndef	MASTER_GOLD
	core::configs::finalize	( );
#endif	//MASTER_GOLD

	strings::finalize		( );
//	build::finalize			( );
//	timing::finalize		( );
	debug::set_log_callback	( NULL );
	logging::finalize		( );
	threading::finalize		( );
	memory::finalize		( );

	XRAY_DESTROY_REFERENCE	( s_core_synchronous_device );

	debug::finalize			( );

	s_initialized			= false;

	if ( testing::run_tests_command_line( ) )
	{
		fixed_string512		message;
		message.assignf		("program exit code: %d", s_engine->get_exit_code());
		debug::notify_xbox_debugger	(message.c_str());
	}
}

xray::fs_new::synchronous_device_interface &	xray::core::get_core_synchronous_device	( )
{
	return									* s_core_synchronous_device.c_ptr( );
}