////////////////////////////////////////////////////////////////////////////
//	Created		: 09.09.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "triangles_mesh_utilities.h"
#include "bsp_tree_utilities.h"
#include "edge_utilities.h"

#include "xray/ai_navigation/sources/delaunay_triangulator.h"
#include <xray/render/engine/model_format.h>

namespace xray {
namespace model_editor {

void get_outer_edges( triangles_mesh_type const& mesh, triangle_ids_type const& triangles, edges_type& result )
{
	edge_to_u32_type edge_count;
	triangle_ids_type::const_iterator const triangles_end = triangles.end();
	for ( triangle_ids_type::const_iterator it = triangles.begin(); it != triangles_end; ++it )
	{
		u32 const triangle_id = *it;
		u32 const first_index	=	mesh.indices[ triangle_id * 3 ];
		u32 const second_index	=	mesh.indices[ triangle_id * 3 + 1 ];
		u32 const third_index	=	mesh.indices[ triangle_id * 3 + 2 ];

		++edge_count[ edge( first_index,second_index ) ];
		++edge_count[ edge( second_index, third_index ) ];
		++edge_count[ edge( third_index, first_index ) ];
	}
	edge_to_u32_type::const_iterator const edge_count_end = edge_count.end();
	for ( edge_to_u32_type::const_iterator it = edge_count.begin(); it != edge_count_end; ++it )
	{
		if ( it->second == 1 )
		{
			edge const& e = it->first;
			result.push_back( edge( e.vertex_index0, e.vertex_index1 ) );
		}
	}

}

void get_vertex_indices_of_edges( edges_type const& edges, input_indices_type& indices )
{
	indices.reserve( edges.size() * 2 );
	edges_type::const_iterator const end_it = edges.end();
	for ( edges_type::const_iterator it = edges.begin(); it != end_it; ++it )
	{
		indices.push_back( it->vertex_index0 );
		indices.push_back( it->vertex_index1 );
	}
	std::sort( indices.begin(), indices.end() );
	indices.erase( std::unique( indices.begin(), indices.end() ), indices.end() );
}

void merge_consecutive_edges( coord_indices_pair const& coordinate_indices, triangles_mesh_type::vertices_type const& vertices, edges_type& edges )
{
	const u32 c_empty_index = u32( -1 );
	typedef associative_vector<u32, u32, debug::vector, std::less<u32> > u32_to_u32;
	u32_to_u32 index_use_count;
	edges_type::iterator const end_it = edges.end();
	for ( edges_type::iterator it = edges.begin(); it != end_it; ++it )
	{
		++index_use_count[ it->vertex_index0 ];
		++index_use_count[ it->vertex_index1 ];
	}

	for ( edges_type::iterator outer_it = edges.begin(); outer_it != end_it; ++outer_it )
	{
		if ( outer_it->vertex_index0 == c_empty_index )
			continue;
		for ( edges_type::iterator inner_it = edges.begin(); inner_it != end_it; ++inner_it )
		{
			if ( &*outer_it == &*inner_it || inner_it->vertex_index0 == c_empty_index )
				continue;
			if ( outer_it->vertex_index1 == inner_it->vertex_index0 && index_use_count[ outer_it->vertex_index1 ] == 2 )
				if ( is_on_segment( coordinate_indices, 
					vertices[ outer_it->vertex_index1 ], 
					vertices[ outer_it->vertex_index0 ],  
					vertices[ inner_it->vertex_index1 ], 
					math::epsilon_5 ) )
				{
					outer_it->vertex_index1 = inner_it->vertex_index1;
					inner_it->vertex_index0 = c_empty_index;
					inner_it->vertex_index1 = c_empty_index;
				}
		}
	}
	edges.erase(
		std::remove( edges.begin(), edges.end(), edges_type::value_type( c_empty_index, c_empty_index ) ),
		edges.end()
	);
}

void initialize_triangles_mesh(  math::float3 const* vertices, u32 vertex_count, u32 const* indices, u32 index_count, triangles_mesh_type& mesh )
{
	R_ASSERT( index_count % 3 == 0 );
	
	u32 const initial_triangle_count = index_count / 3;
	mesh.indices.reserve( index_count );
	for ( u32 i = 0; i < initial_triangle_count; ++i )
	{
		math::float3 const& first	= vertices[ indices[ i * 3 ] ];
		math::float3 const& second	= vertices[ indices[ i * 3 + 1 ] ];
		math::float3 const& third	= vertices[ indices[ i * 3 + 2 ] ];
		if ( math::is_zero( triangle_square( first, second, third ) ) )
			continue;
		mesh.indices.push_back( indices[ i * 3 ] );
		mesh.indices.push_back( indices[ i * 3 + 1 ] );
		mesh.indices.push_back( indices[ i * 3 + 2 ] );
	}
	//mesh.indices.resize( index_count );
	//std::copy( indices, indices + index_count, mesh.indices.begin() );

	mesh.vertices.resize( vertex_count );
	std::copy( vertices, vertices + vertex_count, mesh.vertices.begin() );
	
	mesh.data.resize( mesh.indices.size() / 3 );
	remove_duplicates( mesh );
	
	u32 index = 0;
	triangles_mesh_type::data_type::iterator const triangle_end_it = mesh.data.end();
	for ( triangles_mesh_type::data_type::iterator it = mesh.data.begin(); it != triangle_end_it; ++it )
	{
		math::float3 const& first	= mesh.vertices[ mesh.indices[ index++ ] ];
		math::float3 const& second	= mesh.vertices[ mesh.indices[ index++ ] ];
		math::float3 const& third	= mesh.vertices[ mesh.indices[ index++ ] ];
		it->plane = math::create_plane( first, second, third );
		it->square = triangle_square( first, second, third );
		R_ASSERT( !math::is_zero( it->square ) );
	}
	mesh.build_spatial_tree();
}


void remove_zero_edges( coord_indices_pair const& coordinate_indices, triangles_mesh_type::vertices_type const& vertices, edges_type& edges, float epsilon )
{
	struct is_zero_edge : public std::unary_function<edge const&, bool>
	{
		is_zero_edge( triangles_mesh_type::vertices_type const& vertices, coord_indices_pair const& coordinate_indices, float epsilon  ): 
		m_vertices( &vertices ), m_coordinate_indices( &coordinate_indices ), m_epsilon( epsilon )
		{}
		bool operator()( edge const& e )
		{
			if ( e.vertex_index0 == e.vertex_index1 ) 
				return true;
			if ( is_similar( *m_coordinate_indices,( *m_vertices )[ e.vertex_index0 ], ( *m_vertices )[ e.vertex_index1 ], m_epsilon ) )
				return true;
			return false;
		}
	private:
		 triangles_mesh_type::vertices_type const*	m_vertices;
		 coord_indices_pair const*					m_coordinate_indices;
		 float										m_epsilon;
	};
	edges.erase( 
		std::remove_if( edges.begin(), edges.end(), is_zero_edge( vertices, coordinate_indices, epsilon ) ), 
		edges.end() 
	);
}

void remove_close_contrdirectional_edges ( coord_indices_pair const& coordinate_indices, triangles_mesh_type::vertices_type const& vertices, edges_type& edges )
{
	//TODO: this function in some cases can remove "good" edges. It has to be debugged. 
	u32 const edges_count = edges.size();
	bool* flags = static_cast<bool*>( ALLOCA( sizeof( bool ) * edges_count ) );
	memset( flags, 0, sizeof( bool ) * edges_count );
	for ( u32 outer = 0; outer < edges_count; ++outer )
	{
		if ( flags[ outer ] )
			continue;
		for ( u32 inner = 0; inner < edges_count; ++inner )
		{
			if ( flags[ inner ] || inner == outer )
				continue;
			edge const& outer_edge = edges[ outer ];
			edge const& inner_edge = edges[ inner ];
			u32 third = u32( -1 );
			if ( outer_edge.vertex_index1 != inner_edge.vertex_index0 )
				continue;
			third = inner_edge.vertex_index1;
			if ( is_on_segment( coordinate_indices, vertices[ outer_edge.vertex_index1 ], vertices[ outer_edge.vertex_index0 ], vertices[ third ] ) )
				continue;
			float const square = cross_product( 
				coordinate_indices,	
				vertices[ third ] - vertices[ outer_edge.vertex_index0 ],
				vertices[ third ] - vertices[ outer_edge.vertex_index1 ]
			);
			float const dp		= dot_product( 
				coordinate_indices,	
				vertices[ third ] - vertices[ outer_edge.vertex_index0 ],
				vertices[ third ] - vertices[ outer_edge.vertex_index1 ]
			);
			if ( math::is_zero( square ) && dp < 0.0f )
			{
				flags[ outer ] = true;
				flags[ inner ] = true;
			}
		}
	}
	edges_type result;
	result.reserve( edges_count );
	for ( u32 i = 0; i < edges_count; ++i )
	{
		if ( !flags[ i ] )
			result.push_back( edges[ i ] );
	}
	std::swap( edges, result );
}

void fix_reciprocal_intersections( coord_indices_pair const& coordinate_indices, triangles_mesh_type::vertices_type& vertices, edges_type& edges )
{
	u32 const edge_count = edges.size();
	for ( u32 outer = 0; outer < edges.size(); ++outer )
	{
		for ( u32 inner = 0; inner < edges.size(); ++inner )
		{
			if ( outer == inner )
				continue;
			if ( edges[ inner ].vertex_index0 == edges[ outer ].vertex_index0 || 
				edges[ inner ].vertex_index0 == edges[ outer ].vertex_index1 || 
				edges[ inner ].vertex_index1 == edges[ outer ].vertex_index0 || 
				edges[ inner ].vertex_index1 == edges[ outer ].vertex_index1 ||
				//is_similar( coordinate_indices, vertices[ edges[ inner ].vertex_index0 ], vertices[ edges[ outer ].vertex_index0 ] ) ||
				//is_similar( coordinate_indices, vertices[ edges[ inner ].vertex_index0 ], vertices[ edges[ outer ].vertex_index1 ] ) ||
				is_similar( coordinate_indices, vertices[ edges[ inner ].vertex_index1 ], vertices[ edges[ outer ].vertex_index0 ] ) ||
				is_similar( coordinate_indices, vertices[ edges[ inner ].vertex_index1 ], vertices[ edges[ outer ].vertex_index1 ] ) || 
				is_similar( coordinate_indices, vertices[ edges[ inner ].vertex_index0 ], vertices[ edges[ inner ].vertex_index1 ] ) || 
				is_similar( coordinate_indices, vertices[ edges[ outer ].vertex_index0 ], vertices[ edges[ outer ].vertex_index1 ] ) )
				continue;
			math::float3 result0, result1;
			bool on_the_same_line = false;
			u32 const intersection_count = xray::ai::navigation::segment_intersects_segment( 
											 coordinate_indices,
											 vertices[ edges[ inner ].vertex_index0 ],
											 vertices[ edges[ inner ].vertex_index1 ],
											 vertices[ edges[ outer ].vertex_index0 ],
											 vertices[ edges[ outer ].vertex_index1 ],
											 result0, result1, on_the_same_line	
			);
			
			if ( intersection_count == 2 )
			{
				R_ASSERT( !math::is_infinity( result0.x ) && !math::is_infinity( result0.y ) && !math::is_infinity( result0.z ) );
				R_ASSERT( !math::is_infinity( result1.x ) && !math::is_infinity( result1.y ) && !math::is_infinity( result1.z ) );
				R_ASSERT( !math::is_similar( result0, result1 ) );
				
				u32 first_result_index = (u32)( -1 );
				if ( is_similar( coordinate_indices, result0, vertices[ edges[ inner ].vertex_index0 ] ) )
					first_result_index = edges[ inner ].vertex_index0;
				else if ( is_similar( coordinate_indices, result0, vertices[ edges[ inner ].vertex_index1 ] ) )
					first_result_index = edges[ inner ].vertex_index1;
				else if ( is_similar( coordinate_indices, result0, vertices[ edges[ outer ].vertex_index0 ] ) )
					first_result_index = edges[ outer ].vertex_index0;
				else if ( is_similar( coordinate_indices, result0, vertices[ edges[ outer ].vertex_index1 ] ) )
					first_result_index = edges[ outer ].vertex_index1;
				else
					UNREACHABLE_CODE();

				u32 second_result_index = (u32)( -1 );
				if ( is_similar( result1, vertices[ edges[ inner ].vertex_index0 ] ) )
					second_result_index = edges[ inner ].vertex_index0;
				else if ( is_similar( coordinate_indices, result1, vertices[ edges[ inner ].vertex_index1 ] ) )
					second_result_index = edges[ inner ].vertex_index1;
				else if ( is_similar( coordinate_indices, result1, vertices[ edges[ outer ].vertex_index0 ] ) )
					second_result_index = edges[ outer ].vertex_index0;
				else if ( is_similar( coordinate_indices, result1, vertices[ edges[ outer ].vertex_index1 ] ) )
					second_result_index = edges[ outer ].vertex_index1;
				else
					UNREACHABLE_CODE();
				R_ASSERT( !( edges[ inner ].vertex_index0 == first_result_index  && edges[ inner ].vertex_index1 == second_result_index) );
				R_ASSERT( !( edges[ inner ].vertex_index1 == first_result_index  && edges[ inner ].vertex_index0 == second_result_index) );
				R_ASSERT( !( edges[ outer ].vertex_index0 == first_result_index  && edges[ outer ].vertex_index1 == second_result_index) );
				R_ASSERT( !( edges[ outer ].vertex_index1 == first_result_index  && edges[ outer ].vertex_index0 == second_result_index) );
				if ( edges[ inner ].vertex_index0 != first_result_index && edges[ inner ].vertex_index1 != first_result_index )
				{
					edges[ inner ].vertex_index1 = first_result_index;
					edges[ outer ].vertex_index0 = second_result_index;
					edges.push_back( edge( first_result_index, second_result_index ) );
				}
				else
				{
					edges[ inner ].vertex_index1 = second_result_index;
					edges[ outer ].vertex_index0 = first_result_index;
					edges.push_back( edge( second_result_index, first_result_index ) );
				}
			}
			else if ( intersection_count == 1 )
			{
				R_ASSERT( !math::is_infinity( result0.x ) && !math::is_infinity( result0.y ) && !math::is_infinity( result0.z ) );
				if ( math::is_similar( result0, vertices[ edges[ inner ].vertex_index0 ] ) )
				{
					edges.push_back( edge( edges[ outer ].vertex_index0, edges[ inner ].vertex_index0 ) );
					edges[ outer ].vertex_index0 = edges[ inner ].vertex_index0;
				} 
				else if ( math::is_similar( result0, vertices[ edges[ inner ].vertex_index1 ] ) )
				{
					edges.push_back( edge( edges[ outer ].vertex_index0, edges[ inner ].vertex_index1 ) );
					edges[ outer ].vertex_index0 = edges[ inner ].vertex_index1;
				} 
				else if ( math::is_similar( result0, vertices[ edges[ outer ].vertex_index0 ] ) )
				{
					edges.push_back( edge( edges[ inner ].vertex_index0, edges[ outer ].vertex_index0 ) );
					edges[ inner ].vertex_index0 = edges[ outer ].vertex_index0;
				} 
				else if ( math::is_similar( result0, vertices[ edges[ outer ].vertex_index1 ] ) )
				{
					edges.push_back( edge( edges[ inner ].vertex_index0, edges[ outer ].vertex_index1 ) );
					edges[ inner ].vertex_index0 = edges[ outer ].vertex_index1;
				} 
				else
				{
					u32 const first_result_index = vertices.size();
					vertices.push_back( result0 );
					edges.push_back( edge( first_result_index, edges[ inner ].vertex_index1 ) );
					edges[ inner ].vertex_index1 = first_result_index;
					edges.push_back( edge( first_result_index, edges[ outer ].vertex_index1 ) );
					edges[ outer ].vertex_index1 = first_result_index;
				}
			}
		}
	}
	if ( edge_count != edges.size() )	
	{
		std::sort( edges.begin(), edges.end(), edge_less() );
		edges.erase(
			std::unique( edges.begin(), edges.end(), edge_equals() ),
			edges.end()
		);
	}
}

bool edge_inside_edge( coord_indices_pair const& coordinate_indices, triangles_mesh_type::vertices_type const& vertices, edge const& testee, edge const& bounding_edge  )
{
	math::float3 const& first	= vertices[ bounding_edge.vertex_index0 ];
	math::float3 const& second	= vertices[ bounding_edge.vertex_index1 ];
	math::float3 const& start	= vertices[ testee.vertex_index0 ];
	math::float3 const& finish	= vertices[ testee.vertex_index1 ];
	return ( is_on_segment( coordinate_indices, start, first, second ) && is_on_segment( coordinate_indices, finish, first, second ) );
}

void remove_edges_inside_edges( coord_indices_pair const& coordinate_indices, triangles_mesh_type::vertices_type const& vertices, edges_type& edges )
{
	edges_type::iterator edges_end = edges.end();
	for ( edges_type::iterator outer_it = edges.begin(); outer_it != edges_end; ++outer_it )
	{
		for ( edges_type::iterator inner_it = edges.begin(); inner_it != edges_end; ++inner_it )
		{
			if ( outer_it == inner_it || 
				( outer_it->vertex_index0 == inner_it->vertex_index0 && outer_it->vertex_index1 == inner_it->vertex_index1 ) )
				continue;
			if ( edge_inside_edge( coordinate_indices, vertices, *outer_it, *inner_it ) )
			{
				outer_it->vertex_index0 = inner_it->vertex_index0;
				outer_it->vertex_index1 = inner_it->vertex_index1;
			}
		}
	}
	std::sort( edges.begin(), edges.end(), edge_less() );
	edges.erase(
		std::unique( edges.begin(), edges.end(), edge_equals() ),
		edges.end()
	);
}

void make_vertices_of_edges_unique_in_2d( coord_indices_pair const& coordinate_indices, triangles_mesh_type::vertices_type const& vertices, edges_type& edges, float epsilon  )
{
	edges_type::iterator const edges_end = edges.end();
	for ( edges_type::iterator outer_it = edges.begin(); outer_it != edges_end; ++outer_it )
	{
		for ( edges_type::iterator inner_it = outer_it + 1; inner_it != edges_end; ++inner_it )
		{
			if ( outer_it->vertex_index0 != inner_it->vertex_index0 && 
				 is_similar( coordinate_indices, vertices[ outer_it->vertex_index0 ], vertices[ inner_it->vertex_index0 ], epsilon ) )
				inner_it->vertex_index0 = outer_it->vertex_index0;
			if ( outer_it->vertex_index0 != inner_it->vertex_index1 && 
				is_similar( coordinate_indices, vertices[ outer_it->vertex_index0 ], vertices[ inner_it->vertex_index1 ], epsilon  ) )
				inner_it->vertex_index1 = outer_it->vertex_index0;
			if ( outer_it->vertex_index1 != inner_it->vertex_index0 && 
				is_similar( coordinate_indices, vertices[ outer_it->vertex_index1 ], vertices[ inner_it->vertex_index0 ], epsilon  ) )
				inner_it->vertex_index0 = outer_it->vertex_index1;
			if ( outer_it->vertex_index1 != inner_it->vertex_index1 && 
				is_similar( coordinate_indices, vertices[ outer_it->vertex_index1 ], vertices[ inner_it->vertex_index1 ], epsilon  ) )
				inner_it->vertex_index1 = outer_it->vertex_index1;
		}
	}
}

void remove_empty_triangles( triangles_mesh_type const& mesh, triangle_ids_type& triangles, float square_threshold )
{
	struct empty_triangle: public std::unary_function<u32, bool>
	{
		empty_triangle( triangles_mesh_type const& mesh, float square_threshold )
			:m_mesh( &mesh ), m_square_threshold(square_threshold){}
		bool operator()( u32 triangle_id ) const
		{
			u32 const first_index	= m_mesh->indices[ triangle_id * 3 ];
			u32 const second_index	= m_mesh->indices[ triangle_id * 3 + 1 ];
			u32 const third_index	= m_mesh->indices[ triangle_id * 3 + 2 ];
			if ( first_index == second_index || second_index == third_index || third_index == first_index || 
				m_mesh->data[ triangle_id ].square < m_square_threshold )
				return true;
			else
				return false;
		}
	private:
		triangles_mesh_type const* m_mesh;
		float					   m_square_threshold;
	};
	triangles.erase(  
		std::remove_if( triangles.begin(), triangles.end(), empty_triangle( mesh, square_threshold ) ),
		triangles.end()
	);
}

void replace_triangles_with_their_children	( triangles_mesh_type const& mesh, triangle_ids_type& triangles )
{
	struct triangle_has_children: public std::unary_function<u32, bool>
	{
		triangle_has_children( triangles_mesh_type const& mesh )
			:m_mesh( &mesh ){}
		bool operator()( u32 triangle_id ) const
		{
			return m_mesh->data[ triangle_id ].has_children();
		}
	private:
		triangles_mesh_type const* m_mesh;
	};

	u32 const triangle_count = triangles.size();
	for ( u32 i = 0; i < triangle_count; ++i )
	{
		u32 const triangle_id = triangles[ i ];
		if ( mesh.data[ triangle_id ].children[ 0 ] != bsp_tree_triangle::msc_empty_id )
			triangles.push_back( mesh.data[ triangle_id ].children[ 0 ] );
		if ( mesh.data[ triangle_id ].children[ 1 ] != bsp_tree_triangle::msc_empty_id )
			triangles.push_back( mesh.data[ triangle_id ].children[ 1 ] );
		if ( mesh.data[ triangle_id ].children[ 2 ] != bsp_tree_triangle::msc_empty_id )
			triangles.push_back( mesh.data[ triangle_id ].children[ 2 ] );
	}

	triangles.erase(
		std::remove_if( triangles.begin(), triangles.end(), triangle_has_children( mesh ) ),
		triangles.end()
	);
	std::sort( triangles.begin(), triangles.end() );
	triangles.erase(
		std::unique( triangles.begin(), triangles.end() ),
		triangles.end()
	);
	triangle_ids_type::const_iterator const triangles_end = triangles.end();
	for ( triangle_ids_type::const_iterator it = triangles.begin(); it != triangles_end; ++it )
	{
		R_ASSERT( !mesh.data[ *it ].has_children() );
	}
}

void remove_adjacency_in_coplanar_edges	( u32 triangle_id, math::plane const& divider, triangles_mesh_type& mesh )
{
	R_ASSERT( is_triangle_adjacency_correct( triangle_id, mesh ) );
	float const c_precision = math::epsilon_3;
	math::float3 const& first_vertex	= mesh.vertices[ mesh.indices[ triangle_id * 3 ] ];
	math::float3 const& second_vertex	= mesh.vertices[ mesh.indices[ triangle_id * 3 + 1 ] ];
	math::float3 const& third_vertex	= mesh.vertices[ mesh.indices[ triangle_id * 3 + 2 ] ];
	bool const first_on_plane			= math::is_zero( divider.classify( first_vertex ), c_precision );
	bool const second_on_plane			= math::is_zero( divider.classify( second_vertex ), c_precision );
	bool const third_on_plane			= math::is_zero( divider.classify( third_vertex ), c_precision );
	bsp_tree_triangle& triangle			= mesh.data[ triangle_id ];
	if ( first_on_plane && second_on_plane )
	{
		u32 const neighbour_id = triangle.neighbours[ 0 ];
		if ( neighbour_id != bsp_tree_triangle::msc_empty_id )
		{
			triangle.neighbours[ 0 ] = bsp_tree_triangle::msc_empty_id;
			replace_neighbour_of_triangle( neighbour_id, mesh, triangle_id, bsp_tree_triangle::msc_empty_id );
		}
	}
	if ( second_on_plane && third_on_plane )
	{
		u32 const neighbour_id = triangle.neighbours[ 1 ];
		if ( neighbour_id != bsp_tree_triangle::msc_empty_id )
		{
			triangle.neighbours[ 1 ] = bsp_tree_triangle::msc_empty_id;
			replace_neighbour_of_triangle( neighbour_id, mesh, triangle_id, bsp_tree_triangle::msc_empty_id );
		}
	}
	if ( third_on_plane && first_on_plane )
	{
		u32 const neighbour_id = triangle.neighbours[ 2 ];
		if ( neighbour_id != bsp_tree_triangle::msc_empty_id )
		{
			triangle.neighbours[ 2 ] = bsp_tree_triangle::msc_empty_id;
			replace_neighbour_of_triangle( neighbour_id, mesh, triangle_id, bsp_tree_triangle::msc_empty_id );
		}
	}
}
//http://paulbourke.net/geometry/lineline2d/
bool is_collinear ( coord_indices_pair const& coordinate_indices, 
					math::float3 const& v0,  math::float3 const& v1, 
					math::float3 const& u0,  math::float3 const& u1, 
					float precision )
{
	float const x1 = v0[ coordinate_indices[0] ];
	float const y1 = v0[ coordinate_indices[1] ];
	float const x2 = v1[ coordinate_indices[0] ];
	float const y2 = v1[ coordinate_indices[1] ];
	float const x3 = u0[ coordinate_indices[0] ];
	float const y3 = u0[ coordinate_indices[1] ];
	float const x4 = u1[ coordinate_indices[0] ];
	float const y4 = u1[ coordinate_indices[1] ];
	float const denominator = ( y4 - y3 ) * ( x2 - x1 ) - ( x4 - x3 ) * ( y2 - y1 );
	if ( !math::is_zero( denominator, precision ) )
		return false;
	float const numenator_1 = ( x4 - x3 ) * ( y1 - y3 ) - ( y4 - y3 ) * ( x1 - x3 );
	float const numenator_2 = ( x2 - x1 ) * ( y1 - y3 ) - ( y2 - y1 ) * ( x1 - x3 );
	return math::is_zero( numenator_1, precision ) && math::is_zero( numenator_2, precision );
}

bool is_parallel( coord_indices_pair const& coordinate_indices, 
				 math::float3 const& v0,  math::float3 const& v1, 
				 math::float3 const& u0,  math::float3 const& u1, 
				 float precision )
{
	float const x1 = v0[ coordinate_indices[0] ];
	float const y1 = v0[ coordinate_indices[1] ];
	float const x2 = v1[ coordinate_indices[0] ];
	float const y2 = v1[ coordinate_indices[1] ];
	float const x3 = u0[ coordinate_indices[0] ];
	float const y3 = u0[ coordinate_indices[1] ];
	float const x4 = u1[ coordinate_indices[0] ];
	float const y4 = u1[ coordinate_indices[1] ];
	float const denominator = ( y4 - y3 ) * ( x2 - x1 ) - ( x4 - x3 ) * ( y2 - y1 );
	return math::is_zero( denominator, precision );
}

bool is_codirectional( coord_indices_pair const& coordinate_indices, 
					   math::float3 const& v0,  math::float3 const& v1, 
					   math::float3 const& u0,  math::float3 const& u1, 
					   float precision )
{
	bool const parallel = is_parallel( coordinate_indices, v0, v1, u0, u1, precision );
	return parallel && ( ( v1 - v0 ) | ( u1 - u0 ) ) > 0.0f ;
}

math::float3 get_triangles_mass_center( triangle_ids_type const& triangles, triangles_mesh_type const& mesh )
{
	R_ASSERT( !triangles.empty() );
	triangles_mesh_type::indices_type indices;
	indices.reserve( triangles.size() * 3 );
	triangle_ids_type::const_iterator const triangles_end = triangles.end();
	for ( triangle_ids_type::const_iterator it = triangles.begin(); it != triangles_end; ++it )
	{
		const u32 triangle_id = *it;
		triangles_mesh_type::indices_type::const_iterator const start_iter = mesh.indices.begin() + triangle_id * 3;
		indices.insert( indices.end(), start_iter, start_iter + 3 );
	}
	std::sort( indices.begin(), indices.end() );
	indices.erase(
		std::unique( indices.begin(), indices.end() ),
		indices.end()
	);

	math::float3 mass_center( 0.0f, 0.0f, 0.0f );
	triangles_mesh_type::indices_type::const_iterator const indices_end = indices.end();
	for ( triangles_mesh_type::indices_type::const_iterator it = indices.begin(); it != indices_end; ++it )
	{
		mass_center += mesh.vertices[ *it ];
	}
	mass_center /= static_cast<float>( indices.size() );
	return mass_center;
}

void save_to_lua( triangles_mesh_type::vertices_type& vertices, input_indices_type const& indices, edges_type const& edges, math::float3 const& normal )
{
	configs::lua_config_ptr config			= configs::create_lua_config();
	configs::lua_config_value vertices_cfg	= (*config)[ "vertices" ];
	u32 new_index = 0;
	typedef associative_vector<u32, u32, vector> old_to_new_index_type;
	old_to_new_index_type old_to_new_index;
	input_indices_type::const_iterator const indices_end = indices.end();
	for ( input_indices_type::const_iterator index_it = indices.begin(); index_it != indices_end; ++index_it )
	{
		math::float3 const& v = vertices[ *index_it ];
		vertices_cfg[ new_index ] = v;
		old_to_new_index.insert( std::make_pair( *index_it, new_index++ ) );
	}

	u32 edge_index = 0;
	configs::lua_config_value edges_cfg	= (*config)[ "edges" ];
	edges_type::const_iterator const edges_end = edges.end();
	for ( edges_type::const_iterator edge_it = edges.begin(); edge_it != edges_end; ++edge_it )
	{
		R_ASSERT( old_to_new_index.count( edge_it->vertex_index0 ) );
		R_ASSERT( old_to_new_index.count( edge_it->vertex_index1 ) );
		configs::lua_config_value current_edge_cfg	= edges_cfg[ edge_index++ ];
		current_edge_cfg[ 0 ] = old_to_new_index[ edge_it->vertex_index0 ];
		current_edge_cfg[ 1 ] = old_to_new_index[ edge_it->vertex_index1 ];
	}

	(*config)[ "normal" ] = normal;
	config->save_as( "D:/output.lua", xray::configs::target_sources );
}

void on_surfaces_ready( resources::queries_result& data, geometry_utils::geometry_collector* collector )
{
	if(!data.is_successful())
		return;

	u32 count = data.size();
	
	for(u32 i=0; i<count; i+=2)
	{
		resources::pinned_ptr_const<u8> vertices_ptr	( data[i].get_managed_resource() );
		resources::pinned_ptr_const<u8> indices_ptr		( data[i+1].get_managed_resource() );
		
		memory::reader vertices_reader	( vertices_ptr.c_ptr(), vertices_ptr.size() );
		memory::reader indices_reader	( indices_ptr.c_ptr(), indices_ptr.size() );
	
		u32 vert_struct_size				= sizeof(render::vert_static);
		u32 position_offset					= 0;

		/*u32 icount =*/ indices_reader.r_u32();
		u32 vert_start_offset			= sizeof(u32) + position_offset;

		while(!indices_reader.eof())
		{
			u16 idx0			= indices_reader.r_u16();
			u16 idx1			= indices_reader.r_u16();
			u16 idx2			= indices_reader.r_u16();
			
			vertices_reader.seek(vert_start_offset+vert_struct_size*idx0);
			float3 p0			= vertices_reader.r_float3();

			vertices_reader.seek(vert_start_offset+vert_struct_size*idx1);
			float3 p1			= vertices_reader.r_float3();

			vertices_reader.seek(vert_start_offset+vert_struct_size*idx2);
			float3 p2			= vertices_reader.r_float3();

			if( p0.is_similar(p1) || p0.is_similar(p2) || p1.is_similar(p2) )
				continue;

			collector->add_triangle( p0, p2, p1 );
		}
	}
}

void collect_surfaces( System::String^ model_name, 
					  string_list^ surfaces, 
					  geometry_utils::geometry_collector* collector )
{
	u32 count = surfaces->Count;
	resources::request* requests = ALLOC(resources::request, count*2);
	
	for(u32 i=0; i<count; ++i)
	{
		resources::request& rv = requests[2*i];
		rv.id	= resources::raw_data_class;
		System::String^ filename = System::String::Format("resources/models/{0}.model/render/{1}/vertices", model_name, surfaces[i]);
		rv.path	= strings::duplicate(g_allocator, unmanaged_string(filename).c_str());

		resources::request& ri = requests[2*i+1];
		ri.id		= resources::raw_data_class;
		filename	= System::String::Format("resources/models/{0}.model/render/{1}/indices", model_name, surfaces[i]);
		ri.path		= strings::duplicate(g_allocator, unmanaged_string(filename).c_str());
	}

	resources::query_resources_and_wait(
		requests,
		count*2,
		boost::bind(on_surfaces_ready, _1, collector ),
		g_allocator);


	for(u32 i=0; i<count*2; ++i)
	{
		resources::request& r	= requests[i];
		void* ptr				= (void*)r.path;
		FREE					(ptr);
	}

	FREE(requests);
}

} // namespace model_editor
} // namespace xray