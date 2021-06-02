////////////////////////////////////////////////////////////////////////////
//	Created		: 19.11.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "engine_world.h"
#include <xray/exit_codes.h>

static xray::command_line::key	s_terminate_on_timeout_key	(
	"terminate_on_timeout",
	"", 
	"tests",
	"application will be terminated after timeout",
	"specify time limit (in seconds) as a first argument"
);

using xray::engine::engine_world;

void engine_world::terminate_on_timeout ( float const time_limit )
{
	timing::timer app_closing_timer;
	app_closing_timer.start		( );

	while ( app_closing_timer.get_elapsed_msec() < time_limit * 1000 )
	{
		threading::yield		( 1000 );
	}

	LOGI_ERROR					( "process termination thread", "interval %d sec for application closing expired",
								  time_limit );
	xray::debug::terminate		( "" );
}

void engine_world::initialize_terminate_on_timeout ( )
{
	float time_limit;
	if ( s_terminate_on_timeout_key.is_set_as_number( &time_limit ) )
	{
		threading::spawn		(
			boost::bind( &engine_world::terminate_on_timeout, this, time_limit ),
			"process termination", 
			"process termination thread",
			0,
			3 % threading::core_count(),
			threading::tasks_aware
		);
	}
}

bool engine_world::is_closing_timeout_set			( ) const
{
	return s_terminate_on_timeout_key.is_set();
}