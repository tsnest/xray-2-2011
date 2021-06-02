////////////////////////////////////////////////////////////////////////////
//	Created		: 05.07.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef NAVIGATION_MESH_TYPES_H_INCLUDED
#define NAVIGATION_MESH_TYPES_H_INCLUDED

#include "triangles_mesh.h"

namespace xray {
namespace ai {
namespace navigation {

struct navigation_triangle {
	inline navigation_triangle	( ) :
		color		( xray::math::color( 255, 255, 255, 64 ) ),
		is_marked	( false )
	{
		for ( u32 i = 0; i < 3; ++i )
			neighbours[i] = u32(-1);
		for ( u32 i = 0; i < 3; ++i )
			obstructions[i] = 0;
	}

	inline navigation_triangle	(
			math::plane	const& plane,
			bool const is_passable
	) :
		plane		( plane ),
		color		( xray::math::color( 255, 255, 255, 64 ) ),
		is_passable	( is_passable ),
		is_marked	( false )
	{
		for ( u32 i = 0; i < 3; ++i )
			neighbours[i] = u32(-1);
		for ( u32 i = 0; i < 3; ++i )
			obstructions[i] = 0;
	}

	inline bool is_obstructed( u32 const edge_id ) const
	{
		R_ASSERT( edge_id < 3 );
		return !xray::math::is_zero( obstructions[ edge_id ] );
	}

	math::plane	plane;
	math::color	color;
	u32			neighbours[3];
	float		obstructions[3];
	bool		is_passable;
	bool		is_marked;
}; // struct vertex_simple

struct edge {
	edge( u32 const id0, u32 const id1) :
		vertex_index0( id0 ),
		vertex_index1( id1 )
	{
	}

	u32 vertex_index0;
	u32 vertex_index1;
}; // struct edge

#ifndef MASTER_GOLD

typedef triangles_mesh< navigation_triangle >			triangles_mesh_type;

bool intersect_triangles	( triangles_mesh_type const& triangles_mesh, u32 const triangle_id0, u32 const triangle_id1, bool const could_be_coplanar );

typedef collision::triangles_type						triangles_type;
typedef buffer_vector< collision::triangle_result >		buffer_triangles_type;
typedef debug::vector< math::float3 >					vertices_type;
typedef debug::vector< edge >							edges_type;

typedef debug::vector< math::float3 >					path_type;
typedef debug::vector< u32 >							channel_type;

#else // #ifndef MASTER_GOLD
typedef vectora< math::float3 >							path_type;
#endif // #ifndef MASTER_GOLD

} // namespace navigation
} // namespace ai
} // namespace xray

#endif // #ifndef NAVIGATION_MESH_TYPES_H_INCLUDED