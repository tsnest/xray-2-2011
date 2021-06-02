////////////////////////////////////////////////////////////////////////////
//	Created		: 19.09.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "graph_generator_disconnected_regions_remover.h"
#include "navigation_mesh_functions.h"

using xray::ai::navigation::graph_generator_disconnected_regions_remover;
using xray::ai::navigation::triangles_mesh_type;
using xray::math::float3;

graph_generator_disconnected_regions_remover::graph_generator_disconnected_regions_remover (
		triangles_mesh_type& navigation_mesh,
		float min_agent_radius,
		float min_agent_height
	) :
		m_navigation_mesh	( navigation_mesh ),
		m_min_agent_radius	( min_agent_radius ),
		m_min_agent_height	( min_agent_height )
{
	u32 const triangles_count = navigation_mesh.data.size();
	
	markers_type visited_markers(
		ALLOCA( triangles_count * sizeof( markers_type::value_type ) ),
		triangles_count,
		triangles_count,
		false
	);

	for ( u32 i = 0; i < triangles_count; ++i ) {
		if ( !visited_markers[ i ] && !m_navigation_mesh.data[ i ].is_marked ) {
			if ( !validate_region( i, visited_markers ) )
				remove_region( i );
		}
	}
		
	m_navigation_mesh.remove_marked_triangles( );
}

float3 graph_generator_disconnected_regions_remover::calculate_triangle_center( u32 const triangle_id ) 
{
	R_ASSERT ( triangle_id < m_navigation_mesh.data.size() );
	u32 const* indices = m_navigation_mesh.indices.begin() + triangle_id * 3;
	return ( m_navigation_mesh.vertices[ indices[0] ] + m_navigation_mesh.vertices[ indices[1] ] + m_navigation_mesh.vertices[ indices[2] ] ) / 3.0f;
}

bool graph_generator_disconnected_regions_remover::validate_region ( u32 const triangle_id, markers_type& visited_markers )
{
	float3 triangle_center = calculate_triangle_center( triangle_id );
	visited_markers[ triangle_id ] = true;

	bool result = false;
	if ( can_stand( m_navigation_mesh, triangle_center, triangle_id, m_min_agent_radius ) ) {
		result = true;;
	}

	for ( u32 i = 0; i < 3; ++i ) {
		u32 const neighbour_id = m_navigation_mesh.data[ triangle_id ].neighbours[ i ];
		if ( neighbour_id != u32(-1) && !visited_markers[ neighbour_id ] && m_navigation_mesh.data[ triangle_id ].obstructions[ i ] == 0.0f) {
			result |= validate_region( neighbour_id, visited_markers );
		}
	}
	return result;
}

void graph_generator_disconnected_regions_remover::remove_region ( u32 const triangle_id )
{
	m_navigation_mesh.data[ triangle_id ].is_marked = true;
	for ( u32 i = 0; i < 3; ++i ) {
		u32 const neighbour_id = m_navigation_mesh.data[ triangle_id ].neighbours[ i ];
		if ( neighbour_id != u32(-1) && m_navigation_mesh.data[ triangle_id ].obstructions[ i ] == 0.0f ) {
			if ( m_navigation_mesh.data[ neighbour_id ].is_marked )
				continue;
			remove_region( neighbour_id );
		}
	}
}
