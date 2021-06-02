////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "debug.h"
#include "logging.h"
#include "debug_call_stack.h"
#include "debug_platform.h"

#define _WIN32_WINNT	0x0500
#include <xray/os_preinclude.h>
#undef NOUSER
#undef NOMB
#include <xray/os_include.h>
#include <xray/fixed_string.h>

#include <dxerr.h>
#pragma comment( lib, "dxerr.lib" )

static pcstr s_full_call_stack_line_format	= "%-60s(%-3d) : %-70s : %-36s : 0x%08x";
static pcstr s_call_stack_line_format		= "%-60s       : %-70s : 0x%08x";

#ifdef DEBUG
#	define USE_OWN_ERROR_MESSAGE_WINDOW	1
#else // #ifdef DEBUG
#	define USE_OWN_ERROR_MESSAGE_WINDOW	0
#endif // #ifdef DEBUG

static bool s_error_after_dialog	= false;

struct helper {
	xray::fixed_string512   m_initiator;
	xray::logging::verbosity m_verbosity;


	u32						m_num_first_to_ignore;
	u32						m_num_last_to_ignore;

	helper		(	pcstr const						initiator, 
					xray::logging::verbosity const	verbosity, 
					u32								num_first_to_ignore,
					u32								num_last_to_ignore) :
		m_verbosity				( verbosity ),
		m_num_first_to_ignore	( num_first_to_ignore ),
		m_num_last_to_ignore	( num_last_to_ignore )
	{
		m_initiator		=	initiator;
		m_initiator		+=	":";
	}

	bool	predicate	(
			u32 	call_stack_id,
			u32 	num_call_stack_lines,
			pcstr 	module_name,
			pcstr 	file_name,
			int		line_number,
			pcstr	function,
			u32		address
		)
	{
		if ( call_stack_id < m_num_first_to_ignore )
			return					( true );

		if ( call_stack_id >= num_call_stack_lines - m_num_last_to_ignore )
			return					( true );

		xray::fixed_string4096	string;
		if ( line_number > 0 )
			string.assignf			(s_full_call_stack_line_format, file_name, line_number, function, module_name, address );
		else
			string.assignf			(s_call_stack_line_format, module_name, function, address);

		using namespace xray;

		logging::helper	( __FILE__, __FUNCSIG__, __LINE__, m_initiator.c_str(), m_verbosity )
						( logging::settings(0, logging::settings::flags_log_only_user_string), "%s", string.c_str() );
				
		return						( true );
	}
}; // struct helper

bool xray::core::debug::platform::error_after_dialog	( )
{
	return							( s_error_after_dialog );
}

void xray::debug::dump_call_stack				( pcstr						initiator,
												  logging::verbosity const	verbosity, 
												  u32						num_first_to_ignore, 
												  u32						num_last_to_ignore,
												  _EXCEPTION_POINTERS*		pointers,
												  void*						callstack[])
{
	helper							temp( initiator, verbosity, num_first_to_ignore, num_last_to_ignore );
	core::debug::call_stack::iterate( pointers, callstack, core::debug::call_stack::Callback( &temp, &helper::predicate ), true );		
}

struct error_helper {
	u32		m_ignore_level_count;
	pstr	m_start_buffer;
	pstr	m_buffer;
	u32		m_buffer_size;

							error_helper	(
			u32 const ignore_level_count,
			pstr const buffer,
			u32 const buffer_size
		) :
		m_ignore_level_count	( ignore_level_count ),
		m_start_buffer			( buffer ),
		m_buffer				( buffer ),
		m_buffer_size			( buffer_size )
	{
	}

					bool	predicate	(
			u32		call_stack_id,
			u32		num_call_stack_lines,
			pcstr	module_name,
			pcstr	file_name,
			int		line_number,
			pcstr	function,
			u32		address
		)
	{
		XRAY_UNREFERENCED_PARAMETER	( num_call_stack_lines );
		
		static pcstr s_full_call_stack_line_format	= "%s(%d) : %s : %s : 0x%08x";
		static pcstr s_call_stack_line_format		= "%s       : %s : 0x%08x";

		if ( call_stack_id < m_ignore_level_count)
			return					( true );

		if ( line_number > 0 )
			m_buffer				+= sprintf_s(
				m_buffer,
				m_buffer_size - ( m_buffer - m_start_buffer ),
				s_full_call_stack_line_format,
				file_name,
				line_number,
				function,
				module_name,
				address
			);
		else
			m_buffer				+= sprintf_s(
				m_buffer,
				m_buffer_size - ( m_buffer - m_start_buffer ),
				s_call_stack_line_format,
				module_name,
				function,
				address
			);

		m_buffer					+= sprintf_s( m_buffer, m_buffer_size - ( m_buffer - m_start_buffer ), "\r\n" );
		return						( m_buffer_size - ( m_buffer - m_start_buffer ) >= 384 );
	}
}; // struct helper

void xray::core::debug::platform::on_error	( bool* do_debug_break, char* const message, u32 const message_size, bool* ignore_always, _EXCEPTION_POINTERS* const exception_information, error_type_enum error_type )
{
	XRAY_UNREFERENCED_PARAMETERS	( exception_information, ignore_always, message_size );

	if ( do_debug_break )
		* do_debug_break			= false;

	if ( xray::debug::is_debugger_present() )
		return;

#if USE_OWN_ERROR_MESSAGE_WINDOW
	pstr buffer						= message + xray::strings::length( message );
	
	error_helper					helper (error_type == error_type_assert ? 3 : 0, buffer, u32 ( sizeof( message ) - ( buffer - message ) ) );
	xray::core::debug::call_stack::iterate	( exception_information, NULL, xray::core::debug::call_stack::Callback( &helper, &error_helper::predicate), false );

	buffer							= message + xray::strings::length( message );

	pcstr const	endline				= "\r\n";

	if ( error_type == error_type_assert )
	{
		buffer						+= sprintf_s( buffer, message_size - ( buffer - message ), "%s%sPress CANCEL to abort execution%s", endline, endline, endline );
		buffer						+= sprintf_s( buffer, message_size - ( buffer - message ), "Press TRY AGAIN to continue execution%s", endline );
		buffer						+= sprintf_s( buffer, message_size - ( buffer - message ), "Press CONTINUE to continue execution and ignore all the errors of this type%s%s", endline, endline );
	}
	else
	{
		buffer						+= sprintf_s( buffer, message_size - ( buffer - message ), "%s%sPress OK to abort execution%s", endline, endline, endline );
	}

	int	const result				= 
		MessageBox	(
			GetTopWindow( 0 ),
			message,
			"Fatal Error",
			( error_type == error_type_assert ? MB_CANCELTRYCONTINUE : MB_OK ) | MB_ICONERROR | MB_SYSTEMMODAL
		);

	if ( error_type == error_type_unhandled_exception )
		return;

	s_error_after_dialog			= true;

	switch (result) {
		case IDCANCEL : {
			* do_debug_break		= true;
			xray::core::debug::on_error ( message );
			break;
		}
		case IDTRYAGAIN : {
			s_error_after_dialog	= false;
			break;
		}
		case IDCONTINUE : {
			s_error_after_dialog	= false;
			if ( ignore_always )
				*ignore_always		= true;
			break;
		}
		default : {
			* do_debug_break		= true;
			xray::core::debug::on_error ( message );
			break;
		}
	}
#else // #if USE_OWN_ERROR_MESSAGE_WINDOW
	if ( error_type == error_type_unhandled_exception )
		return;

	* do_debug_break				= true;
	xray::core::debug::on_error		( message );
#endif // #if USE_OWN_ERROR_MESSAGE_WINDOW
}

void xray::core::debug::platform::on_error_message_box	( bool* do_debug_break, char* const message, u32 const message_size, bool* ignore_always, _EXCEPTION_POINTERS* const exception_information, error_type_enum error_type )
{
	XRAY_UNREFERENCED_PARAMETERS	( exception_information, ignore_always, error_type );

	if ( do_debug_break )
		* do_debug_break			= false;

	pstr buffer						= message + xray::strings::length( message );
	pcstr const	endline				= "\r\n";

	buffer							+= sprintf_s( buffer, message_size - (buffer - message), "%s%sPress OK to abort execution%s", endline, endline, endline );
	MessageBox	(
			GetTopWindow(0),
			message,
			"Fatal Error",
			MB_OK | MB_ICONERROR | MB_SYSTEMMODAL
		);

	xray::core::debug::on_error		( message );
}

void xray::core::debug::platform::terminate				( pcstr message, int const error_code  )
{
	if ( *message )
		MessageBox					( 0, message, "Error", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL );

	TerminateProcess				( GetCurrentProcess(), error_code );
	::exit							( error_code );	// to suppress warnings
}

pstr xray::core::debug::platform::fill_format_message	( int const error_code )
{
	pstr							result;
	FormatMessage					( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0, error_code, 0, (LPSTR)&result, 0, 0 );
	return							( result );
}

void xray::core::debug::platform::free_format_message	( pstr const buffer )
{
	LocalFree						( buffer );
}

pcstr xray::core::debug::platform::get_graphics_api_error_description	( int const error_code )
{
	return							DXGetErrorDescription( error_code );
}

bool xray::debug::is_debugger_present( )
{
	return							( !!IS_DEBUGGER_PRESENT() );
}

void xray::core::debug::notify_xbox_debugger ( pcstr message )
{
	XRAY_UNREFERENCED_PARAMETER		( message );
}