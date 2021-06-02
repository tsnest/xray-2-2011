////////////////////////////////////////////////////////////////////////////
//	Created 	: 17.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/facade/engine_renderer.h>
#include <xray/render/engine/world.h>

using xray::render::engine::renderer;

renderer::renderer					( xray::render::engine::world& engine_world ) :
	m_render_engine_world			( engine_world )
{
}

void renderer::initialize			( )
{
	m_render_engine_world.initialize( );
}

u32 renderer::frame_id				( )
{
	return							( m_render_engine_world.frame_id( ) );
}