////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "debug_draw_lines_command.h"
#include <xray/render/facade/debug_renderer.h>
#include <xray/render/engine/world.h>

using xray::render::debug::draw_lines_command;

void draw_lines_command::execute		( )
{
	m_renderer.draw_lines		( m_scene, m_vertices, m_indices );
}

namespace xray {
namespace render {

void defer_execution						( xray::render::base_command& command, xray::render::scene_ptr const& scene )
{
	command.deferred_next					= 0;

	if ( scene->last_command )
		scene->last_command->deferred_next	= &command;
	else
		scene->first_command				= &command;

	scene->last_command						= &command;
}

void defer_execution						( xray::render::base_command& command, xray::render::scene_view_ptr const& scene_view )
{
	command.deferred_next					= 0;

	if ( scene_view->last_command )
		scene_view->last_command->deferred_next	= &command;
	else
		scene_view->first_command			= &command;

	scene_view->last_command				= &command;
}

} // namespace render
} // namespace xray

void draw_lines_command::defer_execution	( )
{
	render::defer_execution		( *this, m_scene );
}