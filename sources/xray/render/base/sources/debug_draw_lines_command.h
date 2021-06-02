////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef DEBUG_DRAW_LINES_COMMAND_H_INCLUDED
#define DEBUG_DRAW_LINES_COMMAND_H_INCLUDED

#include <xray/render/base/engine_wrapper.h>
#include <xray/render/base/debug_renderer.h>
#include <fastdelegate/fastdelegate.h>

namespace xray {
namespace render {
namespace debug {

class debug_renderer;

class draw_lines_command :
	public engine::command,
	private boost::noncopyable
{
public:
	typedef vectora< vertex_colored >	vertices_type;
	typedef vectora< u16 >				indices_type;

public:
	inline				draw_lines_command	( debug_renderer& renderer, vertices_type const& vertices, indices_type const& indices );
	template < int vertex_count, int index_count >
	inline				draw_lines_command	( debug_renderer& renderer, vertex_colored const ( &vertices )[ vertex_count ], u16 const ( &indices )[ index_count ] );
	template < template < typename > class container_type >
	inline				draw_lines_command	( debug_renderer& renderer, container_type < vertex_colored > const& vertices, container_type < u16 > const& indices );
	virtual				~draw_lines_command	( );
	virtual	void		execute				( );

private:
	vertices_type		m_vertices;
	indices_type		m_indices;
	debug_renderer&		m_renderer;
}; // struct command_delegate

} // namespace debug
} // namespace render
} // namespace xray

#include "debug_draw_lines_command_inline.h"

#endif // #ifndef DEBUG_DRAW_LINES_COMMAND_H_INCLUDED