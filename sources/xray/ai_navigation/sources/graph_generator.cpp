////////////////////////////////////////////////////////////////////////////
//	Created		: 04.08.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if XRAY_DEBUG_ALLOCATOR

#include "graph_generator.h"
#include "graph_generator_subdivider.h"
#include <xray/collision/geometry.h>
#include <xray/ai_navigation/engine.h>
#include <xray/render/world.h>
#include <xray/render/facade/game_renderer.h>
#include <xray/render/facade/debug_renderer.h>
#include <xray/render/facade/scene_renderer.h>
#include <xray/collision/api.h>
#include <xray/collision/geometry_instance.h>
#include <xray/memory_debug_allocator.h>
#include <xray/console_command.h>

using xray::ai::navigation::graph_generator;
using xray::collision::ray_triangles_type;

static xray::command_line::key		s_test_navigation( "test_navigation", "", "", "test navigation graph generation" );

graph_generator::graph_generator			( 
	xray::ai::navigation::engine& engine, 
	xray::math::float4x4 const& transform, 
	xray::render::scene_ptr const& scene, 
	xray::render::debug::renderer& renderer,
	xray::ai::navigation::triangles_mesh_type& triangles_mesh
) :
	m_input_triangles					( triangles_mesh ),
	m_engine							( engine ),
	m_transform							( transform ),
	m_scene								( scene ),
	m_renderer							( renderer ),
	m_geometry							( 0 ),
	m_subdivision_max_operation_id		( u32(-1) ),
	m_tessellation_max_operation_id		( u32(-1) ),
	m_merger_max_operation_id			( u32(-1) ),
	m_fuser_max_operation_id			( u32(-1) ),
	m_merge_delaunay_max_operation_id	( u32(-1) ),	
	m_show_point_id						( u32(-1) ),
	m_show_triangle_id					( u32(-1) ),
	m_min_triangle_id_to_draw			( 0 ),
	m_triangle_draw_range				( 500 ),
	m_path_finder_agent_radius			( 0.3f ),
	m_min_agent_radius					( 0.3f ),
	m_min_agent_height					( 0.5f ),
	m_show_processed_model				( false ),
	m_show_triangle_faces				( false ),
	m_show_navigation_mesh				( false ),
	m_show_marked_triangles				( false ),
	m_show_path_channel					( false ),
	m_start_node						( u32(-1) ),
	m_goal_node							( u32(-1) )
{
	float4x4 view_matrix = math::create_camera_at(
		float3(0.f,0.f,0.f),
		float3(1.f,0.f,1.f),
		float3(0.f,1.f,0.f)
	);

	float4x4 projection_matrix = math::create_perspective_projection( 
		90,
		1.34f,
		1,
		100
	);

	m_frustum = math::frustum( mul4x4( view_matrix, projection_matrix) );

	static xray::console_commands::cc_delegate s_subdivision_operation_id_console_command(
		"navigation_subdivision_operation_id",
		boost::bind( &graph_generator::change_subdivision_operation_id, this, _1 ),
		console_commands::command_type_engine_internal
	);

	static xray::console_commands::cc_delegate s_tessellation_operation_id_console_command(
		"navigation_tessellation_operation_id",
		boost::bind( &graph_generator::change_tessellation_operation_id, this, _1 ),
		console_commands::command_type_engine_internal
	);

	static xray::console_commands::cc_delegate s_save_navigation_mesh_console_command(
		"navigation_save_navigation_mesh",
		boost::bind( &graph_generator::save_navigation_mesh_console_command, this, _1 ),
		console_commands::command_type_engine_internal
	);

	static xray::console_commands::cc_delegate s_load_navigation_mesh_console_command(
		"navigation_load_navigation_mesh",
		boost::bind( &graph_generator::load_navigation_mesh_console_command, this, _1 ),
		console_commands::command_type_engine_internal
	);

	static xray::console_commands::cc_delegate s_mark_triangle_console_command(
		"navigation_mark_triangle",
		boost::bind( &graph_generator::mark_triangle, this, _1 ),
		console_commands::command_type_engine_internal
	);

	static xray::console_commands::cc_u32 s_show_point_id_console_command(
		"navigation_show_point_id",
		m_show_point_id,
		u32(0),
		u32(-1),
		false,
		console_commands::command_type_engine_internal
	);

	static xray::console_commands::cc_u32 s_show_triangle_id_console_command(
		"navigation_show_triangle_id",
		m_show_triangle_id,
		u32(0),
		u32(-1),
		false,
		console_commands::command_type_engine_internal
	);

	static xray::console_commands::cc_bool s_show_navigation_mesh_console_command(
		"navigation_show_navigation_mesh",
		m_show_navigation_mesh,
		false,
		console_commands::command_type_engine_internal
	);

	static xray::console_commands::cc_bool s_show_path_channel_console_command(
		"navigation_show_path_channel",
		m_show_path_channel,
		false,
		console_commands::command_type_engine_internal
	);

	static xray::console_commands::cc_delegate s_merger_operation_id_console_command(
		"navigation_merger_operation_id",
		boost::bind( &graph_generator::change_merger_operation_id, this, _1 ),
		console_commands::command_type_engine_internal
	);

	static xray::console_commands::cc_bool s_show_processed_model_console_command(
		"navigation_show_processed_model",
		m_show_processed_model,
		false,
		console_commands::command_type_engine_internal
	);

	static xray::console_commands::cc_bool s_show_marked_triangles_console_command(
		"navigation_show_marked_triangles",
		m_show_marked_triangles,
		false,
		console_commands::command_type_engine_internal
	);

	static xray::console_commands::cc_bool s_show_triangle_faces_console_command(
		"navigation_show_triangle_faces",
		m_show_triangle_faces,
		false,
		console_commands::command_type_engine_internal
	);

	static xray::console_commands::cc_delegate s_change_draw_range_console_command(
		"navigation_set_draw_range",
		boost::bind( &graph_generator::change_draw_range, this, _1 ),
		console_commands::command_type_engine_internal
	);

	static xray::console_commands::cc_float s_change_path_finder_agent_radius_command(
		"navigation_path_finder_agent_radius",
		m_path_finder_agent_radius,
		0.0f,
		10.0f,
		false,
		console_commands::command_type_engine_internal
	);

	static xray::console_commands::cc_float s_change_min_agent_radius_command(
		"navigation_min_agent_radius",
		m_path_finder_agent_radius,
		0.0f,
		10.0f,
		false,
		console_commands::command_type_engine_internal
	);

	static xray::console_commands::cc_float s_change_min_agent_height_command(
		"navigation_min_agent_height",
		m_path_finder_agent_radius,
		0.0f,
		10.0f,
		false,
		console_commands::command_type_engine_internal
	);

	static xray::console_commands::cc_delegate s_change_fuser_operation_id_console_command(
		"navigation_fuser_operation_id",
		boost::bind( &graph_generator::change_fuser_operation_id, this, _1 ),
		console_commands::command_type_engine_internal
	);

	static xray::console_commands::cc_delegate s_change_merge_delaunay_operation_id_console_command(
		"navigation_merge_delaunay_operation_id",
		boost::bind( &graph_generator::change_merge_delaunay_operation_id, this, _1 ),
		console_commands::command_type_engine_internal
	);

	static xray::console_commands::cc_delegate s_generate_mesh_console_command(
		"navigation_generate_mesh",
		boost::bind( &graph_generator::generate_mesh, this, _1 ),
		console_commands::command_type_engine_internal
	);

	u32 const buffer_size	= 256;
	pstr const buffer	= static_cast<pstr>( ALLOCA(buffer_size*sizeof(char)) );
	buffer_string	string( buffer, buffer_size );

	if ( !s_test_navigation.is_set_as_string(&string) )
		return;

	/*
	resources::query_resource	(
		string.c_str(),
		resources::static_model_instance_class,
		boost::bind( &graph_generator::on_resources_loaded, this, _1 ),
		g_allocator
	);
	*/

	//test_delaunay();

}

void graph_generator::reset_channel( ) 
{
	m_channel.clear();
}

bool graph_generator::check_adjacency_to_last_channel_triangle ( u32 const triangle_id ) 
{
	u32 const last_channel_triangle = m_channel.back();
	for ( u32 i = 0; i < 3; ++i ) {
		if ( m_input_triangles.data[ triangle_id ].neighbours[ i ] == last_channel_triangle )
			return true;
	}
	return false;
}

static inline bool true_predicate( xray::collision::ray_triangle_result const& ) 
{
	return true;
}

void graph_generator::ray_mark_triangle ( xray::math::float3 const& pos, xray::math::float3 const& dir )
{
	if ( m_input_triangles.data.size() ) {
		float distance;
		ray_triangles_type ray_triangles( g_allocator );
		m_input_triangles.build_spatial_tree ( );
		m_input_triangles.spatial_tree->ray_query (0, pos, dir, 100, distance, ray_triangles, true_predicate );

		static u32 first_triangle = u32(-1);
		static u32 current_triangle = 0;
		u32 new_triangle_id = u32(-1);

		if ( !ray_triangles.empty() ) {
			if ( first_triangle == ray_triangles[0].triangle_id ) {
				current_triangle = ( current_triangle + 1 ) % ray_triangles.size();
			} else {
				first_triangle = ray_triangles[0].triangle_id;
				current_triangle = 0;
			}
			new_triangle_id = ray_triangles[current_triangle].triangle_id;
		} else {
			first_triangle = u32(-1);
			current_triangle = 0;
			new_triangle_id = u32(-1);
		}

		m_show_triangle_id = new_triangle_id;
	
	}
}

void graph_generator::aabb_mark_triangle ( xray::math::float3 const& pos, xray::math::float3 const& dir )
{
	if ( m_input_triangles.data.size() ) {
		math::aabb			aabb(math::create_invalid_aabb());
		aabb.modify( pos );
		aabb.modify( pos + dir );

		triangles_type ray_triangles( g_allocator );
		m_input_triangles.build_spatial_tree ( );
		m_input_triangles.spatial_tree->aabb_query( 0, aabb, ray_triangles );

		static u32 first_triangle = u32(-1);
		static u32 current_triangle = 0;
		u32 new_triangle_id = u32(-1);

		if ( ray_triangles.size() ) {
			if ( first_triangle == ray_triangles[0].triangle_id ) {
				current_triangle = ( current_triangle + 1 ) % ray_triangles.size();
			} else {
				first_triangle = ray_triangles[0].triangle_id;
				current_triangle = 0;
			}
			new_triangle_id = ray_triangles[current_triangle].triangle_id;
		} else {
			first_triangle = u32(-1);
			current_triangle = 0;
			new_triangle_id = u32(-1);
		}

		m_show_triangle_id = new_triangle_id;
		
	}
}

graph_generator::~graph_generator		( )
{
	m_input_triangles.destroy_spatial_tree();
	clear_resources						( );
//	collision::delete_geometry_instance	( g_allocator, m_geometry );
}

void graph_generator::clear_resources		( )
{
	if ( !m_model )
		return;

//	if ( m_show_processed_model )
//		m_scene->remove_model		( m_scene, m_model->m_render_model );

	m_model								= 0;
}

void graph_generator::on_resources_loaded	( xray::resources::queries_result& resources )
{
	if ( !resources.is_successful ( ) )
		return;

	m_model						= static_cast_resource_ptr<render::static_model_ptr>( resources[0].get_unmanaged_resource() );
	m_geometry					= m_model->m_collision_geom.c_ptr();

	clear_geometry				( );
	add_geometry				( m_model->m_collision_geom.c_ptr(), m_transform );
//	if ( m_show_processed_model )
//		m_renderer.scene().add_model( m_scene, m_model->m_render_model, m_transform);
	generate();
}

void graph_generator::change_subdivision_operation_id	( pcstr const args )
{
	XRAY_SSCANF					( args, "%d", &m_subdivision_max_operation_id );
	generate					( );
}

void graph_generator::change_fuser_operation_id	( pcstr const args )
{
	XRAY_SSCANF					( args, "%d", &m_fuser_max_operation_id );
	generate					( );
}

void graph_generator::change_path_find_object_radius	( pcstr const args )
{
	XRAY_SSCANF					( args, "%f", &m_path_finder_agent_radius );
	find_shortest_path			( true );
}

void graph_generator::change_merger_operation_id	( pcstr const args )
{
	XRAY_SSCANF					( args, "%d", &m_merger_max_operation_id );
	generate					( );
}

void graph_generator::generate_mesh	( pcstr const args )
{
	string_path					file_name;
	XRAY_SSCANF					( args, "%s", file_name );
	
	if ( m_model ) {
		m_input_triangles.destroy_spatial_tree();
		clear_resources						( );
//		collision::delete_geometry_instance	( g_allocator, m_geometry );
	}

	resources::query_resource	(
		file_name,
		resources::static_model_instance_class,
		boost::bind( &graph_generator::on_resources_loaded, this, _1 ),
		g_allocator
	);

}

void graph_generator::mark_triangle	( pcstr const args )
{
	u32 triangle_id = u32(-1);
	XRAY_SSCANF					( args, "%d", &triangle_id );
	if ( triangle_id < m_input_triangles.data.size() )
		m_input_triangles.data[ triangle_id ].is_marked = !m_input_triangles.data[ triangle_id ].is_marked;
}

void graph_generator::change_merge_delaunay_operation_id	( pcstr const args )
{
	XRAY_SSCANF					( args, "%d", &m_merge_delaunay_max_operation_id );
	clear_geometry				( );
	add_geometry				( m_model->m_collision_geom.c_ptr(), m_transform );
	generate					( );
}

void graph_generator::change_draw_range( pcstr const args )
{
	u32 input[2];
	u32 result = XRAY_SSCANF	( args, "%d %d", &input[0], &input[1] );
	if ( result < 1 ) {
		m_min_triangle_id_to_draw = 0;
		m_triangle_draw_range = u32(-1);
		return;
	}

	if ( result < 2 ) {
		m_min_triangle_id_to_draw = input[0];
		return;
	}


	m_min_triangle_id_to_draw = input[0];
	m_triangle_draw_range = input[1];

}

void graph_generator::change_show_processed_model ( pcstr const args )
{
//	if ( m_show_processed_model )
//		m_renderer.scene().remove_model( m_scene, m_model->m_render_model );

	int value;
	XRAY_SSCANF					( args, "%i", &value );
	m_show_processed_model = value ? true : false;

//	if ( m_show_processed_model )
//		m_renderer.scene().add_model( m_scene, m_model->m_render_model, m_transform);
}

void graph_generator::change_tessellation_operation_id	( pcstr const args )
{
	XRAY_SSCANF					( args, "%d", &m_tessellation_max_operation_id );
	clear_geometry				( );
	add_geometry				( m_model->m_collision_geom.c_ptr(), m_transform );
	generate					( );
}

void graph_generator::draw_vertex ( u32 vertex_id )
{
	m_renderer.draw_sphere		(m_scene, m_input_triangles.vertices[vertex_id], .01f, math::color(0, 0, 0));
}

void graph_generator::draw_triangle ( u32 triangle_id )
{
	float3& v0 = m_input_triangles.vertices[ m_input_triangles.indices[triangle_id*3]];
	float3& v1 = m_input_triangles.vertices[ m_input_triangles.indices[triangle_id*3+1]];
	float3& v2 = m_input_triangles.vertices[ m_input_triangles.indices[triangle_id*3+2]];
	
	m_renderer.draw_triangle	(m_scene, v0,v1,v2, math::color(   0, 255, 0, 128 ) );
	m_renderer.draw_triangle	(m_scene, v1,v0,v2, math::color( 255,   0, 0, 128 ) );

	m_renderer.draw_sphere		(m_scene, v0, .1f, math::color(255,   0, 255));
	m_renderer.draw_sphere		(m_scene, v1, .1f, math::color(255, 255,   0));
	m_renderer.draw_sphere		(m_scene, v2, .1f, math::color(  0, 255, 255));

	m_renderer.draw_line		(m_scene, v0, v1, math::color(  0,   0, 255));
	m_renderer.draw_line		(m_scene, v1, v2, math::color(255, 255,   0));
	m_renderer.draw_line		(m_scene, v2, v0, math::color(  0, 255, 255));
}

using xray::math::float3;

int tri_tri_intersect_with_isectline(float3 const& V0, float3 const& V1, float3 const& V2,
				     float3 const& U0, float3 const& U1, float3 const& U2, int *coplanar,
				     float3& isectpt1, float3& isectpt2);

u32 segment_intersects_segment	(
		u32 const (&coordinate_indices)[2],
		float3 const& v0,
		float3 const& v1,
		float3 const& u0,
		float3 const& u1,
		float3& result0,
		float3& result1,
		bool& are_on_the_same_line
	);

void graph_generator::tick					( )
{
	/*
	if ( !m_model )
		return;
	*/

#if 0
	{
		draw_vertex( 114 );
		draw_vertex( 92 );

	}
#endif

	if ( m_show_triangle_id < m_input_triangles.data.size() ) {
		u32 const* indices = m_input_triangles.indices.begin() + m_show_triangle_id * 3;
		string256 str;
		sprintf ( str, "triangle_id: %i\nVertex indices:\n(%i, %i, %i)\nVertices:\n(%f, %f, %f)\n(%f, %f, %f)\n(%f, %f, %f)\nAdjacency:\n(%i, %i, %i)\nObstruction:\n(%f, %f, %f)\nis marked = %i", 
			m_show_triangle_id, 
			indices[0], 
			indices[1], 
			indices[2],
			m_input_triangles.vertices[ indices[0] ].x,
			m_input_triangles.vertices[ indices[0] ].y,
			m_input_triangles.vertices[ indices[0] ].z,

			m_input_triangles.vertices[ indices[1] ].x,
			m_input_triangles.vertices[ indices[1] ].y,
			m_input_triangles.vertices[ indices[1] ].z,

			m_input_triangles.vertices[ indices[2] ].x,
			m_input_triangles.vertices[ indices[2] ].y,
			m_input_triangles.vertices[ indices[2] ].z,

			m_input_triangles.data[ m_show_triangle_id ].neighbours[0],
			m_input_triangles.data[ m_show_triangle_id ].neighbours[1],
			m_input_triangles.data[ m_show_triangle_id ].neighbours[2],

			m_input_triangles.data[ m_show_triangle_id ].obstructions[0],
			m_input_triangles.data[ m_show_triangle_id ].obstructions[1],
			m_input_triangles.data[ m_show_triangle_id ].obstructions[2],

			m_input_triangles.data[ m_show_triangle_id ].is_marked ? 1 : 0
		);
		m_engine.set_navmesh_info( str );
		draw_triangle( m_show_triangle_id );
	} else {
		m_engine.set_navmesh_info( "triangle_id: -1" );
	}

	if ( m_show_point_id < m_input_triangles.vertices.size() ) {
		m_renderer.draw_sphere	( m_scene, m_input_triangles.vertices[m_show_point_id], .1f, math::color(255, 255, 255) );
		string256 str;
		sprintf ( str, "point_id: %i\nCoordinates:\n(%i, %i, %i)", 
			m_show_point_id,
			m_input_triangles.vertices[m_show_point_id].x,
			m_input_triangles.vertices[m_show_point_id].y,
			m_input_triangles.vertices[m_show_point_id].z
		);
		m_engine.set_navmesh_info( str );
	}

	/*
		+		[385]	{x=15.398192 y=3.0733371 z=39.866676}	xray::math::float3
		+		[386]	{x=15.090755 y=3.0733368 z=39.801331}	xray::math::float3
		+		[408]	{x=14.868805 y=3.0733371 z=39.754150}	xray::math::float3

	{
		
		float3 const& v0 = float3(15.398192, 3.0733371, 39.866676);
		float3 const& v1 = float3(15.090755, 3.0733368, 39.801331);
		float3 const& v2 = float3(14.868805, 3.0733371, 39.754150);
		
		float3 const& v0 = m_input_triangles.vertices[ 314 ];
		float3 const& v2 = m_input_triangles.vertices[ 313 ];
		float3 const& v1 = m_input_triangles.vertices[ 320 ];

		u32 coordinate_indices[2] = { 0, 2 };
		float3 segment_vertices[2];
		bool are_on_same_line;
		u32 const result_count = segment_intersects_segment(
			coordinate_indices,
			v0, v1,
			v2, v0,
			segment_vertices[0],
			segment_vertices[1],
			are_on_same_line
		);

		coordinate_indices[0] = 0;
	}

	u32 const debug_edges_count = m_debug_edges.size();
	for ( u32 i = 0; i < debug_edges_count; ++i ) {
		math::float3 v0				= m_input_triangles.vertices[ m_debug_edges[i].vertex_index0 ];
		math::float3 v1				= m_input_triangles.vertices[ m_debug_edges[i].vertex_index1 ];

		m_renderer.debug().draw_line ( m_scene, v0, v1, math::color( 0, 255, 0 ) );

	//	m_renderer.debug().draw_sphere ( m_scene, v0, .1f, math::color(255,   0, 255));
	//	m_renderer.debug().draw_sphere ( m_scene, v1, .1f, math::color(255,   0, 255));
	}
	*/

//	m_renderer.draw_sphere ( m_scene, m_start_position, m_path_finder_agent_radius, math::color( 255, 128, 128 ) );
//	m_renderer.draw_sphere ( m_scene, m_goal_position, m_path_finder_agent_radius, math::color( 128, 128, 255 ) );

	if ( m_show_path_channel ) {
		u32 channel_triangles_count = m_channel.size();
		for ( u32 i = 0 ; i < channel_triangles_count; ++i ) {
			math::float3 v0				= m_input_triangles.vertices[ m_input_triangles.indices[3*m_channel[i]] ] + float3( 0.0f, 0.001f, 0.0f );
			math::float3 v1				= m_input_triangles.vertices[ m_input_triangles.indices[3*m_channel[i] + 1] ] + float3( 0.0f, 0.001f, 0.0f );
			math::float3 v2				= m_input_triangles.vertices[ m_input_triangles.indices[3*m_channel[i] + 2] ] + float3( 0.0f, 0.001f, 0.0f );
			m_renderer.draw_triangle( m_scene, v0, v1, v2, math::color( 0, 0, 255 ) );
		}
	}

	u32 const path_vertices_count = m_path.size();
	for ( u32 i = 0; i < path_vertices_count; ++i ) {
		float3 const& vertex0 = m_path[ i ];
		m_renderer.draw_sphere( m_scene, vertex0, 0.01f, math::color( 255, 0, 0 ) );
		if ( i == 0 )
			continue;
		float3 const& vertex1 = m_path[ i - 1 ];
		m_renderer.draw_line ( m_scene, vertex0, vertex1, math::color( 255, 0, 0 ) );
	}
	/**/

	/**/
	triangles_type triangles(debug::g_mt_allocator);
	if ( m_input_triangles.spatial_tree == 0 )
		return;

	m_input_triangles.spatial_tree->cuboid_query( 0, m_frustum, triangles );

	if ( !m_show_navigation_mesh )
		return;

	//u32 max_triangle_id_to_draw			= m_min_triangle_id_to_draw + m_triangle_draw_range;
	triangles_type::iterator i = triangles.begin();
	triangles_type::iterator const e = triangles.end();
	for ( ; i != e; ++i ) {
		math::float3 v0				= m_input_triangles.vertices[ m_input_triangles.indices[3*(i->triangle_id)] ] + float3( 0.0f, 0.01f, 0.0f );
		math::float3 v1				= m_input_triangles.vertices[ m_input_triangles.indices[3*(i->triangle_id) + 1] ] + float3( 0.0f, 0.01f, 0.0f );
		math::float3 v2				= m_input_triangles.vertices[ m_input_triangles.indices[3*(i->triangle_id) + 2] ] + float3( 0.0f, 0.01f, 0.0f );
		
		if  ( m_input_triangles.data[ i->triangle_id ].is_marked && !m_show_marked_triangles )
			continue;

		if ( m_show_triangle_id == (i->triangle_id) )
			continue;
		
		if ( m_show_triangle_faces )
			m_renderer.draw_triangle( m_scene, v0, v1, v2, m_input_triangles.data[ (i->triangle_id) ].color );
		
		static const math::color obstruction_color(255, 0, 0);
		m_renderer.draw_line	( m_scene, v0, v1, m_input_triangles.data[(i->triangle_id)].is_obstructed(0) || m_input_triangles.data[(i->triangle_id)].neighbours[0] == u32(-1) ? obstruction_color : math::color( 0, 255, 0 ) );
		m_renderer.draw_line	( m_scene, v1, v2, m_input_triangles.data[(i->triangle_id)].is_obstructed(1) || m_input_triangles.data[(i->triangle_id)].neighbours[1] == u32(-1) ? obstruction_color : math::color( 0, 255, 0 ) );
		m_renderer.draw_line	( m_scene, v2, v0, m_input_triangles.data[(i->triangle_id)].is_obstructed(2) || m_input_triangles.data[(i->triangle_id)].neighbours[2] == u32(-1) ? obstruction_color : math::color( 0, 255, 0 ) );
		
	}

}

void graph_generator::add_geometry( 
		xray::collision::geometry* geometry, 
		xray::math::float4x4 const& transform 
	)
{
	u32 const index_count_old			= m_input_triangles.indices.size();
	u32 const index_count				= index_count_old + geometry->index_count( );
	R_ASSERT_CMP						( index_count % 3, ==, 0, "index count is not dividable by 3: %d", index_count );
	u32 const triangle_count			= index_count / 3;
	m_input_triangles.data.reserve					( 8*triangle_count );
	m_input_triangles.data.resize					( triangle_count );

	u32 const* const indices			= geometry->indices( );
	for (u32 i=0; i<triangle_count; ++i)
		R_ASSERT_CMP					( (geometry->indices(i) - indices) % 3, ==, 0 );

	m_input_triangles.indices.reserve				( 8*index_count );

	u32 const vertex_count_old			= m_input_triangles.vertices.size();
	u32 const vertex_count				= vertex_count_old + geometry->vertex_count();
	m_input_triangles.vertices.reserve				( 2*vertex_count );

	for ( u32 i = index_count_old; i < index_count; ++i )
		m_input_triangles.indices.push_back( indices[i-index_count_old] + vertex_count_old );

	float3 const* vertices = geometry->vertices();
	for ( u32 i = vertex_count_old; i < vertex_count; ++i ) {
		m_input_triangles.vertices.push_back( transform.transform ( vertices[i-vertex_count_old] ) );
	}
}

void graph_generator::add_geometry_triangle( 
		float3 const& v0,
		float3 const& v1,
		float3 const& v2
	)
{
	u32 const index_count				= m_input_triangles.indices.size();
	R_ASSERT_CMP						( index_count % 3, ==, 0, "index count is not dividable by 3: %d", index_count );
	u32 const triangle_count			= index_count / 3;
	m_input_triangles.data.resize					( triangle_count + 1 );
	u32 const vertices_count			= m_input_triangles.vertices.size();
	m_input_triangles.indices.push_back( vertices_count );
	m_input_triangles.indices.push_back( vertices_count + 1);
	m_input_triangles.indices.push_back( vertices_count + 2);

	R_ASSERT_CMP(m_input_triangles.indices.size() / 3, ==, m_input_triangles.data.size() );

	m_input_triangles.vertices.push_back( v0 );
	m_input_triangles.vertices.push_back( v1 );
	m_input_triangles.vertices.push_back( v2 );

}

void graph_generator::add_restricted_area( const cuboid_type cuboid )
{
	R_ASSERT_CMP( cuboid.size(), ==, 8 );
	m_restricted_areas.push_back( cuboid );
}

void graph_generator::clear_restricted_areas		( )
{
	m_restricted_areas.clear();
}

#endif // #if XRAY_DEBUG_ALLOCATOR

