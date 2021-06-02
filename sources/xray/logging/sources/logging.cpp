////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.08.2008
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/testing.h>
#include <xray/fs/file_type_pointer.h>
#include <xray/logging/api.h>
#include <xray/logging/log_file.h>
#include <xray/command_line_extensions.h>
#include <xray/core_entry_point.h>
#include "logging.h"
#include "filter_tree_node.h"
#include "globals.h"
#include <xray/debug/debug.h>

#if XRAY_PLATFORM_WINDOWS || XRAY_PLATFORM_XBOX_360
#	pragma warning( push )
#	pragma warning( disable : 4073 )
#	pragma init_seg( lib )
#	pragma warning( pop )
#elif XRAY_PLATFORM_PS3 // #if XRAY_PLATFORM_WINDOWS || XRAY_PLATFORM_XBOX_360
#else // #elseif XRAY_PLATFORM_PS3
#	error please define your platform
#endif // #if XRAY_PLATFORM_WINDOWS || XRAY_PLATFORM_XBOX_360

namespace xray {
namespace logging {

static command_line::key	s_use_console		("console", "", "logging", "turns on console output");
static command_line::key	s_log_to_stdout		("log_to_stdout", "", "logging", "turns on writing to stdout");

static bool					s_initialized_logging	=	false;

void initialize_filters					( );
void finalize_filters					( );
void preinitialize_globals				( );
void finalize_globals					( );

void   generate_log_file_name			(fs_new::native_path_string * out_result, pcstr extension)
{
	ASSERT									(extension);
	ASSERT									(out_result);
	* out_result						= 	fs_new::native_path_string::convert(core::user_data_directory());
	out_result->append_path					(core::application_name());
	fs_new::native_path_string user_name	=	core::user_name();
	if ( user_name.length() )
		out_result->appendf					("_%s", user_name.c_str());

	out_result->appendf						(".%s", extension);
}

void preinitialize						( )
{
	static bool first_time				=	true;
	if ( first_time )
	{
		preinitialize_globals				( );
		initialize_filters					( );
		first_time						=	false;
	}
}

struct logging_preinitializer	{
	logging_preinitializer	( )	{	preinitialize( );	}
};

static logging_preinitializer	s_preinitializer XRAY_INIT_PRIORITY(1);


void initialize							( fs_new::device_file_system_proxy	device, 
										  log_file_usage					log_file_usage )
{
	globals->log_file_usage				=	log_file_usage;
	ASSERT									( !globals->log_file.initialized() );
	if ( log_file_usage != logging::no_log )
	{
		XRAY_CONSTRUCT_REFERENCE			( globals->synchronous_device, fs_new::synchronous_device_interface ) ( device );
		fs_new::native_path_string			file_name;
		generate_log_file_name				( & file_name, "log" );
		XRAY_CONSTRUCT_REFERENCE			( globals->log_file, log_file ) ( globals->log_file_usage, file_name.c_str(), device );
	}

	s_initialized_logging				=	true;
}

void finalize							( )
{
	finalize_filters							( );

	if ( globals->log_file.initialized( )  )
		XRAY_DESTROY_REFERENCE				( globals->log_file );
	if ( globals->synchronous_device.initialized( ) )
		XRAY_DESTROY_REFERENCE				( globals->synchronous_device );

	finalize_console						( );
	finalize_globals						( );
}

verbosity string_to_verbosity			( pcstr in_verbosity )
{
	logging::verbosity	verbosities[]	=	{	silent, error, warning,	info, debug, trace };
	for ( u32 i=0; i<array_size(verbosities); ++i )
		if ( strings::equal(verbosity_to_string(verbosities[i]), in_verbosity) )
			return							verbosities[i];
	
	return									unset;
}

pcstr verbosity_to_string				( int const verbosity )
{
	switch ( verbosity )
	{
		case silent	 :	return				( "silent" );
		case info    :	return				( "info" );
		case warning :	return				( "Warning" );
		case error   :	return				( "ERROR" );
		case debug	 :	return				( "debug" );
		case trace   :	return				( "trace" );
		default		 :	NODEFAULT			( return 0 );
	}
}

bool use_console_for_logging ( )
{
	if ( !s_initialized_logging )
		return								false;

	static bool s_use_console_for_logging	=	testing::run_tests_command_line() || s_use_console;
	return									s_use_console_for_logging;
}

void log_thread_unsafe					( logging::log_flags_enum const log_flags, pcstr message, u32 const string_length, logging::verbosity verbosity )
{
	if ( globals->log_file.initialized() && globals->log_file->initialized() )
		globals->log_file->append		( message, string_length );

	bool const log_to_console_settings	=	(log_flags & logging::log_to_console) != 0;
	bool const log_to_stderr			=	(log_flags & logging::log_to_stderr) != 0;
	bool const use_console_for_testing	=	use_console_for_logging() && verbosity <= warning;

	bool logged_to_stdout				=	false;

	if ( log_to_console_settings || use_console_for_testing )
	{
		static bool s_initialized_console			=	false;
		static bool s_tried_to_initialize_console	=	false;
		if ( !s_tried_to_initialize_console )
		{
			s_initialized_console		=	initialize_console		();
			s_tried_to_initialize_console	=	true;
		}

		if ( s_initialized_console )
		{
			write_to_stdstream				( stdstream_out, "%s", message );
			logged_to_stdout			=	true;
		}
	}

	if ( s_initialized_logging && log_to_stderr )
	{
		initialize_stdstreams_if_needed		( );
		write_to_stdstream					( stdstream_error, "%s", message );		
	}

	if ( s_initialized_logging && s_log_to_stdout && !logged_to_stdout )
	{
		initialize_stdstreams_if_needed		( );
		write_to_stdstream					( stdstream_out, "%s", message );
		logged_to_stdout				=	true;
	}
}

void flush_log_file						( pcstr file_name )
{
	if ( !globals->log_file.initialized() )
		return;

	threading::mutex_raii raii				( globals->log_mutex );

	if ( globals->log_file->initialized() )
		globals->log_file->flush			( file_name );
}

log_file * get_log_file					( )
{
	if ( !globals->log_file.initialized( ) )
		return								NULL;

	return									( globals->log_file.c_ptr( ) );
}

void   write_exit_code_file				( int exit_code )
{
	if ( !get_log_file() )
		return;

	fs_new::native_path_string				file_name;
	generate_log_file_name					(& file_name, "exit_code");

	fs_new::file_type_pointer				file( file_name, * globals->synchronous_device.c_ptr(), 
												  fs_new::file_mode::create_always, fs_new::file_access::write);
	if ( !file )
		return;

	fprintf									((FILE *)file_type_to_handle(file), "%d", exit_code);
}

void   set_format						( format_specifier const & format_expression )
{
	threading::mutex_raii	raii			(globals->log_mutex);
	globals->format.set						(format_expression);
}

} // namespace logging
} // namespace xray