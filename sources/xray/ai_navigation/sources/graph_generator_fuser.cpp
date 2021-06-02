////////////////////////////////////////////////////////////////////////////
//	Created		: 16.08.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "graph_generator_fuser.h"
#include "delaunay_triangulator.h"
#include "graph_generator_adjacency_builder.h"
#include <xray/collision/geometry.h>
#include <xray/collision/geometry_instance.h>

using xray::ai::navigation::graph_generator_fuser;
using xray::ai::navigation::triangles_mesh_type;
using xray::ai::navigation::triangles_type;
using xray::ai::navigation::navigation_triangle;
using xray::math::float3;
using xray::math::is_zero;
using xray::math::is_similar;

inline void graph_generator_fuser::push_triangle( 
		u32 const parent_triangle_id,
		u32 const v0,
		u32 const v1,
		u32 const v2
	)
{
	R_ASSERT ( !is_zero ( length(m_triangles_mesh.vertices[ v0 ] - m_triangles_mesh.vertices[ v1 ])) );
	R_ASSERT ( !is_zero ( length(m_triangles_mesh.vertices[ v0 ] - m_triangles_mesh.vertices[ v2 ])) );
	R_ASSERT ( !is_zero ( length(m_triangles_mesh.vertices[ v1 ] - m_triangles_mesh.vertices[ v2 ])) );

	if  ( m_fuser_current_operation_id++ >= m_fuser_max_operation_id )
		return;

	m_triangles_mesh.indices.push_back( v0 );
	m_triangles_mesh.indices.push_back( v1 );
	m_triangles_mesh.indices.push_back( v2 );

	u32 const new_triangle_id = m_triangles_mesh.data.size();

	m_triangles_mesh.data.push_back( navigation_triangle() );
	m_triangles_mesh.data[ new_triangle_id ].is_passable = true;
	m_triangles_mesh.data[ new_triangle_id ].plane = math::create_plane(m_triangles_mesh.vertices[ v0 ], m_triangles_mesh.vertices[ v1 ], m_triangles_mesh.vertices[ v2 ]);
	m_triangles_mesh.data[ new_triangle_id ].color = math::color( 255, 255, 255, 128 );


	m_triangles_info.push_back ( triangle_info () );

	if ( parent_triangle_id != u32(-1) ) {
		if ( m_triangles_info[ parent_triangle_id ].next_triangle != u32(-1) ) {
			m_triangles_info[ new_triangle_id ].next_triangle = m_triangles_info[ parent_triangle_id ].next_triangle;
		}
		m_triangles_info[ parent_triangle_id ].next_triangle = new_triangle_id;
		m_triangles_mesh.data[ parent_triangle_id ].is_marked = true;
	}

}

inline u32 graph_generator_fuser::create_vertex ( float3 const& vertex )
{
	m_triangles_mesh.vertices.push_back( vertex );
	return m_triangles_mesh.vertices.size() - 1;
}

inline void graph_generator_fuser::connect_triangles( 
		u32 const triangle_id0,
		u32 const triangle_id1,
		u32 const edge_id
	)
{
	
	if ( triangle_id1 == u32(-1) ) {
		m_triangles_mesh.data[ triangle_id0 ].neighbours[ edge_id ] = u32(-1);
		return;
	}

	u32 const* indices0 = m_triangles_mesh.indices.begin() + triangle_id0 * 3;
	u32 const* indices1 = m_triangles_mesh.indices.begin() + triangle_id1 * 3;
	for ( u32 i = 0; i < 3; ++i ) {
		if ( indices0[ edge_id ] == indices1[ (i+1) % 3 ]
			&& indices0[ (edge_id + 1) % 3 ] == indices1[ i ] )
		{
			m_triangles_mesh.data[ triangle_id0 ].neighbours[ edge_id ] = triangle_id1;
			m_triangles_mesh.data[ triangle_id1 ].neighbours[ i ] = triangle_id0;
			return;
		}
	}

	NODEFAULT	();
}

void graph_generator_fuser::subdivide_triangle( 
		u32 const triangle_id,
		u32 const edge_id,
		u32 const connection_triangle_id,
		u32 const connection_vertex_index0,
		u32 const connection_vertex_index1
	)
{

	u32 const edge_vertex_index0 = m_triangles_mesh.indices[ triangle_id * 3 + ((edge_id + 0) % 3 ) ];
	u32 const edge_vertex_index1 = m_triangles_mesh.indices[ triangle_id * 3 + ((edge_id + 1) % 3 ) ];
	u32 const third_vertex_index = m_triangles_mesh.indices[ triangle_id * 3 + ((edge_id + 2) % 3 ) ];

	bool similar[] = {
		connection_vertex_index0 == edge_vertex_index0,
		connection_vertex_index1 == edge_vertex_index1
	};

	u32 new_triangle_ids[] = { u32(-1), u32(-1), u32(-1) };

	if ( !similar[0] ) {
		push_triangle (
			triangle_id,
			edge_vertex_index0,
			connection_vertex_index0,
			third_vertex_index
		);
		new_triangle_ids[0] = m_triangles_mesh.data.size() - 1;
		connect_triangles ( 
			new_triangle_ids[0], 
			m_triangles_mesh.data[ triangle_id ].neighbours[ ( edge_id + 2) % 3 ],
			2
		);
	}

	if ( !similar[1] ) {
		push_triangle (
			triangle_id,
			connection_vertex_index1,
			edge_vertex_index1,
			third_vertex_index
		);
		new_triangle_ids[1] = m_triangles_mesh.data.size() - 1;
		connect_triangles ( 
			new_triangle_ids[1], 
			m_triangles_mesh.data[ triangle_id ].neighbours[ ( edge_id + 1) % 3 ],
			1
		);
	}
	R_ASSERT ( connection_vertex_index0 != connection_vertex_index1 );

	if ( !similar[0] || !similar[1] ) {
		push_triangle (
			triangle_id,
			connection_vertex_index0,
			connection_vertex_index1,
			third_vertex_index
		);
		new_triangle_ids[2] = m_triangles_mesh.data.size() - 1;
		
		if ( !similar[0] )
			connect_triangles (	new_triangle_ids[2], new_triangle_ids[0], 2 );
		else
			connect_triangles (	new_triangle_ids[2], m_triangles_mesh.data[ triangle_id ].neighbours[ ( edge_id + 2) % 3 ], 2 );

		if ( !similar[1] )
			connect_triangles (	new_triangle_ids[2], new_triangle_ids[1], 1 );
		else
			connect_triangles (	new_triangle_ids[2], m_triangles_mesh.data[ triangle_id ].neighbours[ ( edge_id + 1) % 3 ], 1 );

		connect_triangles ( new_triangle_ids[2], connection_triangle_id, 0 );
	} else {
		connect_triangles ( triangle_id, connection_triangle_id, edge_id );
	}

}


#define EPSILON 0.000001
// dist3D_Segment_to_Segment():
//    Input:  two 3D line segments S1 and S2
//    Return: the shortest distance between S1 and S2
float segment_to_segment_distance ( 
		float3 const& v0, 
		float3 const& v1, 
		float3 const& u0, 
		float3 const& u1 
	)
{
	float3   u = v1 - v0;
	float3   v = u1 - u0;
	float3   w = v0 - u0;

	float a = u | u;
	float b = u | v;
	float c = v | v;
	float d = u | w;
	float e = v | w;
	float D = a*c - b*b;
    float    sc, sN, sD = D;      // sc = sN / sD, default sD = D >= 0
    float    tc, tN, tD = D;      // tc = tN / tD, default tD = D >= 0

    // compute the line parameters of the two closest points
	if (D < EPSILON) { // the lines are almost parallel
		sN = 0.0;        // force using point P0 on segment S1
		sD = 1.0;        // to prevent possible division by 0.0 later
		tN = e;
		tD = c;
	}
	else {                // get the closest points on the infinite lines
		sN = (b*e - c*d);
		tN = (a*e - b*d);
		if (sN < 0.0) {       // sc < 0 => the s=0 edge is visible
			sN = 0.0;
			tN = e;
			tD = c;
		}
		else if (sN > sD) {  // sc > 1 => the s=1 edge is visible
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	if (tN < 0.0) {           // tc < 0 => the t=0 edge is visible
		tN = 0.0;
		// recompute sc for this edge
		if (-d < 0.0)
			sN = 0.0;
		else if (-d > a)
			sN = sD;
		else {
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD) {      // tc > 1 => the t=1 edge is visible
		tN = tD;
		// recompute sc for this edge
		if ((-d + b) < 0.0)
			sN = 0;
		else if ((-d + b) > a)
			sN = sD;
		else {
			sN = (-d + b);
			sD = a;
		}
	}
	// finally do the division to get sc and tc
	sc = (xray::math::abs(sN) < EPSILON ? 0.0f : sN / sD);
	tc = (xray::math::abs(tN) < EPSILON ? 0.0f : tN / tD);

	// get the difference of the two closest points
	float3   dP = w + (sc * u) - (tc * v);  // = S1(sc) - S2(tc)

	return length(dP);   // return the closest distance
}

bool graph_generator_fuser::try_fuse_edge ( 
		u32 const triangle_id0, 
		u32 const triangle_id1, 
		u32 const edge_id // Edge from triangle_id0
	)
{
	u32 const indices0[] = {
		m_triangles_mesh.indices[ triangle_id0 * 3 + 0],
		m_triangles_mesh.indices[ triangle_id0 * 3 + 1],
		m_triangles_mesh.indices[ triangle_id0 * 3 + 2]
	};

	u32 const indices1[] = {
		m_triangles_mesh.indices[ triangle_id1 * 3 + 0],
		m_triangles_mesh.indices[ triangle_id1 * 3 + 1],
		m_triangles_mesh.indices[ triangle_id1 * 3 + 2]
	};

	float3 const v0 = m_triangles_mesh.vertices[ indices0[ (edge_id + 0) % 3 ] ];
	float3 const v1 = m_triangles_mesh.vertices[ indices0[ (edge_id + 1) % 3 ] ];

	float3 d	= v1 - v0;
	float d_length = length ( d );
	d /= d_length;

	for ( u32 i = 0; i < 3; ++i ) {
		if ( m_triangles_mesh.data[ triangle_id1 ].neighbours[i] != u32(-1) )
			continue;

		// Check angle and distance between edges
		float3 u0 = m_triangles_mesh.vertices[ indices1[ (i+0)%3 ] ];
		float3 u1 = m_triangles_mesh.vertices[ indices1[ (i+1)%3 ] ];
		
		// Implicity swapped
		// And u0 and u1 are also implicity swapped
		float p1 = (u0 - v0) | d;
		float p0 = (u1 - v0) | d;

		// Skip this edge if it has wrong order
		if ( p1 < p0 )
			continue;

		// Projection robustness checks
		if ( is_zero( p0 ) ) 
			p0 = 0;
		if ( is_similar( p1, d_length ) )
			p1 = d_length;

		// Projection is outside of triangle segment
		if ( p1 <= 0.0f || p0 >= d_length )
			continue;

		static float const distance_epsilon = 0.2f;
		float distance = segment_to_segment_distance ( v0, v1, u0, u1 );
		if ( distance > distance_epsilon )
			continue;

		float3 const u0u1 = normalize( u0 - u1 );
		
		static float const angle_epsilon = 0.9f;
		if ( (u0u1 | d) < angle_epsilon )
			continue;
			
		u32 connection_vertices_indices[4];
		if ( p0 <= 0 ) {
			connection_vertices_indices[0] = indices0[ (edge_id + 0) % 3 ];
			if (p0 == 0)
				connection_vertices_indices[2] = indices1[ (i + 1) % 3 ];
			else
				connection_vertices_indices[2] = create_vertex ( u1 + ((v0 - u1) | u0u1) * u0u1 );
		} else {
			connection_vertices_indices[0] = create_vertex ( v0 + p0 * d );
			connection_vertices_indices[2] = indices1[ (i + 1) % 3 ];
		}
		if ( p1 >= d_length ) {
			connection_vertices_indices[1] = indices0[ (edge_id + 1) % 3 ];
			if ( p1 == d_length )
				connection_vertices_indices[3] = indices1[ (i + 0) % 3 ];
			else
				connection_vertices_indices[3] = create_vertex( u1 + ((v1 - u1) | u0u1) * u0u1 );
		} else {
			connection_vertices_indices[1] = create_vertex( v0 + p1 * d );
			connection_vertices_indices[3] = indices1[ (i + 0) % 3 ];
		}

		u32 connection_triangle_ids[2] = { u32(-1), u32(-1) };
		
		R_ASSERT_CMP ( connection_vertices_indices[1], !=, connection_vertices_indices[2] );

		if ( connection_vertices_indices[0] != connection_vertices_indices[2] ) {
			push_triangle ( 
				u32(-1),
				connection_vertices_indices[2],
				connection_vertices_indices[1],
				connection_vertices_indices[0] 
			);
			connection_triangle_ids[0] = m_triangles_mesh.data.size() - 1;
		}

		if ( connection_vertices_indices[3] != connection_vertices_indices[1] ) {
			push_triangle ( 
				u32(-1),
				connection_vertices_indices[1],
				connection_vertices_indices[2], 
				connection_vertices_indices[3]
			);
			connection_triangle_ids[1] = m_triangles_mesh.data.size() - 1;
		}

		if ( connection_triangle_ids[0] == u32(-1) ) {
			R_ASSERT_CMP ( connection_triangle_ids[1], !=, u32(-1) );
			connection_triangle_ids[0] = connection_triangle_ids[1];
		}
		if ( connection_triangle_ids[1] == u32(-1) ) {
			R_ASSERT_CMP ( connection_triangle_ids[0], !=, u32(-1) );
			connection_triangle_ids[1] = connection_triangle_ids[0];
		}
		if ( connection_triangle_ids[0] != connection_triangle_ids[1] )
			connect_triangles ( connection_triangle_ids[0], connection_triangle_ids[1], 0 );

		subdivide_triangle (
			triangle_id0,
			edge_id,
			connection_triangle_ids[0],
			connection_vertices_indices[0],
			connection_vertices_indices[1]
		);

		subdivide_triangle (
			triangle_id1,
			i,
			connection_triangle_ids[1],
			connection_vertices_indices[3],
			connection_vertices_indices[2]
		);

		m_triangles_mesh.data[ triangle_id0 ].color = math::color( 0, 255, 0, 128 );
		m_triangles_mesh.data[ triangle_id1 ].color = math::color( 0, 255, 0, 128 );

		return true;
	}

	return false;
}

u32 graph_generator_fuser::calculate_childs_count ( u32 triangle_id )
{
	u32 count = 0;
	while ( triangle_id != u32(-1) ) {
		if ( !m_triangles_mesh.data[ triangle_id ].is_marked )
			++count;
		triangle_id = m_triangles_info[triangle_id].next_triangle;
	}
	return count;
}

u32 graph_generator_fuser::get_next_unprocessed_triangle( u32 triangle_id )
{
	while ( triangle_id != u32(-1) ) {
		if ( !m_triangles_mesh.data[ triangle_id ].is_marked )
			return triangle_id;
		triangle_id = m_triangles_info[ triangle_id ].next_triangle;
	}
	return u32(-1);
}

void graph_generator_fuser::collect_child_triangles( u32 triangle_id, triangle_indices_type& triangle_indices )
{
	while ( triangle_id != u32(-1) ) {
		if ( !m_triangles_mesh.data[ triangle_id ].is_marked )
			triangle_indices.push_back( triangle_id );
		triangle_id = m_triangles_info[ triangle_id ].next_triangle;
	}
}

void graph_generator_fuser::fuse_triangle_edges ( 
		u32 const triangle_id, 
		triangles_type& intersected_triangles
	)
{
	triangles_type::iterator i = intersected_triangles.begin();
	triangles_type::iterator const e = intersected_triangles.end();
	for ( ; i != e; ++i ) {

		if ( triangle_id >= (*i).triangle_id ) {
			continue;
		}

		u32 current_triangle_id = get_next_unprocessed_triangle( triangle_id );
		
		if ( current_triangle_id == u32(-1) )
			break;

		bool done = false;
		while ( (current_triangle_id != u32(-1)) && (!done) ) {
			u32 const childs_count = calculate_childs_count( (*i).triangle_id );
			triangle_indices_type child_triangles(
				ALLOCA( childs_count * sizeof( u32 ) ),
				childs_count
			);

			collect_child_triangles ( (*i).triangle_id, child_triangles );

			edge_ids_type edge_ids;
			get_edges_for_fuse( current_triangle_id, edge_ids );

			triangle_indices_type::iterator k = child_triangles.begin();
			triangle_indices_type::iterator const e = child_triangles.end();
			for ( ; k != e; ++k ) {
				edge_ids_type::iterator j = edge_ids.begin();
				for ( edge_ids_type::const_iterator const e = edge_ids.end(); j != e; ++j ) {
					if ( try_fuse_edge ( current_triangle_id, (*k), (*j) ) ) {
						done = true;
						break;
					}
				}
				if ( done )
					break;
			}
			current_triangle_id = get_next_unprocessed_triangle( m_triangles_info[current_triangle_id].next_triangle );
		}
	}

}

void graph_generator_fuser::get_edges_for_fuse( 
		u32 const triangle_id, 
		edge_ids_type& edge_ids 
	)
{
	for ( u32 i = 0; i < 3; ++i ) {
		if ( m_triangles_mesh.data[triangle_id].neighbours[i] == u32(-1)
			&& !m_triangles_mesh.data[triangle_id].is_obstructed(i) ) {
			edge_ids.push_back( i );
		}
	}
}

struct filter_non_fusable_triangles_predicate {
	filter_non_fusable_triangles_predicate( triangles_mesh_type const& triangles_mesh ) :
		m_triangles_mesh( &triangles_mesh )
	{
	}

	inline bool operator() ( xray::collision::triangle_result const& triangle ) 
	{
		if ( !(*m_triangles_mesh).data[ triangle.triangle_id ].is_passable )
			return true;

		for ( u32 i = 0; i < 3; ++i )
			if ( (*m_triangles_mesh).data[ triangle.triangle_id ].neighbours[i] == u32(-1) 
				&& !(*m_triangles_mesh).data[ triangle.triangle_id ].is_obstructed(i) 
			)
				return false;
		return true;
	}

private:
	triangles_mesh_type const* m_triangles_mesh;

}; // struct filter_non_fusable_triangles_predicate

graph_generator_fuser::graph_generator_fuser( triangles_mesh_type& triangles_mesh, u32 const fuser_max_operation_id ) :
	m_triangles_mesh				( triangles_mesh ),
	m_fuser_current_operation_id	( 0 ),
	m_fuser_max_operation_id		( fuser_max_operation_id )
{
	triangles_type triangles ( debug::g_mt_allocator );

	u32 const triangles_count = m_triangles_mesh.data.size();
	m_triangles_info.resize ( triangles_count );

	graph_generator_adjacency_builder ( 
		m_triangles_mesh.indices, 
		m_triangles_mesh.data,
		m_triangles_mesh.data,
		.8f 
	);

	for ( u32 i = 0; i < triangles_count; ++i ) {
		
		if ( !m_triangles_mesh.data[ i ].is_passable )
			continue;

		edge_ids_type edge_ids;
		get_edges_for_fuse( i, edge_ids );

		if ( edge_ids.empty() )
			continue;
		
		math::aabb aabb(math::create_invalid_aabb());
		aabb.modify						( m_triangles_mesh.vertices[m_triangles_mesh.indices[3*i]]		);
		aabb.modify						( m_triangles_mesh.vertices[m_triangles_mesh.indices[3*i + 1]]	);
		aabb.modify						( m_triangles_mesh.vertices[m_triangles_mesh.indices[3*i + 2]]	);

		math::float3 const& center		= aabb.center( );
		math::float3 const& extents		= aabb.extents( );
		float const epsilon				= 0.5f;
		aabb.modify						( center + float3( -epsilon - extents.x, -5.f - extents.y, -epsilon - extents.z ) );
		aabb.modify						( center + float3( +epsilon + extents.x,  5.f + extents.y, +epsilon + extents.z ) );

		triangles.clear();
		m_triangles_mesh.spatial_tree->aabb_query ( 0, aabb, triangles);

		triangles.erase(
			std::remove_if (
				triangles.begin(),
				triangles.end(),
				filter_non_fusable_triangles_predicate( m_triangles_mesh )
			),
			triangles.end()
		);

		fuse_triangle_edges					( i, triangles );
	}

	m_triangles_mesh.remove_marked_triangles ( );
}
