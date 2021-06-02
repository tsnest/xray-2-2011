////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <signal.h>						// for signals
#include <float.h>						// for floating point parameters
#include <new.h>						// for __set_new_mode, _set_new_handler
#include <xray/core/core.h>
#include "debug.h"
#include "debug_platform.h"
#include "debug_bugtrap.h"
#include "debug_float.h"

namespace xray {
namespace core {
namespace debug {
	extern xray::command_line::key	g_fpe_disabled;
} // namespace debug 
} // namespace core
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
	string4096						temp;
	sprintf_s						( temp, "invalid parameter passed (%s in %s (%s, %d))", expression, function, file, line );
	handler_base					( temp );
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
	xray::sprintf					( error, "floating point error ( %s ) ", description );
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

void xray::debug::change_bugtrap_usage	( core::debug::error_mode error_mode, core::debug::bugtrap_usage bugtrap_usage )
{
	core::debug::bugtrap::change_usage						( error_mode, bugtrap_usage );
	core::debug::platform::change_storage_access_handler	( error_mode, storage_access_handler );
	core::debug::postinitialize								( );
}

#ifdef DEBUG
static bool s_fpe_enabled			= false;
#endif // #ifdef DEBUG

static bool s_debug_preinitialized	=	false;

namespace xray {
namespace core {
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
} // namespace core
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
	error							= _controlfp_s( &flags, 0, 0 );
	R_ASSERT						( !error );

	flags							&= ~( /**_EM_INEXACT | _EM_UNDERFLOW|/**/ _EM_OVERFLOW | _EM_ZERODIVIDE | _EM_INVALID | _EM_DENORMAL );

	error							= _controlfp_s( 0, flags, _MCW_EM );
	R_ASSERT						( !error );
}

static void disable_fpe				( )
{
	_clearfp						( );

	u32								flags;
	errno_t							error;
	error							= _controlfp_s( &flags, 0, 0 );
	R_ASSERT						( !error );

	flags							&= ~( /**_EM_INEXACT | _EM_UNDERFLOW|/**/ _EM_OVERFLOW | _EM_ZERODIVIDE | /**_EM_INVALID | /**/_EM_DENORMAL );

	error							= _controlfp_s( 0, flags, _MCW_EM );
//	errno_t const error				= _controlfp_s( 0, 0, _MCW_EM );
	R_ASSERT						( !error );
}

void xray::debug::enable_fpe		( bool value )
{
	if ( s_fpe_enabled == value )
		return;

	s_fpe_enabled					= value;

	if ( s_fpe_enabled )
		::enable_fpe				( );
	else
		disable_fpe					( );
}
#endif // #ifdef DEBUG

void xray::core::debug::on_thread_spawn( )
{
#if defined(DEBUG) && !XRAY_PLATFORM_WINDOWS_64
	if ( !g_fpe_disabled )
		xray::debug::enable_fpe		( true );
#endif // #if defined(DEBUG) && !XRAY_PLATFORM_WINDOWS_64
}