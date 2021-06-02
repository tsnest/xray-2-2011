////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <stdarg.h>
#include <string.h>
#include <xray/exit_codes.h>
#include <xray/debug/extensions.h>
#include <xray/debug/call_stack.h>
#include <xray/stdlib_extensions.h>
#include <xray/linkage_helper.h>
#include "platform.h"
#include "bugtrap.h"
#include "utils.h"
#include "call_stack.h"

XRAY_INCLUDE_TO_LINKAGE(debug_initialize)

static u32								s_process_lock_count		=	0;
static xray::debug::atomic32			s_process_lock_thread_id	=	0;

xray::debug::engine *					s_debug_engine	=	NULL;

static xray::debug::log_callback		s_log_callback		=	NULL;
static xray::debug::atomic32			s_log_disable_counter	=	0;

xray::debug::engine *   xray::debug::debug_engine	( )
{
	return									s_debug_engine;
}

xray::debug::log_callback	xray::debug::get_log_callback	( )
{
	return									s_log_disable_counter ? NULL : s_log_callback;
}

xray::debug::log_callback	xray::debug::set_log_callback	( log_callback const & callback )
{
	log_callback const previous_callback	=	s_log_callback;
	s_log_callback						=	callback;
	return									previous_callback;
}

void	xray::debug::disable_log_callback	( )
{
	interlocked_increment					(s_log_disable_counter);
}

void	xray::debug::enable_log_callback	( )
{
	interlocked_decrement					(s_log_disable_counter);
}

#if !defined(XRAY_STATIC_LIBRARIES) || defined(MASTER_GOLD)
void boost::throw_exception			( std::exception const& exception )
{
	XRAY_UNREFERENCED_PARAMETERS	( &exception );
	FATAL							( "boost::throw_exception: %s", exception.what() );
}
#endif // #ifndef XRAY_STATIC_LIBRARIES

void xray::debug::initialize		( debug::engine * engine )
{
	s_debug_engine					=	engine;
}

void xray::debug::finalize			( )
{
	bugtrap::finalize				( );
}

void xray::debug::postinitialize	( )
{
	bugtrap::initialize				( );
}

void xray::debug::log_call_stack		( pcstr header )
{
	log_callback const log_callback		=	get_log_callback();
	if ( log_callback )
		log_callback				( "debug", false, false, header );
	dump_call_stack					( "debug", false, 1, 0, 0 );
}

static void on_error					( xray::process_error_enum process_error, bool* do_debug_break_parameter, pstr const message, u32 const message_size, bool* ignore_always_parameter )
{
	using namespace xray::debug;
	if ( debug_engine() )
		debug_engine()->flush_log_file	( NULL );

	if ( debug_engine() && debug_engine()->terminate_on_error() && !xray::debug::is_debugger_present() )
		terminate						( "" );

	if ( process_error == xray::process_error_true )
		platform::on_error				(  do_debug_break_parameter, message, message_size, ignore_always_parameter, NULL, xray::debug::platform::error_type_assert);
	else if ( process_error == xray::process_error_to_message_box )
		platform::on_error_message_box	(  do_debug_break_parameter, message, message_size, ignore_always_parameter, NULL, xray::debug::platform::error_type_assert);
	else
		UNREACHABLE_CODE				( );
}

static void output						( char * const message, u32 const max_message_length, pcstr const format, ... )
{
	va_list						argptr;
	va_start					( argptr, format );

	u32 const old_length	=	(u32)strlen(message);

	xray::vsprintf				(message + strlen(message), max_message_length - old_length, format, argptr);

	if ( !xray::debug::debug_engine() || !xray::debug::debug_engine()->is_testing() ) 
	{
		xray::debug::log_callback const log_callback	=	xray::debug::get_log_callback();
		if ( log_callback )
			log_callback		("debug", true, false, message + old_length);
	}

	xray::debug::strings_concat	(message, max_message_length, "\r\n");

	va_end	 					(argptr);
}

static void process						( bool* do_debug_break, xray::process_error_enum process_error, bool* ignore_always, xray::assert_enum assert_type, pcstr reason, pcstr expression, pcstr description, pcstr file, pcstr function, u32 line )
{
	using namespace	xray::debug;

	if ( s_process_lock_thread_id == (long)xray::debug::current_thread_id() )
	{
		++s_process_lock_count;
	}
	else 
	{
		while ( interlocked_compare_exchange(s_process_lock_thread_id, current_thread_id(), 0) != 0 )
			yield							(10);
		s_process_lock_count			=	0;
	}

	u32 size_for_call_stack					= 0;
	if ( (!debug_engine() || !debug_engine()->is_testing()) && !is_debugger_present() ) {
		dump_call_stack						( "debug", true, 2, 0, 0 );
		call_stack_size_calculator			helper;
		xray::debug::call_stack::iterate	( NULL, NULL, boost::bind( & call_stack_size_calculator::predicate, & helper, _1, _2, _3, _4, _5, _6, _7), false );
		size_for_call_stack					= helper.needed_size();
	}


	u32 const max_message_length		=	size_for_call_stack + 4096;
	char * message						=	(char*)ALLOCA(max_message_length);
	message[0]							=	0;

	output									( message, max_message_length, "" );
	output									( message, max_message_length, "Error occurred : %s", reason );
	output									( message, max_message_length, "Expression    : %s", expression );

	if (description)
		output								( message, max_message_length, "Description   : %s", description );

	output									( message, max_message_length, "File          : %s", file );
	output									( message, max_message_length, "Line          : %d", line );
	output									( message, max_message_length, "Function      : %s", function );

	if ( debug_engine()->is_testing() ) 
	{
		debug_engine()->on_testing_exception	(assert_type, message, 0, true);
	}
	else {
		if ( process_error != xray::process_error_false )
		{
			if ( process_error == xray::process_error_to_message_box )
				strings_copy				( message, max_message_length, description );

			on_error						( process_error, do_debug_break, message, max_message_length, ignore_always );
		}
	}
	
	--s_process_lock_count;
	if ( !s_process_lock_count )
		interlocked_exchange				(s_process_lock_thread_id, 0);
}

void xray::debug::on_error		( bool* do_debug_break, xray::process_error_enum process_error, bool* ignore_always, xray::assert_enum assert_type, pcstr reason, pcstr expression, pcstr file, pcstr function, u32 line, pcstr format, ... )
{
	va_list					mark;
	va_start				( mark, format );

	string4096				description;
	vsnprintf				( description, sizeof(description), sizeof(description) - 1, format, mark );

	process					( do_debug_break, process_error, ignore_always, assert_type, reason, expression, description, file, function, line );
}

void xray::debug::on_error		( bool* do_debug_break, xray::process_error_enum process_error, bool* ignore_always, xray::assert_enum assert_type, pcstr reason, pcstr expression, pcstr file, pcstr function, u32 line )
{
	process					( do_debug_break, process_error, ignore_always, assert_type, reason, expression, 0, file, function, line );
}

void xray::debug::on_error		( bool* do_debug_break, xray::process_error_enum process_error, bool* ignore_always, int error_code, pcstr expression, pcstr file, pcstr function, u32 line, pcstr format, ... )
{
	va_list					mark;
	va_start				( mark, format );

	string4096				description;
	vsnprintf				( description, sizeof(description), sizeof(description) - 1, format, mark );

	pcstr const graphics	= debug::platform::get_graphics_api_error_description( error_code );
	if ( graphics ) {
		process				( do_debug_break, process_error, ignore_always, xray::assert_untyped, graphics, expression, description, file, function, line );
		return;
	}

	pstr const reason		= debug::platform::fill_format_message ( error_code );
	process					( do_debug_break, process_error, ignore_always, xray::assert_untyped, reason, expression, description, file, function, line );
	debug::platform::free_format_message	( reason );
}

void xray::debug::on_error		( bool* do_debug_break, xray::process_error_enum process_error, bool* ignore_always, int error_code, pcstr expression, pcstr file, pcstr function, u32 line )
{
	pstr const reason		= debug::platform::fill_format_message ( error_code );
	process					( do_debug_break, process_error, ignore_always, xray::assert_untyped, reason, expression, 0, file, function, line );
	debug::platform::free_format_message	( reason );
}

#define EXCEPTION_EXECUTE_HANDLER       1

static void terminate_impl	( u32 exit_code, pcstr message )
{
	using namespace			xray;

#if XRAY_PLATFORM_WINDOWS || XRAY_PLATFORM_XBOX_360
	__try {
#endif // #if XRAY_PLATFORM_WINDOWS || XRAY_PLATFORM_XBOX_360
	
		debug::debug_engine()->on_terminate	( );

#if XRAY_PLATFORM_WINDOWS || XRAY_PLATFORM_XBOX_360
	} __except (EXCEPTION_EXECUTE_HANDLER) {}
#endif // #if XRAY_PLATFORM_WINDOWS || XRAY_PLATFORM_XBOX_360

	debug::platform::terminate		( message, exit_code );
}

void xray::debug::terminate	( pcstr format, ... )
{
	va_list								args;
	va_start							( args, format );
	string4096							message;
	vsprintf							( message, sizeof(message), format, args );

	u32 const exit_code				=	debug_engine() ? debug_engine()->get_exit_code() : 1;
	terminate_impl						( exit_code + exit_code_called_by_terminate, message );
}

void xray::debug::terminate	( u32 exit_code, pcstr format, ... )
{
	va_list						args;
	va_start					( args, format );
	string4096					message;
	vsprintf					( message, sizeof(message), format, args );
	terminate_impl				( exit_code + exit_code_called_by_terminate, message );
}

void xray::debug::printf	( pcstr format, ... )
{
	va_list	 					mark;
	va_start 					( mark, format );
	string4096					message_buffer;
	vsnprintf					( message_buffer, sizeof( message_buffer ) - 1, format, mark );
	debug::output				( message_buffer );
	va_end	 					( mark );
}
