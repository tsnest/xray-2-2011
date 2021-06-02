////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "graph_generator_extruder.h"
#include "graph_generator_adjacency_builder.h"

using xray::ai::navigation::graph_generator_extruder;
using xray::ai::navigation::triangles_mesh_type;
using xray::math::float3;

graph_generator_extruder::graph_generator_extruder ( triangles_mesh_type& triangles_mesh ) :
	m_triangles_mesh	( triangles_mesh )
{
	extrude_triangles();
}

static inline xray::math::float3 extrude_vertex( xray::math::float3 const& vertex, float const extrude_length )
{
	xray::math::float3 result	= vertex;
	result.y					-= extrude_length;
	return						result;
}

static inline bool valid_triangle( xray::debug::vector<xray::math::float3> const& vertices, u32 const index0, u32 const index1, u32 const index2 )
{
	xray::math::float3 const& vertex0 = vertices[ index0 ];
	xray::math::float3 const& vertex1 = vertices[ index1 ];
	xray::math::float3 const& vertex2 = vertices[ index2 ];
	if ( vertex0.is_similar(vertex1, xray::math::epsilon_3) )
		return					false;

	if ( vertex0.is_similar(vertex2, xray::math::epsilon_3) )
		return					false;

	if ( vertex1.is_similar(vertex2, xray::math::epsilon_3) )
		return					false;

	xray::math::float3 const& normal_10 = normalize(vertex1 - vertex0);
	xray::math::float3 const& normal_20 = normalize(vertex2 - vertex0);
	if ( normal_10.is_similar(normal_20, xray::math::epsilon_3) )
		return					false;

	if ( normal_10.is_similar(-normal_20, xray::math::epsilon_3) )
		return					false;

	return						true;
}


void graph_generator_extruder::push_triangle (
		u32 const v0,
		u32 const v1,
		u32 const v2,
		u32 edge_vertex0,
		u32 edge_vertex1
	)
{
	if ( !valid_triangle( m_triangles_mesh.vertices, v0, v1, v2) ) 
		return;

	m_triangles_mesh.indices.push_back	( v0 );
	m_triangles_mesh.indices.push_back	( v1 );
	m_triangles_mesh.indices.push_back	( v2 );

	u32 const triangle_id = m_triangles_mesh.data.size();
	m_triangles_mesh.data.push_back		( 
		navigation_triangle( 
			math::create_plane( 
				m_triangles_mesh.vertices[ v0 ],
				m_triangles_mesh.vertices[ v1 ],
				m_triangles_mesh.vertices[ v2 ]
			),
			false 
		)
	);
	
	if ( edge_vertex1 < edge_vertex0 )
		std::swap( edge_vertex0, edge_vertex1 );

	m_triangles_mesh.data[ triangle_id ].neighbours[0] = u32(-2);
	m_triangles_mesh.data[ triangle_id ].neighbours[1] = edge_vertex0;
	m_triangles_mesh.data[ triangle_id ].neighbours[2] = edge_vertex1;

}

void graph_generator_extruder::extrude_triangles ( )
{
	float const extrude_length			= 3.f;
	u32 const vertex_count				= m_triangles_mesh.vertices.size( );

	u32 const triangles_count = m_triangles_mesh.data.size();
	u32 const total_edges_count = triangles_count * 3;
	edges_type edges (
		ALLOCA( total_edges_count * sizeof( edge ) ),
		total_edges_count
	);

	graph_generator_adjacency_builder (
		m_triangles_mesh.indices,
		m_triangles_mesh.data,
		m_triangles_mesh.data,
		math::epsilon_5
	);

	for ( u32 i = 0; i < triangles_count; ++i ) {
		float3 const normal = m_triangles_mesh.data[i].plane.normal;

		for ( u32 j = 0; j < 3; ++j ) {
			
			if ( m_triangles_mesh.data[i].neighbours[j] != u32(-1) ) {
				float3 const& other_normal = m_triangles_mesh.data[ m_triangles_mesh.data[i].neighbours[j] ].plane.normal;
				static float3 const up_vector( 0.0f, 1.0f, 0.0f );
				if ( math::is_similar(other_normal | up_vector, 0.0f, 0.2f )
					&& math::is_similar(other_normal | normal, 1.0f, math::epsilon_3 ) )
				{
					continue;		
				}
			}

			u32 edge_vertex_index0 = m_triangles_mesh.indices[ i * 3 + (( j + 0 ) % 3) ];
			u32 edge_vertex_index1 = m_triangles_mesh.indices[ i * 3 + (( j + 1 ) % 3) ];
			if ( edge_vertex_index1 < edge_vertex_index0 )
				std::swap( edge_vertex_index0, edge_vertex_index1 );
			edges.push_back ( edge( edge_vertex_index0, edge_vertex_index1 ) );

		}
	}

	std::sort ( 
		edges.begin(),
		edges.end(),
		less_by_edge_vertices_indices_predicate( )
	);

	edges.erase (
		std::unique(
			edges.begin(),
			edges.end(),
			equal_by_edge_vertices_indices_predicate( )
		),
		edges.end()
	);

	for ( u32 i = 0; i < vertex_count; ++i ) {
//			m_triangles_mesh.vertices.push_back	( extrude_vertex(m_triangles_mesh.vertices[i], 0.001f ) );
			m_triangles_mesh.vertices.push_back	( extrude_vertex(m_triangles_mesh.vertices[i], extrude_length) );
	}

	for ( u32 i = 0; i < edges.size(); ++i ) {
		u32 const indices[] = {
			edges[i].vertex_index0,
			edges[i].vertex_index1,
			vertex_count + indices[0],
			vertex_count + indices[1]
		};
		push_triangle(
			indices[0],
			indices[1],
			indices[2],

			indices[0],
			indices[1]
		);
		push_triangle(
			indices[1],
			indices[2],
			indices[3],

			indices[0],
			indices[1]
		);
	}
}
