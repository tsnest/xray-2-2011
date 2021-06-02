////////////////////////////////////////////////////////////////////////////
//	Created		: 23.06.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef UI_DRAW_VERTICES_COMMAND_H_INCLUDED
#define UI_DRAW_VERTICES_COMMAND_H_INCLUDED

#include <xray/render/facade/ui_renderer.h>
#include <xray/render/facade/base_command.h>

namespace xray {
namespace render {

namespace engine {
	class world;
} // namespace engine

namespace ui {

class draw_vertices_command :
	public base_command,
	private boost::noncopyable
{
public:
							draw_vertices_command	(
								engine::world& render_engine_world,
								scene_view_ptr const& scene_view,
								vertex const* begin,
								vertex const* end,
								memory::base_allocator& allocator,
								u32 primitives_type,
								u32 points_type
							);
	virtual	void			execute					( );
	virtual	void			defer_execution			( );

private:
	typedef base_command	super;

private:
	vectora< ui::vertex >	m_vertices;
	engine::world&			m_render_engine_world;
	scene_view_ptr			m_scene_view;
	u32						m_primitives_type;
	u32						m_points_type;
}; // class draw_vertices_command

} // namespace ui
} // namespace render
} // namespace xray

#endif // #ifndef UI_DRAW_VERTICES_COMMAND_H_INCLUDED