////////////////////////////////////////////////////////////////////////////
//	Created		: 03.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/base/world.h>
#include "command_draw_visual.h"

using xray::render::base_world;
using xray::render::command_draw_visual;

command_draw_visual::command_draw_visual( base_world& world, xray::render::visual_ptr const& object, xray::math::float4x4 const& transform, bool selected )
:m_object(object),
m_transform(transform),
m_world(world),
m_selected(selected)
{}
command_draw_visual::~command_draw_visual	(  )
{

}

void command_draw_visual::execute( )
{
	visual_ptr	v = static_cast_checked<visual*>(m_object.c_ptr());
	//m_world.add_visual(v, m_transform, m_selected);
}

