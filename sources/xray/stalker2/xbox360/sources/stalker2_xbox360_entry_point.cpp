////////////////////////////////////////////////////////////////////////////
//	Created 	: 26.08.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "stalker2_xbox360_application.h"

static xray::uninitialized_reference<stalker2::application>	s_application;

static int s_exit_code					= 0;

struct guard {
	inline	guard						( )
	{
		XRAY_CONSTRUCT_REFERENCE		( s_application, stalker2::application );
	}

	inline	~guard						( )
	{
		s_exit_code						= s_application->get_exit_code();
		XRAY_DESTROY_REFERENCE			( s_application );
	}
}; // struct guard

static void	main_protected				( pvoid )
{
	guard								guard;

	s_application->initialize			( );
	if ( s_application->get_exit_code() )
		return;

	s_application->execute				( );
	if ( s_application->get_exit_code() )
		return;

	s_application->finalize				( );
}

int main								( int const argc, pstr const argv[ ], pstr const env[ ] )
{
	XRAY_UNREFERENCED_PARAMETERS		( argc, argv, env );
	xray::debug::protected_call			( &main_protected, 0 );
	return								( s_exit_code );
}