////////////////////////////////////////////////////////////////////////////
//	Created		: 11.07.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "delaunay_triangulator.h"
#include <xray/math_randoms_generator.h>

using xray::ai::navigation::delaunay_triangulator;
using xray::ai::navigation::triangles_mesh_type;
using xray::math::float3;

float XRAY_AI_NAVIGATION_API cross_product	(
		u32 const (&coordinate_indices)[2],
		float3 const& vertex0,
		float3 const& vertex1
	)
{
	return
		vertex0[ coordinate_indices[0] ] * vertex1[ coordinate_indices[1] ] - 
		vertex0[ coordinate_indices[1] ] * vertex1[ coordinate_indices[0] ];
}

float XRAY_AI_NAVIGATION_API dot_product	(
	u32 const (&coordinate_indices)[2],
	float3 const& vertex0,
	float3 const& vertex1
	)
{
	return
		vertex0[ coordinate_indices[0] ] * vertex1[ coordinate_indices[0] ] + 
		vertex0[ coordinate_indices[1] ] * vertex1[ coordinate_indices[1] ];
}


void xray::ai::navigation::fill_coordinate_indices	( float3 const& normal, u32 (&coordinate_indices)[2] )
{
	float3 const& normal_abs		= float3( xray::math::abs(normal.x), xray::math::abs(normal.y), xray::math::abs(normal.z) );
	if ( normal_abs.x > normal_abs.y ) {
		if ( normal_abs.x > normal_abs.z ) {
			coordinate_indices[0]	= 1;
			coordinate_indices[1]	= 2;

			if ( xray::math::sign(normal.x) == 1.f )
				std::swap			( coordinate_indices[0], coordinate_indices[1] );
		}
		else {
			coordinate_indices[0]	= 0;
			coordinate_indices[1]	= 1;

			if ( xray::math::sign(normal.z) == 1.f )
				std::swap			( coordinate_indices[0], coordinate_indices[1] );
		}
	}
	else {
		if ( normal_abs.z > normal_abs.y ) {
			coordinate_indices[0]	= 0;
			coordinate_indices[1]	= 1;

			if ( xray::math::sign(normal.z) == 1.f )
				std::swap			( coordinate_indices[0], coordinate_indices[1] );
		}
		else {
			coordinate_indices[0]	= 0;
			coordinate_indices[1]	= 2;

			if ( xray::math::sign(normal.y) == -1.f )
				std::swap			( coordinate_indices[0], coordinate_indices[1] );
		}
	}
}

u32 get_third_index	( u32 const (&coordinate_indices)[2] )
{
	switch ( coordinate_indices[0] + coordinate_indices[1] ) {
		case 1 : return	2;
		case 2 : return 1;
		case 3 : return 0;
		default : NODEFAULT( return u32(-1) );
	}
}

inline bool delaunay_triangulator::is_correct_ordering	(
		u32 const (&coordinate_indices)[2],
		float3 v0,
		float3 v1,
		float3 v2
	) const
{
	u32 const third_index = get_third_index( coordinate_indices );
	v0[ third_index ] = 0.0f;
	v1[ third_index ] = 0.0f;
	v2[ third_index ] = 0.0f;

	char sign0 = math::sign( cross_product( coordinate_indices, normalize( v1 - v0 ),  normalize( v2 - v0 ) ) );
//	char sign1 = math::sign( cross_product( coordinate_indices, normalize( v2 - v1 ),  normalize( v0 - v1 ) ) );
//	char sign2 = math::sign( cross_product( coordinate_indices, normalize( v0 - v2 ),  normalize( v1 - v2 ) ) );

	/*
	if ( sign0 != 0 )
		return sign0 == m_ordering_sign;

	if ( sign1 != 0 )
		return sign1 == m_ordering_sign;
	*/

	return (sign0 == m_ordering_sign);
}

float3 try_normalize			( float3 const& value )
{
	float const length	= value.length();
	if ( length == 0.f )
		return			float3( 0.f, 0.f, 0.f );
	return				value / length;
}

float3 find_normal( float3 const& a, float3 const& b, float3 const& c )
{
	float3 const b_minus_a			= normalize(b - a);
	float3 const c_minus_a			= normalize(c - a);

	float3 normal0					= try_normalize( b_minus_a ^ c_minus_a );
	if ( normal0.length() > 0.f )
		return normal0;

	float3 const a_minus_b			= normalize(a - b);
	float3 const c_minus_b			= normalize(c - b);

	float3 normal1 = try_normalize(a_minus_b ^ c_minus_b);
	if ( normal1.length() > 0.f )
		return normal1;

	float3 const b_minus_c			= normalize(b - c);
	float3 const a_minus_c			= normalize(a - c);

	return try_normalize(b_minus_c ^ a_minus_c);
}

bool is_same_side				(
		float3 const& t,
		float3 const& a,
		float3 const& b,
		float3 const& c
	)
{
	using namespace xray::math;

	if (  t == a || t == b )
		return true;

	float3 const b_minus_a			= normalize(b - a);
	float3 const t_minus_a			= normalize(t - a);
	float3 const c_minus_a			= normalize(c - a);

	float3 normal0					= try_normalize(t_minus_a ^ b_minus_a);

	float3 const c_minus_b			= normalize(a - b);
	float3 const a_minus_b			= normalize(c - b);

	float3 const& plane_normal0		= try_normalize(c_minus_b ^ a_minus_b);
	float3 const& normal1			= try_normalize(normalize(t-b) ^ a_minus_b);

	float3 plane_normal				= try_normalize(c_minus_a ^ b_minus_a);
	if ( plane_normal.length() == 0 ) {
		plane_normal = plane_normal0;
		normal0 = normal1;
	}


	R_ASSERT_CMP					( plane_normal.length(), >, 0.f, "triangle's vertices lie on the same line!" );
	float const result				= normal0 | plane_normal;
	if ( result >= 0.f )
		return						true;
	else
		return						false;
}

bool is_point_inside_triangle	(
		float3 const& p,
		float3 const& a,
		float3 const& b,
		float3 const& c
	)
{
	return	is_same_side( p, a, b, c) && is_same_side( p, b, c, a) && is_same_side( p, c, a, b);
}

bool is_point_inside_triangle	(
		u32 const (&coordinate_indices)[2],
		float3 p,
		float3 a,
		float3 b,
		float3 c
	)
{

	u32 const third_index	= get_third_index( coordinate_indices );
	p[ third_index ]	= 0.f;
	a[ third_index ]	= 0.f;
	b[ third_index ]	= 0.f;
	c[ third_index ]	= 0.f;

	return				is_same_side( p, a, b, c) && is_same_side( p, b, c, a) && is_same_side( p, c, a, b);
}

bool is_outside_circumcircle						(
		u32 const (&coordinate_indices)[2],
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

bool delaunay_triangulator::is_vertex_of_the_bounding_triangle	( u32 const vertex_id ) const
{
	return						vertex_id >= (m_vertices_size - 3);
}

u32 delaunay_triangulator::find_triangle (
		u32 const (&coordinate_indices)[2],
		float3 const& vertex,
		u32 start_index,
		bool
	)
{

	for ( u32 i=start_index, n=m_indices.size()/3; i < n; ++i ) {
		/*
		u32 const id0 = m_indices[3*i + 0];
		u32 const id1 = m_indices[3*i + 1];
		u32 const id2 = m_indices[3*i + 2];
		*/
		if (
				is_point_inside_triangle(
					coordinate_indices,
					vertex,
					m_vertices[ m_indices[3*i + 0] ],
					m_vertices[ m_indices[3*i + 1] ],
					m_vertices[ m_indices[3*i + 2] ]
				)
			)
			return		i;
	}

	/*
	if ( !assert_on_failure )
		return			u32(-1);
	*/

	return				u32(-1);
//  	NODEFAULT			( return u32(-1) );
}

u32 delaunay_triangulator::find_neighbour_triangle_id	(
		u32 const triangle_id,
		u32 const vertex_id0,
		u32 const vertex_id1,
		u32& edge_id
	)
{
	R_ASSERT					( m_indices.size() % 3 == 0 );
	for (u32 i=0, n=m_indices.size()/3; i != n; ++i ) {
		if ( i == triangle_id )
			continue;

		if ( m_indices[3*i + 0] == vertex_id1 ) {
			if ( m_indices[3*i + 1] == vertex_id0 ) {
				edge_id			= 0;
				R_ASSERT		( m_indices[3*i + 2] != m_indices[3*triangle_id + 2] );
				return			i;
			}

			R_ASSERT			( m_indices[3*i + 2] != vertex_id0 );
			continue;
		}

		if ( m_indices[3*i + 1] == vertex_id1 ) {
			R_ASSERT			( m_indices[3*i + 0] != vertex_id0 );

			if ( m_indices[3*i + 2] == vertex_id0 ) {
				edge_id			= 1;
				R_ASSERT		( m_indices[3*i + 0] != m_indices[3*triangle_id + 0] );
				return			i;
			}

			continue;
		}

		if ( m_indices[3*i + 2] == vertex_id1 ) {
			if ( m_indices[3*i + 0] == vertex_id0 ) {
				edge_id			= 2;
				R_ASSERT		( m_indices[3*i + 1] != m_indices[3*triangle_id + 1] );
				return			i;
			}

			R_ASSERT			( m_indices[3*i + 1] != vertex_id0 );
			continue;
		}
	}

	return						u32(-1);
}

bool delaunay_triangulator::is_edge_legal		(
		u32 const (&coordinate_indices)[2],
		u32 const triangle_id,
		u32 const edge_id,
		u32 const fourth_vertex_index
	)
{
	u32 const test_indices[] =	{
		m_indices[3*triangle_id + (edge_id + 0)],
		m_indices[3*triangle_id + (edge_id + 1) % 3],
		m_indices[3*triangle_id + (edge_id + 2) % 3],
		fourth_vertex_index
	};
	u32 const vertex_of_the_bounding_triangle[] =	{
		is_vertex_of_the_bounding_triangle( test_indices[0] ) ? 1 : 0,
		is_vertex_of_the_bounding_triangle( test_indices[1] ) ? 1 : 0,
		is_vertex_of_the_bounding_triangle( test_indices[2] ) ? 1 : 0,
		is_vertex_of_the_bounding_triangle( test_indices[3] ) ? 1 : 0,
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
			m_vertices[ test_indices[0] ],
			m_vertices[ test_indices[1] ],
			m_vertices[ test_indices[2] ],
			m_vertices[ test_indices[3] ]
		);
}

static inline bool is_convex		(
		u32 const (&coordinate_indices)[2],
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

bool is_on_segment_impl	(
		u32 const (&coordinate_indices)[2],
		xray::math::float3 const& testee,				// t
		xray::math::float3 const& first_segment_vertex,	// v0
		xray::math::float3 const& second_segment_vertex,// v1
		float epsilon
	)
{
	float const v0_to_v1_squared_length			=
		xray::math::sqr(
			second_segment_vertex[ coordinate_indices[0] ] -
			first_segment_vertex[ coordinate_indices[0] ]
		) + 
		xray::math::sqr(
			second_segment_vertex[ coordinate_indices[1] ] -
			first_segment_vertex[ coordinate_indices[1] ]
		);
	float const v0_to_t_squared_length			=
		xray::math::sqr(
			testee[ coordinate_indices[0] ] -
			first_segment_vertex[ coordinate_indices[0] ]
		) + 
		xray::math::sqr(
			testee[ coordinate_indices[1] ] -
			first_segment_vertex[ coordinate_indices[1] ]
		);
	float const v1_to_t_squared_length			=
		xray::math::sqr(
			testee[ coordinate_indices[0] ] -
			second_segment_vertex[ coordinate_indices[0] ]
		) + 
		xray::math::sqr(
			testee[ coordinate_indices[1] ] -
			second_segment_vertex[ coordinate_indices[1] ]
		);
	float const v0_to_t_len_mul_v1_to_t_length	= xray::math::sqrt( v0_to_t_squared_length * v1_to_t_squared_length );
	float const a2_plus_2ab_plus_b2				= v0_to_t_squared_length + 2 * v0_to_t_len_mul_v1_to_t_length + v1_to_t_squared_length;
	return
		xray::math::is_relatively_similar (
			a2_plus_2ab_plus_b2,
			v0_to_v1_squared_length,
			epsilon
		);
}

bool XRAY_AI_NAVIGATION_API is_on_segment	(
		u32 const (&coordinate_indices)[2],
		xray::math::float3 const& testee,				// t
		xray::math::float3 const& first_segment_vertex,	// v0
		xray::math::float3 const& second_segment_vertex,// v1
		float const epsilon
	)
{
	if (
			is_on_segment_impl(
				coordinate_indices,
				testee,
				first_segment_vertex,
				second_segment_vertex,
				epsilon
			)
		)
		return							true;

	float3 v[] = {
		testee,
		first_segment_vertex,
		second_segment_vertex
	};
	u32 const third_index = get_third_index( coordinate_indices );
	v[0][ third_index ] = 0;
	v[1][ third_index ] = 0;
	v[2][ third_index ] = 0;

	for ( u32 i = 0; i < 3; ++i ) {
		if ( cross_product( coordinate_indices, normalize(v[(i+1)%3] - v[i]), normalize(v[(i+2)%3] - v[i]) ) == 0.f )
			return						( (v[0] - v[1]) | (v[2] - v[0]) ) >= 0.f;
	}

	return								false;
}

bool is_on_segment	(
		xray::math::float3 const& testee,				// t
		xray::math::float3 const& first_segment_vertex,	// v0
		xray::math::float3 const& second_segment_vertex,// v1
		float const epsilon
	)
{
	float const v0_to_v1_squared_length			= squared_length( second_segment_vertex - first_segment_vertex );
	float const v0_to_t_squared_length			= squared_length( testee - first_segment_vertex );
	float const v1_to_t_squared_length			= squared_length( testee - second_segment_vertex );
	float const v0_to_t_len_mul_v1_to_t_length	= xray::math::sqrt( v0_to_t_squared_length * v1_to_t_squared_length );
	float const a2_plus_2ab_plus_b2				= v0_to_t_squared_length + 2 * v0_to_t_len_mul_v1_to_t_length + v1_to_t_squared_length;
	return
		xray::math::is_similar (
			a2_plus_2ab_plus_b2,
			v0_to_v1_squared_length,
			epsilon
		);
}

bool is_on_segment	(
		xray::math::float3 const& testee,				// t
		xray::math::float3 const& first_segment_vertex,	// v0
		xray::math::float3 const& second_segment_vertex	// v1
	)
{
	return	is_on_segment( testee, first_segment_vertex, second_segment_vertex, xray::math::epsilon_7 );
}

void delaunay_triangulator::legalize_edge (
		u32 const (&coordinate_indices)[2],
		u32 const triangle_id,
		u32 const edge_id
	)
{
	u32 neighbour_edge_id			= u32(-1);
	u32 const neighbour_triangle_id	= find_neighbour_triangle_id( triangle_id, m_indices[3*triangle_id + edge_id], m_indices[3*triangle_id + ((edge_id + 1) % 3)], neighbour_edge_id );
	if ( neighbour_triangle_id == u32(-1) )
		return;

	u32 const new_vertex_index	= m_indices[ 3*neighbour_triangle_id + ((neighbour_edge_id + 2) % 3) ];
	u32 const new_indices[] = {
		m_indices[3*triangle_id],
		m_indices[3*triangle_id + 1],
		m_indices[3*triangle_id + 2],
		new_vertex_index
	};

	R_ASSERT_CMP						( new_indices[0], !=, new_vertex_index );
	R_ASSERT_CMP						( new_indices[1], !=, new_vertex_index );
	R_ASSERT_CMP						( new_indices[2], !=, new_vertex_index );

	if (
			!is_convex(
				coordinate_indices,
				m_vertices[ new_indices[edge_id] ],
				m_vertices[ new_indices[3] ],
				m_vertices[ new_indices[(edge_id + 1) % 3] ],
				m_vertices[ new_indices[(edge_id + 2) % 3] ]
			)
		)
		return;

	R_ASSERT						( new_vertex_index != u32(-1) );
	if ( is_edge_legal( coordinate_indices, triangle_id, edge_id, new_vertex_index) )
		return;

	if (
		is_on_segment(
			coordinate_indices,
			m_vertices[ new_indices[(edge_id) % 3] ],
			m_vertices[ new_indices[(edge_id + 2) % 3] ],
			m_vertices[ new_indices[3] ]
		) ||
		is_on_segment(
			coordinate_indices,
			m_vertices[ new_indices[(edge_id + 1) % 3] ],
			m_vertices[ new_indices[(edge_id + 2) % 3] ],
			m_vertices[ new_indices[3] ]
		)
	)
		return;

	if ( !is_correct_ordering( 
			coordinate_indices,
			m_vertices[ new_indices[ edge_id ] ],
			m_vertices[ new_indices[3] ],
			m_vertices[ new_indices[(edge_id+2)%3] ]
		) ||
		!is_correct_ordering( 
			coordinate_indices,
			m_vertices[ new_indices[ (edge_id+2)%3 ] ],
			m_vertices[ new_indices[3] ],
			m_vertices[ new_indices[(edge_id+1)%3] ]
		) )
		return;


	// flip edge and recurse
	m_indices[3*triangle_id + 0]		= new_indices[edge_id];
	m_indices[3*triangle_id + 1]		= new_indices[3];
	m_indices[3*triangle_id + 2]		= new_indices[(edge_id + 2) % 3];

	R_ASSERT							( 
		is_correct_ordering (
			coordinate_indices,
			m_vertices[ m_indices[3*triangle_id + 0] ],
			m_vertices[ m_indices[3*triangle_id + 1] ],
			m_vertices[ m_indices[3*triangle_id + 2] ]
		)
	);

	m_indices[3*neighbour_triangle_id + 0]	= new_indices[(edge_id + 2) % 3];
	m_indices[3*neighbour_triangle_id + 1]	= new_indices[3];
	m_indices[3*neighbour_triangle_id + 2]	= new_indices[(edge_id + 1) % 3];

	R_ASSERT							( 
		is_correct_ordering( 
			coordinate_indices,
			m_vertices[ m_indices[3*neighbour_triangle_id + 0] ],
			m_vertices[ m_indices[3*neighbour_triangle_id + 1] ],
			m_vertices[ m_indices[3*neighbour_triangle_id + 2] ]
		)
	);

	legalize_edge					( coordinate_indices, triangle_id, 0 );
	legalize_edge					( coordinate_indices, neighbour_triangle_id, 1 );
}

void delaunay_triangulator::split_triangle			(
		u32 const (&coordinate_indices)[2],
		u32 const vertex_index,
		u32 const triangle_id,
		u32 const edge_id
	)
{
	u32 const triangle_indices[] = {
		m_indices[3*triangle_id + ((edge_id + 0))],
		m_indices[3*triangle_id + ((edge_id + 1) % 3)],
		m_indices[3*triangle_id + ((edge_id + 2) % 3)],
	};

	R_ASSERT_CMP				( m_indices.size() % 3, ==, 0 );
	u32 const new_triangle_index = m_indices.size() / 3;
	m_indices.push_back			( triangle_indices[0] );
	m_indices.push_back			( vertex_index );
	m_indices.push_back			( triangle_indices[2] );

	R_ASSERT					( 
		is_correct_ordering( 
			coordinate_indices,
			m_vertices[ m_indices[m_indices.size() - 3] ],
			m_vertices[ m_indices[m_indices.size() - 2] ],
			m_vertices[ m_indices[m_indices.size() - 1] ]
		)
	);

	m_indices[3*triangle_id + 0]	= vertex_index;
	m_indices[3*triangle_id + 1]	= triangle_indices[1];
	m_indices[3*triangle_id + 2]	= triangle_indices[2];

	R_ASSERT							( 
		is_correct_ordering( 
			coordinate_indices,
			m_vertices[ m_indices[3*triangle_id + 0] ],
			m_vertices[ m_indices[3*triangle_id + 1] ],
			m_vertices[ m_indices[3*triangle_id + 2] ]
		)
	);

	legalize_edge				( coordinate_indices, new_triangle_index, 2 );
	legalize_edge				( coordinate_indices, triangle_id, 1 );
}

void delaunay_triangulator::add_vertex_inside_triangle_to_triangulation	(
		u32 const (&coordinate_indices)[2],
		u32 const triangle_id,
		u32 const vertex_id
	)
{
	m_indices.push_back			( m_indices[3*triangle_id + 0] );
	m_indices.push_back			( m_indices[3*triangle_id + 1] );
	m_indices.push_back			( vertex_id );

	R_ASSERT					( 
		is_correct_ordering( 
			coordinate_indices,
			m_vertices[ m_indices[m_indices.size() - 3] ],
			m_vertices[ m_indices[m_indices.size() - 2] ],
			m_vertices[ m_indices[m_indices.size() - 1] ]
		)
	);

	m_indices.push_back			( m_indices[3*triangle_id + 1] );
	m_indices.push_back			( m_indices[3*triangle_id + 2] );
	m_indices.push_back			( vertex_id );

	R_ASSERT							( 
		is_correct_ordering( 
			coordinate_indices,
			m_vertices[ m_indices[m_indices.size() - 3] ],
			m_vertices[ m_indices[m_indices.size() - 2] ],
			m_vertices[ m_indices[m_indices.size() - 1] ]
		)
	);

	R_ASSERT							( 
		is_correct_ordering( 
			coordinate_indices,
			m_vertices[ m_indices[3*triangle_id + 2] ],
			m_vertices[ m_indices[3*triangle_id + 0] ],
			m_vertices[ vertex_id ]
		)
	);

	// order of indices (1, then 0), is important here, since we're performing changes in-place
	m_indices[3*triangle_id + 1]	= m_indices[3*triangle_id + 0];
	m_indices[3*triangle_id + 0]	= m_indices[3*triangle_id + 2];
	m_indices[3*triangle_id + 2]	= vertex_id;

	R_ASSERT					( m_indices.size() % 3 == 0 );

	u32 const indices_size		= m_indices.size( );
	legalize_edge				( coordinate_indices, indices_size/3 - 2,	0 );
	legalize_edge				( coordinate_indices, indices_size/3 - 1,	0 );
	legalize_edge				( coordinate_indices, triangle_id,		0 );
}

bool delaunay_triangulator::split_triangles_are_correct	(
		u32 const (&coordinate_indices)[2],
		u32 const vertex_index,
		u32 const triangle_id,
		u32 const edge_id
	) const
{
	u32 const triangle_indices[] = {
		m_indices[3*triangle_id + ((edge_id + 0))],
		m_indices[3*triangle_id + ((edge_id + 1) % 3)],
		m_indices[3*triangle_id + ((edge_id + 2) % 3)],
	};
	return
		is_correct_ordering( 
			coordinate_indices,
			m_vertices[ triangle_indices[0] ],
			m_vertices[ vertex_index ],
			m_vertices[ triangle_indices[2] ]
		) &&
		is_correct_ordering( 
			coordinate_indices,
			m_vertices[ vertex_index ],
			m_vertices[ triangle_indices[1] ],
			m_vertices[ triangle_indices[2] ]
		);
}

bool delaunay_triangulator::split_triangles_are_correct	(
		u32 const (&coordinate_indices)[2],
		u32 const vertex_index,
		u32 const triangle_id,
		u32 const edge_id,
		u32 const neighbour_triangle_id,
		u32 const neighbour_edge_id
	) const
{
	return
		split_triangles_are_correct(
			coordinate_indices,
			vertex_index,
			triangle_id,
			edge_id
		) &&
		split_triangles_are_correct(
			coordinate_indices,
			vertex_index,
			neighbour_triangle_id,
			neighbour_edge_id
		);
}

bool delaunay_triangulator::add_vertex_on_edge_to_triangulation	(
		u32 const (&coordinate_indices)[2],
		u32 const vertex_index,
		u32 const triangle_id,
		u32 const edge_id
	)
{
	u32 neighbour_edge_id;
	u32 const neighbour_triangle_id	=
		find_neighbour_triangle_id(
			triangle_id,
			m_indices[ 3*triangle_id + edge_id ],
			m_indices[ 3*triangle_id + ((edge_id +1) % 3)],
			neighbour_edge_id
		);
	if ( neighbour_triangle_id == u32(-1) ) {
		split_triangle			( coordinate_indices, vertex_index, triangle_id, edge_id );
		return					true;
	}

	if ( !split_triangles_are_correct(coordinate_indices, vertex_index, triangle_id, edge_id, neighbour_triangle_id, neighbour_edge_id) )
		return					false;

	split_triangle				( coordinate_indices, vertex_index, triangle_id, edge_id );
	split_triangle				( coordinate_indices, vertex_index, neighbour_triangle_id, neighbour_edge_id );
	return						true;
}

static inline float squared_length	(
		u32 const (&coordinate_indices)[2],
		float3 const& vector
	)
{
	return
		xray::math::sqr( vector[ coordinate_indices[0] ] ) +
		xray::math::sqr( vector[ coordinate_indices[1] ] );
}

bool XRAY_AI_NAVIGATION_API is_similar ( 
		u32 const (&coordinate_indices)[2],
		float3 const& v0,
		float3 const& v1,
		float epsilon
	)
{
	return xray::math::is_similar( v0[coordinate_indices[0]], v1[coordinate_indices[0]], epsilon ) &&
			xray::math::is_similar( v0[coordinate_indices[1]], v1[coordinate_indices[1]], epsilon );
}

float distance_to_segment_squared( u32 const (&coordinate_indices)[2], float3 v, float3 a, float3 b )
{
	u32 third_index = get_third_index( coordinate_indices );
	v[ third_index ] = 0;
	a[ third_index ] = 0;
	b[ third_index ] = 0;
	return xray::math::abs(cross_product( coordinate_indices, v - a, normalize(b - a) ));
}

bool delaunay_triangulator::add_vertex_to_triangulation		(
		u32 const (&coordinate_indices)[2],
		u32 const vertex_id
	)
{
	float3 const& vertex		= m_vertices[ vertex_id ];
	u32 const triangle_id		= find_triangle( coordinate_indices, vertex );
	
	if ( triangle_id == u32(-1) )
		return false;

	u32 const indices[] = {
		m_indices[3*triangle_id + 0],
		m_indices[3*triangle_id + 1],
		m_indices[3*triangle_id + 2]
	};

	float3 const& v0			= m_vertices[ m_indices[3*triangle_id + 0] ];
	float3 const& v1			= m_vertices[ m_indices[3*triangle_id + 1] ];
	float3 const& v2			= m_vertices[ m_indices[3*triangle_id + 2] ];

	float const epsilon			= math::epsilon_6;
	R_ASSERT_U					( !is_similar(coordinate_indices, v0, vertex, epsilon), "do not pass vertices with distance less than %f", epsilon );
	R_ASSERT_U					( !is_similar(coordinate_indices, v1, vertex, epsilon), "do not pass vertices with distance less than %f", epsilon );
	R_ASSERT_U					( !is_similar(coordinate_indices, v2, vertex, epsilon), "do not pass vertices with distance less than %f", epsilon );

	u32 min_edge_id = u32(-1);
	float min_distance = 0;

	for ( u32 i = 0; i < 3; ++i ) {
		if ( is_vertex_of_the_bounding_triangle( indices[i] ) && is_vertex_of_the_bounding_triangle( indices[(i+1)%3] ) )
			continue;

		float distance = distance_to_segment_squared(coordinate_indices, vertex, m_vertices[ indices[i] ], m_vertices[ indices[(i+1) % 3] ] );
		if ( min_edge_id == u32(-1) || distance < min_distance ) {
			min_edge_id = i;
			min_distance = distance;
		}
	}
	
	if ( min_edge_id != u32(-1) && is_on_segment( coordinate_indices, vertex, m_vertices[ indices[(min_edge_id+0)%3]], m_vertices[ indices[(min_edge_id+1)%3]] ) ) {
		if ( add_vertex_on_edge_to_triangulation( coordinate_indices, vertex_id, triangle_id, min_edge_id ) )
			return true;
	}

	add_vertex_inside_triangle_to_triangulation	( coordinate_indices, triangle_id, vertex_id );
	return true;
}
