////////////////////////////////////////////////////////////////////////////
//	Created		: 30.01.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "ui_world.h"
#include <xray/render/world.h>
#include <xray/render/facade/ui_renderer.h>

namespace xray {
namespace ui {

ui_world::ui_world(
		engine& engine,
		render::ui::renderer& renderer,
		memory::base_allocator& allocator,
		render::scene_view_ptr const& scene_view
	) :
	m_engine			( engine ),
	m_allocator			( allocator ),
	m_renderer			( renderer ),
	m_scene_view		( scene_view ),
	m_base_screen_size	( 1024.0f, 768.0f ),
	m_font_manager		( allocator )
{
	m_timer.start		( );
}

void ui_world::tick()
{
}

void ui_world::clear_resources		( )
{
}

void ui_world::on_device_reset()
{
	m_font_manager.on_device_reset();
}

} // namespace xray
} // namespace ui
