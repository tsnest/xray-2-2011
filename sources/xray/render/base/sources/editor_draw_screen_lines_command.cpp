////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "editor_draw_screen_lines_command.h"
#include "editor_renderer.h"

namespace xray {
namespace render {
namespace editor {

draw_screen_lines_command::~draw_screen_lines_command	( )
{
}

void draw_screen_lines_command::execute		( )
{
	m_renderer.draw_screen_lines_impl( &m_points[0], m_points.size(), m_color, m_width, m_pattern);
}

} // namespace editor
} // namespace render
} // namespace xray
