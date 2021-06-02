////////////////////////////////////////////////////////////////////////////
//	Created		: 03.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/base/world.h>
#include <xray/render/base/platform.h>
#include "command_change_visual_shader.h"

using xray::render::base_world;
using xray::render::command_change_visual_shader;

command_change_visual_shader::command_change_visual_shader( base_world& world, xray::render::visual_ptr const& object, xray::fixed_string<256> shader, xray::fixed_string<256> texture )
:m_world		(world),
m_object		(object),
m_shader		(shader),
m_texture		(texture)
{

}

command_change_visual_shader::~command_change_visual_shader	(  )
{

}

void command_change_visual_shader::execute( )
{
	m_world.platform().change_visual_shader(m_object,m_shader.c_str(),m_texture.c_str());
}

