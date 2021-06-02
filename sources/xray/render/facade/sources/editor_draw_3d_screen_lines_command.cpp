////////////////////////////////////////////////////////////////////////////
//	Created		: 04.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "editor_draw_3d_screen_lines_command.h"
#include <xray/render/engine/world.h>

namespace xray {
namespace render {

void defer_execution	( xray::render::base_command& command, xray::render::scene_ptr const& scene );
void defer_execution	( xray::render::base_command& command, xray::render::scene_view_ptr const& scene_view );

namespace editor {

void draw_3d_screen_lines_command::execute		( )
{
	m_render_engine_world.draw_3D_screen_lines	( m_scene, &m_points[0], m_points.size(), m_color, m_width, m_pattern, m_use_depth);
}

void draw_3d_screen_lines_command::defer_execution( )
{
	render::defer_execution				( *this, m_scene );
}

} // namespace editor
} // namespace render
} // namespace xray