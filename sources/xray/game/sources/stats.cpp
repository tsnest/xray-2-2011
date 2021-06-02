////////////////////////////////////////////////////////////////////////////
//	Created		: 18.02.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "stats.h"
#include <xray/ui/ui.h>
#include <xray/ui/world.h>

using stalker2::stats;

stats::stats(xray::ui::world& ui_world)
:m_ui_world(ui_world)
{
	create	();
}

stats::~stats()
{
	m_ui_world.destroy_window(m_main_window);
}

void stats::draw(render::ui::renderer& w)
{
	m_main_window->draw(w, m_ui_world.get_scene_view());
}

void stats::set_fps_stats(float fps)
{
	string64		buff;
	xray::sprintf	(buff, "%3.2f", fps);
	m_fps->set_text	(buff);
}

void stats::set_camera_stats(float3 const& pos, float3 const& dir)
{
	string64		buff;
	xray::sprintf	(buff, "cam pos:%3.2f %3.2f %3.2f", pos.x, pos.y, pos.z);
	m_camera_position->set_text	(buff);

	xray::sprintf	(buff, "cam dir:%3.2f %3.2f %3.2f", dir.x, dir.y, dir.z);
	m_camera_direction->set_text	(buff);
}

void stats::set_resources_stats( pcstr str )
{
	m_resources_activity->set_text	( str );
}

void stats::set_rtp_controllers_dump	( pcstr str )
{
	m_rtp_controllers_dump->set_text( str );
}

void stats::set_navmesh_info			( pcstr str )
{
	m_navmesh_info->set_text( str );
}

void stats::create()
{
	m_main_window						= m_ui_world.create_window();
	m_main_window->set_visible			(true);
	m_main_window->set_position			(float2(0.0f, 0.0f));
	m_main_window->set_size				(float2(1024.0f, 768.0f));
	
	m_fps								= m_ui_world.create_text();
	m_fps->w()->set_visible				(true);
	m_fps->w()->set_position			(float2(0.0f, 0.0f));
	m_fps->w()->set_size				(float2(50.0f, 20.0f));
	m_fps->set_font						(xray::ui::fnt_arial);
	m_fps->set_text_mode				(xray::ui::tm_default);
	m_fps->set_color					(0xffffffff);
	m_main_window->add_child			(m_fps->w(), true);

	m_camera_position					= m_ui_world.create_text();
	m_camera_position->w()->set_visible	(true);
	m_camera_position->w()->set_position(float2(0.0f, 20.0f));
	m_camera_position->w()->set_size	(float2(100.0f, 20.0f));
	m_camera_position->set_font			(xray::ui::fnt_arial);
	m_camera_position->set_text_mode	(xray::ui::tm_default);
	m_camera_position->set_color		(0xffffffff);
	m_main_window->add_child			(m_camera_position->w(), true);

	m_camera_direction					= m_ui_world.create_text();
	m_camera_direction->w()->set_visible(true);
	m_camera_direction->w()->set_position(float2(0.0f, 40.0f));
	m_camera_direction->w()->set_size	(float2(100.0f, 20.0f));
	m_camera_direction->set_font		(xray::ui::fnt_arial);
	m_camera_direction->set_text_mode	(xray::ui::tm_default);
	m_camera_direction->set_color		(0xffffffff);
	m_main_window->add_child			(m_camera_direction->w(), true);

	m_resources_activity					= m_ui_world.create_text();
	m_resources_activity->w()->set_visible(true);
	m_resources_activity->w()->set_position(float2(0.0f, 60.0f));
	m_resources_activity->w()->set_size	(float2(100.0f, 20.0f));
	m_resources_activity->set_font		(xray::ui::fnt_arial);
	m_resources_activity->set_text_mode	(xray::ui::tm_default);
	m_resources_activity->set_color		(0xffffffff);
	m_main_window->add_child			(m_resources_activity->w(), true);

	
	m_rtp_controllers_dump					= m_ui_world.create_text();
	m_rtp_controllers_dump->w()->set_visible(true);
	m_rtp_controllers_dump->w()->set_position(float2(0.0f, 60.0f));
	m_rtp_controllers_dump->w()->set_size	(float2(100.0f, 20.0f));
	m_rtp_controllers_dump->set_font		(xray::ui::fnt_arial);
	m_rtp_controllers_dump->set_text_mode	(xray::ui::tm_default);
	m_rtp_controllers_dump->set_color		(0xffffffff);
	m_main_window->add_child			(m_rtp_controllers_dump->w(), true);

	m_navmesh_info							= m_ui_world.create_text();
	m_navmesh_info->w()->set_visible		(true);
	m_navmesh_info->w()->set_position		(float2(0.0f, 100.0f));
	m_navmesh_info->w()->set_size			(float2(300.0f, 40.0f));
	m_navmesh_info->set_font				(xray::ui::fnt_arial);
	m_navmesh_info->set_text_mode			(xray::ui::tm_multiline);
	m_navmesh_info->set_color				(0xffffffff);
	m_main_window->add_child			(m_navmesh_info->w(), true);

}
