////////////////////////////////////////////////////////////////////////////
//	Created		: 09.09.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef TRIANGLES_MESH_UTILITIES_H_INCLUDED
#define TRIANGLES_MESH_UTILITIES_H_INCLUDED
#include "bsp_tree_triangle.h"
#include <xray/math_convex.h>
#include <xray/geometry_utils.h>
#include "bsp_typedefs.h"
namespace xray {
namespace model_editor {

struct adjacent_coplanar_triangles
{
private:
	typedef set<u32> triangle_ids_set_type;
public:
	adjacent_coplanar_triangles( u32 triangle_id, triangles_mesh_type const& mesh, triangle_ids_type& result )
	{
		triangle_ids_set_type coincident_set;
		adjacent_coplanar_triangles_impl( triangle_id, mesh, coincident_set );
		result.insert( result.end(), coincident_set.begin(), coincident_set.end() );
	}
private:
	void adjacent_coplanar_triangles_impl( u32 triangle_id, triangles_mesh_type const& mesh, triangle_ids_set_type& result_set )
	{
		R_ASSERT( is_triangle_adjacency_correct( triangle_id, mesh ) );
		R_ASSERT( !mesh.data[ triangle_id ].has_children() );
		result_set.insert( triangle_id );
		bsp_tree_triangle const& current = mesh.data[ triangle_id ];
		for ( u32 i = 0; i < bsp_tree_triangle::msc_neihbours_count; ++i )
		{
			u32 const neighbour_id = current.neighbours[ i ];
			if ( neighbour_id == bsp_tree_triangle::msc_empty_id || result_set.find( neighbour_id ) != result_set.end() )
				continue;
			adjacent_coplanar_triangles_impl( neighbour_id, mesh, result_set );
		}
	}
};


void initialize_triangles_mesh( math::float3 const* vertices, u32 vertex_count, u32 const* indices, u32 index_count, triangles_mesh_type& mesh );

void get_outer_edges( triangles_mesh_type const& triangle_mesh, triangle_ids_type const& triangles, edges_type& result );
void get_vertex_indices_of_edges( edges_type const& edges, input_indices_type& indices );

void merge_consecutive_edges				( coord_indices_pair const& coordinate_indices, triangles_mesh_type::vertices_type const& vertices, edges_type& edges );
void remove_zero_edges						( coord_indices_pair const& coordinate_indices, triangles_mesh_type::vertices_type const& vertices, edges_type& edges, float epsilon );
void remove_close_contrdirectional_edges	( coord_indices_pair const& coordinate_indices, triangles_mesh_type::vertices_type const& vertices, edges_type& edges );
void fix_reciprocal_intersections			( coord_indices_pair const& coordinate_indices, triangles_mesh_type::vertices_type& vertices, edges_type& edges );
void remove_edges_inside_edges				( coord_indices_pair const& coordinate_indices, triangles_mesh_type::vertices_type const& vertices, edges_type& edges );
bool edge_inside_edge						( coord_indices_pair const& coordinate_indices, triangles_mesh_type::vertices_type const& vertices, edge const& testee, edge const& bounding_edge );
void make_vertices_of_edges_unique_in_2d	( coord_indices_pair const& coordinate_indices, triangles_mesh_type::vertices_type const& vertices, edges_type& edges, float epsilon );


void remove_empty_triangles					( triangles_mesh_type const& mesh, triangle_ids_type& triangles, float square_threshold = math::epsilon_6 );
void replace_triangles_with_their_children	( triangles_mesh_type const& mesh, triangle_ids_type& triangles );
void remove_adjacency_in_coplanar_edges		( u32 triangle_id, math::plane const& divider, triangles_mesh_type& mesh );

bool is_collinear							( coord_indices_pair const& coordinate_indices, 
											 math::float3 const& v0,  math::float3 const& v1, 
											 math::float3 const& u0,  math::float3 const& u1, 
											 float precision = math::epsilon_5 );
bool is_parallel							( coord_indices_pair const& coordinate_indices, 
											 math::float3 const& v0,  math::float3 const& v1, 
											 math::float3 const& u0,  math::float3 const& u1, 
											 float precision = math::epsilon_5 );
bool is_codirectional						( coord_indices_pair const& coordinate_indices, 
											 math::float3 const& v0,  math::float3 const& v1, 
											 math::float3 const& u0,  math::float3 const& u1, 
											 float precision = math::epsilon_5 );
math::float3 get_triangles_mass_center		( triangle_ids_type const& triangles, triangles_mesh_type const& mesh );
void save_to_lua( triangles_mesh_type::vertices_type& vertices, input_indices_type const& indices, edges_type const& edges, math::float3 const& normal );

	
void collect_surfaces( System::String^ model_name, string_list^ surfaces, xray::geometry_utils::geometry_collector* );

} // namespace model_editor
} // namespace xray

#endif // #ifndef TRIANGLES_MESH_UTILITIES_H_INCLUDED
