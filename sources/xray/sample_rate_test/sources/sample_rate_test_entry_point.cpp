////////////////////////////////////////////////////////////////////////////
//	Created		: 08.02.2011
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sample_rate_test_application.h"

#pragma comment( lib, "delayimp.lib" )

static xray::uninitialized_reference<xray::sample_rate_test::application>	s_application;

static int s_exit_code			= 0;

struct guard {
	inline	guard				( )
	{
		XRAY_CONSTRUCT_REFERENCE( s_application, xray::sample_rate_test::application );
		s_application->initialize ( );
	}

	inline	~guard				( )
	{
		s_exit_code				= s_application->get_exit_code();
		s_application->finalize	( );
		XRAY_DESTROY_REFERENCE	( s_application );
	}
}; // struct guard

static void	main_protected		( pvoid )
{
	guard						guard;

	if ( s_application->get_exit_code() )
		return;

	s_application->execute		( );
	if ( s_application->get_exit_code() )
		return;

}

int main						( int const argc, pstr const argv[ ], pstr const env[ ] )
{
	XRAY_UNREFERENCED_PARAMETERS( argc, argv, env );

	xray::debug::protected_call	( &main_protected, 0 );
	return						( s_exit_code );
}