////////////////////////////////////////////////////////////////////////////
//	Created		: 04.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef DRAW_3D_SCREEN_LINES_COMMAND_H_INCLUDED
#define DRAW_3D_SCREEN_LINES_COMMAND_H_INCLUDED

#include <xray/render/base/engine_wrapper.h>
#include <xray/render/base/editor_renderer.h>
#include <fastdelegate/fastdelegate.h>

namespace xray {
namespace render {
namespace editor {

class editor_renderer;

class draw_3d_screen_lines_command :
	public engine::command,
	private boost::noncopyable
{
public:
	inline				draw_3d_screen_lines_command	( editor_renderer& renderer, math::float3 const* points, int count, color clr, float width ,u32 pattern, bool use_depth ): 
	m_renderer					( renderer ),
		m_points					( &points[0], &points[count] ),
		m_color						( clr ),
		m_width						( width ),
		m_pattern					( pattern ),
		m_use_depth					( use_depth )
	{ }

	virtual				~draw_3d_screen_lines_command	( );
	virtual	void		execute							( );

private:
	editor_renderer&			m_renderer;
	editor::vector< float3 >	m_points;
	color						m_color; 
	float						m_width;
	u32							m_pattern;
	bool						m_use_depth;

}; // struct command_delegate

} // namespace editor
} // namespace render
} // namespace xray

#endif // #ifndef EDITOR_DRAW_SCREEN_LINES_COMMAND_H_INCLUDED