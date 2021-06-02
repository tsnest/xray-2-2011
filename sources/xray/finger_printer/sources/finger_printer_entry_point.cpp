////////////////////////////////////////////////////////////////////////////
//	Created 	: 26.08.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "finger_printer_application.h"

static xray::uninitialized_reference<application>		s_application;
static int												s_return_code = 0;

struct guard {
	inline	guard						( )
	{
		XRAY_CONSTRUCT_REFERENCE		( s_application, application );
	}

	inline	~guard						( )
	{
		s_return_code					= s_application->return_code( );
		XRAY_DESTROY_REFERENCE			( s_application );
	}
}; // struct guard

struct command_line_args
{
	u32			argc;
	pstr const*	argv;
};

static void	main_protected				( pvoid param )
{
	XRAY_UNREFERENCED_PARAMETERS		( param );

	guard								guard;

	s_application->initialize			( );
	if ( s_application->return_code( ) )
		return;
 
	s_application->execute				( );
	if ( s_application->return_code( ) )
		return;

	s_application->finalize				( );
}

int   main								( int const argc, pstr const argv[ ] )
{
	XRAY_UNREFERENCED_PARAMETERS		( argc, &argv );

	xray::debug::protected_call			( &main_protected, 0 );
	return								( s_return_code );
}