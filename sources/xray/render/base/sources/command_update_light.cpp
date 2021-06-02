////////////////////////////////////////////////////////////////////////////
//	Created		: 22.02.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "command_update_light.h"
#include <xray/render/base/world.h>
#include <xray/render/base/platform.h>

using xray::render::base_world;
using xray::render::command_update_light;

command_update_light::command_update_light( base_world& world, u32 id, xray::render::light_props const& props, bool beditor)
:m_id		( id ),
m_world		( world ),
m_props		( props ),
m_beditor	( beditor )
{
	ASSERT(id);
}

command_update_light::~command_update_light	(  )
{

}

void command_update_light::execute( )
{
	m_world.platform().update_light( m_id, m_props, m_beditor );
}

