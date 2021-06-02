////////////////////////////////////////////////////////////////////////////
//	Created 	: 26.08.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "stalker2_ps3_application.h"

//#include <sys/process.h>
//SYS_PROCESS_PARAM(1001, 128*1024)

static xray::uninitialized_reference<stalker2::application>	s_application;

static int s_exit_code					= 0;
static int s_arguments_count			= 0;
static pstr const* s_arguments			= 0;

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

	u32 buffer_size						= 0;

	u32* const lengths					= xray::pointer_cast<u32*>( ALLOCA( s_arguments_count*sizeof(u32) ) );
	u32* const b						= lengths;
	u32* const e						= lengths + s_arguments_count;
	for (u32* i = b; i != e; ++i ) {
		*i								= xray::strings::length( s_arguments[i-b] );
		buffer_size						+= *i + 1;
	}

	pstr const command_line				= xray::pointer_cast<pstr>( ALLOCA( buffer_size*sizeof(char) ) );
	pstr buffer							= command_line;
	for (u32* i = b; i != e; ++i ) {
		xray::memory::copy				( buffer, buffer_size, s_arguments[i-b], *i );

		buffer							+= *i;
		*buffer++						= ' ';

		R_ASSERT_CMP					( buffer_size, >=, *i + 1 );
		buffer_size						-= *i + 1;
	}

	*--buffer							= 0;

	s_application->initialize			( command_line );
	if ( s_application->get_exit_code() )
		return;

	s_application->execute				( );
	if ( s_application->get_exit_code() )
		return;

	s_application->finalize				( );
}

int main								( int const argc, pstr const argv[ ], pstr const env[ ] )
{
	s_arguments_count					= argc;
	s_arguments							= argv;
	xray::debug::protected_call			( &main_protected, 0 );
	return								( s_exit_code );
}