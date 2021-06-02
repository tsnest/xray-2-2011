////////////////////////////////////////////////////////////////////////////
//	Created		: 29.08.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_system_usage_example_application.h"

#pragma comment( lib, "delayimp.lib" )

static xray::uninitialized_reference<xray::sound_system_usage_example::application>	s_application;

static int s_exit_code			= 0;

struct guard {
	inline	guard				( )
	{
		XRAY_CONSTRUCT_REFERENCE( s_application, xray::sound_system_usage_example::application );
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