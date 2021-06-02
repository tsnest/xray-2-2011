////////////////////////////////////////////////////////////////////////////
//	Created		: 04.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "editor_draw_3d_screen_lines_command.h"
#include "editor_renderer.h"

namespace xray {
namespace render {
namespace editor {

draw_3d_screen_lines_command::~draw_3d_screen_lines_command	( )
{
}

void draw_3d_screen_lines_command::execute		( )
{
	m_renderer.draw_3D_screen_lines_impl( &m_points[0], m_points.size(), m_color, m_width, m_pattern, m_use_depth);
}

} // namespace editor
} // namespace render
} // namespace xray


