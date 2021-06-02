////////////////////////////////////////////////////////////////////////////
//	Created		: 05.07.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "graph_generator_subdivider.h"
#include "graph_generator_predicates.h"
#include <xray/collision/geometry.h>
#include <xray/collision/geometry_instance.h>

using xray::ai::navigation::graph_generator_subdivider;
using xray::ai::navigation::triangles_mesh_type;
using xray::ai::navigation::triangles_type;
using xray::ai::navigation::navigation_triangle;
using xray::math::float3;

int tri_tri_intersect_with_isectline(float3 const& V0, float3 const& V1, float3 const& V2,
				     float3 const& U0, float3 const& U1, float3 const& U2, int *coplanar,
				     float3& isectpt1, float3& isectpt2);

graph_generator_subdivider::graph_generator_subdivider( triangles_mesh_type& triangles_mesh, u32 subdivision_max_operation_id ) :
	m_triangles_mesh					( triangles_mesh ),
	m_subdivision_max_operation_id		( subdivision_max_operation_id ),
	m_subdivision_current_operation_id	( 0 )
{
	subdivide_passable_triangles();
}

float3 const* select_equal_vertices	(
		triangles_mesh_type const& triangles_mesh,
		u32 const triangle_id,
		float3 const& testee
	);

u32 select_vertex_index	(
		float3 const& vertex,
		float3 const* (vertices)[3],
		u32 const* const indices
	)
{
	for (u32 i=0; i<3; ++i) {
		if ( vertex.is_similar(*vertices[i]) )
			return					indices[i];
	}

	return					 		u32(-1);
}

float distance_to_edge ( float3 const& vertex0, float3 const& vertex1, float3 const& point) 
{
	float3 const& t0_t1_direction		= normalize( vertex1 - vertex0 );
	float3 const& t0_s0					= point - vertex0;
	return								length( t0_s0 ^ t0_t1_direction );
}

bool snap_to_edge						( float3 const& vertex0, float3 const& vertex1, float3& point, float const epsilon )
{
	float3 const& t0_t1_direction		= normalize( vertex1 - vertex0 );
	float3 const& t0_s0					= point - vertex0;
	float const distance_to_edge_t0_t1	= length(t0_s0 ^ t0_t1_direction);
	
	if ( distance_to_edge_t0_t1 > epsilon )
		return							false;

	point								= vertex0 + t0_t1_direction*(t0_t1_direction | t0_s0);
	return								true;
}

void correct_point_impl	( triangles_mesh_type const& triangles_mesh, u32 const triangle_id, float3& point )
{
	using namespace xray::math;

	float const epsilon		= .01f;

	u32 const* indices		= &*triangles_mesh.indices.begin() + 3*triangle_id;
	float3 const* triangle_vertices[] = {
		&triangles_mesh.vertices[ indices[0] ],
		&triangles_mesh.vertices[ indices[1] ],
		&triangles_mesh.vertices[ indices[2] ],
	};
	float3 const& triangle_normal	= triangles_mesh.data[triangle_id].plane.normal;

	// 1. puth the point down to plane
	float const d			= -(*triangle_vertices[0] | triangle_normal);
	float const signed_distance_to_plane = (triangle_normal | point) + d;
	point					= point + triangle_normal*signed_distance_to_plane;
	R_ASSERT				( is_zero((triangle_normal | point) + d, epsilon_3) );

	// 2. check if the point is close to triangle vertices
	for ( u32 j=0; j<3; ++j ) {
		float3 const& vertex = *triangle_vertices[j];
		if ( !point.is_similar(vertex, epsilon_3) )
			continue;

		point				= vertex;
		return;
	}

	// 3. check if point is inside triangle
	float3 const new_normals[3] = {
		(point - *triangle_vertices[0]) ^ (point - *triangle_vertices[1]),
		(point - *triangle_vertices[1]) ^ (point - *triangle_vertices[2]),
		(point - *triangle_vertices[2]) ^ (point - *triangle_vertices[0]),
	};

	u32 out_mask	= 0;
	for (u32 i=0; i<3; ++i ) {
		if ( is_zero( new_normals[i].length() ) )
			continue;

		float3 const& new_normal	= normalize( new_normals[i] );
		if ( is_zero( (triangle_normal | new_normal) - 1.f, epsilon ) )
			continue;

//		R_ASSERT			( is_zero( (triangle_normal | -new_normal) - 1.f, epsilon) );
		out_mask			|= 1 << i;
	}

	if ( !out_mask ) {
		// 3.1 try to assign point to edge if it is close to it
	
		float const distances[3] = {
			distance_to_edge ( *triangle_vertices[0], *triangle_vertices[1], point ),
			distance_to_edge ( *triangle_vertices[1], *triangle_vertices[2], point ),
			distance_to_edge ( *triangle_vertices[2], *triangle_vertices[0], point )
		};
		
		u32 min = 0;
		for ( u32 i = 1; i < 3; ++i )
			if ( distances[i] < distances[min] )
				min = i;

//		if ( distances[min] > epsilon )
//			return;

		if ( min == 0 ) {
			snap_to_edge( *triangle_vertices[0], *triangle_vertices[1], point, epsilon);
		} else {
			if ( min == 1 )
				snap_to_edge( *triangle_vertices[1], *triangle_vertices[2], point, epsilon);
			else 
				snap_to_edge( *triangle_vertices[2], *triangle_vertices[0], point, epsilon);
		}
		return;
	}

	// 4. make point be on the triangle edge
	float3 const* vertices[2];
	if ( out_mask & (1 << 0) ) {
		if ( out_mask & (1 << 1) ) {
			point			= *triangle_vertices[1];
			return;
		}

		if ( out_mask & (1 << 2) ) {
			point			= *triangle_vertices[0];
			return;
		}

		vertices[0]			= triangle_vertices[0];
		vertices[1]			= triangle_vertices[1];
	}
	else {
		if ( !(out_mask & (1 << 1)) ) {
			R_ASSERT		( out_mask & (1 << 2) );
			vertices[0]		= triangle_vertices[2];
			vertices[1]		= triangle_vertices[0];
		}
		else {
			if ( out_mask & (1 << 2) ) {
				point		= *triangle_vertices[2];
				return;
			}

			vertices[0]		= triangle_vertices[1];
			vertices[1]		= triangle_vertices[2];
		}
	}

	// make point on edge
	float3 const& t0_t1				= *vertices[1] - *vertices[0];
	float const t0_t1_length		= t0_t1.length( );
	R_ASSERT						( !is_zero(t0_t1_length) );
	float3 const& t0_t1_direction	= t0_t1/t0_t1_length;
	float3 const& t0_s0				= point - *vertices[0];
	point							= *vertices[0] + t0_t1_direction*min( max( 0.f, t0_t1_direction | t0_s0 ), t0_t1_length ) ;
}

void correct_point	( triangles_mesh_type const& triangles_mesh, u32 const triangle_id, float3& point )
{
	using namespace xray::math;

	u32 const* indices		= &*triangles_mesh.indices.begin() + 3*triangle_id;
	float3 const* triangle_vertices[] = {
		&triangles_mesh.vertices[ indices[0] ],
		&triangles_mesh.vertices[ indices[1] ],
		&triangles_mesh.vertices[ indices[2] ],
	};
	float3 const& triangle_normal	= triangles_mesh.data[triangle_id].plane.normal;

	float3 const old_point			= point;
	point							= old_point;

	for (u32 i=0; i<100; ++i) {
		correct_point_impl	( triangles_mesh, triangle_id, point );

		if ( !( is_zero(((point - *triangle_vertices[0]) ^ (point - *triangle_vertices[1])).length()) || is_zero( 1.f - (triangle_normal | normalize((point - *triangle_vertices[0]) ^ (point - *triangle_vertices[1]))) ) ) )
			continue;

		if ( !( is_zero(((point - *triangle_vertices[1]) ^ (point - *triangle_vertices[2])).length()) || is_zero( 1.f - (triangle_normal | normalize((point - *triangle_vertices[1]) ^ (point - *triangle_vertices[2]))) ) ) )
			continue;

		if ( !( is_zero(((point - *triangle_vertices[2]) ^ (point - *triangle_vertices[0])).length()) || is_zero( 1.f - (triangle_normal | normalize((point - *triangle_vertices[2]) ^ (point - *triangle_vertices[0]))) ) ) )
			continue;

		break;
	}
/*
	R_ASSERT				( is_zero(((point - *triangle_vertices[0]) ^ (point - *triangle_vertices[1])).length()) || is_zero( 1.f - (triangle_normal | normalize((point - *triangle_vertices[0]) ^ (point - *triangle_vertices[1]))) ) );
	R_ASSERT				( is_zero(((point - *triangle_vertices[1]) ^ (point - *triangle_vertices[2])).length()) || is_zero( 1.f - (triangle_normal | normalize((point - *triangle_vertices[1]) ^ (point - *triangle_vertices[2]))) ) );
	R_ASSERT				( is_zero(((point - *triangle_vertices[2]) ^ (point - *triangle_vertices[0])).length()) || is_zero( 1.f - (triangle_normal | normalize((point - *triangle_vertices[2]) ^ (point - *triangle_vertices[0]))) ) );
*/
}

graph_generator_subdivider::buffer_indices_type::iterator const graph_generator_subdivider::select_triangle (
		u32 triangle_id, 
		buffer_indices_type& triangles,
		xray::math::float3& segment0,
		xray::math::float3& segment1
		)
{
	R_ASSERT				( !triangles.empty() );
	buffer_indices_type::iterator const result	= triangles.begin() + triangles.size() - 1;

	u32 const* indices0		= &*m_triangles_mesh.indices.begin() + 3*triangle_id;
	math::float3 const& v0	= m_triangles_mesh.vertices[ indices0[0] ];
	math::float3 const& v1	= m_triangles_mesh.vertices[ indices0[1] ];
	math::float3 const& v2	= m_triangles_mesh.vertices[ indices0[2] ];

	u32 const* indices1		= &*m_triangles_mesh.indices.begin() + 3*(*result);
	math::float3 const& u0	= m_triangles_mesh.vertices[ indices1[0] ];
	math::float3 const& u1	= m_triangles_mesh.vertices[ indices1[1] ];
	math::float3 const& u2	= m_triangles_mesh.vertices[ indices1[2] ];

	int coplanar;
	int const intersection_result	=
		tri_tri_intersect_with_isectline(
			v0, v1, v2, u0, u1, u2, &coplanar, segment0, segment1
		);
	R_ASSERT				( intersection_result && !coplanar && !segment0.is_similar(segment1, math::epsilon_7) );

	::correct_point			( m_triangles_mesh, triangle_id, segment0 );
	::correct_point			( m_triangles_mesh, triangle_id, segment1 );

	return					result;

}

bool is_on_segment	(
		xray::math::float3 const& testee,
		xray::math::float3 const& first_segment_vertex,
		xray::math::float3 const& second_segment_vertex	
	);

void graph_generator_subdivider::correct_obstructed_edge  ( 
			u32 const triangle_id,
			float3 const& vertex0,
			float3 const& vertex1
		) 
{
	u32 const* indices  = m_triangles_mesh.indices.begin() + triangle_id * 3;

	if ( is_on_segment ( m_triangles_mesh.vertices[ indices[0] ], vertex0, vertex1 ) )
		if ( is_on_segment ( m_triangles_mesh.vertices[ indices[1] ], vertex0, vertex1 ) ) {
			m_triangles_mesh.data[ triangle_id ].obstructions[0] = 1;
		}

	if ( is_on_segment ( m_triangles_mesh.vertices[ indices[1] ], vertex0, vertex1 ) )
		if ( is_on_segment ( m_triangles_mesh.vertices[ indices[2] ], vertex0, vertex1 ) ) {
			m_triangles_mesh.data[ triangle_id ].obstructions[1] = 1;
		}


	if ( is_on_segment ( m_triangles_mesh.vertices[ indices[2] ], vertex0, vertex1 ) )
		if ( is_on_segment ( m_triangles_mesh.vertices[ indices[0] ], vertex0, vertex1 ) ) {
			m_triangles_mesh.data[ triangle_id ].obstructions[2] = 1;
		}

}

bool graph_generator_subdivider::try_subdivide_triangle	(
		u32 triangle_id,
		buffer_indices_type const& triangles,
		xray::math::float3 const& triangle_vertex0,
		xray::math::float3 const& triangle_vertex1,
		xray::math::float3 const& triangle_vertex2,
		u32 const obstructed_edge_id
	)
{
	R_ASSERT					( !is_similar(triangle_vertex0, triangle_vertex1) );
	R_ASSERT					( !is_similar(triangle_vertex0, triangle_vertex2) );
	R_ASSERT					( !is_similar(triangle_vertex1, triangle_vertex2) );

	xray::math::float3 const b_minus_a	= normalize( triangle_vertex1 - triangle_vertex0 );
	xray::math::float3 const c_minus_a	= normalize( triangle_vertex2 - triangle_vertex0 );

	if ( b_minus_a.is_similar(c_minus_a) || b_minus_a.is_similar(-c_minus_a) )
		return					false;

	if ( m_subdivision_current_operation_id++ >= m_subdivision_max_operation_id )
		return					true;

	buffer_indices_type		new_triangles(
		ALLOCA( triangles.size()*sizeof(buffer_indices_type::value_type) ),
		triangles.size( ),
		triangles.begin( ),
		triangles.end( )
	);

	u32 const* const triangle_indices = &*m_triangles_mesh.indices.begin() + 3*triangle_id;
	float3 const* vertices[] = {
		&m_triangles_mesh.vertices[ triangle_indices[0] ],
		&m_triangles_mesh.vertices[ triangle_indices[1] ],
		&m_triangles_mesh.vertices[ triangle_indices[2] ],
	};
	u32 const indices[]			= {
		select_vertex_index( triangle_vertex0, vertices, triangle_indices ),
		select_vertex_index( triangle_vertex1, vertices, triangle_indices ),
		select_vertex_index( triangle_vertex2, vertices, triangle_indices ),
	};
	float3 const* triangle_vertices[] = {
		&triangle_vertex0,
		&triangle_vertex1,
		&triangle_vertex2,
	};

	for (u32 i=0; i<3; ++i) {
		if ( indices[i] != u32(-1) ) {
			m_triangles_mesh.indices.push_back( indices[i] );
			continue;
		}

		m_triangles_mesh.indices.push_back	( m_triangles_mesh.vertices.size() );
		m_triangles_mesh.vertices.push_back	( *triangle_vertices[i] );
	}

	u32 const new_triangle_id = m_triangles_mesh.data.size();
	m_triangles_mesh.data.push_back	( m_triangles_mesh.data[triangle_id] );

	for ( u32  i = 0; i < 3; ++i )
		m_triangles_mesh.data[ new_triangle_id ].obstructions[i] = 0;

	R_ASSERT_CMP (m_triangles_mesh.indices.size() / 3, ==, m_triangles_mesh.data.size() );

	if ( m_triangles_mesh.data[triangle_id].obstructions[0] > 0 )
		correct_obstructed_edge(
			new_triangle_id, 
			m_triangles_mesh.vertices[ m_triangles_mesh.indices[ 3*triangle_id+0 ]],
			m_triangles_mesh.vertices[ m_triangles_mesh.indices[ 3*triangle_id+1 ]] 
		);

	if ( m_triangles_mesh.data[triangle_id].obstructions[1] > 0 )
		correct_obstructed_edge(
			new_triangle_id, 
			m_triangles_mesh.vertices[ m_triangles_mesh.indices[ 3*triangle_id+1 ]],
			m_triangles_mesh.vertices[ m_triangles_mesh.indices[ 3*triangle_id+2 ]] 
		);

	if ( m_triangles_mesh.data[triangle_id].obstructions[2] > 0 )
		correct_obstructed_edge(
			new_triangle_id, 
			m_triangles_mesh.vertices[ m_triangles_mesh.indices[ 3*triangle_id+2 ]],
			m_triangles_mesh.vertices[ m_triangles_mesh.indices[ 3*triangle_id+0 ]] 
		);

	if ( obstructed_edge_id > 0 )
		m_triangles_mesh.data[ new_triangle_id ].obstructions[ obstructed_edge_id-1 ] = 1;

	subdivide_triangle				( new_triangle_id, new_triangles );

	return						true;
}

float3 const* select_equal_vertices	(
		float3 const& vertex0,
		float3 const& vertex1,
		float3 const& vertex2,
		float3 const& testee
	)
{
	if ( is_similar(vertex0, testee, xray::math::epsilon_3 ) ) {
		R_ASSERT				( vertex1 != testee );
		R_ASSERT				( vertex2 != testee );
		return					&vertex0;
	}
	
	if ( is_similar(vertex1, testee, xray::math::epsilon_3) ) {
		R_ASSERT				( vertex2 != testee );
		return					&vertex1;
	}

	if ( is_similar(vertex2, testee, xray::math::epsilon_3) )
		return					&vertex2;

	return						0;
}

u32 graph_generator_subdivider::get_similar_edge_id ( u32 const triangle_id0, u32 const triangle_id1 )
{
	u32 const* indices0 = m_triangles_mesh.indices.begin() + 3 * triangle_id0;
	u32 const* indices1 = m_triangles_mesh.indices.begin() + 3 * triangle_id1;

	for ( u32 i = 0; i < 3; ++i ) {
		u32 const vertex_index0 = indices0[i];
		u32 const vertex_index1 = indices0[(i+1)%3];

		for ( u32 j = 0; j < 3; ++j ) {
			if ( vertex_index0 == indices1[(j+1)%3] && vertex_index1 == indices1[j] )
				return i;
		}
	}
	return u32(-1);
}

void graph_generator_subdivider::correct_segment_vertex(float3& vertex, u32 const triangle_id )
{
	u32 const* indices = m_triangles_mesh.indices.begin() + 3 * triangle_id;

	float3 projections[3];
	u32 edge_id = u32(-1);
	float distance_to_edge = 0;
	for ( u32 i = 0; i < 3; ++i ) {
		float3 const& vertex0 = m_triangles_mesh.vertices[ indices[i] ];
		float3 const& vertex1 = m_triangles_mesh.vertices[ indices[(i+1) % 3] ];
		float3 k = vertex1 - vertex0;
		float3 p = vertex - vertex0;
		float l = squared_length( k );
		projections[i] = vertex0 + k * ( p | k ) * l;
		float distance = length( vertex - projections[i] );
		if ( i != 0 ) {
			if ( distance_to_edge > distance ) {
				distance_to_edge = distance;
				edge_id = i;
			}
		} else {
			edge_id = i;
			distance_to_edge = distance;
		}
	}

	static const float epsilon = xray::math::epsilon_3;
	if ( math::is_zero( distance_to_edge, epsilon ) ) {
		vertex = projections[ edge_id ];
	}

}

bool graph_generator_subdivider::subdivide_triangle_impl( u32 const triangle_id, buffer_indices_type& triangles )
{
	math::float3 const& triangle_normal = m_triangles_mesh.data[triangle_id].plane.normal;
	math::float3 const v0 = m_triangles_mesh.vertices[m_triangles_mesh.indices[ triangle_id*3 ]];
	math::float3 const v1 = m_triangles_mesh.vertices[m_triangles_mesh.indices[ triangle_id*3+1]];
	math::float3 const v2 = m_triangles_mesh.vertices[m_triangles_mesh.indices[ triangle_id*3+2]];

	float3 segment_vertices[2];
	u32 success_count = 0;
	for ( ; !success_count && !triangles.empty(); ) {
		buffer_indices_type::iterator const found = select_triangle( triangle_id, triangles, segment_vertices[0], segment_vertices[1] );
		R_ASSERT				( found != triangles.end() );
		triangles.erase			( found );

		u32 const similar_edge_id = get_similar_edge_id( triangle_id, *found );
		if ( similar_edge_id != u32(-1) ) {
			m_triangles_mesh.data[ triangle_id ].obstructions[similar_edge_id] = 1;
			if ( !triangles.empty() )
				continue;
			return false;
		}

		correct_segment_vertex( segment_vertices[0], triangle_id );
		correct_segment_vertex( segment_vertices[1], triangle_id );

		// preserve correct triangles vertices order
		math::float3 const* equal_vertices[2] = { 0, 0 };
		equal_vertices[0]					= select_equal_vertices	( v0, v1, v2, segment_vertices[0] );
		equal_vertices[1]					= select_equal_vertices	( v0, v1, v2, segment_vertices[1] );
		if ( equal_vertices[0] ) {
			if ( equal_vertices[1] ) {
				if ( (&v0 != equal_vertices[0]) && (&v0 != equal_vertices[1]) ) {
					m_triangles_mesh.data[ triangle_id ].obstructions[1] = 1;
					if ( !triangles.empty() )
						continue;
					return					false;
				}

				if ( (&v1 != equal_vertices[0]) && (&v1 != equal_vertices[1]) ) {
					m_triangles_mesh.data[ triangle_id ].obstructions[2] = 1;
					if ( !triangles.empty() )
						continue;
					return					false;
				}

				R_ASSERT					( (&v2 != equal_vertices[0]) && (&v2 != equal_vertices[1]) );
				m_triangles_mesh.data[ triangle_id ].obstructions[0] = 1;
				if ( !triangles.empty() )
					continue;
				return						false;
			}
		}

		if ( equal_vertices[0] || equal_vertices[1] ) {
			float3 const* const non_equal_vertex	= equal_vertices[0] ? &segment_vertices[1] : &segment_vertices[0];
			float3 const* const equal_vertex		= equal_vertices[0] ? equal_vertices[0] : equal_vertices[1];
			success_count			+= try_subdivide_triangle( triangle_id, triangles, v0,	v1,	*non_equal_vertex, (equal_vertex == &v0) ? 3 : ((equal_vertex == &v1)  ? 2 : 0) ) ? 1 : 0;
			success_count			+= try_subdivide_triangle( triangle_id, triangles, v1,	v2,	*non_equal_vertex, (equal_vertex == &v1) ? 3 : ((equal_vertex == &v2)  ? 2 : 0) ) ? 1 : 0;
			success_count			+= try_subdivide_triangle( triangle_id, triangles, v2,	v0,	*non_equal_vertex, (equal_vertex == &v2) ? 3 : ((equal_vertex == &v0)  ? 2 : 0) ) ? 1 : 0;

			continue;
		}

		R_ASSERT					( !segment_vertices[0].is_similar(segment_vertices[1], math::epsilon_7) );

		float3 const* triangle_vertices[3]	= { &v0, &v1, &v2};
		float3 const* vertices[3];
		float3 const* last_vertex_on_line_with_segment = 0;
		for (u32 iteration = 0; ; ++iteration ) {
			float3 const& s0_minus_t0 = segment_vertices[0] - *triangle_vertices[0];
			float3 const& s1_minus_t0 = segment_vertices[1] - *triangle_vertices[0];
			float3 s0_cross_s1_t0	= s0_minus_t0 ^ s1_minus_t0;
			if ( math::is_zero( s0_cross_s1_t0.length() ) ) {
				if ( !last_vertex_on_line_with_segment ) {
					last_vertex_on_line_with_segment	= triangle_vertices[0];

					R_ASSERT_CMP		( iteration, <, 1 );

					R_ASSERT			( triangle_vertices[0] == &v0 );
					R_ASSERT			( triangle_vertices[1] == &v1 );
					R_ASSERT			( triangle_vertices[2] == &v2 );

					triangle_vertices[0] = &v1;
					triangle_vertices[1] = &v2;
					triangle_vertices[2] = &v0;

					continue;
				}

				// segment lies completely on the edge
				// here we make this edge TV0 => TV1
				// and S0 will be closer to TV0 than S1
				if ( triangle_vertices[1] != last_vertex_on_line_with_segment ) {
					R_ASSERT			( triangle_vertices[0] != last_vertex_on_line_with_segment );
					R_ASSERT			( triangle_vertices[2] == last_vertex_on_line_with_segment );

					last_vertex_on_line_with_segment	= triangle_vertices[0];

					std::swap			( triangle_vertices[2], triangle_vertices[0] );
					std::swap			( triangle_vertices[2], triangle_vertices[1] );

					if ( s0_minus_t0.squared_length() < s1_minus_t0.squared_length() )
						std::swap		( segment_vertices[0], segment_vertices[1] );
				}
				else {
					if ( s0_minus_t0.squared_length() > s1_minus_t0.squared_length() )
						std::swap		( segment_vertices[0], segment_vertices[1] );
				}

				success_count			+= try_subdivide_triangle( triangle_id, triangles, *triangle_vertices[0],	segment_vertices[0],	*triangle_vertices[2] )	? 1 : 0;
				success_count			+= try_subdivide_triangle( triangle_id, triangles, segment_vertices[0],	segment_vertices[1],	*triangle_vertices[2], 1 )		? 1 : 0;
				success_count			+= try_subdivide_triangle( triangle_id, triangles, segment_vertices[1],	*triangle_vertices[1],	*triangle_vertices[2] )	? 1 : 0;
				break;
			}

			s0_cross_s1_t0.normalize( );
			bool const is_t0		= math::is_zero( (triangle_normal | s0_cross_s1_t0) - 1.f, math::epsilon_3);
//			R_ASSERT				( is_t0 || math::is_zero( (triangle_normal | -s0_cross_s1_t0) - 1.f, math::epsilon_3) );
			if ( !is_t0 )
				std::swap			( segment_vertices[0], segment_vertices[1] );

			float3 const& s0_minus_t1 = segment_vertices[0] - *triangle_vertices[1];
			float3 const& s1_minus_t1 = segment_vertices[1] - *triangle_vertices[1];
			float3 s0_cross_s1_t1	 = s0_minus_t1 ^ s1_minus_t1;
			if ( math::is_zero( s0_cross_s1_t1.length() ) ) {
				if ( !last_vertex_on_line_with_segment ) {
					last_vertex_on_line_with_segment	= triangle_vertices[1];

					R_ASSERT_CMP		( iteration, <, 1 );

					R_ASSERT			( triangle_vertices[0] == &v0 );
					R_ASSERT			( triangle_vertices[1] == &v1 );
					R_ASSERT			( triangle_vertices[2] == &v2 );

					triangle_vertices[0] = &v2;
					triangle_vertices[1] = &v0;
					triangle_vertices[2] = &v1;
					
					continue;
				}

				// segment lies completely on the edge
				// here we make this edge TV0 => TV1
				// and S0 will be closer to TV0 than S1
				R_ASSERT				( triangle_vertices[0] != last_vertex_on_line_with_segment );
				R_ASSERT				( triangle_vertices[2] == last_vertex_on_line_with_segment );

				if ( s0_minus_t1.squared_length() > s1_minus_t1.squared_length() )
					std::swap			( segment_vertices[0], segment_vertices[1] );

				success_count			+= try_subdivide_triangle( triangle_id, triangles, *triangle_vertices[1],	segment_vertices[0],	*triangle_vertices[0] )	? 1 : 0;
				success_count			+= try_subdivide_triangle( triangle_id, triangles, segment_vertices[0],	segment_vertices[1],	*triangle_vertices[0], 1 )		? 1 : 0;
				success_count			+= try_subdivide_triangle( triangle_id, triangles, segment_vertices[1],	*triangle_vertices[2],	*triangle_vertices[0] )	? 1 : 0;
				break;
			}

			s0_cross_s1_t1.normalize( );
			bool const is_t1		= math::is_zero( (triangle_normal | s0_cross_s1_t1) - 1.f, math::epsilon_3);
//			R_ASSERT				( is_t1 || math::is_zero( (triangle_normal | -s0_cross_s1_t1) - 1.f, math::epsilon_3) );
			if ( is_t1 ) {
				vertices[0]			= &v1;
				vertices[1]			= &v2;
				vertices[2]			= &v0;
			}
			else {
				vertices[0]			= &v0;
				vertices[1]			= &v1;
				vertices[2]			= &v2;
			}

			R_ASSERT_CMP			( iteration, <, 4 );

			success_count			+= try_subdivide_triangle( triangle_id, triangles, *vertices[0],	*vertices[1],			segment_vertices[0] )	? 1 : 0;
			success_count			+= try_subdivide_triangle( triangle_id, triangles, *vertices[1],	segment_vertices[1],	segment_vertices[0], 2 )		? 1 : 0;
			success_count			+= try_subdivide_triangle( triangle_id, triangles, *vertices[1],	*vertices[2],			segment_vertices[1] )	? 1 : 0;
			success_count			+= try_subdivide_triangle( triangle_id, triangles, *vertices[2],	*vertices[0],			segment_vertices[1] )	? 1 : 0;
			success_count			+= try_subdivide_triangle( triangle_id, triangles, *vertices[0],	segment_vertices[0],	segment_vertices[1], 2 )		? 1 : 0;
			break;
		}
	}

	if ( success_count )
		return						true;
	return							false;
}

class filter_non_intersected_predicate2 {
public:
	inline filter_non_intersected_predicate2			( triangles_mesh_type& triangles_mesh, u32 const triangle_id ) :
		m_triangles_mesh		( &triangles_mesh ),
		m_triangle_id			( triangle_id )
	{
	}

	inline bool operator( )							( u32 const triangle_id ) const
	{
		return					!intersect_triangles( *m_triangles_mesh, m_triangle_id, triangle_id, false );
	}

private:
	triangles_mesh_type const*	m_triangles_mesh;
	u32							m_triangle_id;
}; // struct filter_obstructions_predicate

void graph_generator_subdivider::subdivide_triangle( u32 const triangle_id, buffer_indices_type& triangles )
{

	triangles.erase					(
		std::remove_if(
			triangles.begin( ),
			triangles.end( ),
			filter_non_intersected_predicate2( m_triangles_mesh, triangle_id )
		),
		triangles.end( )
	);
	
	if ( triangles.empty() ) {
		return;
	}

	m_triangles_mesh.data[triangle_id].color = math::color(0, 255, 0, 64);
	if ( subdivide_triangle_impl( triangle_id, triangles ) ) {
		if (m_subdivision_current_operation_id++ < m_subdivision_max_operation_id )
			m_triangles_mesh.data[triangle_id].is_marked = true;
		return;
	}
}

void graph_generator_subdivider::subdivide_triangle ( u32 const triangle_id, triangles_type& triangles )
{

	u32 const triangles_size			= triangles.size( );
	buffer_indices_type buffer_indices	( ALLOCA( triangles_size*sizeof(u32) ), triangles_size, triangles_size );
	buffer_indices_type::iterator i		= buffer_indices.begin( );
	buffer_indices_type::iterator const e = buffer_indices.end( );
	triangles_type::const_iterator j	= triangles.begin( );
	for ( ; i != e; ++i)
		*i								= (*j++).triangle_id;

	subdivide_triangle					( triangle_id, buffer_indices );
}

class filter_obstructions_predicate2 {
public:
	inline filter_obstructions_predicate2	( triangles_mesh_type const& generator, xray::math::plane const& plane ) :
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

void graph_generator_subdivider::subdivide_passable_triangles()
{
	triangles_type triangles(debug::g_mt_allocator);
	
	u32 triangles_count = m_triangles_mesh.data.size();
	for (u32 i = 0; i<triangles_count; ++i) {
		if (!m_triangles_mesh.data[i].is_passable)
			continue;

		math::aabb						aabb(math::create_invalid_aabb());
		aabb.modify						( m_triangles_mesh.vertices[m_triangles_mesh.indices[3*i]]		);
		aabb.modify						( m_triangles_mesh.vertices[m_triangles_mesh.indices[3*i + 1]]	);
		aabb.modify						( m_triangles_mesh.vertices[m_triangles_mesh.indices[3*i + 2]]	);

		math::float3 const& center		= aabb.center( );
		math::float3 const& extents		= aabb.extents( );
		float const epsilon				= math::epsilon_3;
		aabb.modify						( center + float3( -epsilon - extents.x, -5.f - extents.y, -epsilon - extents.z ) );
		aabb.modify						( center + float3( +epsilon + extents.x,  5.f + extents.y, +epsilon + extents.z ) );

		triangles.clear();
		m_triangles_mesh.spatial_tree->aabb_query	( 0, aabb, triangles );
		triangles.erase				(
			std::remove_if(
				triangles.begin( ),
				triangles.end( ),
				filter_obstructions_predicate2( m_triangles_mesh, m_triangles_mesh.data[i].plane )
			),
			triangles.end( )
		);

		subdivide_triangle( i, triangles );
	}

	m_triangles_mesh.remove_marked_triangles ( );
}

//u32 graph_generator_subdivider::subdivide(triangles_mesh_type& triangles_mesh, u32 const triangles_id0, u32 const triangle_id1 )
//{
//	return						0;
//}

void push_triangle( triangles_mesh_type& triangles_mesh, u32 parent_id, float3 const v0, float3 const v1, float3 const v2 )
{

	R_ASSERT					( !is_similar(v0, v1) );
	R_ASSERT					( !is_similar(v0, v2) );
	R_ASSERT					( !is_similar(v1, v2) );

	xray::math::float3 const b_minus_a	= normalize( v1 - v0 );
	xray::math::float3 const c_minus_a	= normalize( v2 - v0 );

	if ( b_minus_a.is_similar(c_minus_a) || b_minus_a.is_similar(-c_minus_a) )
		return;

	u32 vertices_count = triangles_mesh.vertices.size();

	triangles_mesh.vertices.push_back ( v0 );
	triangles_mesh.vertices.push_back ( v1 );
	triangles_mesh.vertices.push_back ( v2 );

	triangles_mesh.indices.push_back( vertices_count    );
	triangles_mesh.indices.push_back( vertices_count + 1);
	triangles_mesh.indices.push_back( vertices_count + 2);

	triangles_mesh.data.push_back( triangles_mesh.data[ parent_id ] );
}

u32 graph_generator_subdivider::subdivide( triangles_mesh_type& triangles_mesh, u32 const triangle_id, float3 const u0, float3 const u1, float3 const u2 )
{
	math::float3 const& triangle_normal = triangles_mesh.data[triangle_id].plane.normal;
	math::float3 const v0 = triangles_mesh.vertices[triangles_mesh.indices[ triangle_id*3 ]];
	math::float3 const v1 = triangles_mesh.vertices[triangles_mesh.indices[ triangle_id*3+1]];
	math::float3 const v2 = triangles_mesh.vertices[triangles_mesh.indices[ triangle_id*3+2]];

	float3 segment_vertices[2];
//	u32 success_count = 0;

	int coplanar = 0;
	int result = tri_tri_intersect_with_isectline( 
		v0, v1, v2, 
		u0, u1, u2,
		&coplanar,
		segment_vertices[0],
		segment_vertices[1]
	);

	if ( result == 0 || coplanar )
		return 0;

	// preserve correct triangles vertices order
	math::float3 const* equal_vertices[2] = { 0, 0 };
	equal_vertices[0]					= select_equal_vertices	( v0, v1, v2, segment_vertices[0] );
	equal_vertices[1]					= select_equal_vertices	( v0, v1, v2, segment_vertices[1] );
	if ( equal_vertices[0] ) {
		if ( equal_vertices[1] ) {
			return						0;
		}
	}

	if ( equal_vertices[0] || equal_vertices[1] ) {
		float3 const* const non_equal_vertex	= equal_vertices[0] ? &segment_vertices[1] : &segment_vertices[0];
//		float3 const* const equal_vertex		= equal_vertices[0] ? equal_vertices[0] : equal_vertices[1];
		
		push_triangle( triangles_mesh, triangle_id, v0,	v1,	*non_equal_vertex );
		push_triangle( triangles_mesh, triangle_id, v1,	v2,	*non_equal_vertex );
		push_triangle( triangles_mesh, triangle_id, v2,	v0,	*non_equal_vertex );

		return 3;
	}

	R_ASSERT					( !segment_vertices[0].is_similar(segment_vertices[1], math::epsilon_7) );

	float3 const* triangle_vertices[3]	= { &v0, &v1, &v2};
	float3 const* vertices[3];
	float3 const* last_vertex_on_line_with_segment = 0;
	for (u32 iteration = 0; ; ++iteration ) {
		float3 const& s0_minus_t0 = segment_vertices[0] - *triangle_vertices[0];
		float3 const& s1_minus_t0 = segment_vertices[1] - *triangle_vertices[0];
		float3 s0_cross_s1_t0	= s0_minus_t0 ^ s1_minus_t0;
		if ( math::is_zero( s0_cross_s1_t0.length() ) ) {
			if ( !last_vertex_on_line_with_segment ) {
				last_vertex_on_line_with_segment	= triangle_vertices[0];

				R_ASSERT_CMP		( iteration, <, 1 );

				R_ASSERT			( triangle_vertices[0] == &v0 );
				R_ASSERT			( triangle_vertices[1] == &v1 );
				R_ASSERT			( triangle_vertices[2] == &v2 );

				triangle_vertices[0] = &v1;
				triangle_vertices[1] = &v2;
				triangle_vertices[2] = &v0;

				continue;
			}

			// segment lies completely on the edge
			// here we make this edge TV0 => TV1
			// and S0 will be closer to TV0 than S1
			if ( triangle_vertices[1] != last_vertex_on_line_with_segment ) {
				R_ASSERT			( triangle_vertices[0] != last_vertex_on_line_with_segment );
				R_ASSERT			( triangle_vertices[2] == last_vertex_on_line_with_segment );

				last_vertex_on_line_with_segment	= triangle_vertices[0];

				std::swap			( triangle_vertices[2], triangle_vertices[0] );
				std::swap			( triangle_vertices[2], triangle_vertices[1] );

				if ( s0_minus_t0.squared_length() < s1_minus_t0.squared_length() )
					std::swap		( segment_vertices[0], segment_vertices[1] );
			}
			else {
				if ( s0_minus_t0.squared_length() > s1_minus_t0.squared_length() )
					std::swap		( segment_vertices[0], segment_vertices[1] );
			}

			push_triangle ( triangles_mesh, triangle_id, *triangle_vertices[0],	segment_vertices[0],	*triangle_vertices[2] );
			push_triangle ( triangles_mesh, triangle_id, segment_vertices[0],	segment_vertices[1],	*triangle_vertices[2] );
			push_triangle ( triangles_mesh, triangle_id, segment_vertices[1],	*triangle_vertices[1],	*triangle_vertices[2] );
			return 3;
		}

		s0_cross_s1_t0.normalize( );
		bool const is_t0		= math::is_zero( (triangle_normal | s0_cross_s1_t0) - 1.f, math::epsilon_3);
		//			R_ASSERT				( is_t0 || math::is_zero( (triangle_normal | -s0_cross_s1_t0) - 1.f, math::epsilon_3) );
		if ( !is_t0 )
			std::swap			( segment_vertices[0], segment_vertices[1] );

		float3 const& s0_minus_t1 = segment_vertices[0] - *triangle_vertices[1];
		float3 const& s1_minus_t1 = segment_vertices[1] - *triangle_vertices[1];
		float3 s0_cross_s1_t1	 = s0_minus_t1 ^ s1_minus_t1;
		if ( math::is_zero( s0_cross_s1_t1.length() ) ) {
			if ( !last_vertex_on_line_with_segment ) {
				last_vertex_on_line_with_segment	= triangle_vertices[1];

				R_ASSERT_CMP		( iteration, <, 1 );

				R_ASSERT			( triangle_vertices[0] == &v0 );
				R_ASSERT			( triangle_vertices[1] == &v1 );
				R_ASSERT			( triangle_vertices[2] == &v2 );

				triangle_vertices[0] = &v2;
				triangle_vertices[1] = &v0;
				triangle_vertices[2] = &v1;

				continue;
			}

			// segment lies completely on the edge
			// here we make this edge TV0 => TV1
			// and S0 will be closer to TV0 than S1
			R_ASSERT				( triangle_vertices[0] != last_vertex_on_line_with_segment );
			R_ASSERT				( triangle_vertices[2] == last_vertex_on_line_with_segment );

			if ( s0_minus_t1.squared_length() > s1_minus_t1.squared_length() )
				std::swap			( segment_vertices[0], segment_vertices[1] );

			push_triangle ( triangles_mesh, triangle_id, *triangle_vertices[1],	segment_vertices[0],	*triangle_vertices[0] );
			push_triangle ( triangles_mesh, triangle_id, segment_vertices[0],	segment_vertices[1],	*triangle_vertices[0] );
			push_triangle ( triangles_mesh, triangle_id, segment_vertices[1],	*triangle_vertices[2],	*triangle_vertices[0] );
			return 3;
		}

		s0_cross_s1_t1.normalize( );
		bool const is_t1		= math::is_zero( (triangle_normal | s0_cross_s1_t1) - 1.f, math::epsilon_3);
		//			R_ASSERT				( is_t1 || math::is_zero( (triangle_normal | -s0_cross_s1_t1) - 1.f, math::epsilon_3) );
		if ( is_t1 ) {
			vertices[0]			= &v1;
			vertices[1]			= &v2;
			vertices[2]			= &v0;
		}
		else {
			vertices[0]			= &v0;
			vertices[1]			= &v1;
			vertices[2]			= &v2;
		}

		R_ASSERT_CMP			( iteration, <, 4 );

		push_triangle ( triangles_mesh, triangle_id, *vertices[0],	*vertices[1],			segment_vertices[0] );
		push_triangle ( triangles_mesh, triangle_id, *vertices[1],	segment_vertices[1],	segment_vertices[0] );
		push_triangle ( triangles_mesh, triangle_id, *vertices[1],	*vertices[2],			segment_vertices[1] );
		push_triangle ( triangles_mesh, triangle_id, *vertices[2],	*vertices[0],			segment_vertices[1] );
		push_triangle ( triangles_mesh, triangle_id, *vertices[0],	segment_vertices[0],	segment_vertices[1] );
		return 5;
	}
}
