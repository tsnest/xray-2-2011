////////////////////////////////////////////////////////////////////////////
//	Created		: 21.01.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/base/world.h>
#include <xray/render/base/platform.h>
#include "command_remove_visual.h"

using xray::render::base_world;
using xray::render::command_remove_visual;

command_remove_visual::command_remove_visual( base_world& world, u32 id ):
m_world(world),
m_id(id)
{}
command_remove_visual::~command_remove_visual	(  )
{

}

void command_remove_visual::execute( )
{
	m_world.platform().remove_visual( m_id );
}

