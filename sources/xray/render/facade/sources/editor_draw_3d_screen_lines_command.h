////////////////////////////////////////////////////////////////////////////
//	Created		: 04.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef DRAW_3D_SCREEN_LINES_COMMAND_H_INCLUDED
#define DRAW_3D_SCREEN_LINES_COMMAND_H_INCLUDED

#include <xray/render/facade/base_command.h>
#include <xray/render/facade/editor_renderer.h>
#include <fastdelegate/fastdelegate.h>

namespace xray {
namespace render {
namespace editor {

class editor_renderer;

class draw_3d_screen_lines_command :
	public base_command,
	private boost::noncopyable
{
public:
	inline							draw_3d_screen_lines_command	(
				engine::world& render_engine_world,
				scene_ptr const& scene,
				math::float3 const* points,
				int const count,
				math::color const clr,
				float const width,
				u32 const pattern,
				bool const use_depth
			): 
		m_render_engine_world		( render_engine_world ),
		m_scene						( scene ),
		m_points					( &points[0], &points[count] ),
		m_color						( clr ),
		m_width						( width ),
		m_pattern					( pattern ),
		m_use_depth					( use_depth )
	{ }
	virtual	void					execute							( );
	virtual	void					defer_execution					( );

private:
	engine::world&					m_render_engine_world;
	editor::vector< math::float3 >	m_points;
	scene_ptr const					m_scene;
	math::color const				m_color; 
	float const						m_width;
	u32 const						m_pattern;
	bool const						m_use_depth;
}; // struct functor_command

} // namespace editor
} // namespace render
} // namespace xray

#ifdef MASTER_GOLD
#	error class must not be used in MASTER_GOLD
#endif

#endif // #ifndef EDITOR_DRAW_SCREEN_LINES_COMMAND_H_INCLUDED