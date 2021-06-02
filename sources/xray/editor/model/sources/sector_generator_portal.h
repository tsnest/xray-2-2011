////////////////////////////////////////////////////////////////////////////
//	Created		: 06.10.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SECTOR_GENERATOR_PORTAL_H_INCLUDED
#define SECTOR_GENERATOR_PORTAL_H_INCLUDED
#include "bsp_tree_triangle.h"
namespace xray {
namespace model_editor {

class sector_generator_portal {
public:
	typedef u32 sectors_type[2];
	typedef std::pair<math::float3, math::float3> outer_edge;
	typedef debug::vector<outer_edge> outer_edges_type;
	typedef triangles_mesh_type::vertices_type vertices_type;
	typedef triangles_mesh_type::indices_type indices_type;
public:
							sector_generator_portal	();
							sector_generator_portal	( vertices_type const&	vertices, indices_type const& indices, math::plane const& p );
	math::plane const&		get_plane				() const { return m_plane; }
	void					add_sector				( u32 sector_id );
	sectors_type const&		get_sectors				() const { return m_sectors; }
	outer_edges_type		get_outer_edges			() const;
	void					set_id					( u32 id ) { m_id = id; }
	u32						get_id					() const { return m_id; }
	void					render					( render::scene_ptr const& scene, render::debug::renderer& r ) const;
	void					test_if_hanged			( triangles_mesh_type const& mesh );
	bool					is_hanged				() const { return m_hanged_flags != 0; }
	float					get_square				() const { return m_square; }
	bool					is_edge_hanged			( u32 edge_id ) const;
	void					try_to_make_seal		( triangles_mesh_type const& mesh );
private:
	void					initialize_outer_edges	();
	bool					try_to_fix_hanged_edge	( u32 edge_index, triangles_mesh_type const& mesh );
	bool					try_group_of_triangle	( collision::triangles_type::const_iterator triangles_begin,
													  collision::triangles_type::const_iterator triangles_end, 
													  triangles_mesh_type const& mesh, 
													  math::float3 const& first_point, math::float3 const& first_dir,
													  math::float3 const& second_point, math::float3 const& second_dir,
													  float max_distance ) const;
	void					prepare_triangles		( collision::triangles_type& triangles, triangles_mesh_type const& mesh,
													  math::float3 const& first_point, math::float3 const& second_point, 
													  math::float4x4 const& matr ) const;
	bool					test_if_edge_hanged_with_rays	( u32 edge_id, triangles_mesh_type const& mesh );
	bool					test_if_edge_hanged_with_coplanar_edges( u32 edge_id, collision::triangles_type const& triangles, triangles_mesh_type const& mesh );
	
	vertices_type			m_vertices;
	indices_type			m_indices;
	math::plane				m_plane;
	sectors_type			m_sectors;
	edges_type				m_outer_edges;
	u32						m_id;
	bool					m_broken;
	bool					m_hanged;
	float					m_square;
	u32						m_hanged_flags;
	////----------------------------------------------------
	//typedef debug::vector<math::float4x4> matrices_type;
	//matrices_type			m_matrices_for_hanged_edges;
	//struct test_point
	//{
	//	test_point( bool bhanged, math::float3 const& position, math::float3 const& ray_dir, float max_dist )
	//		:hanged( bhanged ), pos( position ), ray_end( pos + ray_dir * max_dist ){}
	//	bool hanged;
	//	math::float3 pos;
	//	math::float3 ray_end;
	//};
	//typedef debug::vector<test_point> test_points_type;
	//test_points_type			m_test_points;
	//triangle_ids_type			m_adjacent_triangles;
	//triangles_mesh_type const*	m_mesh;
}; // class sector_generator_portal

math::cuboid create_cuboid_from_edge( 
				math::float3 const& first, 
				math::float3 const& second, 
				math::float3 const& normal, 
				math::float4x4& m, 
				math::float3 const& pos_ext = math::float3( -math::epsilon_3, math::epsilon_3, math::epsilon_3 ),
				math::float3 const& neg_ext = math::float3( -math::epsilon_3, math::epsilon_3, math::epsilon_3 )
);

bool segment_intersects_cuboid( math::float3 const& point1, math::float3 const& point2, math::cuboid const& c );


struct triangle_intersects_aabb : public std::unary_function<xray::collision::triangle_result const&, bool>
{
	triangle_intersects_aabb( triangles_mesh_type const& mesh, math::float4x4 const& matrix )
		:m_mesh( &mesh ), m_matrix( matrix ){}
	bool operator()( xray::collision::triangle_result const& triangle ) const
	{
		u32 const triangle_id				= triangle.triangle_id;
		math::float3 const& first_point		= m_mesh->vertices[ m_mesh->indices[ triangle_id * 3     ] ];
		math::float3 const& second_point	= m_mesh->vertices[ m_mesh->indices[ triangle_id * 3 + 1 ] ];
		math::float3 const& third_point		= m_mesh->vertices[ m_mesh->indices[ triangle_id * 3 + 2 ] ];

		math::float3 const tri_vertices[3] = { 
			m_matrix.transform_position( first_point ),
			m_matrix.transform_position( second_point ),
			m_matrix.transform_position( third_point )
		};
		math::float3 const c_aabb_center	= math::float3( 0.0f, 0.0f, 0.0f );
		math::float3 const c_aabb_extents	= math::float3( 0.5f, 0.5f, 0.5f );
		int triBoxOverlap(float3 const& boxcenter, float3 const& boxhalfsize, float3 const (&triverts)[3] );//defined in "xray\collision\sources\colliders_aabb_geometry_triangle_intersection.cpp"
		return triBoxOverlap( c_aabb_center, c_aabb_extents, tri_vertices  ) != 0;
	}
private:
	triangles_mesh_type const*	m_mesh;
	math::float4x4				m_matrix;
};

} // namespace model_editor
} // namespace xray

#endif // #ifndef SECTOR_GENERATOR_PORTAL_H_INCLUDED