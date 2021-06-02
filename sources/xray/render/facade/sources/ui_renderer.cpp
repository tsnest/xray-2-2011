////////////////////////////////////////////////////////////////////////////
//	Created 	: 01.02.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/facade/ui_renderer.h>
#include <xray/render/world.h>
#include <xray/render/engine/world.h>
#include "ui_draw_vertices_command.h"

using xray::render::ui::renderer;

renderer::renderer		(
		xray::render::one_way_render_channel& channel,
		xray::memory::base_allocator& allocator,
		xray::render::engine::world& engine_world
	) :
	m_channel				( channel ),
	m_render_engine_world	( engine_world ),
	m_allocator				( allocator )
{
}

void renderer::draw_vertices	(
		xray::render::scene_view_ptr const& scene_view,
		xray::render::ui::vertex const* const begin,
		xray::render::ui::vertex const* const end,
		u32 const primitives_type,
		u32 const points_type
	)
{
	m_channel.owner_push_back	( XRAY_NEW_IMPL( m_allocator, ui::draw_vertices_command ) ( m_render_engine_world, scene_view, begin, end, m_allocator, primitives_type, points_type ) );
}

void renderer::draw_text		(
		xray::render::scene_view_ptr const& scene_view,
		pcstr const& text,
		xray::ui::font const& font,
		xray::float2 const& position,
		xray::math::color const& text_color,
		xray::math::color const& selection_color,
		u32 const max_line_size,
		bool const is_multiline,
		u32 const start_selection,
		u32 const end_selection
	)
{
	vectora< xray::render::ui::vertex > vertices( m_allocator );
	vertices.resize				( strings::length( text ) * 4 );
	m_render_engine_world.draw_text	(
		vertices, 
		text, 
		font, 
		position, 
		text_color, 
		selection_color, 
		max_line_size, 
		is_multiline,
		start_selection, 
		end_selection
	);

	u32 const primitives_type	= 0;
	u32 const points_type		= 0;
	m_channel.owner_push_back	( XRAY_NEW_IMPL( m_allocator, ui::draw_vertices_command ) ( m_render_engine_world, scene_view, &*vertices.begin(), &*vertices.end(), m_allocator, primitives_type, points_type ) );
}