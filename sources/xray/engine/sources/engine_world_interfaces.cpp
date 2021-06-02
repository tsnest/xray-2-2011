////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "engine_world.h"
#include <xray/render/facade/engine_renderer.h>
#include <xray/input/world.h>
#include "apc.h"
#include <xray/render/world.h>

using xray::engine::engine_world;
using xray::math::float4x4;

void engine_world::exit					( int exit_code )
{
	R_ASSERT							( !m_destruction_started );

	set_exit_code						( exit_code );

	threading::interlocked_exchange		( m_destruction_started, 1 );
}

void engine_world::run_renderer_commands					( )
{
	m_render_world->logic_channel().render_process_commands	( false );
}

void engine_world::on_resize								( )
{
}

xray::render::world&	engine_world::get_renderer_world	( )
{
	if ( m_render_world )
		return							*m_render_world;

	while ( !m_render_world ) {
		threading::yield				( 10 );
	}

	return								*m_render_world;
}

xray::sound::world&	engine_world::get_sound_world	( )
{
	while ( !m_sound_world )
		threading::yield				( 1 );

	return								*m_sound_world;
}
//
//float engine_world::get_last_frame_time						( )
//{
//	return								m_last_frame_time;
//}