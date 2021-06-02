////////////////////////////////////////////////////////////////////////////
//	Created	: 19.07.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "graph_generator_merger.h"
#include "constrained_delaunay_triangulator.h"

using xray::ai::navigation::graph_generator_merger;
using xray::ai::navigation::delaunay_triangulator;
using xray::ai::navigation::constrained_delaunay_triangulator;
using xray::math::float3;

void graph_generator_merger::fill_and_sort_edges_indices ( edges_indices_type& edges_indices	)
{

	edges_indices_type::iterator const b = edges_indices.begin();
	edges_indices_type::iterator i = b;
	edges_indices_type::iterator const e = edges_indices.end();
	for (; i != e; ++i )
		*i = i - b;

	std::sort(
		edges_indices.begin(),
		edges_indices.end(),
		compare_constraint_edges_indices_by_second_predicate ( m_constraint_edges )
	);
}

u32 graph_generator_merger::find_edge_id_by_first (u32 const vertex_id )
{
	constraint_edges_type::iterator i = std::lower_bound (
		m_constraint_edges.begin(),
		m_constraint_edges.end(),
		constraint_edge(vertex_id, 0),
		compare_constraint_edges_predicate ()
	);

	if ( i != m_constraint_edges.end() ) {
		return u32( i - m_constraint_edges.begin() );
	}
	return u32(-1);
}

u32 graph_generator_merger::find_edge_id_by_second(u32 const vertex_id, edges_indices_type const& edge_indices )
{
	edges_indices_type::const_iterator i = std::lower_bound(
		edge_indices.begin(),
		edge_indices.end(),
		constraint_edge( 0, vertex_id ),
		compare_constraint_edges_indices_by_second_predicate( m_constraint_edges )
	);

	if ( i != edge_indices.end() )
		return u32( i - edge_indices.begin() );
	return u32(-1);
}

bool is_on_segment	(
		xray::math::float3 const& testee,				// t
		xray::math::float3 const& first_segment_vertex,	// v0
		xray::math::float3 const& second_segment_vertex,// v1
		float const epsilon
	);
bool is_on_segment	(
		xray::math::float3 const& testee,			
		xray::math::float3 const& first_segment_vertex,
		xray::math::float3 const& second_segment_vertex
	);

bool graph_generator_merger::can_remove_vertex ( 
		u32 const (&coordinate_indices)[2],
		float3 const& testee, 
		float3 const& v0, 
		float3 const& v1
	)
{
	/*
	float3 k = v1 - v0;
	float3 p = v - v0;
	float l = squared_length( k );
	if ( xray::math::is_zero( l ) ) {
		if ( p.length() < 0.5 )
			return true;
		return false;
	}

	float3 vesult = v0 + k * ( p | k ) / ( l );
	
	if ( length( v - vesult ) < 0.5 )
		return true;

	return false;
	*/

	if ( ::is_on_segment( coordinate_indices, testee, v0, v1, m_edge_merge_height_epsilon ) ) {
		float angle_factor = normalize( v0 - testee ) | normalize( v1 - testee );
		if ( math::is_similar( angle_factor, -1.0f, m_edge_merge_angle_epsilon ) )
			return true;
	}
	return false;
}

bool graph_generator_merger::try_remove_vertex (
		u32 const (&coordinate_indices)[2],
		u32& vertex_id0,
		u32 const vertex_id1,
		edges_indices_type const& sorted_edges,
		edge_markers_type& edge_markers
	)
{
		u32 edges_start_id = find_edge_id_by_first( vertex_id0 );
		u32 const edges_count = m_constraint_edges.size();
		u32 new_vertex_id = u32(-1);
		u32 edge_to_merge = u32(-1);
		bool can_merge = false;
		for ( u32 i = edges_start_id; i < edges_count; ++i ) {
			if ( m_constraint_edges[i].vertex_index0 !=  vertex_id0 )
				break;

			if ( (m_constraint_edges[i].vertex_index1 == vertex_id1) ) {
				continue;
			}

			// Can't merge if more than 2 edges are using this vertex
			if ( can_merge )
				return false;

			if (
					can_remove_vertex (
						coordinate_indices,
						m_triangles_mesh.vertices[ vertex_id0 ],
						m_triangles_mesh.vertices[ vertex_id1 ],
						m_triangles_mesh.vertices[ m_constraint_edges[i].vertex_index1 ]
					)
				)
			{
				new_vertex_id = m_constraint_edges[i].vertex_index1;
				edge_to_merge = i;
				can_merge = true;
			} else
				return false;
		}
		
		edges_start_id = find_edge_id_by_second( vertex_id0, sorted_edges );
		for ( u32 i = edges_start_id; i < edges_count; ++i ) {
			u32 const edge_id = sorted_edges[i];
			if ( m_constraint_edges[ edge_id ].vertex_index1 != vertex_id0 )
				break;

			if ( (m_constraint_edges[ edge_id ].vertex_index0 == vertex_id1) ) {
				continue;
			}

			// Can't merge if more than 2 edges are using this vertex
			if ( can_merge ) {
				return false;
			}

			if (
					can_remove_vertex (
						coordinate_indices,
						m_triangles_mesh.vertices[ vertex_id0 ],
						m_triangles_mesh.vertices[ vertex_id1 ],
						m_triangles_mesh.vertices[ m_constraint_edges[edge_id].vertex_index0 ]
					)
				)
			{
				new_vertex_id = m_constraint_edges[edge_id].vertex_index0;
				edge_to_merge = edge_id;
				can_merge = true;
			} else 
				return false;
		}

		if ( can_merge ) {
			u32 old_vertex_id = vertex_id0;
			vertex_id0 = new_vertex_id;
			edge_markers[edge_to_merge] = true;
			try_remove_vertex ( coordinate_indices, vertex_id0, old_vertex_id, sorted_edges, edge_markers );
			return true;
		}

		// No similar vertices found
		return false;
}

bool graph_generator_merger::try_merge_edge ( 
		u32 const(&coordinate_indices)[2],
		u32& vertex_index0, 
		u32& vertex_index1,
		edges_indices_type const& sorted_edges,
		edge_markers_type& edge_markers
	)
{
	u32 const old_vertex_index0 = vertex_index0;
	u32 success_count = try_remove_vertex ( coordinate_indices, vertex_index0, vertex_index1, sorted_edges, edge_markers ) ? 1 : 0;
	success_count += try_remove_vertex ( coordinate_indices, vertex_index1, old_vertex_index0, sorted_edges, edge_markers ) ? 1 : 0;
	return success_count > 0;
}

void graph_generator_merger::merge_constraint_edges ( float3 const& normal )
{
	u32 coordinate_indices[2];
	fill_coordinate_indices( normal, coordinate_indices );

	u32 const edges_count = m_constraint_edges.size();
	edge_markers_type edge_markers(
		ALLOCA( edges_count * sizeof( edge_markers_type::value_type )),
		edges_count,
		edges_count,
		false
	);

	edges_indices_type sorted_edges_by_second( 
		ALLOCA( edges_count * sizeof(edges_indices_type::value_type)),
		edges_count,
		edges_count
	);

	typedef xray::buffer_vector< constraint_edge > merged_edges_type;
	
	merged_edges_type merged_edges (
		ALLOCA( edges_count * sizeof( merged_edges_type::value_type ) ),
		edges_count
	);
	
	fill_and_sort_edges_indices( sorted_edges_by_second );

	for ( u32 i = 0; i < edges_count; ++i ) {
		if ( edge_markers[i] )
			continue;
		
		u32 vertex_index0 = m_constraint_edges[i].vertex_index0;
		u32 vertex_index1 = m_constraint_edges[i].vertex_index1;

		if ( (i+1 < edges_count) && 
			(vertex_index0 == m_constraint_edges[i+1].vertex_index0) &&
			(vertex_index1 == m_constraint_edges[i+1].vertex_index1) ) 
		{
			edge_markers[i] = true;
			continue;
		}

		if ( try_merge_edge ( coordinate_indices, vertex_index0, vertex_index1, sorted_edges_by_second, edge_markers ) ) {
			constraint_edge const& edge_info = m_constraint_edges[i];
			
			if ( vertex_index1 < vertex_index0 )
				std::swap( vertex_index0, vertex_index1 );

			merged_edges.push_back ( 
				constraint_edge( 
					vertex_index0, 
					vertex_index1,
					edge_info.is_region_edge,
					edge_info.edge_direction,
					edge_info.obstruction_height
				)
			);
			edge_markers[i] = true;
		}
	}

	merged_edges_type::iterator					j = merged_edges.begin();
	merged_edges_type::iterator	const			e = merged_edges.end();
	for (u32 i = 0; i < edges_count; ++i ) {
		if ( edge_markers[i] )
			if ( j != e ) {
				m_constraint_edges[i] = *j;
				++j;
				edge_markers[i] = false;
			}
	}
	
	R_ASSERT ( j  == e );

	m_constraint_edges.erase (
		std::remove_if (
			m_constraint_edges.begin(),
			m_constraint_edges.end(),
			remove_edge_if_marked_predicate (m_constraint_edges, edge_markers)
		),
		m_constraint_edges.end()
	);

}
