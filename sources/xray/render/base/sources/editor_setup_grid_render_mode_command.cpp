////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "editor_setup_grid_render_mode_command.h"
#include "editor_renderer.h"

namespace xray {
namespace render {
namespace editor {

setup_grid_render_mode_command::~setup_grid_render_mode_command	( )
{
}

void setup_grid_render_mode_command::execute		( )
{
	if( m_density != 0 )
		m_renderer.setup_grid_render_mode_impl	( m_density );
	else
		m_renderer.remove_grid_render_mode_impl	( );

}

} // namespace editor
} // namespace render
} // namespace xray