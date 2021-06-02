////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef DEBUG_DRAW_TRIANGLES_COMMAND_H_INCLUDED
#define DEBUG_DRAW_TRIANGLES_COMMAND_H_INCLUDED

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

class draw_triangles_command :
	public base_command,
	private boost::noncopyable
{
public:
	typedef base_command					super;
	typedef vectora< vertex_colored >		vertices_type;
	typedef vectora< u16 >					indices_type;

public:
	inline				draw_triangles_command	(
							scene_ptr const& scene,
							engine::world& renderer,
							vertices_type const& vertices,
							indices_type const& indices,
							bool use_depth = true
						);
	template < int vertex_count, int index_count >
	inline				draw_triangles_command	(
							scene_ptr const& scene,
							engine::world& renderer,
							memory::base_allocator& allocator,
							vertex_colored const ( &vertices )[ vertex_count ],
							u16 const ( &indices )[ index_count ],
							bool use_depth = true
						);
	template < template < typename > class container_type >
	inline				draw_triangles_command	(	
							scene_ptr const& scene,
							engine::world& renderer,
							memory::base_allocator& allocator,
							container_type < vertex_colored > const& vertices,
							container_type < u16 > const& indices,
							bool use_depth = true
						);
	virtual				~draw_triangles_command	( );
	virtual	void		execute					( );
	virtual	void		defer_execution			( );

private:
	vertices_type			m_vertices;
	indices_type			m_indices;
	scene_ptr				m_scene;
	render::engine::world&	m_renderer;
}; // struct functor_command

} // namespace debug
} // namespace render
} // namespace xray

#include "debug_draw_triangles_command_inline.h"

#endif // #ifndef DEBUG_DRAW_TRIANGLES_COMMAND_H_INCLUDED