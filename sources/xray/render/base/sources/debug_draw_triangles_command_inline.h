////////////////////////////////////////////////////////////////////////////
//	Created 	: 18.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef DEBUG_DRAW_TRIANGLES_COMMAND_INLINE_H_INCLUDED
#define DEBUG_DRAW_TRIANGLES_COMMAND_INLINE_H_INCLUDED

namespace xray {
namespace render {
namespace debug {

inline draw_triangles_command::draw_triangles_command	(
		debug_renderer& renderer,
		vertices_type const& vertices,
		indices_type const& indices
	) :
	m_renderer	( renderer ),
	m_vertices	( vertices ),
	m_indices	( indices )
{
	ASSERT		( m_indices.size( ) >= m_vertices.size( ) );
}

template < int vertex_count, int index_count >
inline draw_triangles_command::draw_triangles_command	(
		debug_renderer& renderer,
		vertex_colored const ( &vertices )[ vertex_count ],
		u16 const ( &indices )[ index_count ]
	) :
	m_renderer	( renderer ),
	m_vertices	( renderer.get_allocator(), &vertices[0], &vertices[0] + vertex_count ),
	m_indices	( renderer.get_allocator(), &indices[0], &indices[0] + index_count )
{
	COMPILE_ASSERT( index_count >= vertex_count, please_remove_redundant_vertices );
}

template < template < typename T > class container_type >
inline draw_triangles_command::draw_triangles_command	(
		debug_renderer& renderer,
		container_type < vertex_colored > const& vertices,
		container_type < u16 > const& indices
	) :
	m_renderer	( renderer ),
	m_vertices	( renderer.get_allocator(), vertices.begin( ), vertices.end( ) ),
	m_indices	( renderer.get_allocator(), indices.begin( ), indices.end( ) )
{
}

} // namespace debug
} // namespace render
} // namespace xray

#endif // #ifndef DEBUG_DRAW_TRIANGLES_COMMAND_INLINE_H_INCLUDED