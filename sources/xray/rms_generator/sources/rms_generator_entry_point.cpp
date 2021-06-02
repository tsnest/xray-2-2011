////////////////////////////////////////////////////////////////////////////
//	Created 	: 26.08.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "rms_generator_application.h"

static xray::uninitialized_reference<rms_generator::application>	s_application;

static int s_exit_code					= 0;

struct guard {
	inline	guard						( )
	{
		XRAY_CONSTRUCT_REFERENCE		( s_application, rms_generator::application );
	}

	inline	~guard						( )
	{
		s_exit_code						= s_application->get_exit_code( );
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
	guard								guard;

	command_line_args* args				=	reinterpret_cast<command_line_args*>(param);

	s_application->initialize			( args->argc, args->argv );
	if ( s_application->get_exit_code( ) )
		return;
 
	s_application->execute				( );
	if ( s_application->get_exit_code( ) )
		return;

	s_application->finalize				( );
}

int   main								( int const argc, pstr const argv[ ], pstr const env[ ] )
{
	XRAY_UNREFERENCED_PARAMETER			( env );
	command_line_args args			=	{ argc, argv };
	xray::debug::protected_call			( &main_protected, &args );
	return								( s_exit_code );
}