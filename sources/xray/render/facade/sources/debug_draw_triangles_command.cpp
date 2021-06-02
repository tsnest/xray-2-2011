////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "debug_draw_triangles_command.h"
#include <xray/render/facade/debug_renderer.h>
#include <xray/render/engine/world.h>

using xray::render::debug::draw_triangles_command;
using xray::render::engine::world;
using xray::render::vertex_colored;

draw_triangles_command::~draw_triangles_command	( )
{
}

void draw_triangles_command::execute			( )
{
	m_renderer.draw_triangles	( m_scene, m_vertices, m_indices );
}

namespace xray {
namespace render {

void defer_execution	( xray::render::base_command& command, xray::render::scene_ptr const& scene );
void defer_execution	( xray::render::base_command& command, xray::render::scene_view_ptr const& scene_view );

} // namespace render
} // namespace xray

void draw_triangles_command::defer_execution		( )
{
	render::defer_execution		( *this, m_scene );
}