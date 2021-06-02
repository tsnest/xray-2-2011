////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef DEBUG_DRAW_LINES_COMMAND_H_INCLUDED
#define DEBUG_DRAW_LINES_COMMAND_H_INCLUDED

#include <xray/render/facade/base_command.h>
#include <xray/render/facade/debug_renderer.h>
#include <fastdelegate/fastdelegate.h>

namespace xray {
namespace render {

namespace engine {
	class world;
} // namespace engine

namespace debug {

class debug_renderer;

class draw_lines_command :
	public base_command,
	private boost::noncopyable
{
public:
	typedef base_command								super;
	typedef vectora< vertex_colored >					vertices_type;
	typedef vectora< vertex_colored >::const_iterator	vertices_type_iterator;
	typedef vectora< u16 >								indices_type;

public:
	inline				draw_lines_command	(
							memory::base_allocator& allocator,
							scene_ptr const& scene,
							engine::world& renderer,
							vertices_type const& vertices,
							indices_type const& indices,
							bool use_depth = true
						);

	inline				draw_lines_command	(
							scene_ptr const& scene,
							engine::world& renderer,
							memory::base_allocator& allocator,
							vertices_type_iterator start_point,
							vertices_type_iterator end_point,
							indices_type const& indices,
							bool use_depth = true
						);

	template < int vertex_count, int index_count >
	inline				draw_lines_command	(
							scene_ptr const& scene,
							engine::world& renderer,
							memory::base_allocator& allocator,
							vertex_colored const ( &vertices )[ vertex_count ],
							u16 const ( &indices )[ index_count ],
							bool use_depth = true
						);
	template < template < typename > class container_type >
	inline				draw_lines_command	(
							scene_ptr const& scene,
							engine::world& renderer,
							memory::base_allocator& allocator,
							container_type < vertex_colored > const& vertices,
							container_type < u16 > const& indices,
							bool use_depth = true
						);
	virtual	void		execute				( );
	virtual	void		defer_execution		( );

private:
	vertices_type		m_vertices;
	indices_type		m_indices;
	scene_ptr			m_scene;
	engine::world&		m_renderer;
}; // struct functor_command

} // namespace debug
} // namespace render
} // namespace xray

#include "debug_draw_lines_command_inline.h"

#endif // #ifndef DEBUG_DRAW_LINES_COMMAND_H_INCLUDED