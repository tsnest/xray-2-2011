////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "graph_generator_tessellator.h"
#include "delaunay_triangulator.h"
#include "constrained_delaunay_triangulator.h"
#include <xray/collision/common_types.h>
#include <xray/math_randoms_generator.h>
#include "delaunay_triangulator.h"
#define __COPLANAR_TESSELLATION_METHOD1

using xray::ai::navigation::graph_generator_tessellator;
using xray::ai::navigation::delaunay_triangulator;
using xray::ai::navigation::triangles_mesh_type;
using xray::math::float3;

bool is_point_inside_triangle	(
	u32 const (&coordinate_indices)[2],
	float3 p,
	float3 a,
	float3 b,
	float3 c
);

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

bool is_on_segment	(
		u32 const (&coordinate_indices)[2],
		xray::math::float3 const& testee,				// t
		xray::math::float3 const& first_segment_vertex,	// v0
		xray::math::float3 const& second_segment_vertex,// v1
		float epsilon
	);

bool is_same_side_segment( 
	float3 const& p1, 
	float3 const& p2, 
	float3 const& a, 
	float3 const& b 
)
{
	return	(((b - a) ^ (p1 - a)) | ((b - a) ^ (p2 - a))) >= 0.f;
}

u32 try_on_segment	(
		u32 const (&coordinate_indices)[2],
		float3 const& v0,
		float3 const& v1,
		float3 const& u0,
		float3 const& u1,
		float3& result
	)
{
	if ( is_on_segment(coordinate_indices, u0, v0, v1) ) {
		result				= u0;
		return				1;
	}

	if ( is_on_segment(coordinate_indices, u1, v0, v1) ) {
		result				= u1;
		return				1;
	}

	if ( is_on_segment(coordinate_indices, v0, u0, u1) ) {
		result				= v0;
		return				1;
	}

	if ( is_on_segment(coordinate_indices, v1, u0, u1) ) {
		result				= v1;
		return				1;
	}

	return					0;
}

// http://paulbourke.net/geometry/lineline2d/
u32 xray::ai::navigation::segment_intersects_segment	(
		u32 const (&coordinate_indices)[2],
		float3 const& v0,
		float3 const& v1,
		float3 const& u0,
		float3 const& u1,
		float3& result0,
		float3& result1,
		bool& are_on_the_same_line
	)
{
	are_on_the_same_line	= false;

	float const& ax = v0[ coordinate_indices[0] ];
	float const& ay = v0[ coordinate_indices[1] ];
	float const& bx = v1[ coordinate_indices[0] ];
	float const& by = v1[ coordinate_indices[1] ];
	float const& cx = u0[ coordinate_indices[0] ];
	float const& cy = u0[ coordinate_indices[1] ];
	float const& dx = u1[ coordinate_indices[0] ];
	float const& dy = u1[ coordinate_indices[1] ];

	float3 u					= u1 - u0;
	u							/= xray::math::sqrt( xray::math::sqr(u[ coordinate_indices[0] ]) + xray::math::sqr(u[ coordinate_indices[1] ]) );
	float3 const& u2			= u0 + u;
	float const& dx_big			= u2[ coordinate_indices[0] ];
	float const& dy_big			= u2[ coordinate_indices[1] ];

	float3 v					= v1 - v0;
	v							/= xray::math::sqrt( xray::math::sqr(v[ coordinate_indices[0] ]) + xray::math::sqr(v[ coordinate_indices[1] ]) );
	float3 const& v2			= v0 + v;
	float const& bx_big			= v2[ coordinate_indices[0] ];
	float const& by_big			= v2[ coordinate_indices[1] ];

	float const denominator_big	= (dy_big - cy)*(bx_big - ax) - (dx_big - cx)*(by_big - ay);
	float const denominator		= (dy - cy)*(bx - ax) - (dx - cx)*(by - ay);

	are_on_the_same_line		= false;
	bool are_parallel			= xray::math::is_zero(denominator_big, xray::math::epsilon_6) || ( denominator == 0 );

	//if ( !are_parallel && xray::math::is_zero(denominator_big, xray::math::epsilon_5) ) {
	//	if ( is_on_segment(coordinate_indices, u0, v0, v1) && is_on_segment(coordinate_indices, u1, v0, v1) )
	//		are_parallel		= true;
	//	else if ( is_on_segment(coordinate_indices, v0, u0, u1) && is_on_segment(coordinate_indices, v1, u0, u1) )
	//		are_parallel		= true;
	//}
	if ( are_parallel ) {
		float const numerator_big	= (bx_big - ax)*(ay - cy) - (by_big - ay)*(ax - cx);
		if ( !xray::math::is_zero(numerator_big, xray::math::epsilon_5) )
			return				try_on_segment( coordinate_indices, v0, v1, u0, u1, result0 );

		are_on_the_same_line	= true;
		bool should_compare_x	= xray::math::abs(ax - bx) >= xray::math::abs(ay - by);

		float const values[]	= {
			should_compare_x ? (ax < bx ? ax : bx) : (ay < by ? ay : by),
			should_compare_x ? (ax < bx ? bx : ax) : (ay < by ? by : ay),
			should_compare_x ? (cx < dx ? cx : dx) : (cy < dy ? cy : dy),
			should_compare_x ? (cx < dx ? dx : cx) : (cy < dy ? dy : cy),
		};

		float3 const* values_3d[] = {
			should_compare_x ? (ax < bx ? &v0 : &v1) : (ay < by ? &v0 : &v1),
			should_compare_x ? (ax < bx ? &v1 : &v0) : (ay < by ? &v1 : &v0),
			should_compare_x ? (cx < dx ? &u0 : &u1) : (cy < dy ? &u0 : &u1),
			should_compare_x ? (cx < dx ? &u1 : &u0) : (cy < dy ? &u1 : &u0),
		};
		if ( values[0] < values[2] ) {
			if ( values[1] < values[2] )
				return			try_on_segment( coordinate_indices, v0, v1, u0, u1, result0 );

			if ( values[3] < values[1] ) {
				result0			= *values_3d[2];
				result1			= *values_3d[3];
				return			(values[2] == values[3]) ? 1 : 2;
			}

			result0				= *values_3d[2];
			result1				= *values_3d[1];
			return				(values[1] == values[2]) ? 1 : 2;
		}

		if ( values[3] < values[0] )
			return				try_on_segment( coordinate_indices, v0, v1, u0, u1, result0 );

		if ( values[1] < values[3] ) {
			result0				= *values_3d[0];
			result1				= *values_3d[1];
			return				(values[0] == values[1]) ? 1 : 2;
		}

		result0					= *values_3d[0];
		result1					= *values_3d[3];
		return					(values[3] == values[0]) ? 1 : 2;
	}

	float const numerator0		= (dx - cx)*(ay - cy) - (dy - cy)*(ax - cx);
	float r0					= numerator0 / denominator;

	float const numerator1		= (bx - ax)*(ay - cy) - (by - ay)*(ax - cx);
	float r1					= numerator1 / denominator;

	float const epsilon			= xray::math::epsilon_7;

	r0							= xray::math::is_similar( r0, 1.0f, epsilon ) ? 1.0f : r0;
	r1							= xray::math::is_similar( r1, 1.0f, epsilon ) ? 1.0f : r1;
	r0							= xray::math::is_similar( r0, 0.0f, epsilon ) ? 0.0f : r0;
	r1							= xray::math::is_similar( r1, 0.0f, epsilon ) ? 0.0f : r1;

	if ( (r0 > 1.f) || (r1 > 1.f) )
		return					try_on_segment( coordinate_indices, v0, v1, u0, u1, result0 );

	if ( (r0 < 0.f) || (r1 < 0.f) )
		return					try_on_segment( coordinate_indices, v0, v1, u0, u1, result0 );

	if ( xray::math::abs(numerator0) >= xray::math::abs(numerator1) )
		result0					= v0 + r0*(v1 - v0);
	else
		result0					= u0 + r1*(u1 - u0);

	return						1;
}
using xray::ai::navigation::segment_intersects_segment;

struct less_by_coordinate_indices {
	less_by_coordinate_indices( u32 const (&coordinate_indices)[2] ) :
		m_coordinate_indices( coordinate_indices )
	{
	}

	inline bool operator() ( float3 const& left, float3 const& right )
	{
		if ( left[m_coordinate_indices[0]] < right[m_coordinate_indices[0]] )
			return true;

		if ( left[m_coordinate_indices[0]] > right[m_coordinate_indices[0]] )
			return false;

		return left[m_coordinate_indices[1]] < right[m_coordinate_indices[1]];
	}

private:
	u32 const* m_coordinate_indices;
}; // struct less_by_coordinate_indices

struct equal_by_coordinate_indices {
	equal_by_coordinate_indices( u32 const (&coordinate_indices)[2] ) :
		m_coordinate_indices( coordinate_indices )
	{
	}
	
	inline bool operator() ( float3& left, float3& right )
	{
		return
			xray::math::is_relatively_similar( left[m_coordinate_indices[0]], right[m_coordinate_indices[0]], xray::math::epsilon_6) &&
			xray::math::is_relatively_similar( left[m_coordinate_indices[1]], right[m_coordinate_indices[1]], xray::math::epsilon_6);
	}

private:
	u32 const* m_coordinate_indices;

}; // struct equal_by_coordinate_indices

u32 segment_intersects_triangle	(
		float3 const& normal,
		float3 const& segment0,
		float3 const& segment1,
		float3 const& vertex0,
		float3 const& vertex1,
		float3 const& vertex2,
		float3 (&result_intersections)[2]
	)
{
	u32 coordinate_indices[2];
	xray::ai::navigation::fill_coordinate_indices	( normal, coordinate_indices );

	float3 intersections[3];

	float3* const begin		= &intersections[0];
	float3* intersection	= begin;
	float3* previous_intersection	= begin;

	bool are_on_the_same_line;
	intersection			+= segment_intersects_segment( coordinate_indices, segment0, segment1, vertex0, vertex1, *intersection, *(intersection + 1), are_on_the_same_line );
	if ( !are_on_the_same_line && (intersection - begin < 2) ) {
		previous_intersection	= intersection;
		intersection		+= segment_intersects_segment( coordinate_indices, segment0, segment1, vertex1, vertex2, *intersection, *(intersection + 1), are_on_the_same_line );
		if ( intersection - begin == 2 ) {
			if ( is_similar( intersections[0], intersections[1] ) )
				intersection = &intersections[1];
		}
		if ( !are_on_the_same_line && (intersection - begin < 2) ) {
			previous_intersection	= intersection;
			intersection		+= segment_intersects_segment( coordinate_indices, segment0, segment1, vertex2, vertex0, *intersection, *(intersection + 1), are_on_the_same_line );
		}
	}

	if ( are_on_the_same_line ) {
		for ( float3* i = previous_intersection, *j = begin; i != intersection; ++i, ++j )
			*j			= *i;
		intersection	= begin + (intersection - previous_intersection);
	}

	if ( intersection - begin == 3 ) {
		std::sort			( begin, intersection, less_by_coordinate_indices( coordinate_indices ) );
		intersection		= std::unique( begin, intersection, equal_by_coordinate_indices( coordinate_indices ) );
	}

	R_ASSERT_CMP			( intersection - begin, <, 3 );

	result_intersections[0]	= intersections[0];
	result_intersections[1]	= intersections[1];

	u32 const vertex_count	= u32(intersection - begin);
	if ( (vertex_count > 1) && xray::math::is_similar( result_intersections[0], result_intersections[1]) )
		return				1;

	return					vertex_count;
}

typedef xray::buffer_vector< float3 >	vertices_type;
typedef xray::buffer_vector< u32 >		indices_type;

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

bool is_on_segment	(
		u32 const (&coordinate_indices)[2],
		xray::math::float3 const& testee,
		xray::math::float3 const& first_segment_vertex,
		xray::math::float3 const& second_segment_vertex,
		float epsilon
	);

float cross_product	(
		u32 const (&coordinate_indices)[2],
		float3 const& vertex0,
		float3 const& vertex1
	);
// http://groups.csail.mit.edu/graphics/classes/6.838/F01/lectures/Delaunay/Delaunay2D.ppt
static inline u32 select_vertex_index ( float3 const& vertex, float3 const* const (&vertices)[6], u32 const* const indices )
{
	for ( u32 i = 0; i < 6; ++i) 
		if ( is_similar(vertex, *vertices[i]) )
			return indices[i];

	return			u32( -1 );
}

u32 select_vertex_index	(
		float3 const& vertex,
		float3 const* (vertices)[3],
		u32 const* const indices
	);

bool graph_generator_tessellator::is_null_square ( 
		float3 const& a,
		float3 const& b,
		float3 const& c
	)
{
	float3 const b_minus_a = normalize( b - a );
	float3 const c_minus_a = normalize( c - a );
	float3 const a_minus_b = normalize( a - b );
	float3 const c_minus_b = normalize( c - b );
	float3 const a_minus_c = normalize( a - c );
	float3 const b_minus_c = normalize( b - c );

	static float const epsilon = math::epsilon_3;

	if ( b_minus_a.is_similar(c_minus_a, epsilon) || b_minus_a.is_similar(-c_minus_a, epsilon) )
		return					true;

	if ( a_minus_b.is_similar(c_minus_b, epsilon) || a_minus_b.is_similar(-c_minus_b, epsilon) )
		return					true;

	if ( a_minus_c.is_similar(b_minus_c, epsilon) || a_minus_c.is_similar(-b_minus_c, epsilon) )
		return					true;

	return false;
}

bool graph_generator_tessellator::add_output_triangle( 
		u32 const triangle_id, 
		float3 const& triangle_vertex0,
		float3 const& triangle_vertex1,
		float3 const& triangle_vertex2,
		bool check_null_square
	)
{
	if ( check_null_square ) {
		static float const epsilon = xray::math::epsilon_3;
		if ( is_similar(triangle_vertex0, triangle_vertex1, epsilon)
				|| is_similar(triangle_vertex0, triangle_vertex2, epsilon)
				|| is_similar(triangle_vertex1, triangle_vertex2, epsilon))
		{
			return false;
		}

		/*		
		xray::math::float3 const b_minus_a	= normalize( triangle_vertex1 - triangle_vertex0 );
		xray::math::float3 const c_minus_a	= normalize( triangle_vertex2 - triangle_vertex0 );

		//!
		if ( b_minus_a.is_similar(c_minus_a, math::epsilon_5) || b_minus_a.is_similar(-c_minus_a, math::epsilon_5) )
			return					false;
		*/
		if ( is_null_square( triangle_vertex0, triangle_vertex1, triangle_vertex2 ) )
			return false;
	}

	if ( m_tessellation_current_operation_id++ >= m_tessellation_max_operation_id )
		return					true;

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
	m_triangles_mesh.data.push_back ( m_triangles_mesh.data[triangle_id] );
	m_triangles_mesh.data[ new_triangle_id ].is_marked = false;
	m_triangle_info.push_back ( triangle_info() );

	if ( m_triangle_info[ triangle_id ].parent == u32(-1) )
		m_triangle_info[ new_triangle_id ].parent = triangle_id;
	else
		m_triangle_info[ new_triangle_id ].parent = m_triangle_info[ triangle_id ].parent;


	m_triangle_info[ new_triangle_id ].next_triangle = m_triangle_info[ triangle_id ].next_triangle;
	m_triangle_info[ triangle_id ].next_triangle = new_triangle_id;
	
	m_triangle_info[ triangle_id ].is_processed = true;
	m_triangles_mesh.data[ triangle_id ].is_marked = true;

	return true;
}

bool is_same_normal	(
		float3 const& v0,
		float3 const& v1,
		float3 const& v2,
		float3 const& normal
	);

inline bool is_on_triangle_edge ( 
		u32 const (&coordinate_indices)[2],
		float3 const& testee, 
		float3 const& v0,
		float3 const& v1,
		float3 const& v2
	)
{
	if ( is_on_segment( coordinate_indices, testee, v0, v1 )
		|| is_on_segment( coordinate_indices, testee, v1, v2 )
		|| is_on_segment( coordinate_indices, testee, v2, v0 ) )
	{
		return true;
	}
	return false;
}
	
XRAY_AI_NAVIGATION_API bool xray::ai::navigation::is_triangle_inside_triangle (
		u32 const (&coordinate_indices)[2],
		float3 const& v0,
		float3 const& v1,
		float3 const& v2,
		float3 const& u0,
		float3 const& u1,
		float3 const& u2
	)
{
	u32 count = 0;
	if ( is_point_inside_triangle( coordinate_indices, v0, u0, u1, u2 ) ) {
		++count;
	} else {
		if ( is_on_triangle_edge ( coordinate_indices, v0, u0, u1, u2 ) ) 
			++count;
	}

	if ( is_point_inside_triangle( coordinate_indices, v1, u0, u1, u2 ) ) {
		++count;
	} else {
		if ( is_on_triangle_edge ( coordinate_indices, v1, u0, u1, u2 ) ) 
			++count;
	}

	if ( is_point_inside_triangle( coordinate_indices, v2, u0, u1, u2 ) ) {
		++count;
	} else {
		if ( is_on_triangle_edge ( coordinate_indices, v2, u0, u1, u2 ) ) 
			++count;
	}

	return count == 3;
}

void graph_generator_tessellator::get_colinear_edge( 
		u32 const (&coordinate_indices)[2],
		u32 const triangle_id0, 
		u32 const triangle_id1,
		u32& colinear_edge_id0,
		u32& colinear_edge_id1
	)
{
	u32 const* indices0 = m_triangles_mesh.indices.begin() + triangle_id0 * 3;
	u32 const* indices1 = m_triangles_mesh.indices.begin() + triangle_id1 * 3;
	for ( u32 i = 0; i < 3; ++i ) {
		float3 const& v0 = m_triangles_mesh.vertices[ indices0[ ( i + 0 ) % 3 ] ];
		float3 const& v1 = m_triangles_mesh.vertices[ indices0[ ( i + 1 ) % 3 ] ];

		for ( u32 j = 0; j < 3; ++j ) {
			float3 const& u0 = m_triangles_mesh.vertices[ indices1[ ( j + 0 ) % 3 ] ];
			float3 const& u1 = m_triangles_mesh.vertices[ indices1[ ( j + 1 ) % 3 ] ];

			if ( (is_on_segment( coordinate_indices, v0, u0, u1 ) && is_on_segment( coordinate_indices, v1, u0, u1 )) 
				|| (is_on_segment( coordinate_indices, u0, v0, v1 ) && is_on_segment( coordinate_indices, u1, v0, v1 ) ) ) {
				colinear_edge_id0 = i;
				colinear_edge_id1 = j;
				return;
			}

		}
	}
}

#ifdef __COPLANAR_TESSELLATION_METHOD1
bool graph_generator_tessellator::tessellate_coplanar_triangles	(
		u32 const triangle_id0,
		u32 const triangle_id1
	)
{
	u32 const indices0[] = {
		m_triangles_mesh.indices[ 3*triangle_id0 + 0 ],	
		m_triangles_mesh.indices[ 3*triangle_id0 + 1 ],	
		m_triangles_mesh.indices[ 3*triangle_id0 + 2 ]
	};

	float3 const v0	= m_triangles_mesh.vertices[ indices0[0] ];
	float3 const v1	= m_triangles_mesh.vertices[ indices0[1] ];
	float3 const v2	= m_triangles_mesh.vertices[ indices0[2] ];

	u32 const indices1[] = {
		m_triangles_mesh.indices[ 3*triangle_id1 + 0 ],	
		m_triangles_mesh.indices[ 3*triangle_id1 + 1 ],	
		m_triangles_mesh.indices[ 3*triangle_id1 + 2 ]
	};

	float3 const u0	= m_triangles_mesh.vertices[ indices1[0] ];
	float3 const u1	= m_triangles_mesh.vertices[ indices1[1] ];
	float3 const u2	= m_triangles_mesh.vertices[ indices1[2] ];

	static int const max_triangle_intersections_count = 11;
	u32 const max_vertex_count	= max_triangle_intersections_count + delaunay_triangulator::get_additional_vertex_count( );
	delaunay_triangulator::vertices_type vertices( 
		ALLOCA(max_vertex_count*sizeof(delaunay_triangulator::vertices_type::value_type)), 
		max_vertex_count 
	);

	float3 single_intersection_vertex;

	u32 coordinate_indices[2];
	fill_coordinate_indices	( m_triangles_mesh.data[triangle_id0].plane.normal, coordinate_indices );

	bool subtract_triangle_flag = true;
	if ( is_triangle_inside_triangle( coordinate_indices, u0, u1, u2, v0, v1, v2 ) ) {
		m_triangle_info[ triangle_id1 ].is_processed = true;
		m_triangles_mesh.data[ triangle_id1 ].is_marked = true;
		subtract_triangle_flag = false;
		//return false;
	}

	/*
	// This is really right
	if ( subtract_triangle_flag && is_triangle_inside_triangle( coordinate_indices, v0, v1, v2, u0, u1, u2 ) ) {
		m_triangle_info[triangle_id0 ].is_processed = true;
		m_triangles_mesh.data[ triangle_id0 ].is_marked = true;
		return true;
	}
	*/

	u32 similar_edge_id1;
	u32 const similar_edge_id0 = get_similar_edge( triangle_id0, triangle_id1, &similar_edge_id1 );
	if ( subtract_triangle_flag && similar_edge_id0 != u32(-1) ) {
		u32 vertex_index0 = indices0[ ((similar_edge_id0 + 2) % 3) ];
		u32 vertex_index1 = indices1[ ((similar_edge_id1 + 2) % 3) ];
		if ( !is_same_side_segment(
				m_triangles_mesh.vertices[ vertex_index0 ],
				m_triangles_mesh.vertices[ vertex_index1 ],
				m_triangles_mesh.vertices[ indices0[ similar_edge_id0 ] ],
				m_triangles_mesh.vertices[ indices0[ (similar_edge_id0+1) % 3 ] ]
		) )
			return false;
	}
	
	u32 colinear_edge_id0 = u32(-1);
	u32 colinear_edge_id1 = u32(-1);
	get_colinear_edge( coordinate_indices, triangle_id0, triangle_id1, colinear_edge_id0, colinear_edge_id1 );
	
	if ( subtract_triangle_flag && colinear_edge_id0 != u32(-1) ) {
		float3 const& v0 = m_triangles_mesh.vertices[ indices0[ ( colinear_edge_id0 + 0 ) % 3 ] ];
		float3 const& v1 = m_triangles_mesh.vertices[ indices0[ ( colinear_edge_id0 + 1 ) % 3 ] ];

		float3 const& testee0 = m_triangles_mesh.vertices[ indices0[ ( colinear_edge_id0 + 2 ) % 3 ] ];
		float3 const& testee1 = m_triangles_mesh.vertices[ indices1[ ( colinear_edge_id1 + 2 ) % 3 ] ];

		if ( !is_same_side_segment( testee0, testee1, v0, v1 ) ) {
			return false;
			/*
			float3 const& u0 = m_triangles_mesh.vertices[ indices1[ ( colinear_edge_id1 + 0 ) % 3 ] ];
			float3 const& u1 = m_triangles_mesh.vertices[ indices1[ ( colinear_edge_id1 + 1 ) % 3 ] ];

			if ( is_on_segment( coordinate_indices, u0, v0, v1 ) )
				vertices.push_back( u0 );
			if ( is_on_segment( coordinate_indices, u1, v0, v1 ) )
				vertices.push_back( u1 );

			static u32 const max_vertices_count = 2;
			buffer_vector< float3 > neighbour_vertices(
				ALLOCA( max_vertices_count * sizeof( float3 ) ),
				max_vertices_count
			);

			if ( is_on_segment( coordinate_indices, v0, u0, u1 ) )
				neighbour_vertices.push_back( v0 );
			if ( is_on_segment( coordinate_indices, v1, u0, u1 ) )
				neighbour_vertices.push_back( v0 );

			last_triangle_id = triangle_id1;
			if ( neighbour_vertices.size() == 2 ) {
				float const squared_length0 = squared_length( neighbour_vertices[0] - u0 );
				float const squared_length1 = squared_length( neighbour_vertices[1] - u0 );
				if ( squared_length1 < squared_length0 )
					std::swap( neighbour_vertices[0], neighbour_vertices[1] );
				add_output_triangle( triangle_id1, u0, neighbour_vertices[0], testee1 );
				add_output_triangle( triangle_id1, neighbour_vertices[0], neighbour_vertices[1], testee1 );
				add_output_triangle( triangle_id1, neighbour_vertices[1], u1, testee1 );

			} else if ( neighbour_vertices.size() ) {
				add_output_triangle( triangle_id1, u0, neighbour_vertices[0], testee1 );
				add_output_triangle( triangle_id1, neighbour_vertices[0], u1, testee1 );
			}
			m_triangle_info[ triangle_id1 ].is_processed = true;
			m_triangles_mesh.data[ triangle_id1 ].is_marked = true;
			*/
		}
	}

	/*
	if ( subtract_triangle_flag )
		if ( !subtract_triangle( coordinate_indices, triangle_id0, triangle_id1, vertices ) )
			return false;
	*/

	// Triangulate vertices inside tessellated triangle
	float3 const* triangle_vertices[] = {
		&u0,
		&u1,
		&u2,
	};

	for ( u32 i = 0; i < 3; ++i ) {
		if ( is_point_inside_triangle( coordinate_indices, *triangle_vertices[i], v0, v1, v2 ) )
			vertices.push_back( *triangle_vertices[i] );
	}

	vertices.push_back ( v0 );
	vertices.push_back ( v1 );
	vertices.push_back ( v2 );

	for ( u32 i = 0; i < vertices.size(); ++i ) {
		for ( u32 j = i+1; j < vertices.size(); ++j ) {
			if ( is_similar( vertices[i], vertices[j], xray::math::epsilon_3 ) ) {
				vertices.erase( vertices.begin() + j );
				--j;
			}
		}
	}

	if ( vertices.size() <= 3 )
		return false;

	R_ASSERT ( vertices.size() >= 3 );

	u32 const max_index_count = 3 * delaunay_triangulator::get_maximum_triangle_count_in_triangulation( vertices.size() );
	delaunay_triangulator::indices_type indices(
		ALLOCA( max_index_count * sizeof( delaunay_triangulator::indices_type::value_type ) ),
		max_index_count
	);

	delaunay_triangulator (
		vertices,
		indices,
		m_triangles_mesh.data[triangle_id0].plane.normal
	);

	R_ASSERT ( (indices.size() % 3) == 0 );

	u32 const triangles_count = indices.size() / 3;

	for ( u32 i = 0; i < triangles_count; ++i ) {
		add_output_triangle ( 
			triangle_id0, 
			vertices[indices[3*i+0]],
			vertices[indices[3*i+1]],
			vertices[indices[3*i+2]],
			false
		);
	}

	m_triangle_info[ triangle_id0 ].is_processed = true;
	
	if ( advance_current_operation_id( ) )
		return true;

	m_triangles_mesh.data[ triangle_id0 ].is_marked = true;
	return true;
}

#else

void graph_generator_tessellator::push_intermediate_edge ( intermediate_edges_type& edges, float3 const& v0, float3 const& v1 )
{
	if ( is_similar( v1, v0 ) )
		return;
	edges.push_back( intermediate_constraint_edge( v0, v1 ) );
}

void graph_generator_tessellator::collect_constrained_edges( 
		u32 const ( &coordinate_indices)[2],
		intermediate_edges_type& intermediate_edges, 
		u32 const triangle_id0, 
		u32 const triangle_id1 
	)
{
	u32 const* indices0 = m_triangles_mesh.indices.begin() + triangle_id0 * 3;
	u32 const* indices1 = m_triangles_mesh.indices.begin() + triangle_id1 * 3;

	for ( u32 i = 0; i < 3; ++i ) {

		float3 const& edge_vertex0 = m_triangles_mesh.vertices[ indices0[i] ];
		float3 const& edge_vertex1 = m_triangles_mesh.vertices[ indices0[(i+1)%3] ];

		float3 intersections[2];
		for ( u32 j = 0; j < 3; ++j ) {
			bool are_on_the_same_line;
			float3 const& u0 = m_triangles_mesh.vertices[ indices1[j] ];
			float3 const& u1 = m_triangles_mesh.vertices[ indices1[(j+1)%3] ];

			u32 result_count = segment_intersects_segment(
				coordinate_indices, 
				edge_vertex0, 
				edge_vertex1, 
				u0, 
				u1, 
				intersections[0],
				intersections[1],
				are_on_the_same_line
			);

			if ( result_count == 1 ) {
				push_intermediate_edge( intermediate_edges, edge_vertex0, intersections[0] );
				push_intermediate_edge( intermediate_edges, edge_vertex1, intersections[0] );
			}
			if ( result_count == 2 ) {
				float const squared_length0 = squared_length( intersections[0] - edge_vertex0 );
				float const squared_length1 = squared_length( intersections[0] - edge_vertex0 );

				if ( squared_length1 < squared_length0 )
					std::swap( intersections[0], intersections[1] );

				push_intermediate_edge( intermediate_edges, edge_vertex0, intersections[0] );
				push_intermediate_edge( intermediate_edges, intersections[0], intersections[1] );
				push_intermediate_edge( intermediate_edges, edge_vertex1, intersections[1] );
			}
		}
	}
}

u32 graph_generator_tessellator::find_vertex_index ( delaunay_triangulator::vertices_type& vertices, float3 const& vertex )
{
	u32 const vertices_count = vertices.size();
	for ( u32 i = 0; i < vertices_count; ++i ) 
		if ( is_similar( vertices[i], vertex ) )
			return i;
	NODEFAULT( return u32(-1) );
}

bool graph_generator_tessellator::tessellate_coplanar_triangles	(
		u32 const triangle_id0,
		u32 const triangle_id1
	)
{
	u32 coordinate_indices[2];
	fill_coordinate_indices	( m_triangles_mesh.data[triangle_id0].plane.normal, coordinate_indices );

	u32 const* indices0 = m_triangles_mesh.indices.begin() + triangle_id0 * 3;
	float3 const v0	= m_triangles_mesh.vertices[ indices0[0] ];
	float3 const v1	= m_triangles_mesh.vertices[ indices0[1] ];
	float3 const v2	= m_triangles_mesh.vertices[ indices0[2] ];

	u32 const* indices1 = m_triangles_mesh.indices.begin() + triangle_id1 * 3;
	float3 const u0	= m_triangles_mesh.vertices[ indices1[0] ];
	float3 const u1	= m_triangles_mesh.vertices[ indices1[1] ];
	float3 const u2	= m_triangles_mesh.vertices[ indices1[2] ];

	static int const max_triangle_intersections_count = 50;
	u32 const max_vertex_count	= max_triangle_intersections_count + delaunay_triangulator::get_additional_vertex_count( );
	delaunay_triangulator::vertices_type vertices( 
		ALLOCA(max_vertex_count*sizeof(delaunay_triangulator::vertices_type::value_type)), 
		max_vertex_count 
	);

	vertices.push_back( v0 );
	vertices.push_back( v1 );
	vertices.push_back( v2 );

	vertices.push_back( u0 );
	vertices.push_back( u1 );
	vertices.push_back( u2 );

	intermediate_edges_type edges;
	collect_constrained_edges( coordinate_indices, edges, triangle_id0, triangle_id1 );
	collect_constrained_edges( coordinate_indices, edges, triangle_id1, triangle_id0 );

	u32 const edges_count = edges.size();
	for ( u32 i = 0; i < edges_count; ++i ) {
		vertices.push_back( edges[i].v0 );
		vertices.push_back( edges[i].v1 );
	}

	for ( u32 i = 0; i < vertices.size(); ++i ) {
		for ( u32 j = i+1; j < vertices.size(); ++j ) {
			if ( is_similar( vertices[i], vertices[j], xray::math::epsilon_3 ) ) {
				vertices.erase( vertices.begin() + j );
				--j;
			}
		}
	}

	constrained_delaunay_triangulator::constraint_edges_type constraint_edges;
	for ( u32 i = 0; i < edges_count; ++i ) {
		constraint_edges.push_back ( 
			constrained_delaunay_triangulator::constraint_edge(
				find_vertex_index( vertices, edges[i].v0 ),  
				find_vertex_index( vertices, edges[i].v1 ) 
			)
		);
	}

	u32 const max_index_count = 3 * delaunay_triangulator::get_maximum_triangle_count_in_triangulation( vertices.size() );
	delaunay_triangulator::indices_type indices(
		ALLOCA( max_index_count * sizeof( delaunay_triangulator::indices_type::value_type ) ),
		max_index_count
	);

	constrained_delaunay_triangulator (
		vertices,
		indices,
		m_triangles_mesh.data[ triangle_id0 ].plane.normal,
		constraint_edges
	);

	R_ASSERT_CMP( indices.size() % 3, ==, 0 );

	u32 const new_triangles_count = indices.size() / 3;
	for ( u32 i = 0; i < new_triangles_count; ++i ) {
		float3 center = ( vertices[ indices[i*3+0] ] + vertices[ indices[i*3+1] ] + vertices[ indices[i*3+2] ] ) / 3;
		if ( is_point_inside_triangle( coordinate_indices, center, v0, v1, v2 ) )
			add_output_triangle( 
				triangle_id0, 
				vertices[ indices[i*3+0] ],
				vertices[ indices[i*3+1] ],
				vertices[ indices[i*3+2] ]
			);
		else if ( is_point_inside_triangle( coordinate_indices, center, u0, u1, u2 ) )
			add_output_triangle( 
				triangle_id1,
				vertices[ indices[i*3+0] ],
				vertices[ indices[i*3+1] ],
				vertices[ indices[i*3+2] ]
			);
	}

	m_triangle_info[ triangle_id0 ].is_processed = true;
	m_triangles_mesh.data[ triangle_id0 ].is_marked = true;

	m_triangle_info[ triangle_id1 ].is_processed = true;
	m_triangles_mesh.data[ triangle_id1 ].is_marked = true;

	return true;
}

#endif

template<typename VertexContainer>
void filter_duplicated_vertices(u32 const (&coordinate_indices)[2],  VertexContainer& vertices )
{
	for ( u32 i = 0; i < vertices.size(); ++i ) {
		for ( u32 j = i+1; j < vertices.size(); ++j ) {
			if ( xray::math::is_similar( vertices[i][coordinate_indices[0]], vertices[j][coordinate_indices[0]], xray::math::epsilon_3 )
				&& xray::math::is_similar( vertices[i][coordinate_indices[1]], vertices[j][coordinate_indices[1]], xray::math::epsilon_3 ) )
			{
				vertices.erase( vertices.begin() + j );
				--j;
			}
		}
	}
}


int tri_tri_intersect_with_isectline(float3 const& V0, float3 const& V1, float3 const& V2,
					 float3 const& U0, float3 const& U1, float3 const& U2, int *coplanar,
					 float3& isectpt1, float3& isectpt2);

class filter_non_coplanar_predicate {
public:
	inline filter_non_coplanar_predicate			( triangles_mesh_type& triangles_mesh, u32 const triangle_id ) :
		m_triangles_mesh		( &triangles_mesh ),
		m_triangle_id			( triangle_id )
	{
	}

	inline bool operator( )							( u32 const triangle_id ) const
	{
		u32 const* indices0		= &*m_triangles_mesh->indices.begin() + 3*m_triangle_id;
		xray::math::float3 const& v0	= m_triangles_mesh->vertices[ indices0[0] ];
		xray::math::float3 const& v1	= m_triangles_mesh->vertices[ indices0[1] ];
		xray::math::float3 const& v2	= m_triangles_mesh->vertices[ indices0[2] ];

		u32 const* indices1		= &*m_triangles_mesh->indices.begin() + 3*triangle_id;
		xray::math::float3 const& u0	= m_triangles_mesh->vertices[ indices1[0] ];
		xray::math::float3 const& u1	= m_triangles_mesh->vertices[ indices1[1] ];
		xray::math::float3 const& u2	= m_triangles_mesh->vertices[ indices1[2] ];

		float3 segment_vertices[2];
		int coplanar;
		int const intersection_result	=
			tri_tri_intersect_with_isectline(
				v0, v1, v2, u0, u1, u2, &coplanar, segment_vertices[0], segment_vertices[1]
			);
		
		if ( intersection_result && coplanar )
			return false;
		return true;
	}

private:
	triangles_mesh_type const*	m_triangles_mesh;
	u32							m_triangle_id;
}; // struct filter_obstructions_predicate

bool graph_generator_tessellator::process_colinear_edge_tests ( 
		u32 const (&coordinate_indices)[2],
		u32 const triangle_id0, 
		u32 const triangle_id1,
		vertices_type& vertices
	)
{
	u32 const* indices0 = m_triangles_mesh.indices.begin() + triangle_id0 * 3;
	float3 const v0	= m_triangles_mesh.vertices[ indices0[0] ];
	float3 const v1	= m_triangles_mesh.vertices[ indices0[1] ];
	float3 const v2	= m_triangles_mesh.vertices[ indices0[2] ];
	
	
	u32 const* indices1 = m_triangles_mesh.indices.begin() + triangle_id1 * 3;
	float3 const u0	= m_triangles_mesh.vertices[ indices1[0] ];
	float3 const u1	= m_triangles_mesh.vertices[ indices1[1] ];
	float3 const u2	= m_triangles_mesh.vertices[ indices1[2] ];

	if ( is_triangle_inside_triangle( coordinate_indices, u0, u1, u2, v0, v1, v2 ) ) {
		for ( u32 i = 0; i < 3; ++i ) {
			float3 const& v0 = m_triangles_mesh.vertices[ indices0[ (i + 0) % 3 ] ];
			float3 const& v1 = m_triangles_mesh.vertices[ indices0[ (i + 1) % 3 ] ];

			for ( u32 j = 0; j < 3; ++j ) {
				if ( is_on_segment(coordinate_indices, m_triangles_mesh.vertices[ indices1[ j ] ], v0, v1 ) )
					vertices.push_back( m_triangles_mesh.vertices[ indices1[ j ] ] );
			}
		}
		m_triangles_mesh.data[ triangle_id1 ].is_marked = true;
		m_triangle_info[ triangle_id1 ].is_processed = true;
		return true;
	}

		
	u32 colinear_edge_id0 = u32(-1);
	u32 colinear_edge_id1;
	get_colinear_edge( coordinate_indices, triangle_id0, triangle_id1, colinear_edge_id0, colinear_edge_id1 );
	if ( colinear_edge_id0 != u32(-1) ) {
		float3 const& v0 = m_triangles_mesh.vertices[ indices0[ colinear_edge_id0 ] ];
		float3 const& v1 = m_triangles_mesh.vertices[ indices0[ (colinear_edge_id0 + 1) % 3 ] ];
		float3 const& testee0 = m_triangles_mesh.vertices[ indices0[ (colinear_edge_id0 + 2) % 3 ] ];
		
		float3 const& testee1 = m_triangles_mesh.vertices[ indices1[ (colinear_edge_id1 + 2) % 3 ] ];
		if ( !is_same_side_segment( testee0, testee1, v0, v1 ) )
		{
			return true;
		}
	}

	return false;
}


//! Instead of this collect intersection vertices
bool graph_generator_tessellator::tessellate_triangle_impl2( u32 const triangle_id, buffer_indices_type& intersected_triangles ) 
{
	intersected_triangles.erase					(
		std::remove_if(
			intersected_triangles.begin( ),
			intersected_triangles.end( ),
			filter_non_coplanar_predicate( m_triangles_mesh, triangle_id )
		),
		intersected_triangles.end( )
	);

	u32 const childs0_count = untessellated_triangles_count( triangle_id );
	buffer_indices_type childs0 (
		ALLOCA( childs0_count * sizeof( buffer_indices_type::value_type ) ),
		childs0_count
	);
	collect_untessellated_triangles( triangle_id, childs0 );

	// Filter non coplanar triangles
	u32 const coordinate_indices[] = { 0, 2 };
	bool was_processed = false;

	for ( u32 j = 0; j < childs0_count; ++j ) {
		u32 current_triangle_id = childs0[ j ];
		vertices_type vertices;

		buffer_indices_type::iterator i = intersected_triangles.begin();
		buffer_indices_type::iterator const e = intersected_triangles.end();
		for (; i != e; ++i ) {
			u32 const childs_count = untessellated_triangles_count( (*i) );
			buffer_indices_type childs (
				ALLOCA( childs_count * sizeof( buffer_indices_type::value_type ) ),
				childs_count
			);
			collect_untessellated_triangles( (*i), childs );

			buffer_indices_type::iterator k = childs.begin();
			for ( buffer_indices_type::iterator const e = childs.end(); k != e; ++k ) {
				if ( process_colinear_edge_tests( coordinate_indices, current_triangle_id, (*k), vertices ) )
					continue;

				subtract_triangle( coordinate_indices, current_triangle_id, (*k), vertices );
			}
		}

		vertices.push_back( m_triangles_mesh.vertices[ m_triangles_mesh.indices[ triangle_id * 3 + 0 ] ] );
		vertices.push_back( m_triangles_mesh.vertices[ m_triangles_mesh.indices[ triangle_id * 3 + 1 ] ] );
		vertices.push_back( m_triangles_mesh.vertices[ m_triangles_mesh.indices[ triangle_id * 3 + 2 ] ] );

		filter_duplicated_vertices ( coordinate_indices, vertices );
		
		if ( vertices.size() > 3 ) {
			was_processed = true;
			u32 const max_indices_count = 3 * delaunay_triangulator::get_maximum_triangle_count_in_triangulation( vertices.size() );
			delaunay_triangulator::indices_type indices(
				ALLOCA( max_indices_count * sizeof( delaunay_triangulator::indices_type ) ),
				max_indices_count
			);

			delaunay_triangulator (
				vertices,
				indices,
				m_triangles_mesh.data[ current_triangle_id ].plane.normal
			);

			R_ASSERT_CMP( indices.size() % 3, ==, 0 );
			u32 const new_triangles_count = indices.size() / 3;
			for ( u32 j = 0; j < new_triangles_count; ++j ) {
				add_output_triangle(
					current_triangle_id,
					vertices[ indices[ j * 3 + 0 ] ],
					vertices[ indices[ j * 3 + 1 ] ],
					vertices[ indices[ j * 3 + 2 ] ],
					false
				);
			}
			m_triangles_mesh.data[ current_triangle_id ].is_marked = true;
		}
	}
	return was_processed;
}
