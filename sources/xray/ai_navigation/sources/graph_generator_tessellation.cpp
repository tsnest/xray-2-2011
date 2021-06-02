////////////////////////////////////////////////////////////////////////////
//	Created		: 27.08.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if XRAY_DEBUG_ALLOCATOR
#include "graph_generator.h"
#include "graph_generator_predicates.h"
#include <xray/math_randoms_generator.h>

using xray::ai::navigation::graph_generator;
using xray::ai::navigation::triangles_mesh_type;
using xray::ai::navigation::navigation_triangle;
using xray::ai::navigation::is_marked_predicate;
using xray::ai::navigation::filter_non_passable_predicate;
using xray::math::float3;

static inline float distance_to_edge ( float3 const& vertex0, float3 const& vertex1, float3 const& point) 
{
	float3 const& t0_t1_direction		= normalize( vertex1 - vertex0 );
	float3 const& t0_s0					= point - vertex0;
	return length( t0_s0 ^ t0_t1_direction );
}

bool snap_to_edge	( float3 const& vertex0, float3 const& vertex1, float3& point, float const epsilon )
{
	float3 const& t0_t1_direction		= normalize( vertex1 - vertex0 );
	float3 const& t0_s0					= point - vertex0;
	float const distance_to_edge_t0_t1	= length(t0_s0 ^ t0_t1_direction);
	
	if ( distance_to_edge_t0_t1 > epsilon )
		return							false;

	point								= vertex0 + t0_t1_direction*(t0_t1_direction | t0_s0);
	return								true;
}

static inline bool is_same_side				(
		float3 const& p1,
		float3 const& p2,
		float3 const& a,
		float3 const& b
	)
{
	return	(((b - a) ^ (p1 - a)) | ((b - a) ^ (p2 - a))) >= 0.f;
}

static inline bool is_point_inside_triangle	(
		float3 const& p,
		float3 const& a,
		float3 const& b,
		float3 const& c
	)
{
	return	is_same_side( p, a, b, c) && is_same_side( p, b, a, c) && is_same_side( p, c, a, b);
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

		R_ASSERT			( is_zero( (triangle_normal | -new_normal) - 1.f, epsilon) );
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

	R_ASSERT				( is_zero(((point - *triangle_vertices[0]) ^ (point - *triangle_vertices[1])).length()) || is_zero( 1.f - (triangle_normal | normalize((point - *triangle_vertices[0]) ^ (point - *triangle_vertices[1]))) ) );
	R_ASSERT				( is_zero(((point - *triangle_vertices[1]) ^ (point - *triangle_vertices[2])).length()) || is_zero( 1.f - (triangle_normal | normalize((point - *triangle_vertices[1]) ^ (point - *triangle_vertices[2]))) ) );
	R_ASSERT				( is_zero(((point - *triangle_vertices[2]) ^ (point - *triangle_vertices[0])).length()) || is_zero( 1.f - (triangle_normal | normalize((point - *triangle_vertices[2]) ^ (point - *triangle_vertices[0]))) ) );
}

int tri_tri_intersect_with_isectline(float3 const& V0, float3 const& V1, float3 const& V2,
					 float3 const& U0, float3 const& U1, float3 const& U2, int *coplanar,
					 float3& isectpt1, float3& isectpt2);

graph_generator::buffer_indices_type::iterator
	graph_generator::select_triangle	(
		triangles_mesh_type const& triangles_mesh,
		graph_generator::buffer_indices_type& intersected_triangles,
		u32 const triangle_id,
		float3& segment0,
		float3& segment1,
		bool& is_coplanar
	)
{
	R_ASSERT				( !intersected_triangles.empty() );
	buffer_indices_type::iterator const result	= intersected_triangles.begin() + intersected_triangles.size() - 1;

	u32 const* indices0		= &*triangles_mesh.indices.begin() + 3*triangle_id;
	math::float3 const& v0	= triangles_mesh.vertices[ indices0[0] ];
	math::float3 const& v1	= triangles_mesh.vertices[ indices0[1] ];
	math::float3 const& v2	= triangles_mesh.vertices[ indices0[2] ];

	u32 const* indices1		= &*triangles_mesh.indices.begin() + 3*(*result);
	math::float3 const& u0	= triangles_mesh.vertices[ indices1[0] ];
	math::float3 const& u1	= triangles_mesh.vertices[ indices1[1] ];
	math::float3 const& u2	= triangles_mesh.vertices[ indices1[2] ];

	int coplanar;
	int const intersection_result	=
		tri_tri_intersect_with_isectline(
			v0, v1, v2, u0, u1, u2, &coplanar, segment0, segment1
		);

	is_coplanar				= coplanar != 0;
	if ( is_coplanar )
		return				result;

	R_ASSERT				( intersection_result && !coplanar && !segment0.is_similar(segment1, .01f) );

	correct_point			( triangles_mesh, triangle_id, segment0 );
	correct_point			( triangles_mesh, triangle_id, segment1 );
																						
	return					result;
}

float3 const* select_equal_vertices	(
		triangles_mesh_type const& triangles_mesh,
		u32 const triangle_id,
		float3 const& testee
	);

bool graph_generator::process_degenerated_cases	(
		triangles_mesh_type& triangles_mesh,
		u32 const triangle_id,
		buffer_indices_type& intersected_triangles,
		float3 const (&segment_vertices)[2],
		u32& success_count,
		bool& should_continue
	)
{
	math::float3 const* equal_vertices[2] = { 0, 0 };
	equal_vertices[0]			= ::select_equal_vertices	( triangles_mesh, triangle_id, segment_vertices[0] );
	equal_vertices[1]			= ::select_equal_vertices	( triangles_mesh, triangle_id, segment_vertices[1] );

	float3 const& vertex0		= triangles_mesh.vertices[ triangles_mesh.indices[3*triangle_id + 0] ];
	float3 const& vertex1		= triangles_mesh.vertices[ triangles_mesh.indices[3*triangle_id + 1] ];
	float3 const& vertex2		= triangles_mesh.vertices[ triangles_mesh.indices[3*triangle_id + 2] ];

	if ( equal_vertices[0] ) {
		if ( equal_vertices[1] ) {
			if ( (&vertex0 != equal_vertices[0]) && (&vertex0 != equal_vertices[1]) ) {
				if ( !intersected_triangles.empty() ) {
					should_continue		= true;
					return				true;
				}

				triangles_mesh.data[triangle_id].color.set( 0, 255, 255, 255 );
				should_continue			= false;
				return					true;
			}

			if ( (&vertex1 != equal_vertices[0]) && (&vertex1 != equal_vertices[1]) ) {
				if ( !intersected_triangles.empty() ) {
					should_continue		= true;
					return				true;
				}

				triangles_mesh.data[triangle_id].color.set( 0, 255, 255, 255 );
				should_continue			= false;
				return					true;
			}

			R_ASSERT					( (&vertex2 != equal_vertices[0]) && (&vertex2 != equal_vertices[1]) );
			if ( !intersected_triangles.empty() ) {
				should_continue			= true;
				return					true;
			}

			triangles_mesh.data[triangle_id].color.set( 0, 255, 255, 255 );
			should_continue				= false;
			return						true;
		}
	}

	if ( !equal_vertices[0] && !equal_vertices[1] )
		return							false;

	float3 const* const non_equal_vertex = equal_vertices[0] ? &segment_vertices[1] : &segment_vertices[0];
	success_count						+= try_tessellate_triangle( triangles_mesh, triangle_id, intersected_triangles, vertex0,	vertex1,	*non_equal_vertex ) ? 1 : 0;
	success_count						+= try_tessellate_triangle( triangles_mesh, triangle_id, intersected_triangles, vertex1,	vertex2,	*non_equal_vertex ) ? 1 : 0;
	success_count						+= try_tessellate_triangle( triangles_mesh, triangle_id, intersected_triangles, vertex2,	vertex0,	*non_equal_vertex ) ? 1 : 0;
	should_continue						= true;
	return								true;
}

typedef xray::buffer_vector< u32 >	indices_type;
typedef xray::buffer_vector< bool >	markers_type;

class is_marked_in_buffer_predicate {
public:
	inline			is_marked_in_buffer_predicate	( indices_type const& indices, markers_type const& markers ) :
		m_indices	( &indices ),
		m_markers	( &markers )
	{
	}

	inline	bool	operator()			( u32 const& vertex_id ) const
	{
		size_t const vertex_id_index	= &vertex_id - m_indices->begin( );
		return							(*m_markers)[ vertex_id_index / 3 ];
	}

private:
	indices_type const* m_indices;
	markers_type const*	m_markers;
}; // class is_marked_in_buffer_predicate

static u32 segment_intersects_segment	(
		u32 (&coordinate_indices)[2],
		float3 const& v0,
		float3 const& v1,
		float3 const& u0,
		float3 const& u1,
		float3& result0,
		float3& result1
	)
{
	float const& ax = v0[ coordinate_indices[0] ];
	float const& ay = v0[ coordinate_indices[1] ];
	float const& bx = v1[ coordinate_indices[0] ];
	float const& by = v1[ coordinate_indices[1] ];
	float const& cx = u0[ coordinate_indices[0] ];
	float const& cy = u0[ coordinate_indices[1] ];
	float const& dx = u1[ coordinate_indices[0] ];
	float const& dy = u1[ coordinate_indices[1] ];

	float const denominator = (bx - ax)*(dy - cy) - (by - ay)*(dx - cx);
	if ( xray::math::is_zero(denominator) ) {
		float const numerator = (ay - cy)*(bx - ax) - (ax - cx)*(by - ay);
		if ( !xray::math::is_zero(numerator) )
			return			0;

		float const values[4] = {
			ax < bx ? ax : bx,
			ax < bx ? bx : ax,
			cx < dx ? cx : dx,
			cx < dx ? dx : cx,
		};

		float3 const* values_3d[2] = {
			cx < dx ? &u0 : &u1,
			cx < dx ? &u1 : &u0,
		};
		if ( values[0] < values[2] ) {
			if ( values[1] <= values[2] )
				return		0;

			if ( values[3] < values[1] ) {
				result0		= *values_3d[0];
				result1		= *values_3d[1];
				return		2;
			}

			result0			= *values_3d[0];
			return			1;
		}

		if ( values[3] <= values[0] )
			return			0;

		if ( values[3] >= values[1] )
			return			0;

		result0				= *values_3d[1];
		return				1;
	}

	float const numerator	= (ay - cy)*(dx - cx) - (ax - cx)*(dy - cy);
	float const r			= numerator / denominator;

	if ( r >= 1.f )
		return				0;

	if ( r <= 0.f )
		return				0;

	result0					= v0 + r*(v1 - v0);
	return					1;
}

static void fill_coordinate_indices	( float3 const& normal, u32 (&coordinate_indices)[2] )
{
	float3 const& normal_abs	= float3( xray::math::abs(normal.x), xray::math::abs(normal.y), xray::math::abs(normal.z) );
	if ( normal_abs.x > normal_abs.y ) {
		if ( normal_abs.x > normal_abs.z ) {
			coordinate_indices[0] = 1;
			coordinate_indices[1] = 2;
		}
		else {
			coordinate_indices[0] = 0;
			coordinate_indices[1] = 1;
		}
	}
	else {
		if ( normal_abs.z > normal_abs.y ) {
			coordinate_indices[0] = 0;
			coordinate_indices[1] = 1;
		}
		else {
			coordinate_indices[0] = 0;
			coordinate_indices[1] = 2;
		}
	}
}

static u32 segment_intersects_triangle	(
		float3 const& normal,
		float3 const& segment0,
		float3 const& segment1,
		float3 const& vertex0,
		float3 const& vertex1,
		float3 const& vertex2,
		float3 (&intersections)[3]
	)
{
	u32 coordinate_indices[2];
	fill_coordinate_indices	( normal, coordinate_indices );

	float3* const begin		= &intersections[0];
	float3* intersection	= begin;

	intersection			+= segment_intersects_segment( coordinate_indices, segment0, segment1, vertex0, vertex1, *intersection, *(intersection + 1));
	if ( (intersection - begin) < 2 )
		intersection		+= segment_intersects_segment( coordinate_indices, segment0, segment1, vertex1, vertex2, *intersection, *(intersection + 1) );
	if ( (intersection - begin) < 2 )
		intersection		+= segment_intersects_segment( coordinate_indices, segment0, segment1, vertex2, vertex0, *intersection, *(intersection + 1) );

	R_ASSERT_CMP			( intersection - begin, <, 3 );
	return					u32(intersection - begin);
}

typedef xray::buffer_vector< float3 >	vertices_type;
typedef xray::buffer_vector< u32 >		indices_type;

static void intersect_coplanar_triangles(
		vertices_type& vertices,
		float3 const& normal,
		float3 const& v0,
		float3 const& v1,
		float3 const& v2,
		float3 const& u0,
		float3 const& u1,
		float3 const& u2
	)
{
	float3 const* test_vertices[] = { &u0, &u1, &u2, &u0	};
	for (u32 i=0; i<3; ++i) {
		float3 intersections[3];
		u32 const vertex_count	= segment_intersects_triangle( normal, *test_vertices[i], *test_vertices[i+1], v0, v1, v2, intersections);
		for (u32 j=0; j<vertex_count; ++j) {
			R_ASSERT			( std::find( vertices.begin(), vertices.end(), intersections[j] ) == vertices.end() );
			vertices.push_back	( intersections[j] );
		}
	}

	for (u32 i=0; i<3; ++i) {
		if ( !is_point_inside_triangle( *test_vertices[i], v0, v1, v2) )
			continue;

		R_ASSERT			( std::find( vertices.begin(), vertices.end(), *test_vertices[i] ) == vertices.end() );
		vertices.push_back	( *test_vertices[i] );
	}
}

struct less_float3_predicate {
	inline bool operator ( )	( float3 const& left, float3 const& right ) const
	{
		if ( left.x < right.x )
			return	true;

		if ( left.x > right.x )
			return	false;

		if ( left.y < right.y )
			return	true;

		if ( left.y > right.y )
			return	false;

		return		left.z < right.z;
	}
}; // struct less_float3_predicate

struct equal_float3_predicate {
	inline bool operator ( )	( float3 const& left, float3 const& right ) const
	{
		if ( left.x != right.x )
			return	false;

		if ( left.y != right.y )
			return	false;

		return		left.z == right.z;
	}
}; // struct equal_float3_predicate

static u32 find_triangle					(
		vertices_type const& vertices,
		indices_type const& indices,
		float3 const& vertex,
		u32 start_index = 0,
		bool assert_on_failure = true
	)
{
	for ( u32 i=start_index, n=indices.size()/3; i < n; ++i ) {
		if (
				is_point_inside_triangle(
					vertex,
					vertices[ indices[3*i + 0] ],
					vertices[ indices[3*i + 1] ],
					vertices[ indices[3*i + 2] ]
				)
			)
			return		i;
	}

	if ( !assert_on_failure )
		return			u32(-1);

	NODEFAULT			( return u32(-1) );
}

static inline bool is_between	(
		xray::math::float3 const& test_vertex,
		xray::math::float3 const& vertex0,
		xray::math::float3 const& vertex1
	)
{
	if ( is_similar(test_vertex, vertex0) )
		return				true;

	float3 const& vertex0_to_vertex1		= vertex1 - vertex0;
	float const vertex0_to_vertex1_length	= vertex0_to_vertex1.length( );
	float3 const& vertex0_to_test			= test_vertex - vertex0;
	float const vertex0_to_test_length		= vertex0_to_test.length( );
	if ( !is_similar( vertex0_to_vertex1, vertex0_to_test*(vertex0_to_vertex1_length/vertex0_to_test_length)) )
		return				false;

	return					vertex0_to_vertex1_length >= vertex0_to_test_length;
}

static u32 find_neighbour_triangle_id	(
		indices_type& indices,
		u32 const triangle_id,
		u32 const vertex_id0,
		u32 const vertex_id1,
		u32& vertex_id2
	)
{
	R_ASSERT					( indices.size() % 3 == 0 );
	for (u32 i=0, n=indices.size()/3; i != n; ++i ) {
		if ( i == triangle_id )
			continue;

		if ( indices[3*i + 0] == vertex_id1 ) {
			if ( indices[3*i + 1] == vertex_id0 ) {
				vertex_id2		= indices[3*i + 2];
				return			i;
			}

			R_ASSERT			( indices[3*i + 2] != vertex_id0 );
			continue;
		}

		if ( indices[3*i + 1] == vertex_id1 ) {
			R_ASSERT			( indices[3*i + 0] != vertex_id0 );

			if ( indices[3*i + 2] == vertex_id0 ) {
				vertex_id2		= indices[3*i + 0];
				return			i;
			}

			continue;
		}

		if ( indices[3*i + 2] == vertex_id1 ) {
			if ( indices[3*i + 0] == vertex_id0 ) {
				vertex_id2		= indices[3*i + 1];
				return			i;
			}

			R_ASSERT			( indices[3*i + 1] != vertex_id0 );
			continue;
		}
	}

	return						u32(-1);
}

static inline bool is_vertex_of_the_bounding_triangle	( vertices_type const& vertices, u32 const vertex_id )
{
	return						vertex_id >= (vertices.size() - 3);
}

static bool is_outside_circumcircle						(
		u32 (&coordinate_indices)[2],
		float3 const& triangle_vertex0,
		float3 const& triangle_vertex1,
		float3 const& triangle_vertex2,
		float3 const& test_vertex
	)
{
	float matrix[3][3] = {
		{
			triangle_vertex0[coordinate_indices[0]] - test_vertex[coordinate_indices[0]],
			triangle_vertex0[coordinate_indices[1]] - test_vertex[coordinate_indices[1]],
			xray::math::sqr( matrix[0][0] ) + xray::math::sqr( matrix[0][1] ),
		},
		{
			triangle_vertex1[coordinate_indices[0]] - test_vertex[coordinate_indices[0]],
			triangle_vertex1[coordinate_indices[1]] - test_vertex[coordinate_indices[1]],
			xray::math::sqr( matrix[1][0] ) + xray::math::sqr( matrix[1][1] ),
		},
		{
			triangle_vertex2[coordinate_indices[0]] - test_vertex[coordinate_indices[0]],
			triangle_vertex2[coordinate_indices[1]] - test_vertex[coordinate_indices[1]],
			xray::math::sqr( matrix[2][0] ) + xray::math::sqr( matrix[2][1] ),
		},
	};

	float const determinant	=
		matrix[0][0] * matrix[1][1] * matrix[2][2] +
		matrix[0][1] * matrix[1][2] * matrix[2][0] +
		matrix[0][2] * matrix[1][0] * matrix[2][1] -
		matrix[0][2] * matrix[1][1] * matrix[2][0] -
		matrix[0][0] * matrix[1][2] * matrix[2][1] -
		matrix[0][1] * matrix[1][0] * matrix[2][2];

	// check if new vertex lies outside circumcircle and therefore has less angle
	// if first three points set up counter-clockwise, then we should check on <= 0.f
	// but we set up them clockwise, and therefore check on >= 0.f
	return				determinant >= 0.f;
}

static bool is_edge_legal		(
		vertices_type const& vertices,
		indices_type& indices,
		u32 (&coordinate_indices)[2],
		u32 const triangle_id,
		u32 const edge_id,
		u32 const fourth_vertex_index
	)
{
	u32 const test_indices[] =	{
		indices[3*triangle_id + (edge_id + 0)],
		indices[3*triangle_id + (edge_id + 1) % 3],
		indices[3*triangle_id + (edge_id + 2) % 3],
		fourth_vertex_index
	};
	u32 const vertex_of_the_bounding_triangle[] =	{
		is_vertex_of_the_bounding_triangle( vertices, test_indices[0] ) ? 1 : 0,
		is_vertex_of_the_bounding_triangle( vertices, test_indices[1] ) ? 1 : 0,
		is_vertex_of_the_bounding_triangle( vertices, test_indices[2] ) ? 1 : 0,
		is_vertex_of_the_bounding_triangle( vertices, test_indices[3] ) ? 1 : 0,
	};

	if ( vertex_of_the_bounding_triangle[0] && vertex_of_the_bounding_triangle[1] )
		return					true;

	u32 const bounding_vertex_count	= vertex_of_the_bounding_triangle[0] + vertex_of_the_bounding_triangle[1] + vertex_of_the_bounding_triangle[2] + vertex_of_the_bounding_triangle[3];
	switch ( bounding_vertex_count ) {
		case 0 : break;
		case 1 : return			!vertex_of_the_bounding_triangle[0] && !vertex_of_the_bounding_triangle[1];
		case 2 : {
			R_ASSERT			( vertex_of_the_bounding_triangle[0] || vertex_of_the_bounding_triangle[1] );
			R_ASSERT			( !(vertex_of_the_bounding_triangle[0] && vertex_of_the_bounding_triangle[1]) );
			R_ASSERT			( vertex_of_the_bounding_triangle[2] || vertex_of_the_bounding_triangle[3] );
			R_ASSERT			( !(vertex_of_the_bounding_triangle[2] && vertex_of_the_bounding_triangle[3]) );
			u32 const index0	= vertex_of_the_bounding_triangle[0] ? test_indices[0] : test_indices[1];
			u32 const index1	= vertex_of_the_bounding_triangle[2] ? test_indices[2] : test_indices[3];
			return				index0 < index1;
		}
		default : NODEFAULT( return true; );
	}

	return
		is_outside_circumcircle(
			coordinate_indices,
			vertices[ test_indices[0] ],
			vertices[ test_indices[1] ],
			vertices[ test_indices[2] ],
			vertices[ test_indices[3] ]
		);
}

static inline float cross_product	(
		u32 (&coordinate_indices)[2],
		float3 const& vertex0,
		float3 const& vertex1
	)
{
	return
		vertex0[ coordinate_indices[0] ] * vertex1[ coordinate_indices[1] ] - 
		vertex0[ coordinate_indices[1] ] * vertex1[ coordinate_indices[0] ];
}

static inline bool is_convex		(
		u32 (&coordinate_indices)[2],
		float3 const& vertex0,
		float3 const& vertex1,
		float3 const& vertex2,
		float3 const& vertex3
	)
{
	float3 const& vertex10	= vertex1 - vertex0;
	float3 const& vertex21	= vertex2 - vertex1;
	char const sign			= xray::math::sign( cross_product(coordinate_indices, vertex10, vertex21 ) );
	float3 const& vertex32	= vertex3 - vertex2;
	if ( sign != xray::math::sign( cross_product(coordinate_indices, vertex21, vertex32 ) ) )
		return				false;

	float3 const& vertex03	= vertex0 - vertex3;
	if ( sign != xray::math::sign( cross_product(coordinate_indices, vertex32, vertex03 ) ) )
		return				false;

	if ( sign != xray::math::sign( cross_product(coordinate_indices, vertex03, vertex10 ) ) )
		return				false;

	return					true;
}

static void legalize_edge		(
		vertices_type const& vertices,
		indices_type& indices,
		u32 (&coordinate_indices)[2],
		u32 const triangle_id,
		u32 const edge_id
	)
{
	u32 new_vertex_index		= u32(-1);
	u32 const neighbour_triangle_id	= find_neighbour_triangle_id( indices, triangle_id, indices[3*triangle_id + edge_id], indices[3*triangle_id + ((edge_id + 1) % 3)], new_vertex_index );
	if ( neighbour_triangle_id == u32(-1) )
		return;

	u32 const new_indices[] = {
		indices[3*triangle_id],
		indices[3*triangle_id + 1],
		indices[3*triangle_id + 2],
		new_vertex_index
	};

	if (
			!is_convex(
				coordinate_indices,
				vertices[ new_indices[edge_id] ],
				vertices[ new_indices[3] ],
				vertices[ new_indices[(edge_id + 1) % 3] ],
				vertices[ new_indices[(edge_id + 2) % 3] ]
			)
		)
		return;

	R_ASSERT						( new_vertex_index != u32(-1) );
	if ( is_edge_legal(vertices, indices, coordinate_indices, triangle_id, edge_id, new_vertex_index) )
		return;

	// flip edge and recurse
	indices[3*triangle_id + 0]		= new_indices[edge_id];
	indices[3*triangle_id + 1]		= new_indices[3];
	indices[3*triangle_id + 2]		= new_indices[(edge_id + 2) % 3];

	indices[3*neighbour_triangle_id + 0]	= new_indices[(edge_id + 2) % 3];
	indices[3*neighbour_triangle_id + 1]	= new_indices[3];
	indices[3*neighbour_triangle_id + 2]	= new_indices[(edge_id + 1) % 3];

	legalize_edge					( vertices, indices, coordinate_indices, triangle_id, 0 );
	legalize_edge					( vertices, indices, coordinate_indices, triangle_id, 2 );

	legalize_edge					( vertices, indices, coordinate_indices, neighbour_triangle_id, 1 );
	legalize_edge					( vertices, indices, coordinate_indices, neighbour_triangle_id, 2 );
}

static void split_triangle			(
		vertices_type const& vertices,
		indices_type& indices,
		u32 (&coordinate_indices)[2],
		u32 const vertex_index,
		u32 const triangle_id,
		u32 const edge_id
	)
{
	u32 const triangle_indices[] = {
		indices[3*triangle_id + ((edge_id + 0))],
		indices[3*triangle_id + ((edge_id + 1) % 3)],
		indices[3*triangle_id + ((edge_id + 2) % 3)],
	};

	R_ASSERT_CMP				( indices.size() % 3, ==, 0 );
	u32 const new_triangle_index	= indices.size() / 3;
	indices.push_back			( triangle_indices[0] );
	indices.push_back			( vertex_index );
	indices.push_back			( triangle_indices[2] );

	indices[3*triangle_id + 0]	= vertex_index;
	indices[3*triangle_id + 1]	= triangle_indices[1];
	indices[3*triangle_id + 2]	= triangle_indices[2];

	legalize_edge				( vertices, indices, coordinate_indices, new_triangle_index, 0 );
	legalize_edge				( vertices, indices, coordinate_indices, new_triangle_index, 2 );

	legalize_edge				( vertices, indices, coordinate_indices, triangle_id, 0 );
	legalize_edge				( vertices, indices, coordinate_indices, triangle_id, 1 );
}

static void add_vertex_on_edge_to_triangulation	(
		vertices_type const& vertices,
		indices_type& indices,
		u32 (&coordinate_indices)[2],
		u32 const vertex_index,
		u32 const triangle_id,
		u32 const edge_id
	)
{
	float3 const& vertex		= vertices[vertex_index];
	u32 const neighbour_triangle_id	= find_triangle( vertices, indices, vertex, triangle_id + 1, false );
	if ( neighbour_triangle_id == u32(-1) ) {
		split_triangle			( vertices, indices, coordinate_indices, vertex_index, triangle_id, edge_id );
		return;
	}

	float3 const& v0			= vertices[ indices[3*neighbour_triangle_id + 0] ];
	float3 const& v1			= vertices[ indices[3*neighbour_triangle_id + 1] ];
	float3 const& v2			= vertices[ indices[3*neighbour_triangle_id + 2] ];
	u32 neighbour_edge_id;
	if ( is_between( vertex, v0, v1) )
		neighbour_edge_id		= 0;
	else if ( is_between( vertex, v1, v2) )
		neighbour_edge_id		= 1;
	else {
		R_ASSERT				( is_between( vertex, v2, v0) );
		neighbour_edge_id		= 2;
	}

	split_triangle				( vertices, indices, coordinate_indices, vertex_index, triangle_id, edge_id );
	split_triangle				( vertices, indices, coordinate_indices, vertex_index, neighbour_triangle_id, neighbour_edge_id );
}

static void add_vertex_inside_triangle_to_triangulation	(
		vertices_type const& vertices,
		indices_type& indices,
		u32 (&coordinate_indices)[2],
		u32 const triangle_id,
		u32 const vertex_id
	)
{
	indices.push_back			( indices[3*triangle_id + 0] );
	indices.push_back			( indices[3*triangle_id + 1] );
	indices.push_back			( vertex_id );

	indices.push_back			( indices[3*triangle_id + 1] );
	indices.push_back			( indices[3*triangle_id + 2] );
	indices.push_back			( vertex_id );

	// order of indices (1, then 0), is important here, since we're performing changes in-place
	indices[3*triangle_id + 1]	= indices[3*triangle_id + 0];
	indices[3*triangle_id + 0]	= indices[3*triangle_id + 2];
	indices[3*triangle_id + 2]	= vertex_id;

	R_ASSERT					( indices.size() % 3 == 0 );

	u32 const indices_size		= indices.size( );
	legalize_edge				( vertices, indices, coordinate_indices, indices_size/3 - 2,	0 );
	legalize_edge				( vertices, indices, coordinate_indices, indices_size/3 - 1,	0 );
	legalize_edge				( vertices, indices, coordinate_indices, triangle_id,		0 );
}

static void add_vertex_to_triangulation		(
		vertices_type const& vertices,
		indices_type& indices,
		u32 (&coordinate_indices)[2],
		u32 const vertex_id
	)
{
	float3 const& vertex		= vertices[ vertex_id ];
	u32 const triangle_id		= find_triangle( vertices, indices, vertex );
	float3 const& v0			= vertices[ indices[3*triangle_id + 0] ];
	float3 const& v1			= vertices[ indices[3*triangle_id + 1] ];
	float3 const& v2			= vertices[ indices[3*triangle_id + 2] ];
	if ( is_between( vertex, v0, v1) ) {
		add_vertex_on_edge_to_triangulation	( vertices, indices, coordinate_indices, vertex_id, triangle_id, 0 );
		return;
	}

	if ( is_between( vertex, v1, v2) ) {
		add_vertex_on_edge_to_triangulation	( vertices, indices, coordinate_indices, vertex_id, triangle_id, 1 );
		return;
	}

	if ( is_between( vertex, v2, v0) ) {
		add_vertex_on_edge_to_triangulation	( vertices, indices, coordinate_indices, vertex_id, triangle_id, 2 );
		return;
	}

	add_vertex_inside_triangle_to_triangulation	( vertices, indices, coordinate_indices, triangle_id, vertex_id );
}

static void add_bounding_triangle	(
		float3 const& normal,
		vertices_type& vertices,
		indices_type& indices,
		u32 (&coordinate_indices)[2]
	)
{
	fill_coordinate_indices		( normal, coordinate_indices );

	float maximum = -xray::math::float_max;
	vertices_type::const_iterator i			= vertices.begin( );
	vertices_type::const_iterator const e	= vertices.end( );
	for ( ; i != e; ++i ) {
		float const local_maximum	= xray::math::max( (*i).x, xray::math::max( (*i).y, (*i).z) );
		if ( maximum < local_maximum )
			maximum					= local_maximum;
	}

	float3 vertex(0.f, 0.f, 0.f);

	vertex[ coordinate_indices[0] ]	= 0.f;
	vertex[ coordinate_indices[1] ]	= 3.f*maximum;
	vertices.push_back	( vertex );

	vertex[ coordinate_indices[0] ]	= 3.f*maximum;
	vertex[ coordinate_indices[1] ]	= 0.f;
	vertices.push_back	( vertex );

	vertex[ coordinate_indices[0] ]	= -3.f*maximum;
	vertex[ coordinate_indices[1] ]	= -3.f*maximum;
	vertices.push_back	( vertex );

	u32 const vertex_count	= vertices.size();
	indices.push_back	( vertex_count - 3 );
	indices.push_back	( vertex_count - 2 );
	indices.push_back	( vertex_count - 1 );
}

class bounding_triangle_filter {
public:
	inline bounding_triangle_filter	( vertices_type const& vertices, indices_type const& indices ) :
		m_vertices		( &vertices ),
		m_indices		( &indices )
	{
	}

	inline bool operator ( )		( u32 const& index ) const
	{
		u32 const triangle_id	= (&index - m_indices->begin( )) / 3;
		return			
			is_vertex_of_the_bounding_triangle( *m_vertices, (*m_indices)[ 3*triangle_id + 0 ] ) ||
			is_vertex_of_the_bounding_triangle( *m_vertices, (*m_indices)[ 3*triangle_id + 1 ] ) ||
			is_vertex_of_the_bounding_triangle( *m_vertices, (*m_indices)[ 3*triangle_id + 2 ] );
	}

private:
	vertices_type const*	m_vertices;
	indices_type const*		m_indices;
}; // struct bounding_triangle_filter

static void remove_bounding_triangle		( vertices_type& vertices, indices_type& indices )
{
	indices.erase					(
		std::remove_if(
			indices.begin( ),
			indices.end( ),
			bounding_triangle_filter( vertices, indices )
		),
		indices.end( )
	);
	vertices.resize					( vertices.size() - 3 );
}

static void create_delaunay_triangulation	( vertices_type& vertices, indices_type& indices, float3 const& normal )
{
	R_ASSERT_CMP					( vertices.size(), >=, 3 );

	xray::math::random32 random(0);
	std::random_shuffle				( vertices.begin(), vertices.end(), random );

	u32 coordinate_indices[2];
	add_bounding_triangle			( normal, vertices, indices, coordinate_indices );

	for (u32 index=0, n=vertices.size() - 3; index != n; ++index )
		add_vertex_to_triangulation	( vertices, indices, coordinate_indices, index );

	remove_bounding_triangle		( vertices, indices );
}

static u32 tester()
{
	//u32 const max_new_vertex_count	= 6;
	//::vertices_type vertices( ALLOCA(max_new_vertex_count*sizeof(::vertices_type::value_type)), max_new_vertex_count );
	//
	//float3 const triangle0[] = {
	//	float3( 0.f, 0.f, 0.f ),
	//	float3( 1.f, 0.f, 0.f ),
	//	float3( 0.f, 0.f, 1.f ),
	//};
	//float3 const triangle1[] = {
	//	float3( .5f, 0.f, -.5f ),
	//	float3( .5f, 0.f,  1.f ),
	//	float3( .75f, 0.f, 1.f ),
	//};
	//intersect_coplanar_triangles	(
	//	vertices,
	//	normalize((triangle0[1] - triangle0[0]) ^ (triangle0[2] - triangle0[0])),
	//	triangle0[0],
	//	triangle0[1],
	//	triangle0[2],
	//	triangle1[0],
	//	triangle1[1],
	//	triangle1[2]
	//);

	vertices_type vertices( ALLOCA(100*sizeof(::vertices_type::value_type)), 100 );
	indices_type indices( ALLOCA(100*sizeof(::indices_type::value_type)), 100 );

	vertices.push_back	( float3(-1.f, 0.f, -1.f) );
	vertices.push_back	( float3(-1.f, 0.f,  1.f) );
	vertices.push_back	( float3( 1.f, 0.f,  1.f) );
	vertices.push_back	( float3( 1.f, 0.f, -1.f) );
	vertices.push_back	( float3(-.2f, 0.f, -.2f) );
	vertices.push_back	( float3(-.2f, 0.f,  .6f) );
	vertices.push_back	( float3( .6f, 0.f,  .6f) );
	vertices.push_back	( float3( .6f, 0.f, -.2f) );

	create_delaunay_triangulation	( vertices, indices, float3(0.f, 1.f, 0.f) );

	return 1;
}

static u32 s_test = tester();

static inline u32 get_additional_delaunay_triangulation_vertex_count( )
{
	return 3;
}

static inline u32 get_maximum_triangle_count_in_delaunay_triangulation( u32 const vertex_count )
{
	return 2*(vertex_count + 3 - 1) - 3;
}

void graph_generator::tessellate_coplanar_triangles	(
		triangles_mesh_type& triangles_mesh,
		u32 const triangle_id,
		buffer_indices_type::iterator const& found,
		buffer_indices_type& intersected_triangles,
		u32& success_count
	)
{
	R_ASSERT			( found != intersected_triangles.end() );

	u32 const* indices0	= &*triangles_mesh.indices.begin() + 3*triangle_id;
	float3 const& v0	= triangles_mesh.vertices[ indices0[0] ];
	float3 const& v1	= triangles_mesh.vertices[ indices0[1] ];
	float3 const& v2	= triangles_mesh.vertices[ indices0[2] ];

	u32 const* indices1	= &*triangles_mesh.indices.begin() + 3*(*found);
	float3 const& u0	= triangles_mesh.vertices[ indices1[0] ];
	float3 const& u1	= triangles_mesh.vertices[ indices1[1] ];
	float3 const& u2	= triangles_mesh.vertices[ indices1[2] ];

	intersected_triangles.erase		( found );

	u32 const max_vertex_count	= 9 + get_additional_delaunay_triangulation_vertex_count( );
	::vertices_type vertices( ALLOCA(max_vertex_count*sizeof(::vertices_type::value_type)), max_vertex_count );
	
	intersect_coplanar_triangles	( vertices, triangles_mesh.data[triangle_id].plane.normal, v0, v1, v2, u0, u1, u2 );
	vertices.push_back	( v0 );
	vertices.push_back	( v1 );
	vertices.push_back	( v2 );

	std::sort			( vertices.begin(), vertices.end(), less_float3_predicate() );
	vertices.erase		(
		std::unique(
			vertices.begin( ),
			vertices.end( ),
			equal_float3_predicate( )
		),
		vertices.end( )
	);

	u32 const max_index_count		= 3*get_maximum_triangle_count_in_delaunay_triangulation( vertices.size() );
	indices_type indices( ALLOCA(max_index_count*sizeof(vertices_type::value_type)), max_index_count );

	create_delaunay_triangulation	( vertices, indices, triangles_mesh.data[triangle_id].plane.normal );

	R_ASSERT_CMP					( indices.size() % 3, ==, 0 );
	u32 const triangles_count		= indices.size() / 3;
	markers_type markers( ALLOCA(triangles_count*sizeof(bool)), triangles_count, triangles_count );
	markers_type::iterator i		= markers.begin( );
	markers_type::iterator const e	= markers.end( );
	for (u32 j=0; i != e; ++i, ++j ) {
		float3 const& a				= vertices[ indices[3*j + 0] ];
		float3 const& b				= vertices[ indices[3*j + 1] ];
		float3 const& c				= vertices[ indices[3*j + 2] ];
		float3 const& median		= (a + b + c)/3.f;
		*i							= is_point_inside_triangle( median, v0, v1, v2 );
	}

	indices.erase					(
		std::remove_if(
			indices.begin( ),
			indices.end( ),
			is_marked_in_buffer_predicate( indices, markers )
		),
		indices.end( )
	);

	R_ASSERT_CMP					( indices.size() % 3, ==, 0 );
	success_count					+= indices.size()/3;
	
	// add new triangles to triangles mesh and recurse on them
}

void graph_generator::tessellate_triangle_generic_case	(
		triangles_mesh_type& triangles_mesh,
		u32 const triangle_id,
		buffer_indices_type& intersected_triangles,
		float3 (&segment_vertices)[2],
		float3 const* const (&triangle_vertices_original)[3],
		u32& success_count
	)
{
	float3 const* triangle_vertices[]	= {
		triangle_vertices_original[0],
		triangle_vertices_original[1],
		triangle_vertices_original[2],
	};

	float3 const& triangle_normal	= triangles_mesh.data[triangle_id].plane.normal;

//	float3 const* vertices[3];
	float3 const* last_vertex_on_line_with_segment = 0;
	for (u32 iteration = 0; ; ++iteration ) {
		float3 const& s0_minus_t0 = segment_vertices[0] - *triangle_vertices[0];
		float3 const& s1_minus_t0 = segment_vertices[1] - *triangle_vertices[0];
		float3 s0_cross_s1_t0	= s0_minus_t0 ^ s1_minus_t0;
		if ( xray::math::is_zero( s0_cross_s1_t0.length() ) ) {
			if ( !last_vertex_on_line_with_segment ) {
				last_vertex_on_line_with_segment	= triangle_vertices[0];

				R_ASSERT_CMP		( iteration, <, 1 );

				R_ASSERT			( triangle_vertices[0] == triangle_vertices_original[0] );
				R_ASSERT			( triangle_vertices[1] == triangle_vertices_original[1] );
				R_ASSERT			( triangle_vertices[2] == triangle_vertices_original[2] );

				triangle_vertices[0] = triangle_vertices_original[1];
				triangle_vertices[1] = triangle_vertices_original[2];
				triangle_vertices[2] = triangle_vertices_original[0];
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

			float3 const triangle_vertices_values[]	= {
				*triangle_vertices[0],
				*triangle_vertices[1],
				*triangle_vertices[2],
			};

			success_count			+= try_tessellate_triangle( triangles_mesh, triangle_id, intersected_triangles, triangle_vertices_values[0],segment_vertices[0],			triangle_vertices_values[2])	? 1 : 0;
			success_count			+= try_tessellate_triangle( triangles_mesh, triangle_id, intersected_triangles, segment_vertices[0],		segment_vertices[1],			triangle_vertices_values[2])	? 1 : 0;
			success_count			+= try_tessellate_triangle( triangles_mesh, triangle_id, intersected_triangles, segment_vertices[1],		triangle_vertices_values[1],	triangle_vertices_values[2])	? 1 : 0;
			return;
		}

		s0_cross_s1_t0.normalize( );
		bool const is_t0		= math::is_zero( (triangle_normal | s0_cross_s1_t0) - 1.f, math::epsilon_3);
		R_ASSERT				( is_t0 || math::is_zero( (triangle_normal | -s0_cross_s1_t0) - 1.f, math::epsilon_3) );
		if ( !is_t0 )
			std::swap			( segment_vertices[0], segment_vertices[1] );

		float3 const& s0_minus_t1 = segment_vertices[0] - *triangle_vertices[1];
		float3 const& s1_minus_t1 = segment_vertices[1] - *triangle_vertices[1];
		float3 s0_cross_s1_t1	 = s0_minus_t1 ^ s1_minus_t1;
		if ( math::is_zero( s0_cross_s1_t1.length() ) ) {
			if ( !last_vertex_on_line_with_segment ) {
				last_vertex_on_line_with_segment	= triangle_vertices[1];

				R_ASSERT_CMP		( iteration, <, 1 );

				R_ASSERT			( triangle_vertices[0] == triangle_vertices_original[0] );
				R_ASSERT			( triangle_vertices[1] == triangle_vertices_original[1] );
				R_ASSERT			( triangle_vertices[2] == triangle_vertices_original[2] );

				triangle_vertices[0] = triangle_vertices_original[2];
				triangle_vertices[1] = triangle_vertices_original[0];
				triangle_vertices[2] = triangle_vertices_original[1];

				continue;
			}

			// segment lies completely on the edge
			// here we make this edge TV0 => TV1
			// and S0 will be closer to TV0 than S1
			R_ASSERT				( triangle_vertices[0] != last_vertex_on_line_with_segment );
			R_ASSERT				( triangle_vertices[2] == last_vertex_on_line_with_segment );

			if ( s0_minus_t1.squared_length() > s1_minus_t1.squared_length() )
				std::swap			( segment_vertices[0], segment_vertices[1] );

			float3 const triangle_vertices_values[]	= {
				*triangle_vertices[0],
				*triangle_vertices[1],
				*triangle_vertices[2],
			};

			success_count			+= try_tessellate_triangle( triangles_mesh, triangle_id, intersected_triangles, triangle_vertices_values[1],segment_vertices[0],			triangle_vertices_values[0])	? 1 : 0;
			success_count			+= try_tessellate_triangle( triangles_mesh, triangle_id, intersected_triangles, segment_vertices[0],		segment_vertices[1],			triangle_vertices_values[0])	? 1 : 0;
			success_count			+= try_tessellate_triangle( triangles_mesh, triangle_id, intersected_triangles, segment_vertices[1],		triangle_vertices_values[2],	triangle_vertices_values[0])	? 1 : 0;
			return;
		}

		s0_cross_s1_t1.normalize( );
		bool const is_t1		= math::is_zero( (triangle_normal | s0_cross_s1_t1) - 1.f, math::epsilon_3);
		R_ASSERT				( is_t1 || math::is_zero( (triangle_normal | -s0_cross_s1_t1) - 1.f, math::epsilon_3) );
		//if ( is_t1 ) {
		//	vertices[0]			= triangle_vertices_original[1];
		//	vertices[1]			= triangle_vertices_original[2];
		//	vertices[2]			= triangle_vertices_original[0];
		//}
		//else {
		//	vertices[0]			= triangle_vertices_original[0];
		//	vertices[1]			= triangle_vertices_original[1];
		//	vertices[2]			= triangle_vertices_original[2];
		//}

		R_ASSERT_CMP			( iteration, <, 4 );

		float3 const triangle_vertices_values[]	= {
			*triangle_vertices[0],
			*triangle_vertices[1],
			*triangle_vertices[2],
		};

		success_count			+= try_tessellate_triangle( triangles_mesh, triangle_id, intersected_triangles, triangle_vertices_values[0],	triangle_vertices_values[1],	segment_vertices[0] )	? 1 : 0;
		success_count			+= try_tessellate_triangle( triangles_mesh, triangle_id, intersected_triangles, triangle_vertices_values[1],	segment_vertices[1],			segment_vertices[0] )	? 1 : 0;
		success_count			+= try_tessellate_triangle( triangles_mesh, triangle_id, intersected_triangles, triangle_vertices_values[1],	triangle_vertices_values[2],	segment_vertices[1] )	? 1 : 0;
		success_count			+= try_tessellate_triangle( triangles_mesh, triangle_id, intersected_triangles, triangle_vertices_values[2],	triangle_vertices_values[0],	segment_vertices[1] )	? 1 : 0;
		success_count			+= try_tessellate_triangle( triangles_mesh, triangle_id, intersected_triangles, triangle_vertices_values[0],	segment_vertices[0],			segment_vertices[1] )	? 1 : 0;
		return;
	}
}

bool graph_generator::tessellate_triangle_impl		(
		triangles_mesh_type& triangles_mesh,
		u32 const triangle_id,
		buffer_indices_type& intersected_triangles
	)
{
	float3 const* const triangle_vertices_original[]	= {
		&triangles_mesh.vertices[ triangles_mesh.indices[3*triangle_id + 0] ],
		&triangles_mesh.vertices[ triangles_mesh.indices[3*triangle_id + 1] ],
		&triangles_mesh.vertices[ triangles_mesh.indices[3*triangle_id + 2] ],
	};

	float3 segment_vertices[2];
	u32 success_count = 0;
	for ( ; !success_count && !intersected_triangles.empty(); ) {
		bool is_coplanar;
		buffer_indices_type::iterator const found = select_triangle( triangles_mesh, intersected_triangles, triangle_id, segment_vertices[0], segment_vertices[1], is_coplanar );
		if ( is_coplanar ) {
			tessellate_coplanar_triangles	( triangles_mesh, triangle_id, found, intersected_triangles, success_count );
			continue;
		}

		R_ASSERT					( found != intersected_triangles.end() );
		intersected_triangles.erase	( found );

		bool should_continue;
		if ( process_degenerated_cases(triangles_mesh, triangle_id, intersected_triangles, segment_vertices, success_count, should_continue ) ) {
			if ( should_continue )
				continue;

			return					false;
		}

		R_ASSERT					( !segment_vertices[0].is_similar(segment_vertices[1]) );

		tessellate_triangle_generic_case( triangles_mesh, triangle_id, intersected_triangles, segment_vertices, triangle_vertices_original, success_count );
	}

	if ( success_count )
		return						true;

	triangles_mesh.data[triangle_id].color.set( 0, 255, 255, 255 );
	return							false;
}

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

bool graph_generator::try_tessellate_triangle	(
		triangles_mesh_type& triangles_mesh,
		u32 const triangle_id, 
		buffer_indices_type const& intersected_triangles,
		float3 const& triangle_vertex0,
		float3 const& triangle_vertex1,
		float3 const& triangle_vertex2
	)
{
	R_ASSERT					( !is_similar(triangle_vertex0, triangle_vertex1) );
	R_ASSERT					( !is_similar(triangle_vertex0, triangle_vertex2) );
	R_ASSERT					( !is_similar(triangle_vertex1, triangle_vertex2) );

	xray::math::float3 const b_minus_a	= normalize( triangle_vertex1 - triangle_vertex0 );
	xray::math::float3 const c_minus_a	= normalize( triangle_vertex2 - triangle_vertex0 );
	if ( b_minus_a.is_similar(c_minus_a) || b_minus_a.is_similar(-c_minus_a) )
		return					false;

//	R_ASSERT					( xray::math::is_zero( (normalize(b_minus_a ^ c_minus_a) | triangle.normal) - 1.f, .1f ) );

	buffer_indices_type		new_triangles(
		ALLOCA( intersected_triangles.size()*sizeof(buffer_indices_type::value_type) ),
		intersected_triangles.size( ),
		intersected_triangles.begin( ),
		intersected_triangles.end( )
	);

	u32 const* const triangle_indices = &*triangles_mesh.indices.begin() + 3*triangle_id;
	float3 const* vertices[] = {
		&triangles_mesh.vertices[ triangle_indices[0] ],
		&triangles_mesh.vertices[ triangle_indices[1] ],
		&triangles_mesh.vertices[ triangle_indices[2] ],
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
			triangles_mesh.indices.push_back( indices[i] );
			continue;
		}

		triangles_mesh.indices.push_back	( triangles_mesh.vertices.size() );
		triangles_mesh.vertices.push_back	( *triangle_vertices[i] );
	}

	triangles_mesh.data.push_back ( triangles_mesh.data[triangle_id] );
	tessellate_triangle			( triangles_mesh, triangles_mesh.data.size() - 1, new_triangles );
	return						true;
}

class filter_non_intersected_predicate {
public:
	inline filter_non_intersected_predicate			( triangles_mesh_type& triangles_mesh, u32 const triangle_id ) :
		m_triangles_mesh		( &triangles_mesh ),
		m_triangle_id			( triangle_id )
	{
	}

	inline bool operator( )							( u32 const triangle_id ) const
	{
		return					!intersect_triangles( *m_triangles_mesh, m_triangle_id, triangle_id, true );
	}

private:
	triangles_mesh_type const*	m_triangles_mesh;
	u32							m_triangle_id;
}; // struct filter_obstructions_predicate

void graph_generator::tessellate_triangle			( triangles_mesh_type& triangles_mesh, u32 const triangle_id, buffer_indices_type& triangles )
{
	triangles.erase					(
		std::remove_if(
			triangles.begin( ),
			triangles.end( ),
			::filter_non_intersected_predicate ( triangles_mesh, triangle_id )
		),
		triangles.end( )
	);

	if ( triangles.empty() ) {
		triangles_mesh.data[ triangle_id ].color.set( 0, 255, 255, 255 );
		return;
	}

	triangles_mesh.data[ triangle_id ].color.set	( 255, 0, 0, 255 );
	R_ASSERT										( triangles_mesh.data[ triangle_id ].is_passable );

	if ( !tessellate_triangle_impl( triangles_mesh, triangle_id, triangles ) ) {
		// remove original triangle from the mesh
		// since it has just been tessellated
		triangles_mesh.data[ triangle_id ].is_marked = true;
		return;
	}

	triangles_mesh.data[ triangle_id ].color.set	( 0, 255, 255, 255 );
}

void graph_generator::tessellate_triangle			( triangles_mesh_type& triangles_mesh, u32 const triangle_id, triangles_type& triangles )
{
	triangles.erase					(
		std::remove_if(
			triangles.begin( ),
			triangles.end( ),
			filter_non_passable_predicate( triangles_mesh )
		),
		triangles.end( )
	);

	u32 const triangles_size			= triangles.size( );
	buffer_indices_type buffer_indices	( ALLOCA( triangles_size*sizeof(u32) ), triangles_size, triangles_size );
	buffer_indices_type::iterator i		= buffer_indices.begin( );
	buffer_indices_type::iterator const e = buffer_indices.end( );
	triangles_type::const_iterator j	= triangles.begin( );
	for ( ; i != e; ++i )
		*i								= (*j).triangle_id;

	tessellate_triangle					( triangles_mesh, triangle_id, buffer_indices );
}

void graph_generator::check_vertices_usage	( triangles_mesh_type& triangles_mesh )
{
	typedef debug::vector<bool>			vertices_type;
	vertices_type vertices				( triangles_mesh.vertices.size(), false );

	{
		triangles_mesh_type::indices_type::const_iterator i			= triangles_mesh.indices.begin( );
		triangles_mesh_type::indices_type::const_iterator const e	= triangles_mesh.indices.end( );
		for ( ; i != e; ++i )
			vertices[ *i ]					= true;
	}

	{
		vertices_type::const_iterator i			= vertices.begin( );
		vertices_type::const_iterator const e	= vertices.end( );
		for (u32 j=0; i != e; ++i, ++j )
			R_ASSERT					( *i );
	}
}

void graph_generator::tessellate_passable_triangles	( triangles_mesh_type& triangles_mesh )
{
	triangles_type triangles			( debug::g_mt_allocator );

	u32 const triangle_count			= triangles_mesh.data.size( );
	for ( u32 i=0; i<triangle_count; ++i ) {
		if ( !triangles_mesh.data[i].is_passable )
			continue;

		math::aabb						aabb;
		aabb.invalidate					( );
		aabb.modify						( triangles_mesh.vertices[triangles_mesh.indices[3*i]]		);
		aabb.modify						( triangles_mesh.vertices[triangles_mesh.indices[3*i + 1]]	);
		aabb.modify						( triangles_mesh.vertices[triangles_mesh.indices[3*i + 2]]	);

		math::float3 const& center		= aabb.center( );
		math::float3 const& extents		= aabb.extents( );
		float const epsilon				= math::epsilon_3;
		aabb.modify						( center + float3( -epsilon - extents.x, -5.f - extents.y, -epsilon - extents.z ) );
		aabb.modify						( center + float3( +epsilon + extents.x,  5.f + extents.y, +epsilon + extents.z ) );

		triangles_mesh.spatial_tree->aabb_query	( 0, aabb, triangles );
		tessellate_triangle				( triangles_mesh, i, triangles );
	}

	triangles_mesh.indices.erase		(
		std::remove_if(
			triangles_mesh.indices.begin( ),
			triangles_mesh.indices.end( ),
			is_marked_predicate( triangles_mesh )
		),
		triangles_mesh.indices.end( )
	);

	triangles_mesh.data.erase			(
		std::remove_if(
			triangles_mesh.data.begin( ),
			triangles_mesh.data.end( ),
			is_marked_predicate( triangles_mesh )
		),
		triangles_mesh.data.end( )
	);

	check_vertices_usage				( triangles_mesh );
}

#endif // #if XRAY_DEBUG_ALLOCATOR