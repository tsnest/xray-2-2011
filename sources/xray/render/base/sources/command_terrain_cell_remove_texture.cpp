////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "command_terrain_cell_remove_texture.h"
#include <xray/render/base/world.h>
#include <xray/render/base/platform.h>

using xray::render::base_world;
using xray::render::buffer_fragment;
using xray::render::command_terrain_cell_remove_texture;
using xray::render::texture_string;

command_terrain_cell_remove_texture::command_terrain_cell_remove_texture( base_world& world, xray::render::visual_ptr v, u32 tex_user_id)
:m_world		(world),
m_visual		(v),
m_tex_user_id	(tex_user_id)
{
}

command_terrain_cell_remove_texture::~command_terrain_cell_remove_texture	()
{
}

void command_terrain_cell_remove_texture::execute()
{
	m_world.platform().terrain_remove_cell_texture( m_visual, m_tex_user_id);
}

