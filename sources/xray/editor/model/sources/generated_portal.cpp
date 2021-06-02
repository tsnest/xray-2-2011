////////////////////////////////////////////////////////////////////////////
//	Created		: 12.09.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "generated_portal.h"
#include "triangles_mesh_utilities.h"
#include "bsp_tree_utilities.h"
#pragma managed(push, off)
#include <xray/render/facade/debug_renderer.h>
#pragma managed(pop)

namespace xray {
namespace model_editor {
struct is_empty_triangle : public std::unary_function<triangle_ids_type::value_type, bool>
{
	is_empty_triangle( triangles_mesh_type const& mesh ):m_mesh( &mesh ){}
	bool operator()( triangle_ids_type::value_type triangle_id )
	{
		triangles_mesh_type::indices_type::value_type const first	= m_mesh->indices[ triangle_id * 3 ];
		triangles_mesh_type::indices_type::value_type const second	= m_mesh->indices[ triangle_id * 3 + 1 ];
		triangles_mesh_type::indices_type::value_type const third	= m_mesh->indices[ triangle_id * 3 + 2 ];
		return first == second || second == third || third == first;
	}
private:
	triangles_mesh_type const* m_mesh;
};

generated_portal::generated_portal( triangles_mesh_type const& mesh, triangle_ids_type const& triangles ):
m_mesh( &mesh ),
m_triangles( triangles ),
m_square( 0.0f ),
m_slimness( 0.0f )
{
	this->update_square_slimness_and_outer_edges();
}

void generated_portal::update_square_slimness_and_outer_edges()
{
	m_square	= 0.0f;
	m_slimness	= 0.0f;
	m_outer_edges.clear();
	if ( m_triangles.empty() )
		return;
	float perimeter = 0.0f;
	triangle_ids_type::const_iterator const end_it = m_triangles.end();
	for ( triangle_ids_type::const_iterator it = m_triangles.begin(); it != end_it; ++it )
	{
		u32 const triangle_id		= *it;
		u32 const first_index		= m_mesh->indices[ triangle_id * 3 ];
		u32 const second_index		= m_mesh->indices[ triangle_id * 3 + 1 ];
		u32 const third_index		= m_mesh->indices[ triangle_id * 3 + 2 ];
		math::float3 const& first	= m_mesh->vertices[ first_index ];
		math::float3 const& second	= m_mesh->vertices[ second_index ];
		math::float3 const& third	= m_mesh->vertices[ third_index ];
		m_square += triangle_square( first, second, third );
		if ( m_mesh->data[ triangle_id ].neighbours[ 0 ] == bsp_tree_triangle::msc_empty_id )
		{
			m_outer_edges.push_back( edge( first_index, second_index ) );
			perimeter += ( second - first ).length();
		}
		if ( m_mesh->data[ triangle_id ].neighbours[ 1 ] == bsp_tree_triangle::msc_empty_id )
		{
			m_outer_edges.push_back( edge( second_index, third_index ) );
			perimeter += ( third - second ).length();
		}
		if ( m_mesh->data[ triangle_id ].neighbours[ 2 ] == bsp_tree_triangle::msc_empty_id )
		{
			m_outer_edges.push_back( edge( third_index, first_index ) );
			perimeter += ( first - third ).length();
		}
	}
	coord_indices_pair coordinate_indices;
	xray::ai::navigation::fill_coordinate_indices( this->get_plane().normal, coordinate_indices );
	merge_consecutive_edges( coordinate_indices, m_mesh->vertices, m_outer_edges );
	if ( !math::is_zero( perimeter ) )
		m_slimness = m_square / perimeter;
	R_ASSERT( !math::is_zero( perimeter ) );
}

//void generated_portal::check_hanged ( edges_buffer_type const& edges, indices_buffer_type const& indices, edges_buffer_type const& edges_by_finish  )
//{
//	m_hanged_flags.clear();
//	float hanged_length = 0.0f;
//	float perimeter = 0.0f;
//	edges_type::const_iterator const edge_end = m_outer_edges.end();
//	for ( edges_type::const_iterator edge_it = m_outer_edges.begin(); edge_it != edge_end; ++edge_it )
//	{
//		math::float3 const& edge_vector = m_mesh->vertices[ edge_it->vertex_index1 ] - m_mesh->vertices[ edge_it->vertex_index0 ];
//		float const edge_len = edge_vector.length();
//		perimeter += edge_len;
//		if ( std::binary_search( edges.begin(), edges.end(), *edge_it, edge_less() ) || math::is_zero( edge_len ) )
//		{
//			m_hanged_flags.push_back( false );
//			continue;
//		}
//		math::float3 normal = edge_vector / edge_len;
//
//		bool const indices_found = 
//			std::binary_search( indices.begin(), indices.end(), edge_it->vertex_index0 ) &&
//		 	std::binary_search( indices.begin(), indices.end(), edge_it->vertex_index1 );
//		bool const start_codirectional_found =
//			find_codirectional_edge( edges, edge_it->vertex_index0, 0, normal ) ||
//			find_codirectional_edge( edges_by_finish, edge_it->vertex_index0, 1, -normal );
//		bool const finish_codirectional_found	= 
//			find_codirectional_edge( edges, edge_it->vertex_index1, 0, -normal ) ||
//			find_codirectional_edge( edges_by_finish, edge_it->vertex_index1, 1, normal );
//		if ( indices_found && ( start_codirectional_found || finish_codirectional_found ) )
//		{
//			m_hanged_flags.push_back( false );
//		}
//		else
//		{
//			m_hanged_flags.push_back( true );
//			hanged_length += edge_len;
//		}
//	}
//	if ( !math::is_zero( perimeter ) )
//		m_rel_hanged_length = hanged_length / perimeter;
//}
//
//bool generated_portal::find_codirectional_edge( edges_buffer_type const& edges, u32 start_index, u32 compare_by_index, math::float3 const& direction ) const
//{
//	edge edge_to_search( bsp_tree_triangle::msc_empty_id , bsp_tree_triangle::msc_empty_id );
//	edge_to_search.indices[ compare_by_index ] = start_index;
//
//	typedef std::pair<edges_buffer_type::const_iterator, edges_buffer_type::const_iterator> pair_of_edge_it;
//	pair_of_edge_it const edge_range = std::equal_range( 
//		edges.begin(), edges.end(), 
//		edge_to_search, 
//		edge_less_by_index( compare_by_index ) );
//	if ( edge_range.first != edge_range.second )
//	{
//		for ( edges_buffer_type::const_iterator by_start_it = edge_range.first; by_start_it != edge_range.second; ++by_start_it )
//		{
//			math::float3 const& current = ( m_mesh->vertices[ by_start_it->vertex_index1 ] - m_mesh->vertices[ by_start_it->vertex_index0 ] );
//			float const len			= current.length();
//			float const dp			= direction | current;
//			if ( dp > 0.0f && math::is_similar( len, dp, math::epsilon_3 ) )
//				return true;
//		}
//	}
//	return false;
//}
//
void generated_portal::render( render::scene_ptr const& scene, render::debug::renderer& r ) const
{
	math::color const line_color	( 128, 128, 200 );
	math::color const plane_color	( 128, 128, 200, 128 );

	triangle_ids_type::const_iterator const end_id = m_triangles.end();
	for ( triangle_ids_type::const_iterator id_it = m_triangles.begin(); id_it != end_id; ++id_it )
	{
		u32 const triangle_id		= *id_it;
		math::float3 const& first	= m_mesh->vertices[ m_mesh->indices[ triangle_id * 3 ] ];
		math::float3 const& second	= m_mesh->vertices[ m_mesh->indices[ triangle_id * 3 + 1 ] ];
		math::float3 const& third	= m_mesh->vertices[ m_mesh->indices[ triangle_id * 3 + 2 ] ];
		r.draw_triangle( scene, first, second, third, plane_color );
		r.draw_triangle( scene, first, third, second, plane_color );
	}
	math::color const edge_color( 128, 128, 200 );
	u32 const edges_count = m_outer_edges.size();
	for ( u32 i = 0; i < edges_count; ++i )
	{
		r.draw_line( scene, m_mesh->vertices[ m_outer_edges[i].vertex_index0 ], m_mesh->vertices[ m_outer_edges[i].vertex_index1 ], edge_color );
	}
}

void extract_portals_from_mesh( triangles_mesh_type& mesh, triangle_ids_type const& triangles, generated_portals_type& portals )
{
	static float const c_min_square_threshold	= 1e-4f;
	static float const c_min_slimness_threshold	= 5e-3f;
	triangle_ids_type::const_iterator const id_end = triangles.end();
	for ( triangle_ids_type::const_iterator id_it = triangles.begin(); id_it != id_end; ++id_it )
	{
		u32 const triangle_id = *id_it;
		if ( mesh.data[ triangle_id ].portal_id != bsp_tree_triangle::msc_empty_id )
			continue;
		triangle_ids_type adjacent_triangles;
		//simplified_adjacent_coincident_triangles( triangle_id, mesh, adjacent_triangles );
		adjacent_coplanar_triangles( triangle_id, mesh, adjacent_triangles );
		u32 const portal_id = portals.size();
		triangle_ids_type::const_iterator const adj_end = adjacent_triangles.end();
		for ( triangle_ids_type::const_iterator adj_it = adjacent_triangles.begin(); adj_it != adj_end; ++adj_it )
		{
			mesh.data[ *adj_it ].portal_id = portal_id;
		}
		adjacent_triangles.erase( 
			std::remove_if( adjacent_triangles.begin(), adjacent_triangles.end(), is_empty_triangle( mesh ) ),
			adjacent_triangles.end()
		);
		generated_portal portal( mesh, adjacent_triangles );
		if ( portal.get_square() > c_min_square_threshold && portal.get_slimness() > c_min_slimness_threshold )
			portals.push_back( portal );
	}
}

sector_generator_portal generated_portal::to_sector_generator_portal() const
{
	triangles_mesh_type::indices_type indices_of_triangles;
	indices_of_triangles.reserve( m_triangles.size() * 3 );
	triangle_ids_type::const_iterator const trianles_end = m_triangles.end();
	for ( triangle_ids_type::const_iterator it = m_triangles.begin(); it != trianles_end; ++it )
	{
		indices_of_triangles.insert( indices_of_triangles.end(), m_mesh->indices.begin() + ( *it ) * 3, m_mesh->indices.begin() + ( *it ) * 3 + 3 );
	}
	std::sort( indices_of_triangles.begin(), indices_of_triangles.end() );
	indices_of_triangles.erase(
		std::unique( indices_of_triangles.begin(), indices_of_triangles.end() ),
		indices_of_triangles.end()
	);
	typedef associative_vector<u32, u32, vector> old_to_new_index_type;
	old_to_new_index_type old_to_new_index;

	sector_generator_portal::vertices_type vertices;
	triangles_mesh_type::indices_type::const_iterator const indices_end = indices_of_triangles.end();
	for ( triangles_mesh_type::indices_type::const_iterator it = indices_of_triangles.begin(); it != indices_end; ++it )
	{
		old_to_new_index[ *it ] = vertices.size();
		vertices.push_back( m_mesh->vertices[ *it ] );
	}

	sector_generator_portal::indices_type indices;
	indices.reserve( m_triangles.size() * 3 );
	for ( triangle_ids_type::const_iterator it = m_triangles.begin(); it != trianles_end; ++it )
	{
		u32 const first_index	= m_mesh->indices[ (*it) * 3 ];
		u32 const second_index	= m_mesh->indices[ (*it) * 3 + 1 ];
		u32 const third_index	= m_mesh->indices[ (*it) * 3 + 2 ];
		indices.push_back( old_to_new_index[ first_index ] );
		indices.push_back( old_to_new_index[ second_index ] );
		indices.push_back( old_to_new_index[ third_index ] );
	}
	return sector_generator_portal( vertices, indices, this->get_plane() );
}


} // namespace model_editor
} // namespace xray