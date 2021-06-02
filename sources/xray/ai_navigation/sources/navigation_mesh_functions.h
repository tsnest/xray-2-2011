////////////////////////////////////////////////////////////////////////////
//	Created		: 15.09.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef NAVIGATION_MESH_FUNCTIONS_H_INCLUDED
#define NAVIGATION_MESH_FUNCTIONS_H_INCLUDED

#ifndef MASTER_GOLD

#include "navigation_mesh_types.h"

namespace xray {
namespace ai {
namespace navigation {

float distance_to_segment					( float3 const& v, float3 const& s0, float3 const& s1 );

inline u32 get_similar_edge					( triangles_mesh_type const& triangles_mesh, u32 triangle_id0, u32 const triangle_id1 )
{
	for ( u32 i = 0; i < 3; ++i ) 
		if ( triangles_mesh.data[ triangle_id0 ].neighbours[ i ] == triangle_id1 )
			return i;
	return u32(-1);
}

float calculate_triangle_width (
		triangles_mesh_type const& triangles_mesh,
		float3 const vertex,
		u32 const triangle_id,
		u32 const edge_id,
		bool check_angle = false
	);

bool can_stand( 
		triangles_mesh_type const& triangles_mesh,
		float3 const vertex,
		u32 const triangle_id,
		float agent_radius
	);
bool is_null_square ( 
		float3 const& a,
		float3 const& b,
		float3 const& c
	);

} // namespace navigation
} // namespace ai
} // namespace xray

#endif // #ifndef MASTER_GOLD

#endif // #ifndef NAVIGATION_MESH_FUNCTIONS_H_INCLUDED