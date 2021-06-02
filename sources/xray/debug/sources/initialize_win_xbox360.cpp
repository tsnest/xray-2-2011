////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <signal.h>						// for signals
#include <float.h>						// for floating point parameters
#include <new.h>						// for __set_new_mode, _set_new_handler
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <xray/debug/extensions.h>
#include <xray/debug/pointer_cast.h>
#include "platform.h"
#include "bugtrap.h"
#include "float.h"
#include "utils.h"

#if XRAY_PLATFORM_WINDOWS
#	include <xmmintrin.h>
#endif // #if XRAY_PLATFORM_WINDOWS

namespace xray {

namespace command_line {
	bool key_is_set( pcstr key_raw );
} // namespace command_line

} // namespace xray

static void handler_base				(pcstr reason_string)
{
	XRAY_UNREFERENCED_PARAMETER		( reason_string );
	FATAL							( "error handler is invoked: %s", reason_string );
}

static void invalid_parameter_handler	(const wchar_t *expression, const wchar_t *function, const wchar_t *file, unsigned int line, uintptr_t)
{
	XRAY_UNREFERENCED_PARAMETERS	( expression, function, file, line );
#ifdef DEBUG
	wchar_t							temp[4096];
	swprintf_s						( temp, L"invalid parameter passed (%s in %s (%s, %d))", expression, function, file, line );
	handler_base					( xray::pointer_cast<pstr>(temp) );
#else // #ifdef _DEBUG
	handler_base					( "invalid parameter passed" );
#endif // #ifdef _DEBUG
}

static int out_of_memory_handler		(size_t size)
{
	XRAY_UNREFERENCED_PARAMETER		( size );
	handler_base					( "out of memory" );
	return							(0);
}

static void pure_call_handler			()
{
	handler_base					( "pure virtual function call" );
}

static void abort_handler				(int signal)
{
	XRAY_UNREFERENCED_PARAMETER		( signal );
	handler_base					( "application is aborting" );
}

static void floating_point_handler		(int signal, int error_code)
{
	XRAY_UNREFERENCED_PARAMETER		( signal );

	pcstr	description				= "";
	switch ( error_code ) {
		case _FPE_INVALID : {
			description				= "invalid instruction (SNaN, probably, uninitialized variable)";
			break;
		}
		case _FPE_DENORMAL : {
			description				= "denormal occured";
			break;
		}
		case _FPE_ZERODIVIDE : {
			description				= "division by zero";
			break;
		}
		case _FPE_OVERFLOW : {
			description				= "overflow";
			break;
		}
		case _FPE_UNDERFLOW : {
			description				= "underflow";
			break;
		}
		case _FPE_INEXACT : {
			description				= "inexact result";
			break;
		}
		case _FPE_UNEMULATED : {
			description				= "_FPE_UNEMULATED";
			break;
		}
		case _FPE_SQRTNEG : {
			description				= "negative value passed to sqrt";
			break;
		}
		case _FPE_STACKOVERFLOW : {
			description				= "stack overflow";
			break;
		}
		case _FPE_STACKUNDERFLOW : {
			description				= "stack underflow";
			break;
		}
		case _FPE_EXPLICITGEN : {
			description				= "someone raised signal SIGFPE";
			break;
		}
	}

	string256						error;
	sprintf_s						( error, "floating point error ( %s ) ", description );
	handler_base					( error );
}

static void illegal_instruction_handler	(int signal)
{
	XRAY_UNREFERENCED_PARAMETER		( signal );
	handler_base					( "illegal instruction" );
}

static void termination_handler			(int signal)
{
	XRAY_UNREFERENCED_PARAMETER		( signal );
	handler_base					( "termination with exit code 3" );
}

static void storage_access_handler		(int signal)
{
	XRAY_UNREFERENCED_PARAMETER		( signal );
	handler_base					("illegal storage access");
}

void xray::debug::change_bugtrap_usage	( debug::error_mode error_mode, debug::bugtrap_usage bugtrap_usage )
{
	debug::bugtrap::change_usage					( error_mode, bugtrap_usage );
	debug::platform::change_storage_access_handler	( error_mode, storage_access_handler );
	debug::postinitialize							( );
}

#ifdef DEBUG
static bool s_fpe_enabled			= false;
#endif // #ifdef DEBUG

static bool s_debug_preinitialized	=	false;

namespace xray {
namespace debug {

void preinitialize ()
{
	if ( s_debug_preinitialized )
		return;

	s_debug_preinitialized			= true;

	error_mode error_mode			= error_mode_verbose;
	bugtrap_usage bugtrap_usage		= native_bugtrap;
	//	s_bugtrap_usage				= bugtrap_usage;
	bugtrap::change_usage			( error_mode, bugtrap_usage );

	platform::setup_storage_access_handler	( error_mode, storage_access_handler );

	signal							( SIGABRT,			abort_handler );
	signal							( SIGABRT_COMPAT,	abort_handler );
	signal							( SIGFPE,			( void ( __cdecl * ) ( int ) )( &floating_point_handler ) );
	signal							( SIGILL,			illegal_instruction_handler );
	signal							( SIGINT,			0 );
	signal							( SIGTERM,			termination_handler );

	_set_abort_behavior				( 0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT );

	_set_invalid_parameter_handler	( &invalid_parameter_handler );

	_set_new_mode					( 1 );
	_set_new_handler				( &out_of_memory_handler );

	_set_purecall_handler			( &pure_call_handler );
}

} // namespace debug
} // namespace xray

#ifdef DEBUG
bool xray::debug::is_fpe_enabled	( )
{
	return							s_fpe_enabled;
}

static void enable_fpe				( )
{
	_clearfp						( );

	u32								flags;
	errno_t							error;
	error							= _controlfp_s( &flags, 0, _MCW_EM );
	R_ASSERT						( !error );

	flags							&= ~( /**_EM_INEXACT | _EM_UNDERFLOW|/**/ _EM_INVALID | _EM_ZERODIVIDE | _EM_DENORMAL | _EM_OVERFLOW );

	error							= _controlfp_s( 0, flags, _MCW_EM );
	R_ASSERT						( !error );

#if XRAY_PLATFORM_WINDOWS
	_MM_SET_EXCEPTION_MASK			( _MM_MASK_UNDERFLOW | _MM_MASK_INEXACT | _MM_MASK_DENORM );
#endif
}

static void disable_fpe				( )
{
	_clearfp						( );

	u32								flags;
	errno_t							error;
	error							= _controlfp_s( &flags, 0, _MCW_EM );
	R_ASSERT_U						( !error );

	flags							|= _EM_INEXACT | _EM_UNDERFLOW | _EM_INVALID | _EM_ZERODIVIDE | _EM_DENORMAL | _EM_OVERFLOW;

	error							= _controlfp_s( 0, flags, _MCW_EM );
	R_ASSERT_U						( !error );
	
#if XRAY_PLATFORM_WINDOWS
	_MM_SET_EXCEPTION_MASK			( 0 );
#endif
}

void xray::debug::enable_fpe		( bool value )
{
	if ( s_fpe_enabled == value )
		return;

	s_fpe_enabled					= value;

	if ( s_fpe_enabled )
		::enable_fpe				( );
	else
		::disable_fpe				( );
}
#endif // #ifdef DEBUG
