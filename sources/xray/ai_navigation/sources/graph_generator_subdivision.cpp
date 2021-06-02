////////////////////////////////////////////////////////////////////////////
//	Created		: 23.08.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if XRAY_DEBUG_ALLOCATOR

#include "graph_generator.h"
#include <xray/collision/geometry_instance.h>
#include <xray/collision/geometry.h>

using xray::ai::navigation::graph_generator;
using xray::ai::navigation::triangle_to_test;
using xray::math::float3;
using xray::math::float2;

int tri_tri_intersect_with_isectline(float3 const& V0, float3 const& V1, float3 const& V2,
					 float3 const& U0, float3 const& U1, float3 const& U2, int *coplanar,
					 float3& isectpt1, float3& isectpt2);

static inline bool check_edge	( float3 const& vertex0, float3 const& vertex1, float3& point, float const epsilon )
{
	float3 const& t0_t1_direction		= normalize( vertex1 - vertex0 );
	float3 const& t0_s0					= point - vertex0;
	float const distance_to_edge_t0_t1	= length(t0_s0 ^ t0_t1_direction);
	if ( distance_to_edge_t0_t1 >= epsilon )
		return							false;

	point								= vertex0 + t0_t1_direction*(t0_t1_direction | t0_s0);
	return								true;
}

static inline void correct_point_impl	( graph_generator::subdivided_triangle const& triangle, float3& point )
{
	using namespace xray::math;

	float const epsilon		= .01f;

	// 1. puth the point down to plane
	float const d			= -(triangle.vertices[0] | triangle.normal);
	float const signed_distance_to_plane = (triangle.normal | point) + d;
	point					= point + triangle.normal*signed_distance_to_plane;
	R_ASSERT				( is_zero((triangle.normal | point) + d, epsilon_3) );

	// 2. check if the point is close to triangle vertices
	for ( u32 j=0; j<3; ++j ) {
		if ( !point.is_similar(triangle.vertices[j], epsilon_3) )
			continue;

		point				= triangle.vertices[j];
		return;
	}

	// 3. check if point is inside triangle
	float3 const new_normals[3] = {
		(point - triangle.vertices[0]) ^ (point - triangle.vertices[1]),
		(point - triangle.vertices[1]) ^ (point - triangle.vertices[2]),
		(point - triangle.vertices[2]) ^ (point - triangle.vertices[0]),
	};

	u32 out_mask	= 0;
	for (u32 i=0; i<3; ++i ) {
		if ( is_zero( new_normals[i].length() ) )
			continue;

		float3 const& new_normal	= normalize( new_normals[i] );
		if ( is_zero( (triangle.normal | new_normal) - 1.f, epsilon ) )
			continue;

		R_ASSERT			( is_zero( (triangle.normal | -new_normal) - 1.f, epsilon) );
		out_mask			|= 1 << i;
	}

	if ( !out_mask ) {
		// 3.1 try to assign point to edge if it is close to it
		if ( !check_edge( triangle.vertices[0], triangle.vertices[1], point, epsilon) ) {
			if ( !check_edge( triangle.vertices[1], triangle.vertices[2], point, epsilon) )
				check_edge( triangle.vertices[2], triangle.vertices[0], point, epsilon);
		}
		return;
	}

	// 4. make point be on the triangle edge
	float3 const* vertices[2];
	if ( out_mask & (1 << 0) ) {
		if ( out_mask & (1 << 1) ) {
			point			= triangle.vertices[1];
			return;
		}

		if ( out_mask & (1 << 2) ) {
			point			= triangle.vertices[0];
			return;
		}

		vertices[0]			= &triangle.vertices[0];
		vertices[1]			= &triangle.vertices[1];
	}
	else {
		if ( !(out_mask & (1 << 1)) ) {
			R_ASSERT		( out_mask & (1 << 2) );
			vertices[0]		= &triangle.vertices[2];
			vertices[1]		= &triangle.vertices[0];
		}
		else {
			if ( out_mask & (1 << 2) ) {
				point		= triangle.vertices[2];
				return;
			}

			vertices[0]		= &triangle.vertices[1];
			vertices[1]		= &triangle.vertices[2];
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

static inline void correct_point( graph_generator::subdivided_triangle const& triangle, float3& point )
{
	using namespace xray::math;

	float3 const old_point	= point;
	point					= old_point;

	for (u32 i=0; i<10; ++i) {
		correct_point_impl	( triangle, point );

		if ( !( is_zero(((point - triangle.vertices[0]) ^ (point - triangle.vertices[1])).length()) || is_zero( 1.f - (triangle.normal | normalize((point - triangle.vertices[0]) ^ (point - triangle.vertices[1]))) ) ) )
			continue;

		if ( !( is_zero(((point - triangle.vertices[1]) ^ (point - triangle.vertices[2])).length()) || is_zero( 1.f - (triangle.normal | normalize((point - triangle.vertices[1]) ^ (point - triangle.vertices[2]))) ) ) )
			continue;

		if ( !( is_zero(((point - triangle.vertices[2]) ^ (point - triangle.vertices[0])).length()) || is_zero( 1.f - (triangle.normal | normalize((point - triangle.vertices[2]) ^ (point - triangle.vertices[0]))) ) ) )
			continue;

		break;
	}

	R_ASSERT				( is_zero(((point - triangle.vertices[0]) ^ (point - triangle.vertices[1])).length()) || is_zero( 1.f - (triangle.normal | normalize((point - triangle.vertices[0]) ^ (point - triangle.vertices[1]))) ) );
	R_ASSERT				( is_zero(((point - triangle.vertices[1]) ^ (point - triangle.vertices[2])).length()) || is_zero( 1.f - (triangle.normal | normalize((point - triangle.vertices[1]) ^ (point - triangle.vertices[2]))) ) );
	R_ASSERT				( is_zero(((point - triangle.vertices[2]) ^ (point - triangle.vertices[0])).length()) || is_zero( 1.f - (triangle.normal | normalize((point - triangle.vertices[2]) ^ (point - triangle.vertices[0]))) ) );
}

template < typename ContainerType >
graph_generator::triangles_type::iterator graph_generator::select_triangle	(
		subdivided_triangle& triangle,
		ContainerType& triangles,
		float3& segment0,
		float3& segment1
	) const
{
	R_ASSERT				( !triangles.empty() );
	triangles_type::iterator const result	= triangles.begin() + triangles.size() - 1;

	u32 const* indices1		= m_geometry->get_geometry()->indices( (*result).triangle_id );
	math::float3 const& u0	= m_transformed_vertices[ indices1[0] ];
	math::float3 const& u1	= m_transformed_vertices[ indices1[1] ];
	math::float3 const& u2	= m_transformed_vertices[ indices1[2] ];

	int coplanar;
	int const intersection_result	=
		tri_tri_intersect_with_isectline(
			triangle.vertices[0], triangle.vertices[1], triangle.vertices[2], u0, u1, u2, &coplanar, segment0, segment1
		);
	R_ASSERT				( intersection_result && !coplanar && !segment0.is_similar(segment1, .01f) );

	::correct_point			( triangle, segment0 );
	::correct_point			( triangle, segment1 );
																						
	return					result;
}

static inline bool is_between	(
		xray::math::float3 const& test_vertex,
		xray::math::float3 const& vertex0,
		xray::math::float3 const& vertex1
	)
{
	if ( is_similar(test_vertex, vertex0) )
		return				true;

	return					is_similar( normalize(vertex1 - vertex0), normalize(test_vertex - vertex0) );
}

static inline void correct_obstructed_edge	(
		xray::math::float3 const& test_vertex0,
		xray::math::float3 const& test_vertex1,
		xray::math::float3 const& test_vertex2,
		xray::math::float3 const& triangle_vertex0,
		xray::math::float3 const& triangle_vertex1,
		graph_generator::obstructed_edges& obstructed_edge
	)
{
	if ( is_between( test_vertex0, triangle_vertex0, triangle_vertex1 ) ) {
		if ( is_between( test_vertex1, triangle_vertex0, triangle_vertex1 ) ) {
			obstructed_edge	= graph_generator::obstructed_edges( obstructed_edge | graph_generator::obstructed_edge_ab );
			return;
		}

		if ( is_between( test_vertex2, triangle_vertex0, triangle_vertex1 ) ) {
			obstructed_edge	= graph_generator::obstructed_edges( obstructed_edge | graph_generator::obstructed_edge_ca );
			return;
		}
	}

	if ( !is_between( test_vertex1, triangle_vertex0, triangle_vertex1 ) )
		return;

	if ( !is_between( test_vertex2, triangle_vertex0, triangle_vertex1 ) )
		return;

	obstructed_edge			= graph_generator::obstructed_edges( obstructed_edge | graph_generator::obstructed_edge_bc );
}

template < typename ContainerType >
bool graph_generator::try_subdivide_triangle	(
		ContainerType const& triangles,
		subdivided_triangle const& triangle,
		xray::math::float3 const& triangle_vertex0,
		xray::math::float3 const& triangle_vertex1,
		xray::math::float3 const& triangle_vertex2,
		obstructed_edges obstructed_edges
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

	if ( (triangle.obstructed_edges & obstructed_edge_ab) == obstructed_edge_ab )
		correct_obstructed_edge	( triangle_vertex0, triangle_vertex1, triangle_vertex2, triangle.vertices[0], triangle.vertices[1], obstructed_edges );

	if ( (triangle.obstructed_edges & obstructed_edge_bc) == obstructed_edge_bc )
		correct_obstructed_edge	( triangle_vertex0, triangle_vertex1, triangle_vertex2, triangle.vertices[1], triangle.vertices[2], obstructed_edges );

	if ( (triangle.obstructed_edges & obstructed_edge_ca) == obstructed_edge_ca )
		correct_obstructed_edge	( triangle_vertex0, triangle_vertex1, triangle_vertex2, triangle.vertices[2], triangle.vertices[0], obstructed_edges );

	buffer_triangles_type		new_triangles(
		ALLOCA( triangles.size()*sizeof(buffer_triangles_type::value_type) ),
		triangles.size( ),
		triangles.begin( ),
		triangles.end( )
	);

	subdivided_triangle				new_triangle;
	new_triangle.vertices[0]	= triangle_vertex0;
	new_triangle.vertices[1]	= triangle_vertex1;
	new_triangle.vertices[2]	= triangle_vertex2;
	new_triangle.normal			= triangle.normal;
	new_triangle.obstructed_edges = obstructed_edges;

	subdivide_triangle			( new_triangle, new_triangles );
	return						true;
}

xray::math::float3 const* graph_generator::select_equal_vertices	(
		subdivided_triangle const& triangle,
		float3 const& testee
	)
{
	if ( is_similar(triangle.vertices[0], testee) ) {
		R_ASSERT				( triangle.vertices[1] != testee );
		R_ASSERT				( triangle.vertices[2] != testee );
		return					&triangle.vertices[0];
	}
	
	if ( is_similar(triangle.vertices[1], testee) ) {
		R_ASSERT				( triangle.vertices[2] != testee );
		return					&triangle.vertices[1];
	}

	if ( is_similar(triangle.vertices[2], testee) )
		return					&triangle.vertices[2];

	return						0;
}

inline void graph_generator::add_output_triangle	( subdivided_triangle const& triangle )
{
	m_subdivided_triangles.push_back( triangle );
}

template < typename ContainerType >
bool graph_generator::subdivide_triangle_impl	( subdivided_triangle& triangle, ContainerType& triangles, bool root )
{
	float3 segment_vertices[2];
	subdivided_triangle segment_triangle;
	u32 success_count = 0;
	for ( ; !success_count && !triangles.empty(); ) {
		typename ContainerType::iterator const found = select_triangle( triangle, triangles, segment_vertices[0], segment_vertices[1] );
		R_ASSERT				( found != triangles.end() );
		triangles.erase			( found );

		// preserve correct triangles vertices order
		math::float3 const* equal_vertices[2] = { 0, 0 };
		equal_vertices[0]					= select_equal_vertices	( triangle, segment_vertices[0] );
		equal_vertices[1]					= select_equal_vertices	( triangle, segment_vertices[1] );
		if ( equal_vertices[0] ) {
			if ( equal_vertices[1] ) {
				if ( (&triangle.vertices[0] != equal_vertices[0]) && (&triangle.vertices[0] != equal_vertices[1]) ) {
					triangle.obstructed_edges	= obstructed_edges(triangle.obstructed_edges | obstructed_edge_bc);
					if ( !triangles.empty() )
						continue;

					
					triangle.color.m_value		= root ? math::color_xrgb( 0, 255, 255 ) : math::color_xrgb( 0, 0, 255 );
					if ( m_subdivision_current_operation_id++ < m_subdivision_max_operation_id )
						add_output_triangle	( triangle );
					
					
					return					false;
				}

				if ( (&triangle.vertices[1] != equal_vertices[0]) && (&triangle.vertices[1] != equal_vertices[1]) ) {
					triangle.obstructed_edges	= obstructed_edges(triangle.obstructed_edges | obstructed_edge_ca);
					if ( !triangles.empty() )
						continue;

					triangle.color.m_value		= root ? math::color_xrgb( 0, 255, 255 ) : math::color_xrgb( 0, 0, 255 );
					if ( m_subdivision_current_operation_id++ < m_subdivision_max_operation_id )
						add_output_triangle	( triangle );
					
					return					false;
				}

				R_ASSERT					( (&triangle.vertices[2] != equal_vertices[0]) && (&triangle.vertices[2] != equal_vertices[1]) );
				triangle.obstructed_edges	= obstructed_edges(triangle.obstructed_edges | obstructed_edge_ab);
				if ( !triangles.empty() )
					continue;

				triangle.color.m_value		= root ? math::color_xrgb( 0, 255, 255 ) : math::color_xrgb( 0, 0, 255 );
				
				
				if ( m_subdivision_current_operation_id++ < m_subdivision_max_operation_id )
					add_output_triangle		( triangle );
				
				
				return						false;
			}
		}

		if ( equal_vertices[0] || equal_vertices[1] ) {
			float3 const* const non_equal_vertex	= equal_vertices[0] ? &segment_vertices[1] : &segment_vertices[0];
			float3 const* const equal_vertex		= equal_vertices[0] ? equal_vertices[0] : equal_vertices[1];
			success_count			+=
				try_subdivide_triangle(
					triangles,
					triangle,
					triangle.vertices[0],
					triangle.vertices[1],
					*non_equal_vertex,
					graph_generator::obstructed_edges(
					((equal_vertex == &triangle.vertices[0]) ? obstructed_edge_ca : (equal_vertex == &triangle.vertices[1] ? obstructed_edge_bc : obstructed_edge_none)) |
						(((triangle.obstructed_edges & obstructed_edge_ab) == obstructed_edge_ab) ? obstructed_edge_ab : obstructed_edge_none)
					)
				) ? 1 : 0;


			success_count			+= try_subdivide_triangle( triangles, triangle, triangle.vertices[1],	triangle.vertices[2],	*non_equal_vertex, graph_generator::obstructed_edges( ((equal_vertex == &triangle.vertices[1]) ? obstructed_edge_ca : obstructed_edge_none) | (((triangle.obstructed_edges & obstructed_edge_bc) == obstructed_edge_bc) ? obstructed_edge_ab : obstructed_edge_none) ) ) ? 1 : 0;
			success_count			+= try_subdivide_triangle( triangles, triangle, triangle.vertices[2],	triangle.vertices[0],	*non_equal_vertex, graph_generator::obstructed_edges( ((equal_vertex == &triangle.vertices[2]) ? obstructed_edge_ca : obstructed_edge_none) | (((triangle.obstructed_edges & obstructed_edge_ca) == obstructed_edge_ca) ? obstructed_edge_ab : obstructed_edge_none) ) ) ? 1 : 0;
			continue;
		}

		R_ASSERT					( !segment_vertices[0].is_similar(segment_vertices[1]) );

		obstructed_edges obstructed_edges = triangle.obstructed_edges;
		float3 const* triangle_vertices[3]	= { &triangle.vertices[0], &triangle.vertices[1], &triangle.vertices[2] };
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

					R_ASSERT			( triangle_vertices[0] == &triangle.vertices[0] );
					R_ASSERT			( triangle_vertices[1] == &triangle.vertices[1] );
					R_ASSERT			( triangle_vertices[2] == &triangle.vertices[2] );

					triangle_vertices[0] = &triangle.vertices[1];
					triangle_vertices[1] = &triangle.vertices[2];
					triangle_vertices[2] = &triangle.vertices[0];

					graph_generator::obstructed_edges	new_obstructed_edges = obstructed_edge_none;

					if ( (obstructed_edges & obstructed_edge_ab) == obstructed_edge_ab )
						new_obstructed_edges = graph_generator::obstructed_edges( new_obstructed_edges | obstructed_edge_ca);

					if ( (obstructed_edges & obstructed_edge_bc) == obstructed_edge_bc )
						new_obstructed_edges = graph_generator::obstructed_edges( new_obstructed_edges | obstructed_edge_ab);

					if ( (obstructed_edges & obstructed_edge_ca) == obstructed_edge_ca )
						new_obstructed_edges = graph_generator::obstructed_edges( new_obstructed_edges | obstructed_edge_bc);

					obstructed_edges	= new_obstructed_edges;
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

					graph_generator::obstructed_edges	new_obstructed_edges = obstructed_edge_none;

					if ( (obstructed_edges & obstructed_edge_ab) == obstructed_edge_ab )
						new_obstructed_edges = graph_generator::obstructed_edges( new_obstructed_edges | obstructed_edge_bc);

					if ( (obstructed_edges & obstructed_edge_bc) == obstructed_edge_bc )
						new_obstructed_edges = graph_generator::obstructed_edges( new_obstructed_edges | obstructed_edge_ca);

					if ( (obstructed_edges & obstructed_edge_ca) == obstructed_edge_ca )
						new_obstructed_edges = graph_generator::obstructed_edges( new_obstructed_edges | obstructed_edge_ab);

					obstructed_edges	= new_obstructed_edges;

					if ( s0_minus_t0.square_length() < s1_minus_t0.square_length() )
						std::swap		( segment_vertices[0], segment_vertices[1] );
				}
				else {
					if ( s0_minus_t0.square_length() > s1_minus_t0.square_length() )
						std::swap		( segment_vertices[0], segment_vertices[1] );
				}

				success_count			+= try_subdivide_triangle( triangles, triangle, *triangle_vertices[0],	segment_vertices[0],	*triangle_vertices[2],	(obstructed_edges & obstructed_edge_ca) ? obstructed_edge_ca : obstructed_edge_none)	? 1 : 0;
				success_count			+= try_subdivide_triangle( triangles, triangle, segment_vertices[0],	segment_vertices[1],	*triangle_vertices[2],	obstructed_edge_ab)		? 1 : 0;
				success_count			+= try_subdivide_triangle( triangles, triangle, segment_vertices[1],	*triangle_vertices[1],	*triangle_vertices[2],	(obstructed_edges & obstructed_edge_bc) ? obstructed_edge_bc : obstructed_edge_none)	? 1 : 0;
				break;
			}

			s0_cross_s1_t0.normalize( );
			bool const is_t0		= math::is_zero( (triangle.normal | s0_cross_s1_t0) - 1.f, math::epsilon_3);
			R_ASSERT				( is_t0 || math::is_zero( (triangle.normal | -s0_cross_s1_t0) - 1.f, math::epsilon_3) );
			if ( !is_t0 )
				std::swap			( segment_vertices[0], segment_vertices[1] );

			float3 const& s0_minus_t1 = segment_vertices[0] - *triangle_vertices[1];
			float3 const& s1_minus_t1 = segment_vertices[1] - *triangle_vertices[1];
			float3 s0_cross_s1_t1	 = s0_minus_t1 ^ s1_minus_t1;
			if ( math::is_zero( s0_cross_s1_t1.length() ) ) {
				if ( !last_vertex_on_line_with_segment ) {
					last_vertex_on_line_with_segment	= triangle_vertices[1];

					R_ASSERT_CMP		( iteration, <, 1 );

					R_ASSERT			( triangle_vertices[0] == &triangle.vertices[0] );
					R_ASSERT			( triangle_vertices[1] == &triangle.vertices[1] );
					R_ASSERT			( triangle_vertices[2] == &triangle.vertices[2] );

					triangle_vertices[0] = &triangle.vertices[2];
					triangle_vertices[1] = &triangle.vertices[0];
					triangle_vertices[2] = &triangle.vertices[1];

					graph_generator::obstructed_edges	new_obstructed_edges = obstructed_edge_none;
					if ( (obstructed_edges & obstructed_edge_ab) == obstructed_edge_ab )
						new_obstructed_edges = graph_generator::obstructed_edges(new_obstructed_edges | obstructed_edge_bc);

					if ( (obstructed_edges & obstructed_edge_bc) == obstructed_edge_bc )
						new_obstructed_edges = graph_generator::obstructed_edges(new_obstructed_edges | obstructed_edge_ca);

					if ( (obstructed_edges & obstructed_edge_ca) == obstructed_edge_ca )
						new_obstructed_edges = graph_generator::obstructed_edges(new_obstructed_edges | obstructed_edge_ab);

					obstructed_edges	= new_obstructed_edges;

					continue;
				}

				// segment lies completely on the edge
				// here we make this edge TV1 => TV2
				// and S0 will be closer to TV1 than S1
				R_ASSERT				( triangle_vertices[0] != last_vertex_on_line_with_segment );
				R_ASSERT				( triangle_vertices[2] == last_vertex_on_line_with_segment );

				if ( s0_minus_t1.square_length() > s1_minus_t1.square_length() )
					std::swap			( segment_vertices[0], segment_vertices[1] );

				success_count			+= try_subdivide_triangle( triangles, triangle, *triangle_vertices[1],	segment_vertices[0],	*triangle_vertices[0],	(obstructed_edges & obstructed_edge_ab) ? obstructed_edge_ca : obstructed_edge_none)	? 1 : 0;
				success_count			+= try_subdivide_triangle( triangles, triangle, segment_vertices[0],	segment_vertices[1],	*triangle_vertices[0],	obstructed_edge_ab)		? 1 : 0;
				success_count			+= try_subdivide_triangle( triangles, triangle, segment_vertices[1],	*triangle_vertices[2],	*triangle_vertices[0],	(obstructed_edges & obstructed_edge_ca) ? obstructed_edge_ab : obstructed_edge_none)	? 1 : 0;
				break;
			}

			s0_cross_s1_t1.normalize( );
			bool const is_t1		= math::is_zero( (triangle.normal | s0_cross_s1_t1) - 1.f, math::epsilon_3);
			R_ASSERT				( is_t1 || math::is_zero( (triangle.normal | -s0_cross_s1_t1) - 1.f, math::epsilon_3) );
			if ( is_t1 ) {
				vertices[0]			= &triangle.vertices[1];
				vertices[1]			= &triangle.vertices[2];
				vertices[2]			= &triangle.vertices[0];

				graph_generator::obstructed_edges	new_obstructed_edges = obstructed_edge_none;

				if ( (obstructed_edges & obstructed_edge_ab) == obstructed_edge_ab )
					new_obstructed_edges = graph_generator::obstructed_edges(new_obstructed_edges | obstructed_edge_ca);

				if ( (obstructed_edges & obstructed_edge_bc) == obstructed_edge_bc )
					new_obstructed_edges = graph_generator::obstructed_edges(new_obstructed_edges | obstructed_edge_ab);

				if ( (obstructed_edges & obstructed_edge_ca) == obstructed_edge_ca )
					new_obstructed_edges = graph_generator::obstructed_edges(new_obstructed_edges | obstructed_edge_bc);

				obstructed_edges	= new_obstructed_edges;
			}
			else {
				vertices[0]			= &triangle.vertices[0];
				vertices[1]			= &triangle.vertices[1];
				vertices[2]			= &triangle.vertices[2];
			}

			R_ASSERT_CMP			( iteration, <, 4 );

			if ( m_subdivision_current_operation_id < m_subdivision_max_operation_id ) {
				subdivided_triangle		temp;
				temp.color.set			( 255, 0, 0, 255 );

				temp.vertices[0]	= *vertices[0];
				temp.vertices[1]	= *vertices[1];
				temp.vertices[2]	= segment_vertices[0];
				m_subdivided_triangles.push_back( temp );

				temp.vertices[0]	= *vertices[1];
				temp.vertices[1]	= segment_vertices[1];
				temp.vertices[2]	= segment_vertices[0];
				m_subdivided_triangles.push_back( temp );

				temp.vertices[0]	= *vertices[1];
				temp.vertices[1]	= *vertices[2];
				temp.vertices[2]	= segment_vertices[1];
				m_subdivided_triangles.push_back( temp );

				temp.vertices[0]	= *vertices[2];
				temp.vertices[1]	= *vertices[0];
				temp.vertices[2]	= segment_vertices[1];
				m_subdivided_triangles.push_back( temp );

				temp.vertices[0]	= *vertices[0];
				temp.vertices[1]	= segment_vertices[0];
				temp.vertices[2]	= segment_vertices[1];
				m_subdivided_triangles.push_back( temp );
			}

			success_count			+= try_subdivide_triangle( triangles, triangle, *vertices[0],	*vertices[1],			segment_vertices[0],	(obstructed_edges & obstructed_edge_ab) ? obstructed_edge_ab : obstructed_edge_none )	? 1 : 0;
			success_count			+= try_subdivide_triangle( triangles, triangle, *vertices[1],	segment_vertices[1],	segment_vertices[0],	obstructed_edge_bc)		? 1 : 0;
			success_count			+= try_subdivide_triangle( triangles, triangle, *vertices[1],	*vertices[2],			segment_vertices[1],	(obstructed_edges & obstructed_edge_bc) ? obstructed_edge_ab : obstructed_edge_none )	? 1 : 0;
			success_count			+= try_subdivide_triangle( triangles, triangle, *vertices[2],	*vertices[0],			segment_vertices[1],	(obstructed_edges & obstructed_edge_ca) ? obstructed_edge_ab : obstructed_edge_none )	? 1 : 0;
			success_count			+= try_subdivide_triangle( triangles, triangle, *vertices[0],	segment_vertices[0],	segment_vertices[1],	obstructed_edge_bc)		? 1 : 0;
			break;
		}
	}

	if ( success_count )
		return						true;

	triangle.color.m_value			= root ? math::color_xrgb( 0, 255, 255 ) : math::color_xrgb( 0, 0, 255 );
	if ( m_subdivision_current_operation_id++ < m_subdivision_max_operation_id )
		add_output_triangle			( triangle );

	return							false;
}

template < typename ContainerType >
void graph_generator::subdivide_triangle( subdivided_triangle& triangle, ContainerType& triangles, bool root )
{
	triangles.erase					(
		std::remove_if(
			triangles.begin( ),
			triangles.end( ),
			filter_non_intersected_predicate( *this, triangle )
		),
		triangles.end( )
	);

	if ( triangles.empty() ) {
		triangle.color.m_value		= root ? math::color_xrgb( 0, 255, 255 ) : math::color_xrgb( 0, 0, 255 );
		if ( m_subdivision_current_operation_id++ < m_subdivision_max_operation_id )
			add_output_triangle	( triangle );
		return;
	}

	u32 const triangle_index	= m_subdivided_triangles.size( );
	triangle.color.set			( 255, 0, 0, 255 );
	bool triangle_added			= true;
	if ( m_subdivision_current_operation_id < m_subdivision_max_operation_id )
		add_output_triangle		( triangle );
	else
		triangle_added			= false;

	if ( !subdivide_triangle_impl( triangle, triangles, root ) ) {
		if ( triangle_added )
			m_subdivided_triangles.erase( m_subdivided_triangles.begin() + triangle_index );
		return;
	}

	if ( !triangle_added )
		return;

	if ( m_subdivision_current_operation_id < m_subdivision_max_operation_id ) {
		m_subdivided_triangles.erase				( m_subdivided_triangles.begin() + triangle_index );

		if ( !root )
			return;

		subdivided_triangles_type::iterator i		= m_subdivided_triangles.begin( ) + triangle_index;
		subdivided_triangles_type::iterator const e	= m_subdivided_triangles.end( );
		for ( ; i != e; ++i )
			(*i).color.set							( 0, 255, 255, 255 );

		return;
	}

	m_subdivided_triangles[triangle_index].color.set( 255, 255, 0, 255 );
}

class filter_obstructions_predicate {
public:
	inline filter_obstructions_predicate	( graph_generator const& generator, xray::math::plane const& plane ) :
		m_generator				( &generator ),
		m_plane					( &plane )
	{
	}

		inline bool operator( )	( xray::collision::triangle_result const& triangle_result ) const
	{
		triangle_to_test const& triangle = m_generator->navigation_triangles()[ triangle_result.triangle_id ];

		if ( triangle.is_walkable )
			return				true;

		if ( xray::math::abs(triangle.plane.normal | m_plane->normal) < 1.f )
			return				false;

		if ( xray::math::is_similar(triangle.plane.d, m_plane->d, xray::math::epsilon_5) )
			return				true;

		return					false;
	}

private:
	graph_generator const*		m_generator;
	xray::math::plane const*	m_plane;
}; // struct filter_obstructions_predicate

void graph_generator::subdivide_walkable_triangles	( )
{
	m_subdivision_current_operation_id	= 0;

	m_subdivided_triangles.clear	( );
	m_subdivided_triangles.reserve	( 2*m_navigation_triangles.size() );

	triangles_type triangles		( debug::g_mt_allocator );

	navigation_triangles_type::iterator i		= m_navigation_triangles.begin( );
	navigation_triangles_type::iterator const e	= m_navigation_triangles.end( );
	for (u32 j=0; i != e; ++i, ++j ) {
		if ( !(*i).is_walkable )
			continue;

		u32 const* const indices	= (*i).indices;
		subdivided_triangle		triangle;
		triangle.vertices[0]		= m_transformed_vertices[ indices[0] ];
		triangle.vertices[1]		= m_transformed_vertices[ indices[1] ];
		triangle.vertices[2]		= m_transformed_vertices[ indices[2] ];
		triangle.normal				= (*i).plane.normal;
		triangle.obstructed_edges	= obstructed_edge_none;

		math::aabb aabb;
		aabb.invalidate				( );
		aabb.modify					( triangle.vertices[0] );
		aabb.modify					( triangle.vertices[1] );
		aabb.modify					( triangle.vertices[2] );

		math::float3 const& center	= aabb.center( );
		math::float3 const& extents	= aabb.extents( );
		float const epsilon			= 0.001f;
		aabb.modify					( center + float3( -epsilon - extents.x, -5.f - extents.y, -epsilon - extents.z ) );
		aabb.modify					( center + float3( +epsilon + extents.x,  5.f + extents.y, +epsilon + extents.z ) );

		triangles.clear				( );
		m_geometry->aabb_query		( 0, aabb, triangles );
		triangles.erase				(
			std::remove_if(
				triangles.begin( ),
				triangles.end( ),
				filter_obstructions_predicate( *this, (*i).plane )
			),
			triangles.end( )
		);

		subdivide_triangle			( triangle, triangles, true );
	}
}

#endif // #if XRAY_DEBUG_ALLOCATOR