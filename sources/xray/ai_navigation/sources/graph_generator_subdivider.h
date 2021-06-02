////////////////////////////////////////////////////////////////////////////
//	Created		: 05.07.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef GRAPH_GENERATOR_SUBDIVIDER_H_INCLUDED
#define GRAPH_GENERATOR_SUBDIVIDER_H_INCLUDED

#include "triangles_mesh.h"
#include "navigation_mesh_types.h"
#include <xray/collision/api.h>

namespace xray {

namespace ai {
namespace navigation {

class graph_generator_subdivider : private boost::noncopyable {
public:
					graph_generator_subdivider		( triangles_mesh_type& triangles_mesh, u32 subdivision_max_operation_id );

private:
	enum obstructed_edges {
		obstructed_edge_none	= 0,
		obstructed_edge_ab		= u32(1) << 0,
		obstructed_edge_bc		= u32(1) << 1,
		obstructed_edge_ca		= u32(1) << 2,
	}; // enum obstructed_edges

	typedef buffer_vector< u32 > buffer_indices_type;

	buffer_indices_type::iterator const select_triangle (
						u32 triangle_id, 
						buffer_indices_type& triangles,
						math::float3& segment0,
						math::float3& segment1
					);

			bool	try_subdivide_triangle			(
						u32 triangle_id,
						buffer_indices_type const& triangles,
						math::float3 const& triangle_vertex0,
						math::float3 const& triangle_vertex1,
						math::float3 const& triangle_vertex2,
						u32 const obstructed_edge_id = 0
					);
			void correct_obstructed_edge  ( 
						u32 const triangle_id,
						float3 const& vertex0,
						float3 const& vertex1
					) ;


			void		correct_segment_vertex			(float3& vertex, u32 const triangle_id );
			u32			get_similar_edge_id				( u32 const triangle_id0, u32 const triangle_id1 );
			bool		subdivide_triangle_impl			( u32 triangle_id, buffer_indices_type& triangles );
			void		subdivide_triangle				( u32 triangle_id, buffer_indices_type& triangles );
			void		subdivide_triangle				( u32 triangle_id, triangles_type& triangles );
			void		subdivide_passable_triangles	( );

public:
	// subdivides triangle_id0 using triangle_id1
	static	u32			subdivide				( triangles_mesh_type& triangles_mesh, u32 const triangle_id0, u32 const triangle_id1 );
	static	u32			subdivide				( triangles_mesh_type& triangles_mesh, u32 const triangle_id0, float3 const v0, float3 const v1, float3 const v2 );

private:
	triangles_mesh_type&	m_triangles_mesh;
	u32	const						m_subdivision_max_operation_id;
	u32									m_subdivision_current_operation_id;
}; // class graph_generator_subdivider

} // namespace navigation
} // namespace ai
} // namespace xray

#endif // #ifndef GRAPH_GENERATOR_SUBDIVIDER_H_INCLUDED