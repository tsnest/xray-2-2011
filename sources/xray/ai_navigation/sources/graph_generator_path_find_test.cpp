////////////////////////////////////////////////////////////////////////////
//	Created		: 09.09.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "graph_generator.h"
#include "navigation_mesh_functions.h"
#include "path_finder_channel.h"
#include "path_finder_modified_funnel.h"
#include <xray/collision/api.h>
#include <xray/collision/geometry_instance.h>
#include "path_finder_modified_funnel.h"

using xray::ai::navigation::graph_generator;
using xray::math::float3;
using xray::collision::ray_triangles_type;

static inline bool true_predicate( xray::collision::ray_triangle_result const& ) 
{
	return true;
}

struct distance_predicate {
	inline bool	operator( ) ( xray::collision::ray_triangle_result const& left, xray::collision::ray_triangle_result const& right )
	{
		return			( left.distance < right.distance );
	}
	
	inline bool	operator( ) ( xray::collision::ray_object_result const& left, xray::collision::ray_object_result const& right )
	{
		return			( left.distance < right.distance );
	}
}; // distance_predicate

u32 graph_generator::pick_triangle_by_ray( float3 const& position, float3 const& direction, float3& projected_vertex ) 
{
		ray_triangles_type ray_triangles( g_allocator );
		m_input_triangles.build_spatial_tree ( );
		float distance;
		m_input_triangles.spatial_tree->ray_query (0, position, direction, 100, distance, ray_triangles, true_predicate );
		std::sort( ray_triangles.begin( ), ray_triangles.end( ), distance_predicate( ) );

		if ( !ray_triangles.empty() ) {
			projected_vertex = position + direction * ray_triangles[0].distance;
			return ray_triangles[0].triangle_id;
		}

		return u32(-1);
}

void graph_generator::set_start ( float3 const& position, float3 const& direction )
{
	float3 projected_position;
	u32 const node = pick_triangle_by_ray( position, direction, projected_position );
	if ( node != u32(-1) ) {
		if ( !can_stand( m_input_triangles, projected_position, node, m_path_finder_agent_radius ) ) {
			m_start_node = u32(-1);
			m_start_position = float3 ( );
			return;
		}
	}
	m_start_node = node;
	m_start_position = projected_position;

	find_shortest_path( true );
}

void graph_generator::set_goal ( float3 const& position, float3 const& direction ) 
{
	float3 projected_position;
	u32 const node = pick_triangle_by_ray( position, direction, projected_position );
	if ( node != u32(-1) ) {
		if ( !can_stand( m_input_triangles, projected_position, node, m_path_finder_agent_radius ) ) {
			m_goal_node = u32(-1);
			m_goal_position = float3 ( );
			return;
		}
	}
	m_goal_node = node;
	m_goal_position = projected_position;

	find_shortest_path( true );
}

void graph_generator::find_shortest_path ( bool build_channel ) 
{
	if ( m_start_node != u32(-1) && m_goal_node != u32(-1) ) {
		
		if ( build_channel )
			path_finder_channel( m_input_triangles, m_channel, m_start_node, m_goal_node, m_start_position, m_goal_position, m_path_finder_agent_radius );

		m_path.clear();
		if ( !m_channel.empty() ) {
				path_finder_modified_funnel( m_input_triangles, m_channel, m_start_position, m_goal_position, m_path_finder_agent_radius, m_path );
//				path_finder_funnel( m_input_triangles, m_channel, m_start_position, m_goal_position, m_path );
		}
	}
}

bool graph_generator::check_adjacency( u32 const triangle_id0, u32 const triangle_id1 )
{
	for ( u32 i = 0; i < 3; ++i ) {
		if ( m_input_triangles.data[ triangle_id0 ].neighbours[i] == triangle_id1 )
			return true;
	}
	return false;
}

void graph_generator::add_channel_triangle ( float3 const& position, float3 const& direction ) 
{
	float3 projected_vertex;
	u32 const triangle_id = pick_triangle_by_ray ( position, direction, projected_vertex );
	if ( triangle_id == u32(-1) ) 
		return;

	if ( m_channel.empty() ) {
		m_start_position = projected_vertex;
		m_channel.push_back( triangle_id );
		m_start_node = triangle_id;
		return;
	}

	if ( check_adjacency( triangle_id, m_channel.back() ) ) {
		m_goal_position = projected_vertex;
		m_channel.push_back( triangle_id );
		m_goal_node = triangle_id;
	}
}
