////////////////////////////////////////////////////////////////////////////
//	Created		: 04.08.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GRAPH_GENERATOR_H_INCLUDED
#define GRAPH_GENERATOR_H_INCLUDED

#if XRAY_DEBUG_ALLOCATOR

#include <xray/render/facade/model.h>
#include <xray/collision/common_types.h>
#include <xray/ui/ui.h>
#include <xray/ui/sources/ui_window.h>
#include <xray/configs.h>
#include "triangles_mesh.h"
#include "navigation_mesh_types.h"
#include "path_finder_funnel.h"

namespace xray {

namespace collision {
	class geometry_instance;
} // namespace collision

namespace render {
namespace debug {

	class renderer;
	
} // namespace game
} // namespace render

namespace ai {
namespace navigation {

struct engine;

class XRAY_AI_NAVIGATION_API graph_generator : private core::noncopyable {
public:
					graph_generator		( 
						ai::navigation::engine& engine, 
						float4x4 const& transform,
						xray::render::scene_ptr const& scene,
						xray::render::debug::renderer& renderer,
						ai::navigation::triangles_mesh_type& triangles_mesh
					);

					~graph_generator			( );
			void	tick						( );
			void	clear_resources				( );
			void	generate					( );

			void	ray_mark_triangle			( float3 const& position, float3 const& direction );
			void	aabb_mark_triangle			( float3 const& position, float3 const& direction );
			void	set_frustum					( math::frustum const& frustum ) { m_frustum = frustum; }

			// Funnel pathfind algorithm tests
			void	set_start					( float3 const& position, float3 const& direction );
			void	set_goal					( float3 const& position, float3 const& direction );
			void	reset_channel				( );
			void	find_channel				( );		
			void	find_shortest_path			( bool build_channel );
			void	add_channel_triangle		( float3 const& position, float3 const& direction );
			bool	check_adjacency				( u32 const triangle_id0, u32 const triangle_id1 );

			void	add_geometry				( xray::collision::geometry* geometry, float4x4 const& transform );
			void	add_geometry_triangle		( float3 const& v0, float3 const& v1, float3 const& v2 );
			void	save_geometry				( pcstr filename );
			void	clear_geometry				( );

			typedef fixed_vector< float3, 8 >	cuboid_type;
			void	add_restricted_area			( const cuboid_type cuboid);
			void	clear_restricted_areas		( );

private:
			u32		pick_triangle_by_ray		( float3 const& position, float3 const& direction, float3& projected_vertex );

			void	on_resources_loaded			( resources::queries_result& resource );
			void	on_binary_config_resource	( resources::queries_result& query_result );
			void	transform_vertices			( );
			void	mark_walkable_triangles		( );

			void	draw_triangle				( u32 triangle_id );
			void	draw_vertex					( u32 vertex_id );
			
			void	test_delaunay				( );

private:
	typedef collision::triangles_type			triangles_type;
	typedef buffer_vector< collision::triangle_result >	buffer_triangles_type;
	typedef debug::vector< math::float3 >		vertices_type;

private:
	math::float4x4	const			m_transform;
	render::static_model_ptr		m_model;
	render::debug::renderer&		m_renderer;
	render::scene_ptr const&		m_scene;
	ai::navigation::engine&			m_engine;
	collision::geometry*			m_geometry;


	edges_type						m_debug_edges;
	
private:
	void	change_subdivision_operation_id		( pcstr const args );
	void	change_tessellation_operation_id	( pcstr const args );
	void	change_merger_operation_id			( pcstr const args );
	void	change_fuser_operation_id			( pcstr const args );
	void	change_merge_delaunay_operation_id	( pcstr const args );
	void	change_draw_range					( pcstr const args );
	void	change_path_find_object_radius		( pcstr const args );
	void	mark_triangle						( pcstr const args );
	void	generate_mesh						( pcstr const args );
	void	save_navigation_mesh_console_command( pcstr const args );
	void	load_navigation_mesh_console_command( pcstr const args );

public:
	void	change_show_navigation_mesh				( bool flag ) { m_show_navigation_mesh = flag; }
	void	change_show_triangle_faces				( bool flag ) { m_show_triangle_faces = flag; }
	void	change_show_processed_model				( pcstr const args );

	path_type							m_path;

private:
	bool	check_adjacency_to_last_channel_triangle( u32 const triangle_id );

	u32									m_start_node;
	u32									m_goal_node;
	float3								m_start_position;
	float3								m_goal_position;

	path_finder_funnel::channel_type	m_channel;

private:

	u32								m_subdivision_max_operation_id;
	u32								m_tessellation_max_operation_id;
	u32								m_merger_max_operation_id;
	u32								m_fuser_max_operation_id;
	u32								m_merge_delaunay_max_operation_id;
	u32								m_show_point_id;
	u32								m_show_triangle_id;
	u32								m_min_triangle_id_to_draw;
	u32								m_triangle_draw_range;
	float							m_path_finder_agent_radius;
	math::frustum					m_frustum;
	bool							m_show_processed_model;
	bool							m_show_triangle_faces;
	bool							m_show_navigation_mesh;
	bool							m_show_path_channel;
	bool							m_show_marked_triangles;
	vertices_type					m_transformed_vertices;
//	navigation_triangles_type		m_navigation_triangles;

//
// Navigation mesh building parameters
//
	float							m_min_agent_radius;
	float							m_min_agent_height;
	float							m_max_slope;
	float							m_max_fuse_distance;

///////
public:
	typedef buffer_vector< u32 >		buffer_indices_type;
	typedef debug::vector<cuboid_type>	restricted_areas_type;

	triangles_mesh_type&				m_input_triangles;
	restricted_areas_type				m_restricted_areas;

private:
	static	void		fill_triangles					( collision::geometry_instance* geometry, triangles_mesh_type& result, math::float4x4 transform );
	static	void		mark_passable_triangles			( triangles_mesh_type& result );
	static	void		remove_invalid_triangles		( triangles_mesh_type& result );
	static	void		remove_non_passable_triangles	( triangles_mesh_type& result );
	static	void		remove_disconnected_regions		( triangles_mesh_type& result );
	static	void		remove_duplicates				( triangles_mesh_type& triangles_mesh );
	static	void		remove_unused_vertices			( triangles_mesh_type& triangles_mesh );

	static	void		remove_restricted_areas			( triangles_mesh_type& triangles_mesh, restricted_areas_type& areas );
	static	void		remove_areas_in_cuboid			( triangles_mesh_type& triangles_mesh, triangles_type& triangles, cuboid_type& cuboid );

public:
	static	void		save_navigation_mesh		( triangles_mesh_type& navigation_mesh, pcstr file_name );
	static	void		load_navigation_mesh		( triangles_mesh_type& navigation_mesh, configs::binary_config_ptr config );

private:
	static	bool		check_edge				( float3 const& vertex0, float3 const& vertex1, float3& point, float const epsilon );
}; // class graph_generator

} // namespace navigation
} // namespace ai
} // namespace xray

#endif // #ifndef MASTER_GOLD

#endif // #if XRAY_DEBUG_ALLOCATOR
