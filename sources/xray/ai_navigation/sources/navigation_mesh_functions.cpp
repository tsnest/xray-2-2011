////////////////////////////////////////////////////////////////////////////
//	Created		: 15.09.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "navigation_mesh_functions.h"

using xray::ai::navigation::triangles_mesh_type;
using xray::ai::navigation::distance_to_segment;
using xray::math::float3;

// Angle between ab and bc
static inline bool is_angle_obtuse ( float3 const& a, float3 const& b, float3 const& c )
{
	return ((a-b) | (c-b)) <= 0.0f;
}

static inline bool is_constrained( triangles_mesh_type const& triangles_mesh, u32 const triangle_id, u32 const edge_id ) 
{
	return ( triangles_mesh.data[ triangle_id ].neighbours[ edge_id ] == u32(-1) ) || ( triangles_mesh.data[ triangle_id ].obstructions[ edge_id ] != 0.0f );
}

float search_triangle_width ( 
		triangles_mesh_type const& triangles_mesh,
		float3 const& vertex,
		u32 const triangle_id,
		u32 const edge_id,
		float distance,
		bool check_angle = true
	)
{
	u32 const* indices = triangles_mesh.indices.begin() + triangle_id * 3;
	float3 const u = triangles_mesh.vertices[ indices[ edge_id ] ];
	float3 const v = triangles_mesh.vertices[ indices[ ( edge_id + 1 ) % 3 ] ];

	if ( check_angle && (is_angle_obtuse( vertex, u, v ) || is_angle_obtuse( vertex, v, u )) )
		return distance;

	float const distance0 = distance_to_segment ( vertex, u, v );
	if ( distance0 > distance )
		return distance;

	if ( is_constrained( triangles_mesh, triangle_id, edge_id ) )
		return distance0;

	u32 const opposite_triangle_id = triangles_mesh.data[ triangle_id ].neighbours[ edge_id ];
	u32 const similar_edge_id = get_similar_edge ( triangles_mesh, opposite_triangle_id, triangle_id );
	u32 const other_edges[] = {
		(similar_edge_id + 1) % 3,
		(similar_edge_id + 2) % 3
	};
	distance = search_triangle_width( triangles_mesh, vertex, opposite_triangle_id, other_edges[0], distance, check_angle );
	return search_triangle_width( triangles_mesh, vertex, opposite_triangle_id, other_edges[1], distance, check_angle );
}

namespace xray {
namespace ai {
namespace navigation {

float distance_to_segment					( float3 const& v, float3 const& s0, float3 const& s1 )
{
	float3 const& s1_minus_s0		= s1 - s0;
	float const s1_minus_s0_length	= length( s1_minus_s0 );
	R_ASSERT						( !xray::math::is_zero( s1_minus_s0_length ) );
	float3 const& direction			= s1_minus_s0 / s1_minus_s0_length;
	float3 const& v_minus_s0		= v - s0;
	float const projection			= v_minus_s0 | direction;
	if ( projection < 0 )
		return						length( v_minus_s0 );

	if ( projection > s1_minus_s0_length )
		return						length( v - s1 );

	return							length( v_minus_s0 - direction*projection );
}

float calculate_triangle_width (
		triangles_mesh_type const& triangles_mesh,
		float3 const vertex,
		u32 const triangle_id,
		u32 const edge_id,
		bool check_angle
	)
{
	u32 const* indices = triangles_mesh.indices.begin() + triangle_id * 3;
	float3 const& u0 = triangles_mesh.vertices[ indices[ edge_id ] ];
	float3 const& u1 = triangles_mesh.vertices[ indices[ (edge_id+1)%3 ] ];

	float const distance = xray::math::min( length( vertex - u0 ), length( vertex - u1 ) );
	if ( is_angle_obtuse( vertex, u0, u1 ) || is_angle_obtuse( vertex, u1, u0 ) )
		return distance;
	if ( is_constrained ( triangles_mesh, triangle_id, edge_id ) )
		return distance_to_segment ( vertex, u0, u1 );

	return search_triangle_width( triangles_mesh, vertex, triangle_id, edge_id, distance, check_angle);
}

bool can_stand( 
		triangles_mesh_type const& triangles_mesh,
		float3 const vertex,
		u32 const triangle_id,
		float agent_radius
	)
{
	for ( u32 i = 0; i < 3; ++i ) {
		if ( calculate_triangle_width( triangles_mesh, vertex, triangle_id, i, false ) < agent_radius )
			return false;
	}
	return true;
}

bool is_null_square ( 
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

} // namespace navigation
} // namespace ai
} // namespace xray