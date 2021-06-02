////////////////////////////////////////////////////////////////////////////
//	Created		: 18.08.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/core_debug_engine.h>
#include "build_extensions.h"
#include <xray/testing.h>

#include <xray/os_include.h>
#include <xray/core_entry_point.h>
#include <xray/fs/windows_hdd_file_system.h>
#include <xray/fs/synchronous_device_interface.h>
#include <xray/fs/device_utils.h>
#include <xray/logging/log_file.h>

#if XRAY_PLATFORM_WINDOWS || XRAY_PLATFORM_XBOX_360
#	pragma warning( push )
#	pragma warning( disable : 4074 )
#	pragma init_seg( compiler )
#	pragma warning( pop )
#elif XRAY_PLATFORM_PS3 // #if XRAY_PLATFORM_WINDOWS || XRAY_PLATFORM_XBOX_360
#else // #elseif XRAY_PLATFORM_PS3
#	error please define your platform
#endif // #if XRAY_PLATFORM_WINDOWS || XRAY_PLATFORM_XBOX_360

namespace xray {
namespace command_line	{
bool	key_is_set	( pcstr key_raw );
} // namespace command_line

namespace core {

static pcstr s_suppress_debug_window_on_crash_string	=	"suppress_debug_window_on_crash";

static xray::command_line::key	s_suppress_debug_window_on_crash (
	s_suppress_debug_window_on_crash_string,
	"", 
	"tests",
	"application will not show debug window on crash",
	"boolean"
);

bool   suppress_debug_window_on_crash ()
{
	static u32 s_out_result						=	u32(-1);
	if ( s_out_result == u32(-1) )
	{
		if ( xray::command_line::initialized() )
			s_out_result						=	s_suppress_debug_window_on_crash;
		else
			s_out_result						=	xray::command_line::key_is_set(s_suppress_debug_window_on_crash_string);
	}

	return											!!s_out_result;
}

static command_line::key	s_dont_check_multithreaded_safety("dont_check_multithreaded_safety", 
															  "",
															  "threading",
															  "turn off checks of parallel use of code that is not multithreaded");

bool initialized		();

bool	core_debug_engine::dont_check_multithreaded_safety		() const 
{ 
	return									s_dont_check_multithreaded_safety;
}

bool	core_debug_engine::terminate_on_error		() const 
{ 
	return									build::print_build_id_command_line() || 
											testing::run_tests_command_line() ||
											suppress_debug_window_on_crash(); 
}

void    core_debug_engine::on_terminate			() const
{
	logging::flush_log_file							();
	logging::log_file * const log_file	=	logging::get_log_file();
	if ( log_file )
		log_file->on_terminate				();
}

int		core_debug_engine::get_exit_code	() const
{
	return									0;
}

bool	core_debug_engine::core_initialized		() const
{
	return									core::initialized();
}

u32		core_debug_engine::build_station_build_id	() const 
{
	return									build::build_station_build_id(); 
}

void	core_debug_engine::flush_log_file			(pcstr file_name) const
{
	logging::flush_log_file							(file_name);
}

bool	core_debug_engine::is_testing				( ) const
{
	return									testing::is_testing();
}

void	core_debug_engine::on_testing_exception	( assert_enum			assert_type, 
 										  pcstr					description, 
										 _EXCEPTION_POINTERS*	exception_information,
										 bool					is_assertion) const
{
	testing::on_exception					( assert_type, description, exception_information, is_assertion );
}

pcstr	core_debug_engine::current_directory		( ) const
{
	return									fs_new::get_current_directory().c_str();
}

bool	core_debug_engine::create_folder_r			( pcstr path, bool create_last ) const
{
	using namespace							fs_new;
	
	synchronous_device_interface const & device	=	get_core_synchronous_device( );
	return									fs_new::create_folder_r(device, 
																	native_path_string::convert(path), 
																	create_last);
}

#if !XRAY_PLATFORM_PS3

void	core_debug_engine::generate_debug_file_name	(string_path &				file_name,
											 _SYSTEMTIME const * const	date_time,
											 pcstr const 				report_id,
											 pcstr const 				extension) const
{
	pcstr const application_id			=	core::application_name();
	sprintf_s					(
		file_name,
		"%s/%s%s%d%s%s%s%02d%02d%02d-%02d%02d%02d%s",
		core::user_data_directory(),
		application_id,
		*application_id ? "-#" : "#",
		build::build_station_build_id(),
		"_",
		report_id,
		*report_id ? "_" : "",
		date_time->wYear % 100,
		date_time->wMonth,
		date_time->wDay,
		date_time->wHour,
		date_time->wMinute,
		date_time->wSecond,
		extension
	);
}
#else

void	core_debug_engine::generate_debug_file_name	(string_path &				file_name,
											 _SYSTEMTIME const * const	date_time,
											 pcstr const				report_id,
											 pcstr const				extension) const
{
	XRAY_UNREFERENCED_PARAMETERS			(file_name, date_time, report_id, extension);
}

#endif // #if 0

pcstr	core_debug_engine::bugtrap_application_name () const
{
	static fixed_string512			bugtrap_application_name;
	static bool initialized		=	false;
	if ( !initialized )
	{
		bugtrap_application_name.assignf	("%s-#%d", application_name(), build::build_station_build_id() );
		initialized				=	true;
	}

	return							bugtrap_application_name.c_str();
}

struct debug_initializer
{
	debug_initializer	( )
	{
		debug::initialize	( & m_engine );
	}

	core_debug_engine	m_engine;
};

static debug_initializer	s_preinitializer XRAY_INIT_PRIORITY(0);

} // namespace core
} // namespace xray
