////////////////////////////////////////////////////////////////////////////
//	Created		: 22.02.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/base/world.h>
#include <xray/render/base/platform.h>
#include "command_remove_light.h"

using xray::render::base_world;
using xray::render::command_remove_light;

command_remove_light::command_remove_light( base_world& world, u32 id, bool beditor ):
m_world		( world ),
m_id		( id ),
m_beditor	( beditor )
{}
command_remove_light::~command_remove_light	(  )
{

}

void command_remove_light::execute( )
{
	m_world.platform().remove_light( m_id, m_beditor );
}

