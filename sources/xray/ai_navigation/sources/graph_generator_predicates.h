////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef GRAPH_GENERATOR_PREDICATES_H_INCLUDED
#define GRAPH_GENERATOR_PREDICATES_H_INCLUDED

namespace xray {
namespace ai {
namespace navigation {

class filter_non_passable_predicate {
public:
	inline filter_non_passable_predicate			( triangles_mesh_type& triangles_mesh ) :
		m_triangles_mesh		( &triangles_mesh )
	{
	}

	inline	bool	operator()			( u32 const& vertex_id ) const
	{
		size_t const vertex_id_index	= &vertex_id - &*m_triangles_mesh->indices.begin( );
		return					!m_triangles_mesh->data[ vertex_id_index / 3 ].is_passable;
	}

	inline bool operator( )							( xray::collision::triangle_result const& triangle ) const
	{
		return					!m_triangles_mesh->data[triangle.triangle_id].is_passable;
	}

	inline bool operator( )							( navigation_triangle const& triangle )
	{
		return					!triangle.is_passable;
	}

private:
	triangles_mesh_type const*	m_triangles_mesh;
}; // struct filter_obstructions_predicate

} // namespace navigation
} // namespace ai
} // namespace xray

#endif // #ifndef GRAPH_GENERATOR_PREDICATES_H_INCLUDED