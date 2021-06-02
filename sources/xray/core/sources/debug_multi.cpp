////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "debug_platform.h"
#include "debug.h"

#pragma message( XRAY_TODO("PS3: core dump generation") )
#pragma message( XRAY_TODO("PS3: save debug data") )

bool xray::core::debug::platform::error_after_dialog	( )
{
	return						( false );
}

void xray::debug::dump_call_stack					( pcstr						initiator, 
													  logging::verbosity const	verbosity, 
													  u32 						num_first_to_ignore, 
													  u32 						num_last_to_ignore, 
													  _EXCEPTION_POINTERS*		pointers, 
													  void*						callstack[] )
{
	XRAY_UNREFERENCED_PARAMETERS( initiator, verbosity, num_first_to_ignore, num_last_to_ignore, pointers, callstack );
}

void xray::core::debug::platform::on_error				( bool* do_debug_break, char* const message, u32 message_size, bool* ignore_always, _EXCEPTION_POINTERS* const exception_information, error_type_enum error_type )
{
	XRAY_UNREFERENCED_PARAMETERS( do_debug_break, ignore_always, exception_information, error_type, message_size );
	xray::core::debug::on_error	( message );
}

void xray::core::debug::platform::on_error_message_box	( bool* do_debug_break, char* const message, u32 message_size, bool* ignore_always, _EXCEPTION_POINTERS* const exception_information, error_type_enum error_type )
{
	XRAY_UNREFERENCED_PARAMETERS( do_debug_break, ignore_always, exception_information, error_type, message_size );
	xray::core::debug::on_error	( message );
}

pstr xray::core::debug::platform::fill_format_message	( int const error_code )
{
	XRAY_UNREFERENCED_PARAMETER	( error_code );
	return						( "" );
}

void xray::core::debug::platform::free_format_message	( pstr const buffer )
{
	XRAY_UNREFERENCED_PARAMETER	( buffer );
}

void xray::core::debug::platform::terminate				( pcstr message, int const error_code )
{
	XRAY_UNREFERENCED_PARAMETER	( message );
	::exit						( error_code );
}