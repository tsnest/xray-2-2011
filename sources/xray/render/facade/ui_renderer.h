////////////////////////////////////////////////////////////////////////////
//	Created 	: 01.02.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_FACADE_UI_RENDERER_H_INCLUDED
#define XRAY_RENDER_FACADE_UI_RENDERER_H_INCLUDED

#include <xray/render/api.h>
#include <xray/render/engine/base_classes.h>

namespace xray {

namespace ui {
	struct font;
} // namespace ui

namespace render {

namespace engine {
	class world;
} // namespace engine

namespace editor {
	class renderer;
} // namespace editor

namespace game {
	class renderer;
} // namespace game

class one_way_render_channel;

namespace ui {

struct vertex
{
	inline vertex	( ) { }

	inline vertex	( float const x, float const y, float const z, u32 const color, float const u, float const v ) :
		m_position	( x, y, z, 0.f ),
		m_color		( color ),
		m_uv		( u, v )
	{
	}

	float4			m_position;
	u32				m_color;
	float2			m_uv;
}; // struct vertex

class XRAY_RENDER_API renderer : private xray::core::noncopyable {
private:
	friend class editor::renderer;
	friend class game::renderer;
					renderer		( one_way_render_channel& channel, memory::base_allocator& allocator, engine::world& engine_world );

public:
			void	draw_vertices	( scene_view_ptr const& scene, ui::vertex const* begin, ui::vertex const* end, u32 primitives_type, u32 points_type );

	template < int Count >
	inline	void	draw_vertices	( scene_view_ptr const& scene_view, vertex const (&vertices)[Count], u32 primitives_type, u32 points_type )
	{
		draw_vertices	( scene_view, array_begin( vertices ), array_end( vertices ), primitives_type, points_type );
	}

			void	draw_text		(
						scene_view_ptr const& scene_view,
						pcstr const& text,
						xray::ui::font const& font,
						float2 const& position,
						math::color const& text_color,
						math::color const& selection_color,
						u32 max_line_size,
						bool is_multiline,
						u32 start_selection,
						u32 end_selection
					);
private:
	one_way_render_channel&	m_channel;
	engine::world&			m_render_engine_world;
	memory::base_allocator&	m_allocator;
}; // class renderer

} // namespace ui
} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_FACADE_GAME_RENDERER_H_INCLUDED