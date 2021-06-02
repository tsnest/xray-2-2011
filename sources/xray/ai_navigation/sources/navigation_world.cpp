////////////////////////////////////////////////////////////////////////////
//	Created		: 29.07.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "navigation_world.h"
#include "graph_generator.h"
#include <xray/input/world.h>
#include <xray/input/keyboard.h>
#include <xray/ai_navigation/sources/navigation_mesh_types.h>

#ifndef MASTER_GOLD
#	include <xray/ai_navigation/sources/path_finder_channel.h>
#	include <xray/ai_navigation/sources/path_finder_modified_funnel.h>
#endif // #ifndef MASTER_GOLD

#include <xray/configs.h>
#include "navigation_mesh_functions.h"

using xray::ai::navigation::navigation_world;
using namespace xray::configs;


static xray::command_line::key		s_navigation_debug_controls( "navigation_debug_controls", "", "", "ai navigation debug controls enabled" );

navigation_world::navigation_world			( xray::ai::navigation::engine& engine, xray::render::scene_ptr const& scene, xray::render::debug::renderer& renderer ) :
	m_graph_generator	( 0 ),
	m_engine			( engine ),
	m_renderer			( renderer )
{
//	float4x4 const transform	= math::create_translation( float3( 0.f, 0.f, 10.f ) );
	float4x4 const transform	= math::create_translation( float3( 0.f, 0.f, 0.f ) );
#if XRAY_DEBUG_ALLOCATOR
	m_graph_generator	= NEW( graph_generator) ( engine, transform, scene, renderer, m_navigation_mesh );
#else // #if XRAY_DEBUG_ALLOCATOR
	XRAY_UNREFERENCED_PARAMETER	( scene );
#endif // #if XRAY_DEBUG_ALLOCATOR
}

navigation_world::~navigation_world			( )
{
#if XRAY_DEBUG_ALLOCATOR
	DELETE				( m_graph_generator );
#endif // #if XRAY_DEBUG_ALLOCATOR
}

void navigation_world::clear_resources		( )
{
#if XRAY_DEBUG_ALLOCATOR
	if ( m_graph_generator )
		m_graph_generator->clear_resources	( );
#endif // #if XRAY_DEBUG_ALLOCATOR
}

void navigation_world::tick					( )
{
#if XRAY_DEBUG_ALLOCATOR
	if ( m_graph_generator )
		m_graph_generator->tick				( );
#endif // #if XRAY_DEBUG_ALLOCATOR
}

#ifdef DEBUG
void navigation_world::debug_render			(
		xray::input::world& input,
		xray::math::float3 const& position,
		xray::math::float3 const& direction,
		xray::math::frustum const& frustum
	)
{
	m_graph_generator->set_frustum( frustum );

	if ( !s_navigation_debug_controls.is_set() ) {
		return;
	}

	static bool last_state[] = { false, false, false, false, false, false, false, false };
	if ( last_state[0] != input.get_keyboard()->is_key_down( input::key_f1 ) ) {
		last_state[0] = input.get_keyboard()->is_key_down( input::key_f1 );
		if ( last_state[0] )
			m_graph_generator->ray_mark_triangle( position, direction );
	}

	if ( last_state[1] != input.get_keyboard()->is_key_down( input::key_f2 ) ) {
		last_state[1] = input.get_keyboard()->is_key_down( input::key_f2 );
		if ( last_state[1] )
			m_graph_generator->aabb_mark_triangle( position, direction );
	}

	if ( last_state[2] != input.get_keyboard()->is_key_down( input::key_f3 ) ) {
		last_state[2] = input.get_keyboard()->is_key_down( input::key_f3 );
		if ( last_state[2] ) {
			static bool last_show_state = false;
			last_show_state = !last_show_state;
			string64 s;
			sprintf (s, "%i", last_show_state );
			m_graph_generator->change_show_processed_model ( s );
		}
	}

	if ( last_state[3] != input.get_keyboard()->is_key_down( input::key_f4 ) ) {
		last_state[3] = input.get_keyboard()->is_key_down( input::key_f4 );
		if ( last_state[3] ) {
			static bool last_show_state = false;
			last_show_state = !last_show_state;
			m_graph_generator->change_show_triangle_faces ( last_show_state );
		}
	}

#if 1
	if ( last_state[5] != input.get_keyboard()->is_key_down( input::key_f5 ) ) {
		last_state[5] = input.get_keyboard()->is_key_down( input::key_f5 );
		if ( last_state[5] ) {
			//m_graph_generator->set_start( position, direction );
			m_start_position = position;
			m_start_node = get_node_id_at( position );
		}
	}

	if ( last_state[6] != input.get_keyboard()->is_key_down( input::key_f6 ) ) {
		last_state[6] = input.get_keyboard()->is_key_down( input::key_f6 );
		if ( last_state[6] ) {
			m_goal_position = position;
			m_goal_node = get_node_id_at( position );
		}
	}

	if ( last_state[7] != input.get_keyboard()->is_key_down( input::key_f7 ) ) {
		last_state[7] = input.get_keyboard()->is_key_down( input::key_f7 );
		if ( last_state[7] ) {
			find_path(
				m_start_node, 
				m_start_position, 
				m_goal_node, 
				m_goal_position,
				0.3f,
				m_graph_generator->m_path
			);
		}
	}

#else
	if ( last_state[5] != input.get_keyboard()->is_key_down( input::key_f5 ) ) {
		last_state[5] = input.get_keyboard()->is_key_down( input::key_f5 );
		if ( last_state[5] ) {
			m_graph_generator->add_channel_triangle( position, direction );
		}
	}

	if ( last_state[6] != input.get_keyboard()->is_key_down( input::key_f6 ) ) {
		last_state[6] = input.get_keyboard()->is_key_down( input::key_f6 );
		if ( last_state[6] ) {
			m_graph_generator->reset_channel();
		}
	}


	if ( last_state[7] != input.get_keyboard()->is_key_down( input::key_f7 ) ) {
		last_state[7] = input.get_keyboard()->is_key_down( input::key_f7 );
		if ( last_state[7] ) {
			m_graph_generator->find_shortest_path ( false );
		}
	}

#endif


}

#endif

void navigation_world::on_binary_config_resource ( xray::resources::queries_result& resources ) 
{
#ifndef MASTER_GOLD
	if ( resources.is_successful() ) {
		m_navigation_mesh.indices.clear();
		m_navigation_mesh.vertices.clear();
		m_navigation_mesh.data.clear();
		m_navigation_mesh.destroy_spatial_tree();

		if ( m_navigation_mesh.spatial_tree )
			m_navigation_mesh.destroy_spatial_tree ( );
		graph_generator::load_navigation_mesh( m_navigation_mesh, static_cast_resource_ptr<binary_config_ptr>( resources[0].get_unmanaged_resource() ) );
		m_navigation_mesh.build_spatial_tree	( );
	}
#endif // #ifndef MASTER_GOLD
}

void navigation_world::load_navmesh		( pcstr project_name )
{
	pstr full_path = 0;
	STR_JOINA( full_path, "resources/projects/", project_name, "/navmesh.lua" );

	resources::query_resource	(
		full_path,
		resources::binary_config_class,
		boost::bind( &navigation_world::on_binary_config_resource, this, _1 ),
		g_allocator
	);
}

bool is_point_inside_triangle	(
	u32 const (&coordinate_indices)[2],
	xray::math::float3 p,
	xray::math::float3 a,
	xray::math::float3 b,
	xray::math::float3 c
);

u32	navigation_world::get_node_id_at		( xray::math::float3 const& position )
{
	float min_distance = 0;
	u32 min_node_id = u32(-1);

#ifndef MASTER_GOLD
	u32 const coordinate_indices[2] = { 0, 2 };
	u32 const triangles_count = m_navigation_mesh.data.size();
	for ( u32 i = 0; i < triangles_count; ++i ) {
		u32 const* indices = m_navigation_mesh.indices.begin() + i * 3;
		float3 const& v0 = m_navigation_mesh.vertices[ indices[0] ];
		float3 const& v1 = m_navigation_mesh.vertices[ indices[1] ];
		float3 const& v2 = m_navigation_mesh.vertices[ indices[2] ];

		if ( is_point_inside_triangle( coordinate_indices,  position, v0, v1, v2 ) ) {
			float distance = create_plane( v0, v1, v2 ).distance( position );
			if ( min_node_id == u32(-1) ) {
				min_node_id = i;
				min_distance = distance;
			} else {
				if ( distance < min_distance ) {
					min_node_id = i;
					min_distance = distance;
				}
			}
		}
	}
#endif // #ifndef MASTER_GOLD

	return min_node_id;
}

u32	navigation_world::get_node_id_after_move	(
		u32 const previous_triangle_id, 
		xray::math::float3 const& previous_position, 
		xray::math::float3 const& new_position 
	)
{
	XRAY_UNREFERENCED_PARAMETERS( previous_triangle_id, &previous_position );
	//u32 const* const indices	= m_navigation_mesh.indices.begin() + previous_triangle_id * 3;
	//if	(
	//		is_point_inside_triangle(
	//			coordinate_indices,
	//			new_position,
	//			m_navigation_mesh.vertices[ indices[0] ],
	//			m_navigation_mesh.vertices[ indices[1] ],
	//			m_navigation_mesh.vertices[ indices[2] ]
	//		)
	//	)
	//	return					previous_triangle_id;

	return						get_node_id_at( new_position );
}

bool navigation_world::find_path (
		u32 start_node,
		xray::math::float3 const& start_position,
		u32 goal_node,
		xray::math::float3 const& goal_position,
		float agent_radius,
		xray::ai::navigation::path_type& path
	)
{
#ifndef MASTER_GOLD
	path_finder_funnel::channel_type channel;
	path_finder_channel(
		m_navigation_mesh,
		channel,
		start_node,
		goal_node,
		start_position,
		goal_position,
		agent_radius
	);

	if ( channel.empty() )
		return false;

	path_finder_modified_funnel(
		m_navigation_mesh,
		channel,
		start_position,
		goal_position,
		agent_radius,
		path
	);
#endif // #ifndef MASTER_GOLD

	return true;
}

bool navigation_world::can_stand	( 
		xray::math::float3 const& vertex,
		u32 const triangle_id,
		float const agent_radius
	)
{
#ifndef MASTER_GOLD
	return					xray::ai::navigation::can_stand( m_navigation_mesh, vertex, triangle_id, agent_radius );
#else // #ifndef MASTER_GOLD
	return					false;
#endif // #ifndef MASTER_GOLD
}