////////////////////////////////////////////////////////////////////////////
//	Created		: 09.07.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "engine_world.h"
#include "apc.h"
#include <xray/render/facade/engine_renderer.h>

using xray::engine::engine_world;

void engine_world::initialize_build	( pcstr const project_id )
{
	if ( threading::g_debug_single_thread )
		return;

	apc::assign_thread_id			( apc::build, u32(-1) );

	threading::spawn				(
		boost::bind( &engine_world::build, this, project_id ), 
		"build resources", 
		"build resources", 
		0,
		7 % threading::core_count()
	);
}

void engine_world::build_tick		( )
{
	xray::resources::dispatch_callbacks	( );
}

void engine_world::build			( pcstr const project_id )
{
	XRAY_UNREFERENCED_PARAMETER		( project_id );

	apc::assign_thread_id			( apc::build, threading::current_thread_id( ) );
	apc::process					( apc::build );

	for (;;) {
		if ( m_destruction_started )
			break;

		build_tick					( );
		threading::yield			( 10 );
	}
	
	apc::process					( apc::build );
}

void engine_world::build_clear_resources	( )
{
	resources::dispatch_callbacks	( );
}