////////////////////////////////////////////////////////////////////////////
//	Created		: 23.06.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "ui_draw_vertices_command.h"
#include <xray/render/engine/world.h>

using xray::render::ui::draw_vertices_command;

draw_vertices_command::draw_vertices_command	(
		xray::render::engine::world& render_engine_world,
		xray::render::scene_view_ptr const& scene_view,
		xray::render::ui::vertex const* const begin,
		xray::render::ui::vertex const* const end,
		xray::memory::base_allocator& allocator,
		u32 const primitives_type,
		u32 const points_type
	) :
	super					( true ),
	m_vertices				( allocator, begin, end ),
	m_render_engine_world	( render_engine_world ),
	m_scene_view			( scene_view ),
	m_primitives_type		( primitives_type ),
	m_points_type			( points_type )
{
}

void draw_vertices_command::execute				( )
{
	m_render_engine_world.draw_ui_vertices	( &*m_vertices.begin(), m_vertices.size(), m_primitives_type, m_points_type );
}

namespace xray {
namespace render {

void defer_execution	( xray::render::base_command& command, xray::render::scene_ptr const& scene );
void defer_execution	( xray::render::base_command& command, xray::render::scene_view_ptr const& scene_view );

} // namespace render
} // namespace xray

void draw_vertices_command::defer_execution		( )
{
	render::defer_execution	( *this, m_scene_view );
}