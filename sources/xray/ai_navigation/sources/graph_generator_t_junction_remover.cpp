////////////////////////////////////////////////////////////////////////////
//	Created		: 22.08.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "graph_generator_t_junction_remover.h"

using xray::ai::navigation::graph_generator_t_junction_remover;
using xray::ai::navigation::triangles_mesh_type;

graph_generator_t_junction_remover::graph_generator_t_junction_remover( triangles_mesh_type& triangles_mesh ) :
	m_triangles_mesh( triangles_mesh )
{
	u32 const triangles_count = triangles_mesh.data.size();
	u32 total_edges_count = triangles_count * 3;
	edges_type sorted_edges_by_first_index(
		ALLOCA( total_edges_count * sizeof( edges_type::value_type ) ),
		total_edges_count
	);
	edges_type sorted_edges_by_second_index(
		ALLOCA( total_edges_count * sizeof( edges_type::value_type ) ),
		total_edges_count
	);

	for ( u32 i = 0; i < triangles_count; ++i ) {
		for ( u32 j = 0; j < 3; ++j ) {
			u32 edge_vertex_index0 = triangles_mesh.indices[ i * 3 + j ];
			u32 edge_vertex_index1 = triangles_mesh.indices[ i * 3 + ((j+1)%3) ];

			if ( edge_vertex_index1 < edge_vertex_index0 )
				std::swap( edge_vertex_index0, edge_vertex_index1 );

			sorted_edges_by_first_index.push_back ( edge_type( edge_vertex_index0, edge_vertex_index1 ) );
			sorted_edges_by_second_index.push_back ( edge_type( edge_vertex_index0, edge_vertex_index1 ) );
		}
	}

	std::sort (
		sorted_edges_by_first_index.begin(),
		sorted_edges_by_first_index.end(),
		less_edges_by_vertices_indices_predicate(0, 1)
	);
	std::sort (
		sorted_edges_by_second_index.begin(),
		sorted_edges_by_second_index.end(),
		less_edges_by_vertices_indices_predicate(1, 0)
	);

	for ( u32 i = 0; i < triangles_count; ++i ) {
		if ( remove_t_junctions( 
				i,
				sorted_edges_by_first_index, 
				sorted_edges_by_second_index
			)
		)
		{
			m_triangles_mesh.data[i].is_marked = true;
		}
	}
	
	m_triangles_mesh.remove_marked_triangles();
}

void graph_generator_t_junction_remover::try_subdivide_triangle (
		u32 const triangle_id,
		u32 const t_junction_vertex_index,
		u32 const edge_id,
		edges_type& sorted_edges_by_first_index,
		edges_type& sorted_edges_by_second_index
	)
{
	u32 indices[] = {
		m_triangles_mesh.indices[ triangle_id * 3 + 0],
		m_triangles_mesh.indices[ triangle_id * 3 + 1],
		m_triangles_mesh.indices[ triangle_id * 3 + 2],
	};

	u32 new_triangle_id = m_triangles_mesh.data.size();
	m_triangles_mesh.indices.push_back( t_junction_vertex_index );
	m_triangles_mesh.indices.push_back( indices[ (edge_id+2)%3 ] );
	m_triangles_mesh.indices.push_back( indices[ (edge_id+0)%3 ] );
	m_triangles_mesh.data.push_back ( m_triangles_mesh.data[ triangle_id ] );

	m_triangles_mesh.data[ new_triangle_id ].obstructions[0] = 0;
	m_triangles_mesh.data[ new_triangle_id ].obstructions[1] = m_triangles_mesh.data[ triangle_id ].obstructions[ (edge_id+2)%3 ];
	m_triangles_mesh.data[ new_triangle_id ].obstructions[2] = m_triangles_mesh.data[ triangle_id ].obstructions[ (edge_id+0)%3 ];;

	if ( remove_t_junctions( 
			new_triangle_id,
			sorted_edges_by_first_index,
			sorted_edges_by_second_index
		) )
	{
		m_triangles_mesh.data[ new_triangle_id ].is_marked = true;
	}

	new_triangle_id = m_triangles_mesh.data.size();
	m_triangles_mesh.indices.push_back( t_junction_vertex_index );
	m_triangles_mesh.indices.push_back( indices[ (edge_id+1)%3 ] );
	m_triangles_mesh.indices.push_back( indices[ (edge_id+2)%3 ] );
	m_triangles_mesh.data.push_back ( m_triangles_mesh.data[ triangle_id ]);

	m_triangles_mesh.data[ new_triangle_id ].obstructions[0] = m_triangles_mesh.data[ triangle_id ].obstructions[ (edge_id+0)%3 ];
	m_triangles_mesh.data[ new_triangle_id ].obstructions[1] = m_triangles_mesh.data[ triangle_id ].obstructions[ (edge_id+1)%3 ];;
	m_triangles_mesh.data[ new_triangle_id ].obstructions[2] = 0;

	if ( remove_t_junctions( 
			new_triangle_id,
			sorted_edges_by_first_index,
			sorted_edges_by_second_index
		) )
	{
		m_triangles_mesh.data[ new_triangle_id ].is_marked = true;
	}

}

bool is_on_segment	(
		xray::math::float3 const& testee,				// t
		xray::math::float3 const& first_segment_vertex,	// v0
		xray::math::float3 const& second_segment_vertex	// v1
	);

bool is_on_segment	(
		xray::math::float3 const& testee,				// t
		xray::math::float3 const& first_segment_vertex,	// v0
		xray::math::float3 const& second_segment_vertex,// v1
		float const epsilon
	);

u32 graph_generator_t_junction_remover::find_t_junction(
		u32 const vertex_index0,
		u32 const vertex_index1,
		edges_type& sorted_edges,
		u32 const (&sort_order)[2]
	)
{
	edges_type::iterator k;
	if ( sort_order[0] ) {
		k = std::lower_bound (
			sorted_edges.begin(),
			sorted_edges.end(),
			edge_type ( 0, vertex_index0 ),
			less_edges_by_vertices_indices_predicate( sort_order[0], sort_order[1] )
		);
	} else {
		k = std::lower_bound (
			sorted_edges.begin(),
			sorted_edges.end(),
			edge_type ( vertex_index0, 0 ),
			less_edges_by_vertices_indices_predicate( sort_order[0], sort_order[1] )
		);
	}

	float3 const& vertex0 = m_triangles_mesh.vertices[ vertex_index0 ];
	float3 const& vertex1 = m_triangles_mesh.vertices[ vertex_index1 ];

	edges_type::iterator const e = sorted_edges.end();
	for ( ; k != e && (*k).indices[sort_order[0]] == vertex_index0; ++k ) {
		if ( (*k).indices[sort_order[1]] !=  vertex_index1 && is_on_segment( m_triangles_mesh.vertices[ (*k).indices[sort_order[1]] ], vertex0, vertex1, math::epsilon_7 ) ) {
			return (*k).indices[sort_order[1]];
		}
	}
	return u32(-1);
}

bool graph_generator_t_junction_remover::remove_t_junctions ( 
		u32 const triangle_id,
		edges_type& sorted_edges_by_first_index,
		edges_type& sorted_edges_by_second_index
	)
{
	u32 const* indices = m_triangles_mesh.indices.begin() + triangle_id * 3;

	for ( u32 j = 0; j < 3; ++j ) {
		
		u32 sort_order[2] = { 0, 1 };
		u32 t_junction_vertex_index = find_t_junction( indices[(j+0)%3], indices[(j+1)%3], sorted_edges_by_first_index, sort_order );
		if ( t_junction_vertex_index != u32(-1) ) {
			try_subdivide_triangle( triangle_id, t_junction_vertex_index, j, sorted_edges_by_first_index, sorted_edges_by_second_index );
			return true;
		}

		sort_order[0] = 1;
		sort_order[1] = 0;
		t_junction_vertex_index = find_t_junction( indices[(j+0)%3], indices[(j+1)%3], sorted_edges_by_second_index, sort_order );
		if ( t_junction_vertex_index != u32(-1) ) {
			try_subdivide_triangle( triangle_id, t_junction_vertex_index, j, sorted_edges_by_first_index, sorted_edges_by_second_index );
			return true;
		}

		sort_order[0] = 0;
		sort_order[1] = 1;
		t_junction_vertex_index = find_t_junction( indices[(j+1)%3], indices[(j+0)%3], sorted_edges_by_first_index, sort_order );
		if ( t_junction_vertex_index != u32(-1) ) {
			try_subdivide_triangle( triangle_id, t_junction_vertex_index, j, sorted_edges_by_first_index, sorted_edges_by_second_index );
			return true;
		}

		sort_order[0] = 1;
		sort_order[1] = 0;
		t_junction_vertex_index = find_t_junction( indices[(j+1)%3], indices[(j+0)%3], sorted_edges_by_second_index, sort_order );
		if ( t_junction_vertex_index != u32(-1) ) {
			try_subdivide_triangle( triangle_id, t_junction_vertex_index, j, sorted_edges_by_first_index, sorted_edges_by_second_index );
			return true;
		}
	}
	return false;
}
