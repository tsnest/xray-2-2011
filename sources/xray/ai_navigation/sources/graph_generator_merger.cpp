////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "graph_generator_merger.h"
#include "graph_generator_predicates.h"
#include "graph_generator_adjacency_builder.h"
#include "constrained_delaunay_triangulator.h"

using xray::ai::navigation::graph_generator_merger;
using xray::ai::navigation::triangles_mesh_type;
using xray::ai::navigation::constrained_delaunay_triangulator;
using xray::ai::navigation::triangles_mesh;
using xray::ai::navigation::edges_type;

u32 const region_colors_count = 7;
xray::math::color const region_colors[region_colors_count] = {
	xray::math::color(   0, 255,   0, 64 ),
	xray::math::color(   0,   0, 255, 64 ),
	xray::math::color( 255, 255, 255, 64 ),
	xray::math::color(   0,   0, 255, 64 ),
	xray::math::color( 255, 255,   0, 64 ),
	xray::math::color( 255,   0, 255, 64 ),
	xray::math::color(   0, 255, 255, 64 ),
};

graph_generator_merger::graph_generator_merger(
		triangles_mesh_type& triangles_mesh,
		u32 const merge_max_operation_id,
		float const edge_merge_angle_epsilon,
		float const edge_merge_height_epsilon,
		u32 const delaunay_max_operation_id,
		edges_type& debug_edges
	) :
		m_triangles_mesh				( triangles_mesh ),
		m_merge_max_operation_id		( merge_max_operation_id ),
		m_debug_edges					( debug_edges ),
		m_edge_merge_angle_epsilon		( edge_merge_angle_epsilon ),
		m_edge_merge_height_epsilon		( edge_merge_height_epsilon ),
		m_merge_current_operation_id	(0),
		m_current_region_color			( 0 ),
		m_delaunay_max_operation_id		( delaunay_max_operation_id )
{
	u32 const triangles_count	= m_triangles_mesh.data.size();
	graph_generator_adjacency_builder ( 
		m_triangles_mesh.indices, 
		m_triangles_mesh.data,
		m_triangles_mesh.data,
		.8f
	);

	for ( u32 i = 0; i < triangles_count; ++i ) {
		if ( !m_triangles_mesh.data[i].is_marked && m_triangles_mesh.data[i].is_passable )
			triangulate_region ( i );
	}

	m_triangles_mesh.remove_marked_triangles ( );
	graph_generator_adjacency_builder ( 
		m_triangles_mesh.indices, 
		m_triangles_mesh.data,
		m_triangles_mesh.data,
		.8f
	);
}

typedef constrained_delaunay_triangulator::constraint_edge constraint_edge;

u32 graph_generator_merger::get_constraint_index_by_vertices_indices (
		u32 vertex_index0,
		u32 vertex_index1
	)
{
	if ( vertex_index1 < vertex_index0 )
		std::swap	( vertex_index1, vertex_index0 );

	constraint_edges_type::iterator k = std::lower_bound (
		m_constraint_edges.begin(),
		m_constraint_edges.end(),
		constraint_edge( vertex_index0, vertex_index1 ),
		compare_constraint_edges_predicate ( )
	);

	constraint_edges_type::iterator const e = m_constraint_edges.end();
	for ( ; k != e && (*k).vertex_index0 == vertex_index0; ++k ) {
		if ( vertex_index1 == (*k).vertex_index1 )
			return ( k - m_constraint_edges.begin() );
	}

	return u32(-1);

}

void graph_generator_merger::mark_constraint_edges    ( u32 const triangle_id )
{
	u32 const* indices	= &*m_triangles_mesh.indices.begin() + 3 * triangle_id;
	float3 const median = (m_triangles_mesh.vertices[ indices[0] ] + m_triangles_mesh.vertices[ indices[1] ] + m_triangles_mesh.vertices[ indices[2] ]) / 3.f;
	for ( u32 i = 0; i < 3; ++i ) {
		u32 constraint_id		= get_constraint_index_by_vertices_indices ( indices[ i ], indices[ ((i + 1) % 3) ] );
		if ( constraint_id == u32(-1) )
			continue;

		if ( m_constraint_edges[ constraint_id ].is_region_edge ) {
			float value = ( m_triangles_mesh.vertices[ indices[ i ] ] ^ m_triangles_mesh.vertices[ indices[ ((i + 1) % 3) ] ] ) | m_constraint_edges[ constraint_id ].edge_direction;
			if ( value < 0 ) {
				m_triangles_mesh.data[ triangle_id ].is_marked = true;
				return;
			}
		}

		m_triangles_mesh.data[triangle_id].obstructions[i] = m_constraint_edges[ constraint_id ].obstruction_height;
	}
}

void graph_generator_merger::triangulate_region ( u32 const triangle_id )
{
	R_ASSERT				( !m_triangles_mesh.data[ triangle_id ].is_marked );

	constrained_delaunay_triangulator::input_indices_type vertex_indices;
	m_constraint_edges.clear();
	
	u32 const vertices_count = m_triangles_mesh.vertices.size();

	if (m_merge_current_operation_id++ >= m_merge_max_operation_id )
		return;

	float3 const& normal = m_triangles_mesh.data[ triangle_id ].plane.normal;
	collect_constraint_edges				( triangle_id, m_triangles_mesh.data[ triangle_id ].plane.normal );
	if ( vertices_count != m_triangles_mesh.vertices.size() )
		remove_duplicated_edge_vertices		( vertices_count, normal );
	for ( u32 i = 0; i < 3; ++i ) {
		subdivide_constraint_edges				( normal );
		if ( vertices_count != m_triangles_mesh.vertices.size() )
			remove_duplicated_edge_vertices		( vertices_count, normal );
		merge_constraint_edges					( normal );
	}
	//subdivide_constraint_edges				( m_triangles_mesh.data[ triangle_id ].plane.normal );

	for ( constraint_edges_type::iterator i = m_constraint_edges.begin(), e = m_constraint_edges.end(); i != e; ++i ) {
		vertex_indices.push_back ( (*i).vertex_index0 );
		vertex_indices.push_back ( (*i).vertex_index1 );

		m_debug_edges.push_back ( edge( (*i).vertex_index0, (*i).vertex_index1 ) );
	}

	std::sort				( vertex_indices.begin(), vertex_indices.end() );
	vertex_indices.erase	(
		std::unique( 
			vertex_indices.begin(),
			vertex_indices.end()
		),
		vertex_indices.end()
	);

	//! Debug
	debug::vector< float3 > vertices;
	for ( u32 i = 0 ; i < vertex_indices.size(); ++i ) {
		vertices.push_back ( m_triangles_mesh.vertices[ vertex_indices[i] ] );
	}

	u32 const max_indices_count	= 3 * delaunay_triangulator::get_maximum_triangle_count_in_triangulation( vertex_indices.size() );
	constrained_delaunay_triangulator::indices_type indices (
		ALLOCA( sizeof( u32 ) * max_indices_count ),
		max_indices_count
	);

	//!
	if ( vertex_indices.size() < 3 )
		return;

	/*
	if ( m_merge_current_operation_id != 30 ) {
		return;
	}
		u32 const pop_count = vertex_indices.size() - m_delaunay_max_operation_id;
		for	( u32 q = 0; q < pop_count; ++q)
			vertex_indices.pop_back();
	*/

	dump_constrained_triangulation_input(
		m_triangles_mesh.vertices,
		vertex_indices,
		m_constraint_edges,
		m_triangles_mesh.data[triangle_id].plane.normal
	);

	R_ASSERT( 
		is_correct_triangulation_input_data( 
			m_triangles_mesh.data[triangle_id].plane.normal,
			m_triangles_mesh.vertices, 
			vertex_indices, 
			m_constraint_edges 
		)
	);

	constrained_delaunay_triangulator (																																		
		m_triangles_mesh.vertices,
		vertex_indices, 
		indices,
		m_triangles_mesh.data[triangle_id].plane.normal,
		m_constraint_edges,
		0, 
		u32(-1)
	);

	std::sort						(
		m_constraint_edges.begin(),
		m_constraint_edges.end(),
		compare_constraint_edges_predicate ( )
	);

	m_current_region_color = ( m_current_region_color + 1) % region_colors_count;

	u32 const indices_count			= m_triangles_mesh.indices.size();
	R_ASSERT_CMP					( indices.size() % 3, ==, 0 );
	u32 const new_indices_count		= indices.size();
	u32 const new_triangles_count	= new_indices_count / 3;
	for ( u32 i  = 0; i < new_triangles_count; ++i ) {
		m_triangles_mesh.indices.push_back ( indices[3*i+0] ) ;
		m_triangles_mesh.indices.push_back ( indices[3*i+1] ) ;
		m_triangles_mesh.indices.push_back ( indices[3*i+2] ) ;

		u32 const new_triangle_id		= m_triangles_mesh.data.size();
		m_triangles_mesh.data.push_back ( navigation_triangle() );
		m_triangles_mesh.data[ new_triangle_id ].plane = m_triangles_mesh.data[ triangle_id ].plane;
		m_triangles_mesh.data[ new_triangle_id ].is_passable = true;
		m_triangles_mesh.data[ new_triangle_id ].color = region_colors[ m_current_region_color ];

		mark_constraint_edges			( new_triangle_id );
	}

	triangle_connections_type triangle_connections;
	triangle_connections.resize ( new_triangles_count );
	graph_generator_adjacency_builder ( indices, triangle_connections );

	u32 const triangles_count		= indices_count / 3;
	triangle_connections_type::iterator i = triangle_connections.begin();
	triangle_connections_type::iterator const e = triangle_connections.end();
	for (; i != e; ++i ) {
		for ( u32 j = 0; j < 3; ++j ) {
			if ( (*i).neighbours[j] != u32(-1) && m_triangles_mesh.data[ (*i).neighbours[j]+triangles_count ].is_marked ) {
				u32 const triangle_id = triangles_count + u32( i - triangle_connections.begin() );
				u32 const vertex_index0 = m_triangles_mesh.indices[ triangle_id * 3 + ((j+0)%3) ];
				u32 const vertex_index1 = m_triangles_mesh.indices[ triangle_id * 3 + ((j+1)%3) ];

				if ( get_constraint_index_by_vertices_indices( vertex_index0, vertex_index1 ) == u32(-1) ) {
					m_triangles_mesh.data[ triangle_id ].is_marked = true;
					break;
				}
			}
		}
	}
}
