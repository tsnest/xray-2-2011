////////////////////////////////////////////////////////////////////////////
//	Created		: 29.08.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "graph_generator_merger.h"
#include "navigation_mesh_types.h"
#include <xray/math_randoms_generator.h>

using xray::ai::navigation::graph_generator_merger;
using xray::ai::navigation::triangles_mesh_type;
using xray::ai::navigation::constrained_delaunay_triangulator;
using xray::ai::navigation::triangles_mesh;
using xray::ai::navigation::vertices_type;
using xray::math::float3;

float project_vertex (float3 const& v0, float3 const& v1, float3 const& p);

struct compare_indices_by_vertices_predicate2 {
public:
	compare_indices_by_vertices_predicate2( vertices_type::iterator first, float3 const& v0, float3 const& v1, u32 const third_index) :
		m_first	( first ),
		m_v0	( v0 ),
		m_v1	( v1 ),
		m_third_index( third_index )
	{
	}

	inline bool operator() ( u32 const left, u32 const right) const {
		float3 left_vertex = *(m_first + left);
		float3 right_vertex = *(m_first + right);
		left_vertex[m_third_index] = 0;
		right_vertex[m_third_index] = 0;

		if (project_vertex ( m_v0, m_v1, left_vertex ) < project_vertex ( m_v0, m_v1, right_vertex ) )
			return true;
		return false;
	}

private:
	vertices_type::iterator					m_first;
	float3									m_v0;
	float3									m_v1;
	u32										m_third_index;
}; // class less_vertex_index_predicate

typedef xray::buffer_vector< u32 > indices_type;
struct remove_vertices_by_unique_indices_predicate2 {
	remove_vertices_by_unique_indices_predicate2 (
			vertices_type::iterator vertices_begin,
			indices_type const& unique_vertices_indices
		) :
		m_vertices_begin			( vertices_begin ),
		m_unique_vertices_indices	( &unique_vertices_indices )
	{
	}

		inline bool operator() ( xray::math::float3 const& vertex ) const
	{
		u32 const index = u32(&vertex - m_vertices_begin);
		return		(*m_unique_vertices_indices)[ index ] != index;
	}

private:
	vertices_type::iterator			m_vertices_begin;
	indices_type const*				m_unique_vertices_indices;
}; // struct remove_vertices_by_unique_indices_predicate

void graph_generator_merger::remove_duplicated_edge_vertices ( u32 const old_vertices_count, float3 const& normal )
{
	u32 coordinate_indices[2];
	fill_coordinate_indices( normal, coordinate_indices );
	u32 third_index;
	switch ( coordinate_indices[0] + coordinate_indices[1] ) {
	case 1: third_index = 2; break;
	case 2: third_index = 1; break;
	case 3: third_index = 0; break;
	default: NODEFAULT();
	}

	triangles_mesh_type::vertices_type::iterator first =  m_triangles_mesh.vertices.begin() + old_vertices_count;
	triangles_mesh_type::vertices_type::iterator last =  m_triangles_mesh.vertices.end();

	u32 const	vertices_count = last - first;
	indices_type sorted_indices ( 
		ALLOCA( vertices_count * sizeof(indices_type::value_type) ),
		vertices_count,
		vertices_count
	);

	indices_type::iterator const b = sorted_indices.begin();
	indices_type::iterator const e = sorted_indices.end();
	for ( indices_type::iterator i = b; i != e; ++i )
		*i						= u32(i - b);

	float3 middle_vertices[2];
	math::random32 random( 0 );
	for ( u32 i = 0; i < 100; ++i ) {
		middle_vertices[0] = float3(0, 0, 0);
		middle_vertices[1] = float3(0, 0, 0);

		u32 const half_vertices_count = vertices_count / 2;
		for ( u32 j = 0; j < half_vertices_count; ++j )
			middle_vertices[0] += *(first + sorted_indices[j] );

		for ( u32 j = half_vertices_count; j < vertices_count; ++j )
			middle_vertices[1] += *(first + sorted_indices[j] );

		middle_vertices[0]	/= float(half_vertices_count);
		middle_vertices[1]	/= float(vertices_count - half_vertices_count);

		middle_vertices[0][third_index] = 0;
		middle_vertices[1][third_index] = 0;
		if ( !is_similar( middle_vertices[0], middle_vertices[1], xray::math::epsilon_3 ) )
			break;

		std::random_shuffle( sorted_indices.begin(), sorted_indices.end(), random );
	}

	bool similar = true;
	for ( u32 i = 0; i < vertices_count; ++i )
		if ( !is_similar( coordinate_indices, *first, *(first+i) ) ) {
			similar = false;
			break;
		}

	if ( similar ) {
		middle_vertices[0] = float3( 0.0f, 0.0f, 0.0f );
		middle_vertices[1] = float3( 1.0f, 0.0f, 0.0f );
	}

	R_ASSERT ( !is_similar( middle_vertices[0], middle_vertices[1] ) );

	std::sort (
		sorted_indices.begin( ),
		sorted_indices.end( ),
		compare_indices_by_vertices_predicate2 (
			first, 
			middle_vertices[0], 
			middle_vertices[1],
			third_index
		)
	);

	indices_type unique_vertices_indices (
		ALLOCA( vertices_count * sizeof(indices_type::value_type) ),
		vertices_count,
		vertices_count,
		u32(-1)
	);

	for ( indices_type::iterator j = b; j != e; ++j ) {
		if ( unique_vertices_indices[ *j ] != u32(-1) )
			continue;

		static float const epsilon = math::epsilon_3;

		for ( indices_type::iterator i = j; i != e; ++i ) {
			if (
					project_vertex(
						middle_vertices[0],
						middle_vertices[1],
						*(first + (*j))
			) + epsilon
						<
					project_vertex(
						middle_vertices[0],
						middle_vertices[1],
						*(first + (*i))
					)
				)
			{
				break;
			}

			if ( is_similar( coordinate_indices, *(first + (*i)), *(first + (*j)), epsilon ) )
				unique_vertices_indices[ *i ] = *j;
		}
	}

	// !
	for ( u32 i = 0, n = unique_vertices_indices.size(); i != n; ++i ) {
		u32& real_index					= unique_vertices_indices[ unique_vertices_indices[i] ];
		if ( real_index	 <= i ) {
			unique_vertices_indices[i]	= real_index;
			continue;
		}

		real_index						= i;
		unique_vertices_indices[i]		= i;
	}

	m_triangles_mesh.vertices.erase (
		std::remove_if(
			first,
			last,
			remove_vertices_by_unique_indices_predicate2( 
				first, 
				unique_vertices_indices
			)
		),
		last
	);

	// reindexing unique vertices indices
	for ( u32 i = 0, j = 0; i != vertices_count; ++i ) {
        if ( unique_vertices_indices[i] == i )
			unique_vertices_indices[i]    = j++;
		else
			unique_vertices_indices[i]    = unique_vertices_indices[ unique_vertices_indices[i] ];
	}

	constraint_edges_type::iterator i = m_constraint_edges.begin();
	constraint_edges_type::iterator const e2 = m_constraint_edges.end();
	for ( ; i != e2; ++i) {

		/* Debug
		u32 const old_vertex_index0 = i->vertex_index0;
		u32 const old_vertex_index1 = i->vertex_index1;
		u32 index = i - m_constraint_edges.begin();
		*/

		if ( i->vertex_index0 >= old_vertices_count ) {
			i->vertex_index0 = old_vertices_count + unique_vertices_indices[ i->vertex_index0 - old_vertices_count ];
		}
		if ( i->vertex_index1 >= old_vertices_count ) {
			i->vertex_index1 = old_vertices_count + unique_vertices_indices[ i->vertex_index1 - old_vertices_count ];
		}

		/* debug
		u32 const new_vertex_index0 = i->vertex_index0;
		u32 const new_vertex_index1 = i->vertex_index1;
		*/

		if ( i->vertex_index1 < i->vertex_index0 )
			std::swap( i->vertex_index0, i->vertex_index1 );
	}

	std::sort (
		m_constraint_edges.begin(),
		m_constraint_edges.end(),
		compare_constraint_edges_predicate ( )
	);

	u32 const edges_count = m_constraint_edges.size();
	markers_type edge_markers (
		ALLOCA( edges_count * sizeof( markers_type::value_type ) ),
		edges_count,
		edges_count,
		false
	);

	for ( u32 i = 0; i < edges_count; ++i ) {
		u32 const vertex_index0 = m_constraint_edges[i].vertex_index0;
		u32 const vertex_index1 = m_constraint_edges[i].vertex_index1;

		if ( edge_markers[i] )
			continue;

		if ( vertex_index0 == vertex_index1 ) {
			edge_markers[i] = true;
			continue;
		}

		float height = m_constraint_edges[i].obstruction_height;
		bool is_region_edge = m_constraint_edges[i].is_region_edge;
		float3 edge_direction = m_constraint_edges[i].edge_direction;
		for ( u32 j = i + 1; j < edges_count; ++j ) {
			if  ( vertex_index0 != m_constraint_edges[j].vertex_index0 || vertex_index1 != m_constraint_edges[j].vertex_index1 ) {
				break;
			}
			height = math::max( height, m_constraint_edges[j].obstruction_height );
			is_region_edge |= m_constraint_edges[j].is_region_edge;
			if ( m_constraint_edges[j].is_region_edge )
				edge_direction = m_constraint_edges[j].edge_direction;
			edge_markers[j] = true;
		}
		m_constraint_edges[i].is_region_edge = is_region_edge;
		m_constraint_edges[i].obstruction_height = height;
		m_constraint_edges[i].edge_direction = edge_direction;
	}

	m_constraint_edges.erase (
		std::remove_if (
			m_constraint_edges.begin(),
			m_constraint_edges.end(),
			remove_edge_if_marked_predicate (m_constraint_edges, edge_markers)
		),
		m_constraint_edges.end()
	);

	m_constraint_edges.erase(
		std::unique(
			m_constraint_edges.begin(),
			m_constraint_edges.end(),
			equal_constraint_edges_predicate ( )
		),
		m_constraint_edges.end()
	);

}
