////////////////////////////////////////////////////////////////////////////
//	Created 	: 22.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "game_renderer.h"
#include <xray/render/base/world.h>
#include <xray/render/base/world.h>
#include <xray/render/base/platform.h>
#include "command_processor.h"
#include "command_add_static.h"
#include "command_delegate.h"
#include "command_add_visual.h"
#include "command_update_visual.h"
#include "command_remove_visual.h"
#include "command_render_visuals.h"
#include "command_change_visual_shader.h"
#include "command_update_visual_vertex_buffer.h"
#include "command_add_light.h"
#include "command_update_light.h"
#include "command_remove_light.h"
#include "command_terrain_add_cell.h"
#include "command_terrain_remove_cell.h"

#include "debug_renderer.h"

using xray::render::game::game_renderer;
using xray::render::base_world;
using xray::render::buffer_fragment;
using xray::resources::managed_resource_ptr;

game_renderer::game_renderer				( base_world& world, xray::render::platform& platform ) :
	m_world								( world ),
	m_platform							( platform )
{
	m_debug				= NEW ( debug::debug_renderer )	( world.commands(), *xray::render::logic::g_allocator, platform );
}

game_renderer::~game_renderer				( )
{
	DELETE				( m_debug );
}

void game_renderer::add_static		( managed_resource_ptr const& collision, managed_resource_ptr const& geometry, managed_resource_ptr const& visuals )
{
	m_world.commands( ).push_command	( L_NEW( command_add_static ) ( m_world, collision, geometry, visuals ) );
}

void game_renderer::draw_static		( )
{
	m_world.commands( ).push_command	( L_NEW( command_delegate ) ( boost::bind( &game_renderer::draw_static_impl, this ) ) );
}

void game_renderer::draw_static_impl	( )
{
	//m_world.draw_static					( );
}

void game_renderer::draw_frame		( )
{
	m_world.commands().push_command	( L_NEW( command_delegate ) ( boost::bind( &xray::render::base_world::draw_frame_logic, &m_world ) ) );
}

void game_renderer::set_command_push_thread_id	( )
{
	m_world.commands( ).set_push_thread_id ( );
}

void game_renderer::set_command_processor_frame_id	( u32 frame_id )
{
	m_world.commands( ).set_frame_id( frame_id );
}


// void game_renderer::draw_visual		( xray::render::visual_ptr v, xray::math::float4x4 const& transform, bool selected )
// {
// 	m_world.commands( ).push_command	( L_NEW( command_draw_visual ) ( m_world, v, transform, selected ) );
// }

void game_renderer::add_visual				( u32 id, xray::render::visual_ptr v, xray::math::float4x4 const& transform, bool selected, bool system_object )
{
	m_world.commands( ).push_command	( L_NEW( command_add_visual ) ( m_world, id, v, transform, selected, system_object, false ) );
}

void game_renderer::remove_visual			( u32 id ) 
{
	m_world.commands( ).push_command	( L_NEW( command_remove_visual ) ( m_world, id ) );
}

void game_renderer::update_visual			( u32 id, xray::math::float4x4 const& transform, bool selected ) 
{
	m_world.commands( ).push_command	( L_NEW( command_update_visual ) ( m_world, id, transform, selected  ) );
}

void game_renderer::render_visuals			(  ) 
{
	m_world.commands( ).push_command	( L_NEW( command_render_visuals ) ( m_world ) );
}

void game_renderer::update_visual_vertex_buffer	( xray::render::visual_ptr const& object, xray::vectora<buffer_fragment> const& fragments )
{
	m_world.commands( ).push_command	( L_NEW( command_update_visual_vertex_buffer ) ( m_world, object, fragments, *xray::render::editor::g_allocator ) );
}

void game_renderer::change_visual_shader		( xray::render::visual_ptr const& object, char const* shader, char const* texture )
{
	m_world.commands( ).push_command	( L_NEW( command_change_visual_shader ) ( m_world, object, shader, texture ) );
}

xray::render::debug::renderer& game_renderer::debug	( )
{
	ASSERT	( m_debug );
	return	*m_debug;
}

void game_renderer::initialize_command_queue	( xray::render::engine::command* null_command )
{
	m_world.commands().initialize_queue	( null_command );
}

void game_renderer::add_light				( u32 id, xray::render::light_props const& props)
{
	m_world.commands( ).push_command	( L_NEW( command_add_light ) ( m_world, id, props, false) );
}

void game_renderer::update_light				( u32 id, xray::render::light_props const& props)
{
	m_world.commands( ).push_command	( L_NEW( command_update_light ) ( m_world, id, props, false) );
}

void game_renderer::remove_light				( u32 id)
{
	m_world.commands( ).push_command	( L_NEW( command_remove_light ) ( m_world, id, false) );
}

void game_renderer::terrain_add_cell			( xray::render::visual_ptr v )
{
	m_world.commands( ).push_command	( L_NEW( command_terrain_add_cell ) ( m_world, v, false) );
}

void game_renderer::terrain_remove_cell		( xray::render::visual_ptr v)
{
	m_world.commands( ).push_command	( L_NEW( command_terrain_remove_cell ) ( m_world, v, false) );
}
