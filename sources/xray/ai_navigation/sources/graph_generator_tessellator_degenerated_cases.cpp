////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "graph_generator_tessellator.h"

using xray::ai::navigation::graph_generator_tessellator;
using xray::ai::navigation::triangles_mesh_type;
using xray::math::float3;

float3 const* select_equal_vertices	(
		float3 const& vertex0,
		float3 const& vertex1,
		float3 const& vertex2,
		float3 const& testee
	);

bool graph_generator_tessellator::process_degenerated_cases(
		u32 triangle_id,
		float3 const (&segment_vertices)[2]
	)
{
	float3 const vertex0		= m_triangles_mesh.vertices[ m_triangles_mesh.indices[3*triangle_id + 0] ];
	float3 const vertex1		= m_triangles_mesh.vertices[ m_triangles_mesh.indices[3*triangle_id + 1] ];
	float3 const vertex2		= m_triangles_mesh.vertices[ m_triangles_mesh.indices[3*triangle_id + 2] ];

	math::float3 const* equal_vertices[2] = { 0, 0 };
	equal_vertices[0]			= select_equal_vertices	( vertex0, vertex1, vertex2, segment_vertices[0] );
	equal_vertices[1]			= select_equal_vertices	( vertex0, vertex1, vertex2, segment_vertices[1] );


	if ( equal_vertices[0] && equal_vertices[1] )
		return true;

	if ( !equal_vertices[0] && !equal_vertices[1] )
		return							false;

	float3 const* const non_equal_vertex = equal_vertices[0] ? &segment_vertices[1] : &segment_vertices[0];
	u32 success_count					= 0;
	success_count						+= add_output_triangle( triangle_id, vertex0,	vertex1,	*non_equal_vertex ) ? 1 : 0;
	success_count						+= add_output_triangle( triangle_id, vertex1,	vertex2,	*non_equal_vertex ) ? 1 : 0;
	success_count						+= add_output_triangle( triangle_id, vertex2,	vertex0,	*non_equal_vertex ) ? 1 : 0;
	return								true;

}
