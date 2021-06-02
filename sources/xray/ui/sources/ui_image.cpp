////////////////////////////////////////////////////////////////////////////
//	Created		: 30.01.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "ui_image.h"
#include <xray/render/world.h>
#include <xray/render/facade/ui_renderer.h>

namespace xray {
namespace ui {

ui_image::ui_image( memory::base_allocator& a )
:ui_window		( a ),
m_color			( 0xffffffff ),
m_tex_coords	( 0.0f,0.0f,1.0f,1.0f ), 
m_point_type	( 1 )
{}

ui_image::~ui_image( )
{}

void ui_image::init_texture( pcstr texture_name )
{
	if(0==strings::compare_insensitive(texture_name,"ui_scull"))
	{
		m_tex_coords.set(0,0,1,1);
		m_point_type	=	1;
	}else
	if(0==strings::compare_insensitive(texture_name,"ui_rect"))
	{
		m_tex_coords.set(0,0,1,1);
		m_point_type	=	2;
	}else
		UNREACHABLE_CODE();
}

void ui_image::draw( xray::render::ui::renderer& renderer, xray::render::scene_view_ptr const& scene_view )
{
	int prim_type			= 0;
	
	float2 pos				= get_position();
	float2 size				= get_size();
	client_to_screen		(*this, pos);		//need absolute position

//	u32 max_verts			= 1*4; // use TriangleList+ IndexBuffer(0-1-2, 3-2-1)

	xray::render::ui::vertex const vertices[] = {
		xray::render::ui::vertex( pos.x,		pos.y+size.y,	0.0f, m_color, m_tex_coords.x,	m_tex_coords.w ),
		xray::render::ui::vertex( pos.x,		pos.y,			0.0f, m_color, m_tex_coords.x,	m_tex_coords.y ),
		xray::render::ui::vertex( pos.x+size.x,	pos.y+size.y,	0.0f, m_color, m_tex_coords.z,	m_tex_coords.w ),
		xray::render::ui::vertex( pos.x+size.x,	pos.y,			0.0f, m_color, m_tex_coords.z,	m_tex_coords.y )
	};

	renderer.draw_vertices	( scene_view, vertices, prim_type, m_point_type );

	ui_window::draw			( renderer, scene_view );
}

} // namespace xray
} // namespace ui