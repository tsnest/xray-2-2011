#include "pch.h"
#include "ui_renderer.h"
#include "command_processor.h"
#include <xray/render/base/world.h>
#include <xray/render/base/platform.h>

using xray::render::ui_renderer;
using xray::render::ui_command;
using xray::render::ui::command;
using xray::render::ui::shader;
using xray::render::ui::vertex;

ui_renderer::ui_renderer( xray::render::base_world& world, xray::render::platform& platform )
:m_world(world),
m_platform(platform)
{}

ui_renderer::~ui_renderer()
{}

command* ui_renderer::create_command(u32 max_verts, int prim_type, int point_type)
{
	ui_command* cmd = L_NEW(ui_command)(max_verts, prim_type, point_type, m_platform);
	return cmd;
}

command* ui_renderer::create_command(vertex* begin, vertex* end, int prim_type, int point_type)
{
	ui_command* cmd = L_NEW(ui_command)(begin, end, prim_type, point_type, m_platform);
	return cmd;
}

ui_command::ui_command(u32 max_verts, int prim_type, int point_type, xray::render::platform& p)
:m_prim_type(prim_type), m_point_type(point_type), m_platform(p), m_count(0)
{
	R_ASSERT(max_verts);
	m_verts.resize	(max_verts);
}

ui_command::ui_command( vertex* begin, vertex* end, int prim_type, int point_type, xray::render::platform& p)
:m_prim_type(prim_type), m_point_type(point_type), m_platform(p), m_count(u32(end - begin)), m_verts( begin, end)
{
}

ui_command::~ui_command()
{
}

void ui_command::set_shader(shader& shader)
{
	XRAY_UNREFERENCED_PARAMETER(shader);
}

void ui_command::push_point(float x, float y, float z, u32 c, float u, float v)
{
	ui::vertex& vertex_item		= m_verts[m_count];
	vertex_item.m_position.set	(x, y, z, 1);
	vertex_item.m_uv.set		(u, v);
	vertex_item.m_color			= c;
	++m_count;
}

void ui_command::execute()
{
	R_ASSERT(m_count);
	//implement drawing here
	m_platform.draw_ui_vertices	(&m_verts.front(), m_count, m_prim_type, m_point_type);
//	if ( (m_prim_type == 1) && (m_point_type == 0) && (m_verts.size() > 2) )
//		LOG_DEBUG	( "[%d] ui_command::execute", m_platform.frame_id() );
}
