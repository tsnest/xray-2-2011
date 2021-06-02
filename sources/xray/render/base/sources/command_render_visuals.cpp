////////////////////////////////////////////////////////////////////////////
//	Created		: 21.01.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/base/world.h>
#include "command_render_visuals.h"

using xray::render::base_world;
using xray::render::command_render_visuals;

command_render_visuals::command_render_visuals( base_world& world )
:m_world(world)
{}

command_render_visuals::~command_render_visuals	(  )
{

}

void command_render_visuals::execute( )
{
//	m_world.render_visuals			( );
}

