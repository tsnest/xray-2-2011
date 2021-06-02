////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef EDITOR_DRAW_SCREEN_LINES_COMMAND_H_INCLUDED
#define EDITOR_DRAW_SCREEN_LINES_COMMAND_H_INCLUDED

#include <xray/render/facade/base_command.h>
#include <xray/render/facade/editor_renderer.h>
#include <fastdelegate/fastdelegate.h>
#include <xray/render/engine/vertex_colored.h>

namespace xray {
namespace render {
namespace editor {

class editor_renderer;

class draw_screen_lines_command :
	public base_command,
	private boost::noncopyable
{
private:
	typedef base_command								super;

public:
	typedef editor::vector< vertex_colored >			vertices_type;
	typedef editor::vector< u16 >						indices_type;

public:
	inline				draw_screen_lines_command	(
			engine::world& render_engine_world,
			scene_ptr const& scene,
			math::float2 const* const points,
			int const count,
			math::color const color,
			float const width,
			u32 const pattern
		) : 
		super					( true ),
		m_render_engine_world	( render_engine_world ),
		m_scene					( scene ),
		m_points				( &points[0], &points[count] ),
		m_color					( color ),
		m_width					( width ),
		m_pattern				( pattern )
	{
	}

	virtual	void		execute						( );
	virtual	void		defer_execution				( );

private:
	editor::vector< float2 >	m_points;
	engine::world&				m_render_engine_world;
	scene_ptr const				m_scene;
	math::color const			m_color; 
	float const					m_width;
	u32 const					m_pattern;
}; // struct functor_command

} // namespace editor
} // namespace render
} // namespace xray
#ifdef MASTER_GOLD
#error class must not be used in MASTER_GOLD
#endif
#endif // #ifndef EDITOR_DRAW_SCREEN_LINES_COMMAND_H_INCLUDED