////////////////////////////////////////////////////////////////////////////
//	Created 	: 07.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <float.h>		// for _clearfp

#if XRAY_PLATFORM_XBOX_360
#	include <xtl.h>
#else 
#	include <windows.h>
#endif

#include <xray/debug/call_stack.h>
#include <xray/debug/extensions.h>
#include "platform.h"
#include "bugtrap.h"
#include "utils.h"

typedef LONG WINAPI UnhandledExceptionFilterType(struct _EXCEPTION_POINTERS *pExceptionInfo);

static xray::debug::error_mode			s_error_mode = xray::debug::error_mode_verbose;
static UnhandledExceptionFilterType*			s_previous_handler = 0;

static bool s_show_dialog_for_unhandled_exceptions	=	true;

void set_show_dialog_for_unhandled_exceptions	( bool show )
{
	s_show_dialog_for_unhandled_exceptions	=	show;
}

bool show_dialog_for_unhandled_exceptions	( )
{
	return	s_show_dialog_for_unhandled_exceptions && 
			(xray::debug::debug_engine() && !xray::debug::debug_engine()->terminate_on_error());
}

static void prologue											( _EXCEPTION_POINTERS* const exception_information )
{
	using namespace xray::debug;
	platform::format_message				( );

	SYSTEMTIME								date_time;
	GetLocalTime							(&date_time);

	string_path								log_file_name;

	platform::prologue_dump_call_stack	( exception_information );
	debug_engine()->generate_debug_file_name	( log_file_name, & date_time, "error_report", ".log");

	debug_engine()->flush_log_file			( log_file_name );

	bugtrap::add_file						( log_file_name );
	platform::save_minidump					( date_time, exception_information );
}

static void epilogue											( _EXCEPTION_POINTERS* const exception_information )
{
	xray::debug::call_stack::finalize_symbols	( );

	if ( s_previous_handler )
		s_previous_handler		( exception_information );

	xray::debug::platform::terminate	( "", 2 );
}

static LONG WINAPI unhandled_exception_handler_on_top_of_bugtrap( _EXCEPTION_POINTERS* const exception_information )
{
	prologue					( exception_information );

	if ( xray::debug::is_debugger_present( ) )
		DEBUG_BREAK				( );

	epilogue					( exception_information );
#if !defined(NDEBUG)
	return						( EXCEPTION_CONTINUE_SEARCH );
#else
#pragma message (XRAY_TODO("Lain 2 Dima: what return here?"))
	return						( EXCEPTION_CONTINUE_SEARCH );
#endif // #if !defined(NDEBUG) || !XRAY_PLATFORM_XBOX_360
}

static LONG WINAPI unhandled_exception_handler					( _EXCEPTION_POINTERS* const exception_information )
{
// seems this check is obsolete, but for a case its left commented
// 	if ( !xray::debug::debug_engine() || !xray::debug::debug_engine()->core_initialized() )
// 	{
// 		xray::debug::platform::prologue_dump_call_stack	( exception_information );
// 		xray::debug::terminate	( "" );
// 	}

	if ( !xray::debug::bugtrap::initialized( ) )
		xray::debug::bugtrap::initialize	( );

	prologue					( exception_information );
	epilogue					( exception_information );

#if !defined(NDEBUG)
	return						( EXCEPTION_CONTINUE_SEARCH );
#else
#pragma message (XRAY_TODO("Lain 2 Dima: what return here?"))
	return						( EXCEPTION_CONTINUE_SEARCH );
#endif // #if !defined(NDEBUG) || !XRAY_PLATFORM_XBOX_360
}

int   xray::debug::unhandled_exception_filter	( int const exception_code, _EXCEPTION_POINTERS* const exception_information )
{
#ifdef DEBUG
	_clearfp					( );	// clear floating-point exceptions flag
#endif // #ifdef DEBUG

	UnhandledExceptionFilterType* fake_handler = 0;
	XRAY_ANALYSIS_ASSUME		( fake_handler != 0 );
	UnhandledExceptionFilterType* current_handler = SetUnhandledExceptionFilter( fake_handler );
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
					description		= PREFIX "The exponent of a floating point operation is greater than the length allowed by the corresponding type.";
					break;
				}
				case _FPE_UNDERFLOW : {
					description		= PREFIX "The exponent of a floating point operation is less than the length allowed by the corresponding type.";
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
			description			= "FLT_OVERFLOW : The exponent of a floating point operation is greater than the length allowed by the corresponding type.";
			break;
		}
		case EXCEPTION_FLT_STACK_CHECK : {
			description			= "FLT_STACK_CHECK : The stack has overflowed or underflowed, because of a floating point operation.";
			break;
		}
		case EXCEPTION_FLT_UNDERFLOW : {
			description			= "FLT_UNDERFLOW : The exponent of a floating point operation is less than the length allowed by the corresponding type.";
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

	if ( debug_engine() && debug_engine()->is_testing() ) 
	{
		debug_engine()->on_testing_exception(xray::assert_untyped, description, exception_information, false);
		return					EXCEPTION_EXECUTE_HANDLER;
	}

	log_callback const log_callback	=	xray::debug::get_log_callback();
	if ( log_callback )
	{	
		log_callback			( "debug", true, false, "" );
		log_callback			( "debug", true, false, description );
		log_callback			( "debug", true, false, "" );
	}

	if ( show_dialog_for_unhandled_exceptions( ) )
	{
		char						buffer[8192];
		strcpy_s					(buffer, description);
		strcat_s					(buffer, "\r\n\r\n");
		xray::debug::platform::on_error	(NULL, buffer, NULL, exception_information, xray::debug::platform::error_type_unhandled_exception);
	}

	if ( debug_engine() && debug_engine()->terminate_on_error() )
	{
		xray::debug::dump_call_stack	("", true, 0, 4, exception_information );
 		xray::debug::terminate	( "" );
	}
 	else
		current_handler			( exception_information );

	return						( EXCEPTION_CONTINUE_SEARCH );
}

void xray::debug::protected_call							( protected_function_type* function_to_call, pvoid argument )
{
	debug::set_thread_stack_guarantee	( );

	__try {
		( *function_to_call	)	( argument );
	}
	__except ( unhandled_exception_filter ( GetExceptionCode( ), GetExceptionInformation( ) ) ) {
		(void)0;
	}
}

void xray::debug::platform::setup_unhandled_exception_handler	( error_mode const error_mode )
{
	s_error_mode				= error_mode;
	s_previous_handler			= SetUnhandledExceptionFilter( &unhandled_exception_handler );
}

void xray::debug::platform::change_unhandled_exception_handler ( error_mode const error_mode )
{
	s_error_mode				= error_mode;
}

void xray::debug::bugtrap::setup_unhandled_exception_handler	( )
{
	s_previous_handler			= SetUnhandledExceptionFilter( &unhandled_exception_handler_on_top_of_bugtrap );
}