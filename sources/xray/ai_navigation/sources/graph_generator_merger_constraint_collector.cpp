////////////////////////////////////////////////////////////////////////////
//	Created		: 25.08.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "graph_generator_merger.h"
#include <xray/collision/geometry.h>
#include <xray/collision/geometry_instance.h>

using xray::ai::navigation::graph_generator_merger;
using xray::ai::navigation::triangles_mesh_type;
using xray::ai::navigation::navigation_triangle;
using xray::ai::navigation::constrained_delaunay_triangulator;
using xray::math::float3;

void graph_generator_merger::add_constraint_edge(
		u32 const index0, 
		u32 const index1, 
		bool const is_region_edge,
		xray::math::float3 const& edge_direction,
		float obstruction_height )
{
	if ( index0 < index1 ) {
		m_constraint_edges.push_back( 
			constraint_edge (
				index0,
				index1,
				is_region_edge,
				edge_direction,
				obstruction_height
			)
		);
		m_constraint_edges[ m_constraint_edges.size() - 1 ].start_index = m_constraint_edges.size() - 1;
	} else {
		m_constraint_edges.push_back( 
			constraint_edge (
				index1,
				index0,
				is_region_edge,
				edge_direction,
				obstruction_height
			)
		);
		m_constraint_edges[ m_constraint_edges.size() - 1 ].start_index = m_constraint_edges.size() - 1;
	}
}

class filter_obstructions_predicate3 {
public:
	inline filter_obstructions_predicate3	( triangles_mesh_type const& generator, xray::math::plane const& plane ) :
		m_triangles_mesh		( &generator ),
		m_plane					( &plane )
	{
	}

		inline bool operator( )	( xray::collision::triangle_result const& triangle_result ) const
	{
		navigation_triangle const& triangle = m_triangles_mesh->data[ triangle_result.triangle_id ];

		if ( triangle.is_passable )
			return				true;

		if ( xray::math::abs(triangle.plane.normal | m_plane->normal) < 1.f )
			return				false;

		if ( xray::math::is_similar(triangle.plane.d, m_plane->d, xray::math::epsilon_3) )
			return				true;

		return					false;
	}

private:
	triangles_mesh_type const*	m_triangles_mesh;
	xray::math::plane const*	m_plane;
}; // struct filter_obstructions_predicate

int tri_tri_intersect_with_isectline (
	float3 const& V0, float3 const& V1, float3 const& V2,
	float3 const& U0, float3 const& U1, float3 const& U2, int *coplanar,
	float3& isectpt1, float3& isectpt2
);

static inline u32 select_equal_vertex_index (
		triangles_mesh_type const& triangles_mesh,
		u32 const triangle_id,
		float3 const& testee
	)
{
	u32 const* indices = triangles_mesh.indices.begin() + 3 * triangle_id;
	float3 const* vertices[] = {
		&triangles_mesh.vertices[ indices[0] ],
		&triangles_mesh.vertices[ indices[1] ],
		&triangles_mesh.vertices[ indices[2] ],
	};

	if ( is_similar( testee, *vertices[0] ) )
		return indices[0];

	if ( is_similar( testee, *vertices[1] ) )
		return indices[1];

	if ( is_similar( testee, *vertices[2] ) )
		return indices[2];

	return u32(-1);
}

inline bool is_adjanced( triangles_mesh_type const& triangles_mesh, u32 const triangle_id0, u32 const triangle_id1 ) 
{
	for ( u32 i = 0; i < 3; ++i )
		if ( triangles_mesh.data[ triangle_id0 ].neighbours[ i ] == triangle_id1 )
			return true;
	return false;
}

void graph_generator_merger::collect_obstructed_constraint_edges ( u32 const triangle_id )
{
	u32 coordinate_indices[2] = { 0, 2 };

	triangles_type triangles(debug::g_mt_allocator);

	math::aabb						aabb(math::create_invalid_aabb());
	aabb.modify						( m_triangles_mesh.vertices[m_triangles_mesh.indices[3*triangle_id + 0]] );
	aabb.modify						( m_triangles_mesh.vertices[m_triangles_mesh.indices[3*triangle_id + 1]] );
	aabb.modify						( m_triangles_mesh.vertices[m_triangles_mesh.indices[3*triangle_id + 2]] );

	math::float3 const& center		= aabb.center( );
	math::float3 const& extents		= aabb.extents( );
	float const epsilon				= math::epsilon_3;
	aabb.modify						( center + float3( -epsilon - extents.x, -5.f - extents.y, -epsilon - extents.z ) );
	aabb.modify						( center + float3( +epsilon + extents.x,  5.f + extents.y, +epsilon + extents.z ) );

	m_triangles_mesh.spatial_tree->aabb_query	( 0, aabb, triangles );

	triangles.erase (
		std::remove_if(
			triangles.begin(),
			triangles.end(),
			filter_obstructions_predicate3( m_triangles_mesh, m_triangles_mesh.data[ triangle_id ].plane )
		),
		triangles.end()
	);

	u32 const* indices0 = m_triangles_mesh.indices.begin() + 3 * triangle_id;
	float3 const vertices0[] = {
			m_triangles_mesh.vertices[ indices0[0] ],
			m_triangles_mesh.vertices[ indices0[1] ],
			m_triangles_mesh.vertices[ indices0[2] ],
	};

	triangles_type::iterator triangle;
	while ( !triangles.empty() ) {
		triangle = triangles.end() - 1;
		u32 const intersector_id = triangle->triangle_id;

		u32 const* indices1 = m_triangles_mesh.indices.begin() + 3 * ( triangle->triangle_id );
		float3 const* vertices1[] = {
			&m_triangles_mesh.vertices[ indices1[0] ],
			&m_triangles_mesh.vertices[ indices1[1] ],
			&m_triangles_mesh.vertices[ indices1[2] ],
		};

		int coplanar;
		float3 segment_vertices[2];
		int intersection = tri_tri_intersect_with_isectline(
			vertices0[0], vertices0[1], vertices0[2],
			*vertices1[0], *vertices1[1], *vertices1[2],
			&coplanar,
			segment_vertices[0],
			segment_vertices[1]
		);

		triangles.erase( triangle );

		if ( is_similar( coordinate_indices, segment_vertices[0], segment_vertices[1] ) )
			continue;

		if ( intersection == 0 || coplanar || is_adjanced( m_triangles_mesh, triangle_id, triangle->triangle_id ) )
			continue;

		u32 segment_indices[2];
		segment_indices[0] = select_equal_vertex_index ( m_triangles_mesh, triangle_id, segment_vertices[0] );
		segment_indices[1] = select_equal_vertex_index ( m_triangles_mesh, triangle_id, segment_vertices[1] );
		
		/**/
		float obstruction_height = 1.0f;
		if ( segment_indices[0] != u32(-1) && segment_indices[1] != u32(-1) ) {
			
			if ( m_triangles_mesh.data[ triangle->triangle_id ].is_passable )
				continue;

			if ( m_triangles_mesh.data[ intersector_id ].neighbours[0] == u32(-2) ) {

				if ( segment_indices[1] < segment_indices[0] )
					std::swap( segment_indices[0], segment_indices[1] );
				if ( segment_indices[0] == m_triangles_mesh.data[ intersector_id ].neighbours[1] 
					&& segment_indices[1] == m_triangles_mesh.data[ intersector_id ].neighbours[2] )
				{
					continue;
				}
			}
		}
		/**/

//		if ( segment_indices[0] == u32(-1) ) {
			segment_indices[0] = m_triangles_mesh.vertices.size();
			m_triangles_mesh.vertices.push_back( segment_vertices[0] );
//		}

//		if ( segment_indices[1] == u32(-1) ) {
			segment_indices[1] = m_triangles_mesh.vertices.size();
			m_triangles_mesh.vertices.push_back( segment_vertices[1] );
//		}


		if ( m_triangles_mesh.data[ intersector_id ].neighbours[0] == u32(-2) ) {
			float3 const intersector_edge0 = m_triangles_mesh.vertices[ m_triangles_mesh.data[ intersector_id ].neighbours[1] ];
			float3 const intersector_edge1 = m_triangles_mesh.vertices[ m_triangles_mesh.data[ intersector_id ].neighbours[2] ];
			
			float3 const& segment_vertices0 = m_triangles_mesh.vertices[ segment_indices[0] ];
			float3 const& segment_vertices1 = m_triangles_mesh.vertices[ segment_indices[1] ];

			float const squared_length0 = length(intersector_edge0 - segment_vertices0);
			float const squared_length1 = length(intersector_edge1 - segment_vertices0);

			if ( squared_length1 < squared_length0 ) {
				obstruction_height = math::max( (intersector_edge0 - segment_vertices1).y, (intersector_edge1 - segment_vertices0).y );
			} else {
				obstruction_height = math::max( (intersector_edge0 - segment_vertices0).y, (intersector_edge1 - segment_vertices1).y );
			}
		}

		if ( m_triangles_mesh.data[ triangle->triangle_id ].is_passable )
			obstruction_height = 1.0f;

		add_constraint_edge(
			segment_indices[0],
			segment_indices[1],
			false,
			float3(),
			obstruction_height
		);
		m_constraint_edges[ m_constraint_edges.size() - 1 ].triangle_id = triangle->triangle_id;
	}
}

bool graph_generator_merger::can_traverse ( u32 const triangle_id, float3 const& region_normal ) 
{
	if ( triangle_id == u32(-1) )
		return false;

	if ( !m_triangles_mesh.data[ triangle_id ].is_passable )
		return false;

	if ( (m_triangles_mesh.data[ triangle_id ].plane.normal | region_normal) < .9f )
		return false;
	return true;
}

void graph_generator_merger::collect_constraint_edges ( u32 const triangle_id, float3 const& region_normal )
{
	R_ASSERT	( !m_triangles_mesh.data[ triangle_id ].is_marked );

	m_triangles_mesh.data[ triangle_id ].color = math::color( 255, 0, 0, 64 );
	m_triangles_mesh.data[ triangle_id ].is_marked = true;

	u32 connected_edges_count = 0;

	R_ASSERT( m_triangles_mesh.data[ triangle_id ].is_passable );
	if ( (m_triangles_mesh.data[ triangle_id ].plane.normal | region_normal) < .9f  || !m_triangles_mesh.data[ triangle_id ].is_passable )
		return;

	u32 const* edge_connections = m_triangles_mesh.data[ triangle_id ].neighbours;

	float3 const vertices[] = {
		m_triangles_mesh.vertices[ m_triangles_mesh.indices[ triangle_id * 3 + 0] ],
		m_triangles_mesh.vertices[ m_triangles_mesh.indices[ triangle_id * 3 + 1] ],
		m_triangles_mesh.vertices[ m_triangles_mesh.indices[ triangle_id * 3 + 2] ],
	};

	float3 const median = ((vertices[0]) + (vertices[1]) + (vertices[2])) / 3.f;
	for ( u32 i = 0; i < 3; ++i ) {
		if ( !can_traverse( edge_connections[i], region_normal ) ) {
			u32 const vertices_count = m_triangles_mesh.vertices.size();
			m_triangles_mesh.vertices.push_back( vertices[i] );
			m_triangles_mesh.vertices.push_back( vertices[(i+1)%3] );
			add_constraint_edge( 
				vertices_count, 
				vertices_count + 1,
				true,
				vertices[i] ^ vertices[(i+1) % 3],
				0
			);
		} else {
			++connected_edges_count;
			if ( !m_triangles_mesh.data[ edge_connections[i] ].is_marked )
				collect_constraint_edges( edge_connections[i], region_normal );
		}
	}

	collect_obstructed_constraint_edges( triangle_id );

	if ( connected_edges_count == 3 ) {
		m_triangles_mesh.data[ triangle_id ].color = math::color( 0, 255, 0, 64);
	}
	m_triangles_mesh.data[ triangle_id ].is_marked = true;
}

