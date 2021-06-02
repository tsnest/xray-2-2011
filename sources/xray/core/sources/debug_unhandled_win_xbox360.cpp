////////////////////////////////////////////////////////////////////////////
//	Created 	: 07.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "debug.h"
#include "debug_platform.h"
#include "debug_bugtrap.h"
#include "debug_internal.h"
#include "logging.h"
#include "core_entry_point.h"
#include "build_extensions.h"
#include "debug_call_stack.h"
#include <xray/fs_path.h>
#include <float.h>		// for _clearfp
#include "testing_impl.h"

#define _WIN32_WINNT	0x0500
#include <xray/os_preinclude.h>
#undef NOUSER
#undef NOMB
#include <xray/os_include.h>

typedef LONG WINAPI UnhandledExceptionFilterType(struct _EXCEPTION_POINTERS *pExceptionInfo);

static xray::core::debug::error_mode			s_error_mode = xray::core::debug::error_mode_verbose;
static UnhandledExceptionFilterType*			s_previous_handler = 0;

static bool s_show_dialog_for_unhandled_exceptions	=	true;

void set_show_dialog_for_unhandled_exceptions	( bool show )
{
	s_show_dialog_for_unhandled_exceptions	=	show;
}

bool show_dialog_for_unhandled_exceptions	( )
{
	return	s_show_dialog_for_unhandled_exceptions;
}

void xray::core::debug::detail::generate_file_name					(
		string_path& file_name,
		SYSTEMTIME const& date_time,
		pcstr application_id,
		pcstr const report_id,
		pcstr const extension
	)
{
	sprintf_s					(
		file_name,
		"%s%s%s%d%s%s%s%02d%02d%02d-%02d%02d%02d%s",
		user_data_directory(),
		application_id,
		*application_id ? "-#" : "#",
		build_station_build_id(),
		"_",
		report_id,
		*report_id ? "_" : "",
		date_time.wYear % 100,
		date_time.wMonth,
		date_time.wDay,
		date_time.wHour,
		date_time.wMinute,
		date_time.wSecond,
		extension
	);
}

static void prologue											( _EXCEPTION_POINTERS* const exception_information )
{
	xray::core::debug::platform::format_message	( );

	SYSTEMTIME					date_time;
	GetLocalTime				(&date_time);

	string_path					temp;

	xray::core::debug::platform::prologue_dump_call_stack	( exception_information );
	xray::core::debug::detail::generate_file_name			( temp, date_time, xray::core::application_name(), "error_report", ".log");

	string_path					log_file_name;
	xray::strings::copy			( log_file_name, temp );

	string_path					portable_log_file_name;
	xray::strings::copy			( portable_log_file_name, log_file_name );
	xray::logging::flush		( xray::fs::convert_to_portable_in_place(portable_log_file_name) );

	xray::core::debug::bugtrap::add_file		( log_file_name );
	xray::core::debug::platform::save_minidump	( date_time, exception_information );
}

static void epilogue											( _EXCEPTION_POINTERS* const exception_information )
{
	xray::core::debug::call_stack::finalize_symbols	( );

	if ( s_previous_handler )
		s_previous_handler		( exception_information );

	xray::core::debug::platform::terminate	( "", 2 );
}

static LONG WINAPI unhandled_exception_handler_on_top_of_bugtrap( _EXCEPTION_POINTERS* const exception_information )
{
	prologue					( exception_information );

	if ( xray::debug::is_debugger_present( ) )
		DEBUG_BREAK				( );

	epilogue					( exception_information );
#if !defined(NDEBUG)
	return						( EXCEPTION_CONTINUE_SEARCH );
#endif // #if !defined(NDEBUG) || !XRAY_PLATFORM_XBOX_360
}

static LONG WINAPI unhandled_exception_handler					( _EXCEPTION_POINTERS* const exception_information )
{
	if ( !xray::core::initialized( ) )
	{
		xray::core::debug::platform::prologue_dump_call_stack	( exception_information );
		xray::debug::terminate	( "" );
	}

	if ( !xray::core::debug::bugtrap::initialized( ) )
		xray::core::debug::bugtrap::initialize	( );

	prologue					( exception_information );
	epilogue					( exception_information );

#if !defined(NDEBUG)
	return						( EXCEPTION_CONTINUE_SEARCH );
#endif // #if !defined(NDEBUG) || !XRAY_PLATFORM_XBOX_360
}

int   xray::debug::unhandled_exception_filter	( int const exception_code, _EXCEPTION_POINTERS* const exception_information )
{
#ifdef DEBUG
	_clearfp					( );	// clear floating-point exceptions flag
#endif // #ifdef DEBUG

	UnhandledExceptionFilterType* current_handler = SetUnhandledExceptionFilter( 0 );
	if ( !current_handler )
		current_handler			= &unhandled_exception_handler;

	SetUnhandledExceptionFilter	( current_handler );

	pcstr	description			= "<no description>";
	switch ( exception_code ) {
		case EXCEPTION_ACCESS_VIOLATION : {
			description			= "ACCESS_VIOLATION : The thread attempts to read from or write to a virtual address for which it does not have access.";
			break;
		}
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED : {
			description			= "ARRAY_BOUNDS_EXCEEDED : The thread attempts to access an array element that is out of bounds, \r\nand the underlying hardware supports bounds checking.";
			break;
		}
		case EXCEPTION_BREAKPOINT : {
			description			= "BREAKPOINT : A breakpoint is encountered.";
			break;
		}
		case EXCEPTION_DATATYPE_MISALIGNMENT : {
			description			= "DATATYPE_MISALIGNMENT : The thread attempts to read or write data that is misaligned on hardware that does not provide alignment. For example, 16-bit values must be aligned on 2-byte boundaries, 32-bit values on 4-byte boundaries, and so on.";
			break;
		}
		case EXCEPTION_FLT_DENORMAL_OPERAND : {
			description			= "FLT_DENORMAL_OPERAND : One of the operands in a floating point operation is denormal. A denormal value is one that is too small to represent as a standard floating point value.";
			break;
		}
		case EXCEPTION_FLT_DIVIDE_BY_ZERO : {
			description			= "FLT_DIVIDE_BY_ZERO : The thread attempts to divide a floating point value by a floating point divisor of 0 (zero).";
			break;
		}
		case EXCEPTION_FLT_INEXACT_RESULT : {
			description			= "FLT_INEXACT_RESULT : The result of a floating point operation cannot be represented exactly as a decimal fraction.";
			break;
		}
		case EXCEPTION_FLT_INVALID_OPERATION : {
#if 0
#				define PREFIX	"FLT_INVALID_OPERATION : "
			switch ( exception_information->ContextRecord->ExtendedRegisters[2] ) {
				case _FPE_INVALID : {
					description		= PREFIX "Invalid instruction (SNaN, probably, uninitialized variable)";
					break;
				}
				case _FPE_DENORMAL : {
					description		= PREFIX "Denormal occured";
					break;
				}
				case _FPE_ZERODIVIDE : {
					description		= PREFIX "The thread attempts to divide a floating point value by a floating point divisor of 0 (zero).";
					break;
				}
				case _FPE_OVERFLOW : {
					description		= PREFIX "The exponent of a floating point operation is greater than the magnitude allowed by the corresponding type.";
					break;
				}
				case _FPE_UNDERFLOW : {
					description		= PREFIX "The exponent of a floating point operation is less than the magnitude allowed by the corresponding type.";
					break;
				}
				case _FPE_INEXACT : {
					description		= PREFIX "The result of a floating point operation cannot be represented exactly as a decimal fraction.";
					break;
				}
				case _FPE_UNEMULATED : {
					description		= PREFIX "Unemulated instruction occured";
					break;
				}
				case _FPE_SQRTNEG : {
					description		= PREFIX "An attempt to take a square root from the negative floating point number";
					break;
				}
				case _FPE_STACKOVERFLOW : {
					description		= PREFIX "The stack has overflowed, because of a floating point operation.";
					break;
				}
				case _FPE_STACKUNDERFLOW : {
					description		= PREFIX "The stack has underflowed, because of a floating point operation.";
					break;
				}
				case _FPE_EXPLICITGEN : {
					description		= PREFIX "Someone raised signal SIGFPE or a corresponding exception";
					break;
				}
				default : {
					description		= PREFIX "An unknown floating point exception.";
					break;
				}
			}
#				undef PREFIX
#else // #if 0
			description			= "FLT_INVALID_OPERATION : An unknown floating point exception.";
#endif // #if 0
			break;
		}
		case EXCEPTION_FLT_OVERFLOW : {
			description			= "FLT_OVERFLOW : The exponent of a floating point operation is greater than the magnitude allowed by the corresponding type.";
			break;
		}
		case EXCEPTION_FLT_STACK_CHECK : {
			description			= "FLT_STACK_CHECK : The stack has overflowed or underflowed, because of a floating point operation.";
			break;
		}
		case EXCEPTION_FLT_UNDERFLOW : {
			description			= "FLT_UNDERFLOW : The exponent of a floating point operation is less than the magnitude allowed by the corresponding type.";
			break;
		}
		case EXCEPTION_ILLEGAL_INSTRUCTION : {
			description			= "ILLEGAL_INSTRUCTION : The thread tries to execute an invalid instruction.";
			break;
		}
		case EXCEPTION_INT_DIVIDE_BY_ZERO : {
			description			= "INT_DIVIDE_BY_ZERO : The thread tries to access a page that is not present, and the system is unable to load the page. For example, this exception might occur if a network connection is lost while running a program over a network.";
			break;
		}
		case EXCEPTION_INT_OVERFLOW : {
			description			= "INT_OVERFLOW : The result of an integer operation causes a carry out of the most significant bit of the result.";
			break;
		}
		case EXCEPTION_INVALID_DISPOSITION : {
			description			= "INVALID_DISPOSITION : An exception handler returns an invalid disposition to the exception dispatcher. Programmers using a high-level language such as C should never encounter this exception.";
			break;
		}
		case EXCEPTION_NONCONTINUABLE_EXCEPTION : {
			description			= "NONCONTINUABLE_EXCEPTION : The thread attempts to continue execution after a non-continuable exception occurs.";
			break;
		}
		case EXCEPTION_PRIV_INSTRUCTION : {
			description			= "PRIV_INSTRUCTION : The thread attempts to execute an instruction with an operation that is not allowed in the current computer mode.";
			break;
		}
		case EXCEPTION_SINGLE_STEP : {
			description			= "SINGLE_STEP : A trace trap or other single instruction mechanism signals that one instruction is executed.";
			break;
		}
		case EXCEPTION_STACK_OVERFLOW : {
			description			= "STACK_OVERFLOW : The thread uses up its stack.";
			break;
		}
	}

	// here we catch Structured Exception in case
	// when raising exceptions is turned off in debugger
 	if ( xray::debug::is_debugger_present() )
 		DEBUG_BREAK				( );

	if ( xray::testing::is_testing() ) {
		xray::testing::on_exception	(xray::assert_untyped, description, exception_information, false);
		return					EXCEPTION_EXECUTE_HANDLER;
	}

	LOGI_ERROR					( "debug", "" );
	LOGI_ERROR					( "debug", description );
	LOGI_ERROR					( "debug", "" );

	if ( show_dialog_for_unhandled_exceptions( ) )
	{
		char						buffer[8192];
		strings::copy				(buffer, description);
		strings::append				(buffer, "\r\n\r\n");
		xray::core::debug::platform::on_error	(NULL, buffer, NULL, exception_information, xray::core::debug::platform::error_type_unhandled_exception);
	}

	if ( xray::testing::run_tests_command_line() || 
		 xray::build::print_build_id_command_line() )
 		xray::debug::terminate	( "" );
 	else
		current_handler			( exception_information );

	return						( EXCEPTION_CONTINUE_SEARCH );
}

void xray::debug::protected_call							( protected_function_type* function_to_call, pvoid argument )
{
	core::debug::set_thread_stack_guarantee	( );

	__try {
		( *function_to_call	)	( argument );
	}
	__except ( unhandled_exception_filter ( GetExceptionCode( ), GetExceptionInformation( ) ) ) {
	}
}

void xray::core::debug::platform::setup_unhandled_exception_handler	( error_mode const error_mode )
{
	s_error_mode				= error_mode;
	s_previous_handler			= SetUnhandledExceptionFilter( &unhandled_exception_handler );
}

void xray::core::debug::platform::change_unhandled_exception_handler ( error_mode const error_mode )
{
	s_error_mode				= error_mode;
}

void xray::core::debug::bugtrap::setup_unhandled_exception_handler	( )
{
	s_previous_handler			= SetUnhandledExceptionFilter( &unhandled_exception_handler_on_top_of_bugtrap );
}