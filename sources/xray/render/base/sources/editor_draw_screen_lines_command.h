////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef EDITOR_DRAW_SCREEN_LINES_COMMAND_H_INCLUDED
#define EDITOR_DRAW_SCREEN_LINES_COMMAND_H_INCLUDED

#include <xray/render/base/engine_wrapper.h>
#include <xray/render/base/editor_renderer.h>
#include <fastdelegate/fastdelegate.h>

namespace xray {
namespace render {
namespace editor {

class editor_renderer;

class draw_screen_lines_command :
	public engine::command,
	private boost::noncopyable
{
public:
	typedef editor::vector< vertex_colored >			vertices_type;
	typedef editor::vector< u16 >						indices_type;

public:
	inline				draw_screen_lines_command	( editor_renderer& renderer, math::float2 const* points, int count, color clr, float width ,u32 pattern ): 
						m_renderer					( renderer ),
						m_points					( &points[0], &points[count] ),
						m_color						( clr ),
						m_width						( width ),
						m_pattern					( pattern )
						{ }

	virtual				~draw_screen_lines_command	( );
	virtual	void		execute						( );

private:
	editor_renderer&			m_renderer;
	editor::vector< float2 >	m_points;
	color						m_color; 
	float						m_width;
	u32							m_pattern;

}; // struct command_delegate

} // namespace editor
} // namespace render
} // namespace xray

#endif // #ifndef EDITOR_DRAW_SCREEN_LINES_COMMAND_H_INCLUDED