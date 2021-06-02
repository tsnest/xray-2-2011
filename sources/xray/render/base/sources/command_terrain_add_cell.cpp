////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "command_terrain_add_cell.h"
#include <xray/render/base/world.h>
#include <xray/render/base/platform.h>
#include <xray/memory_macros.h>

namespace xray{
namespace render{

command_terrain_add_cell::command_terrain_add_cell(	base_world& world, visual_ptr v, bool beditor )
:m_world					(world),
m_visual					(v),
m_beditor					(beditor)
{
}

command_terrain_add_cell::~command_terrain_add_cell	()
{
}

void command_terrain_add_cell::execute()
{
	m_world.platform().terrain_add_cell( m_visual, m_beditor );
}

} //namespace render
} //namespace xray
