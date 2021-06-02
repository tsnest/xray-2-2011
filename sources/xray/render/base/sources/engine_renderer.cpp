////////////////////////////////////////////////////////////////////////////
//	Created 	: 17.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "engine_renderer.h"
#include "command_processor.h"
#include "command_view_matrix.h"
#include "command_projection_matrix.h"
#include "command_delegate.h"
#include <xray/render/base/platform.h>
#include <xray/render/base/world.h>
#include <xray/render/base/game_renderer.h>
#include <xray/render/base/debug_renderer.h>
#include <xray/render/base/editor_renderer.h>

using xray::render::engine::engine_renderer;

engine_renderer::engine_renderer				( xray::render::base_world& world, xray::render::platform& platform ) :
	m_world							( world ),
	m_platform						( platform )
{
}

engine_renderer::~engine_renderer				( )
{
}

bool engine_renderer::run				( bool wait_for_command )
{
	return							( m_world.commands().run ( wait_for_command ) );
}

bool engine_renderer::run_editor_commands				( bool wait_for_command )
{
	return							( m_world.commands_editor().run	( wait_for_command ) );
}

u32 engine_renderer::frame_id			( )
{
	return							( m_platform.frame_id( ) );
}

void engine_renderer::set_view_matrix	( xray::math::float4x4 const& matrix )
{
	m_world.commands().push_command	( L_NEW( command_view_matrix ) ( m_platform, matrix ) );
}

void engine_renderer::set_projection_matrix( xray::math::float4x4 const& matrix )
{
	m_world.commands().push_command	( L_NEW( command_projection_matrix ) ( m_platform, matrix ) );
}

void engine_renderer::test_cooperative_level	( )
{
	m_platform.test_cooperative_level();
}

void engine_renderer::draw_frame		( )
{
	m_world.game().debug().tick		( );
	m_world.editor().tick			( ); 
	
	m_platform.draw_frame			( );
}

void engine_renderer::flush_debug_commands	( )
{
	m_world.game().debug().tick();
	m_world.editor().debug().tick();
}

void engine_renderer::purge_orders		( )
{
	m_world.commands().purge_orders	( );
}
