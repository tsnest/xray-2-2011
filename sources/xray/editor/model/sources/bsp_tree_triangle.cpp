////////////////////////////////////////////////////////////////////////////
//	Created		: 29.08.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "bsp_tree_triangle.h"
#include "bsp_tree_utilities.h"
#include <xray/math_randoms_generator.h>
namespace xray {
namespace model_editor {
static float const c_square_threshold = 1e-4f/*math::epsilon_5*/;
//------------------------------------------------------------------------------------------------------------
#pragma message( XRAY_TODO( "The following code duplicates code from graph_generator::remove_duplicates" ) )
static inline float project_vertex (float3 const& v0, float3 const& v1, float3 const& p)
{
	return (p - v0) | normalize( v1 - v0 );
}

class compare_indices_by_vertices_predicate {
public:
	compare_indices_by_vertices_predicate( triangles_mesh_type const& triangles_mesh, float3 const& v0, float3 const& v1) :
		m_v0(v0),
		m_v1(v1),
		m_triangles_mesh		( &triangles_mesh )
	{
	}

	inline bool operator() ( u32 const left, u32 const right) const {
		if (project_vertex ( m_v0, m_v1, m_triangles_mesh->vertices[ left ] ) < project_vertex ( m_v0, m_v1, m_triangles_mesh->vertices[ right ] ))
			return true;
		return false;
	}

private:
	triangles_mesh_type const*	m_triangles_mesh;
	float3									m_v0;
	float3									m_v1;
}; // class less_vertex_index_predicate

typedef xray::buffer_vector<u32> indices_type;
typedef debug::vector< math::float3 >		vertices_type;

struct remove_vertices_by_unique_indices_predicate {
	remove_vertices_by_unique_indices_predicate (
			vertices_type const& vertices,
			indices_type const& unique_vertices_indices
		) :
		m_vertices_begin			( vertices.begin() ),
		m_unique_vertices_indices	( &unique_vertices_indices )
	{
	}

		inline bool operator() ( xray::math::float3 const& vertex ) const
	{
		u32 const index = u32(&vertex - m_vertices_begin);
		return		(*m_unique_vertices_indices)[ index ] != index;
	}

private:
	vertices_type::const_iterator	m_vertices_begin;
	indices_type const*				m_unique_vertices_indices;
}; // struct remove_vertices_by_unique_indices_predicate

struct compare_triangles_by_vertices_indices_predicate {
	compare_triangles_by_vertices_indices_predicate( triangles_mesh_type const& triangles_mesh ) :
		m_triangles_mesh	( &triangles_mesh )
	{
	}

	inline bool operator() ( u32 const left, u32 const right ) {
		triangles_mesh_type::indices_type const& indices = m_triangles_mesh->indices;
		for (u32 i = 0; i < 3; ++i ) {
			if ( indices[ 3*left+i ] < indices[ 3*right+i ] )
				return				true;

			if ( indices[ 3*left+i ] > indices[ 3*right+i ] )
				return				false;
		}
		return						false;
	}

private:
	triangles_mesh_type const*	m_triangles_mesh;
}; // struct compare_triangles_by_vertices_indices_predicate

inline bool is_zero_square ( 
		float3 const& vertex0,
		float3 const& vertex1,
		float3 const& vertex2
	)
{

	if ( is_similar ( vertex0, vertex1 )
		|| is_similar( vertex0, vertex2 )
		|| is_similar( vertex1, vertex2 ) )
	{
		return true;
	}

	xray::math::float3 const b_minus_a	= normalize( vertex1 - vertex0 );
	xray::math::float3 const c_minus_a	= normalize( vertex2 - vertex0 );

	if ( b_minus_a.is_similar(c_minus_a) || b_minus_a.is_similar(-c_minus_a) )
		return					true;

	return false;
}

void remove_duplicates ( triangles_mesh_type& triangles_mesh, bool remove_duplicate_triangles )
{

	u32 const	vertices_count = triangles_mesh.vertices.size();
	indices_type sorted_indices ( 
		ALLOCA( vertices_count * sizeof(indices_type::value_type) ),
		vertices_count,
		vertices_count
	);

	indices_type::iterator const b = sorted_indices.begin();
	indices_type::iterator const e = sorted_indices.end();
	for ( indices_type::iterator i = b; i != e; ++i )
		*i						= u32(i - b);

	float3 middle_vertices[2];
	math::random32 random( 0 );
	for ( u32 i = 0; i < 3; ++i ) {
		middle_vertices[0] = float3(0, 0, 0);
		middle_vertices[1] = float3(0, 0, 0);

		u32 const half_vertices_count = vertices_count / 2;
		for ( u32 j = 0; j < half_vertices_count; ++j )
			middle_vertices[0] += triangles_mesh.vertices[ sorted_indices[j] ];

		for ( u32 j = half_vertices_count; j < vertices_count; ++j )
			middle_vertices[1] += triangles_mesh.vertices[ sorted_indices[j] ];

		middle_vertices[0]	/= float(half_vertices_count);
		middle_vertices[1]	/= float(vertices_count - half_vertices_count);

		if ( !is_similar( middle_vertices[0], middle_vertices[1], xray::math::epsilon_3 ) )
			break;

		std::random_shuffle( sorted_indices.begin(), sorted_indices.end(), random );
	}

	std::sort (
		sorted_indices.begin( ),
		sorted_indices.end( ),
		compare_indices_by_vertices_predicate( triangles_mesh, middle_vertices[0], middle_vertices[1] )
	);

	indices_type unique_vertices_indices (
		ALLOCA( vertices_count * sizeof(indices_type::value_type) ),
		vertices_count,
		vertices_count,
		u32(-1)
	);

	for ( indices_type::iterator j = b; j != e; ++j ) {
		if ( unique_vertices_indices[ *j ] != u32(-1) )
			continue;

		for ( indices_type::iterator i = j; i != e; ++i ) {
			if (
					project_vertex(
						middle_vertices[0],
						middle_vertices[1],
						triangles_mesh.vertices[*j]
			) + math::epsilon_3
						<
					project_vertex(
						middle_vertices[0],
						middle_vertices[1],
						triangles_mesh.vertices[*i]
					)
				)
			{
				break;
			}

			if ( is_similar(triangles_mesh.vertices[*i], triangles_mesh.vertices[*j], math::epsilon_3 ) )
				unique_vertices_indices[ *i ] = *j;
		}
	}

	// !
	for ( u32 i = 0, n = unique_vertices_indices.size(); i != n; ++i ) {
		u32& real_index					= unique_vertices_indices[ unique_vertices_indices[i] ];
		if ( real_index	 <= i ) {
			unique_vertices_indices[i]	= real_index;
			continue;
		}

		real_index						= i;
		unique_vertices_indices[i]		= i;
	}

	triangles_mesh.vertices.erase (
		std::remove_if(
			triangles_mesh.vertices.begin(),
			triangles_mesh.vertices.end(),
			remove_vertices_by_unique_indices_predicate( triangles_mesh.vertices, unique_vertices_indices )
		),
		triangles_mesh.vertices.end()
	);

	// reindexing unique vertices indices
	for ( u32 i = 0, j = 0; i != vertices_count; ++i ) {
        if ( unique_vertices_indices[i] == i )
			unique_vertices_indices[i]    = j++;
		else
			unique_vertices_indices[i]    = unique_vertices_indices[ unique_vertices_indices[i] ];
	}
	
	u32 const	triangles_count		= triangles_mesh.data.size();
	for ( u32 i = 0; i < triangles_count; ++i ) {
		for ( u32 j = 0; j < 3; ++j )
			triangles_mesh.indices[ 3*i+j ] = unique_vertices_indices[triangles_mesh.indices[ 3*i+j ]];
		
		u32* indices = triangles_mesh.indices.begin() + 3*i;
		u32 smallest_index			= 0;
		if ( indices[1] < indices[0] )
			smallest_index			= 1;

		if ( indices[2] < indices[ smallest_index ] )
			smallest_index			= 2;
		
		switch ( smallest_index ) {
			case 0: break;
			case 1: {
				std::swap ( indices[0], indices[1] );
				std::swap ( indices[1], indices[2] );
				//std::swap ( triangles_mesh.data[i].obstructions[0],  triangles_mesh.data[i].obstructions[2]);
				//std::swap ( triangles_mesh.data[i].obstructions[0],  triangles_mesh.data[i].obstructions[1]);
				break;
			}
			case 2: {
				std::swap ( indices[0], indices[2] );
				std::swap ( indices[1], indices[2] );
				//std::swap ( triangles_mesh.data[i].obstructions[0],  triangles_mesh.data[i].obstructions[1]);
				//std::swap ( triangles_mesh.data[i].obstructions[0],  triangles_mesh.data[i].obstructions[2]);
				break;
			}
			default: NODEFAULT( );
		}
	}
	if ( !remove_duplicate_triangles )
		return;
	indices_type triangle_indices(
		ALLOCA( triangles_count * sizeof( indices_type::value_type ) ),
		triangles_count,
		triangles_count
	);

	indices_type::iterator const tb = triangle_indices.begin();
	indices_type::iterator const te = triangle_indices.end();
	for ( indices_type::iterator i = tb; i != te; ++i )
		*i = u32(i - tb);

	std::sort( tb, te, compare_triangles_by_vertices_indices_predicate ( triangles_mesh ));
	for ( indices_type::iterator i = tb; i != te - 1; ++i ) {
		bool equal = true;
		for ( u32 j = 0; j < 3; ++j ) {
			if ( triangles_mesh.indices[ 3*(*i) + j ] != triangles_mesh.indices[ 3*(*(i+1)) + j ] ) {
				equal = false;
				break;
			}		
		}

		float3 const* vertices[] = {
			&triangles_mesh.vertices[ triangles_mesh.indices[ 3*(*i) + 0 ] ],
			&triangles_mesh.vertices[ triangles_mesh.indices[ 3*(*i) + 1 ] ],
			&triangles_mesh.vertices[ triangles_mesh.indices[ 3*(*i) + 2 ] ]
		};

		
		if ( equal || is_zero_square( *vertices[0], *vertices[1], *vertices[2] ) ) 
			triangles_mesh.data[ *i ].is_marked = true;
	}
	triangles_mesh.remove_marked_triangles ( );
	R_ASSERT_CMP ( triangles_mesh.indices.size() % 3, ==, 0);
}

void remove_duplicate_triangles	( triangles_mesh_type const& triangles_mesh, triangle_ids_type& triangles )
{
	struct triangles_equal : public std::binary_function<u32, u32, bool>
	{
		triangles_equal ( triangles_mesh_type const& mesh )
			:m_mesh( &mesh ){}
		bool operator() ( u32 left, u32 right ) const
		{
			return  m_mesh->indices[ left * 3 ]		== m_mesh->indices[ right * 3 ] &&
					m_mesh->indices[ left * 3 + 1 ] == m_mesh->indices[ right * 3 + 1 ] &&
					m_mesh->indices[ left * 3 + 2 ] == m_mesh->indices[ right * 3 + 2 ];
		}
	private:
		triangles_mesh_type const* m_mesh;
	};
	std::sort( triangles.begin(), triangles.end(), compare_triangles_by_vertices_indices_predicate ( triangles_mesh ) );
	triangles.erase(
		std::unique( triangles.begin(), triangles.end(), triangles_equal( triangles_mesh ) ),
		triangles.end()
	);
}

//------------------------------------------------------------------------------------

bsp_tree_triangle::side calculate_side( math::plane const& p, u32 triangle_id, triangles_mesh_type const& mesh, float precision )
{
	if ( mesh.data[ triangle_id ].is_coplanar( p, precision ) )
		return bsp_tree_triangle::coplanar;
	u32 positive = 0, negative = 0;
	for ( u32 i = 0; i < 3; ++i )
	{
		math::float3 const& vertex	= mesh.vertices[ mesh.indices[ triangle_id * 3 + i] ];
		float const	 dist		= p.classify( vertex );
		if ( math::is_zero( dist, precision ) )
			continue;
		else if ( dist > 0.0f )
			++positive;
		else
			++negative;
	}
	if ( positive > 0 && negative == 0) 
		return bsp_tree_triangle::front; 
	else if( positive == 0 && negative > 0 ) 
	{
		return bsp_tree_triangle::back;
	}
	else if( positive == 0 && negative == 0 )
	{
		if ( mesh.data[ triangle_id ].is_coplanar( p ) )
			return bsp_tree_triangle::coplanar;
		else
		{
			return ( p.normal | mesh.data[ triangle_id ].plane.normal ) > 0.0f  ? bsp_tree_triangle::front : bsp_tree_triangle::back;
		}
	}
	else 
		return bsp_tree_triangle::spanning; 
}

bool is_convex_polygon_set( triangle_ids_type const& triangle_ids, triangles_mesh_type const& mesh )
{
	const triangle_ids_type::const_iterator end_it = triangle_ids.end();
	for ( triangle_ids_type::const_iterator it = triangle_ids.begin(); it != end_it; ++it )
	{
		math::plane const& p =  mesh.data[ *it ].plane;
		for ( triangle_ids_type::const_iterator other_it = triangle_ids.begin(); other_it != end_it; ++other_it )
		{
			if ( *it == *other_it )
				continue;
			bsp_tree_triangle::side side = calculate_side( p, *other_it, mesh );
			if ( side == bsp_tree_triangle::spanning || side == bsp_tree_triangle::back )
				return false;
		}
	}
	return true;
}

void replace_neighbour_of_triangle( u32 triangle_id, triangles_mesh_type& mesh, u32 old_neighbour_id, u32 new_neighbour_id )
{
	if ( mesh.data[ triangle_id ].neighbours[ 0 ] == old_neighbour_id )
		mesh.data[ triangle_id ].neighbours[ 0 ] = new_neighbour_id;
	else if ( mesh.data[ triangle_id ].neighbours[ 1 ] == old_neighbour_id )
		mesh.data[ triangle_id ].neighbours[ 1 ] = new_neighbour_id;
	else if ( mesh.data[ triangle_id ].neighbours[ 2 ] == old_neighbour_id )
		mesh.data[ triangle_id ].neighbours[ 2 ] = new_neighbour_id;
	else
		UNREACHABLE_CODE();
}

static bool triangle_split_to_two( math::plane const& p, u32 triangle_id, u32 coord_offset, triangles_mesh_type& mesh, triangle_ids_type& positive_ids, triangle_ids_type& negative_ids, edge_to_u32_type* edges )
{
	u32	const peak_index				= mesh.indices[ triangle_id * 3 + coord_offset ];
	u32	const first_vertex_index		= mesh.indices[ triangle_id * 3 + ( coord_offset + 1 ) % 3 ];
	u32 const second_vertex_index		= mesh.indices[ triangle_id * 3 + ( coord_offset + 2 ) % 3 ];
	math::float3 const peak				= mesh.vertices[ peak_index ];
	math::float3 const first_vertex		= mesh.vertices[ first_vertex_index ];
	math::float3 const second_vertex	= mesh.vertices[ second_vertex_index ];
	float3 intersection;
	if ( p.intersect_segment( first_vertex, second_vertex, intersection ) ) 
	{
		u32 const right_neighbour_id	= mesh.data[ triangle_id ].neighbours[ coord_offset ];
		u32 const bottom_neighbour_id	= mesh.data[ triangle_id ].neighbours[ ( coord_offset + 1 ) % 3 ];
		u32 const left_neighbour_id		= mesh.data[ triangle_id ].neighbours[ ( coord_offset + 2 ) % 3 ];
		R_ASSERT( right_neighbour_id	== bsp_tree_triangle::msc_empty_id || is_triangle_adjacency_correct( right_neighbour_id, mesh ) );
		R_ASSERT( bottom_neighbour_id	== bsp_tree_triangle::msc_empty_id || is_triangle_adjacency_correct( bottom_neighbour_id, mesh ) );
		R_ASSERT( left_neighbour_id		== bsp_tree_triangle::msc_empty_id || is_triangle_adjacency_correct( left_neighbour_id, mesh ) );

		if ( !is_between( intersection, first_vertex, second_vertex ) )
			make_point_between( first_vertex, second_vertex, intersection );
		if ( !math::is_zero( p.classify( intersection ) ) )
			intersection = p.project( intersection );
		u32 intersection_index	= u32( -1 );
		if ( math::is_similar( intersection, first_vertex ) )
			return false;
		else if ( math::is_similar( intersection, second_vertex ) )
			return false;
		else
		{
			intersection_index = mesh.vertices.size();
			mesh.vertices.push_back( intersection );
		}

		u32 first_triangle_id = bsp_tree_triangle::msc_empty_id;
		float const first_triangle_square	= triangle_square( peak, first_vertex, intersection );
		if ( first_triangle_square > c_square_threshold )
		{
			first_triangle_id		= mesh.data.size();
			mesh.indices.push_back( peak_index );
			mesh.indices.push_back( first_vertex_index );
			mesh.indices.push_back( intersection_index );
			bsp_tree_triangle first_triangle( mesh.data[ triangle_id ].plane, __LINE__ );
			first_triangle.divider	= mesh.data[ triangle_id ].divider;
			first_triangle.square = triangle_square( peak, first_vertex, intersection );
			first_triangle.parent = triangle_id;
			mesh.data.push_back( first_triangle );
			mesh.data[ triangle_id ].children[0] = first_triangle_id;
			mesh.data[ first_triangle_id ].neighbours[ 0 ]	= right_neighbour_id;
			mesh.data[ first_triangle_id ].neighbours[ 1 ]	= bottom_neighbour_id;
			bsp_tree_triangle::side const side = calculate_side( p, first_triangle_id, mesh );
			if ( side == bsp_tree_triangle::front )
			{
				positive_ids.push_back( first_triangle_id );
			}
			else
			{
				R_ASSERT( side == bsp_tree_triangle::back );
				negative_ids.push_back( first_triangle_id  );
			}
		}
		if ( right_neighbour_id != bsp_tree_triangle::msc_empty_id )
		{
			replace_neighbour_of_triangle( right_neighbour_id, mesh, triangle_id, first_triangle_id );
		}
		R_ASSERT( right_neighbour_id	== bsp_tree_triangle::msc_empty_id || is_triangle_adjacency_correct( right_neighbour_id, mesh ) );

		u32 second_triangle_id	= bsp_tree_triangle::msc_empty_id;
		float const second_triangle_square	= triangle_square( peak, intersection, second_vertex );
		if ( second_triangle_square > c_square_threshold )
		{
			second_triangle_id	= mesh.data.size();
			mesh.indices.push_back( peak_index );
			mesh.indices.push_back( intersection_index );
			mesh.indices.push_back( second_vertex_index );
			bsp_tree_triangle second_triangle( mesh.data[ triangle_id ].plane, __LINE__ );
			second_triangle.divider	= mesh.data[ triangle_id ].divider;
			second_triangle.square = triangle_square( peak, intersection, second_vertex );
			second_triangle.parent = triangle_id;
			mesh.data.push_back( second_triangle );
			R_ASSERT( mesh.data.back().plane.valid() );
			mesh.data[ triangle_id ].children[1] = second_triangle_id;
			mesh.data[ second_triangle_id ].neighbours[ 1 ]	= bottom_neighbour_id;
			mesh.data[ second_triangle_id ].neighbours[ 2 ] = left_neighbour_id;
			bsp_tree_triangle::side const side = calculate_side( p, second_triangle_id, mesh );
			if ( side == bsp_tree_triangle::front )
			{
				positive_ids.push_back( second_triangle_id );
			}
			else
			{
				R_ASSERT( side == bsp_tree_triangle::back );
				negative_ids.push_back( second_triangle_id );
			}

		}
		if ( left_neighbour_id != bsp_tree_triangle::msc_empty_id )
		{
			replace_neighbour_of_triangle( left_neighbour_id, mesh, triangle_id, second_triangle_id );
		}
		R_ASSERT( left_neighbour_id		== bsp_tree_triangle::msc_empty_id || is_triangle_adjacency_correct( left_neighbour_id, mesh ) );

		if ( bottom_neighbour_id != bsp_tree_triangle::msc_empty_id )
		{
			if ( first_triangle_id != bsp_tree_triangle::msc_empty_id || second_triangle_id != bsp_tree_triangle::msc_empty_id )
			{
				R_ASSERT( first_triangle_id == bsp_tree_triangle::msc_empty_id	|| mesh.data[ first_triangle_id ].is_neighbour_of( bottom_neighbour_id ) );
				R_ASSERT( second_triangle_id == bsp_tree_triangle::msc_empty_id || mesh.data[ second_triangle_id ].is_neighbour_of( bottom_neighbour_id ) );
				R_ASSERT( std::find( positive_ids.begin(), positive_ids.end(), bottom_neighbour_id ) ==  positive_ids.end() );
				R_ASSERT( std::find( negative_ids.begin(), negative_ids.end(), bottom_neighbour_id ) ==  negative_ids.end() );
				split_triangle_with_point( bottom_neighbour_id, mesh,
					triangle_id, intersection_index, first_triangle_id, second_triangle_id );
			}
			else
			{
				replace_neighbour_of_triangle( bottom_neighbour_id, mesh, triangle_id, bsp_tree_triangle::msc_empty_id );
			}
		}
		clear_triangle_neighbours( triangle_id, mesh );
		R_ASSERT( first_triangle_id == bsp_tree_triangle::msc_empty_id	|| is_triangle_adjacency_correct( first_triangle_id, mesh ) );
		R_ASSERT( second_triangle_id == bsp_tree_triangle::msc_empty_id || is_triangle_adjacency_correct( second_triangle_id, mesh ) );
		R_ASSERT( right_neighbour_id == bsp_tree_triangle::msc_empty_id || is_triangle_adjacency_correct( right_neighbour_id, mesh ) );
		R_ASSERT( left_neighbour_id	 == bsp_tree_triangle::msc_empty_id || is_triangle_adjacency_correct( left_neighbour_id, mesh ) );
		if ( edges != NULL )
		{
			edge_to_u32_type::iterator edge_it = edges->find( edge( first_vertex_index, second_vertex_index ) );
			if ( edge_it != edges->end() )
			{
				u32 const id = edge_it->second;
				edges->erase( edge_it );
				edges->insert( std::make_pair( edge( first_vertex_index, intersection_index ), id ) );
				edges->insert( std::make_pair( edge( intersection_index, second_vertex_index ), id ) );
			}
		}
		return true;
	}
	return false;
}

static bool triangle_split_to_three( math::plane const& p, u32 triangle_id, u32 coord_offset, triangles_mesh_type& mesh, triangle_ids_type& positive_ids, triangle_ids_type& negative_ids, edge_to_u32_type* edges )
{
	u32	const peak_index				= mesh.indices[ triangle_id * 3 + coord_offset ];
	u32	const first_vertex_index		= mesh.indices[ triangle_id * 3 + ( coord_offset + 1 ) % 3 ];
	u32 const second_vertex_index		= mesh.indices[ triangle_id * 3 + ( coord_offset + 2 ) % 3 ];
	math::float3 const peak				= mesh.vertices[ peak_index ];
	math::float3 const first_vertex		= mesh.vertices[ first_vertex_index ];
	math::float3 const second_vertex	= mesh.vertices[ second_vertex_index ];
	
	float3 first_intersection, second_intersection;
	if ( !( p.intersect_segment( peak, first_vertex, first_intersection ) && p.intersect_segment( peak, second_vertex, second_intersection ) ) )
		return false;

	float const first_triangle_square	= triangle_square( peak, first_intersection, second_intersection );
	float const second_triangle_square	= triangle_square( first_intersection, first_vertex, second_intersection );
	float const third_triangle_square	= triangle_square( second_intersection, first_vertex, second_vertex );
	if ( first_triangle_square < c_square_threshold && second_triangle_square < c_square_threshold && third_triangle_square < c_square_threshold )
	{
		reset_triangle_adjacency( triangle_id, mesh );
		return true;
	}
	u32 const right_neighbour_id		= mesh.data[ triangle_id ].neighbours[ coord_offset ];
	u32 const bottom_neighbour_id		= mesh.data[ triangle_id ].neighbours[ ( coord_offset + 1 ) % 3 ];
	u32 const left_neighbour_id			= mesh.data[ triangle_id ].neighbours[ ( coord_offset + 2 ) % 3 ];
	R_ASSERT( right_neighbour_id	== bsp_tree_triangle::msc_empty_id || is_triangle_adjacency_correct( right_neighbour_id, mesh ) );
	R_ASSERT( bottom_neighbour_id	== bsp_tree_triangle::msc_empty_id || is_triangle_adjacency_correct( bottom_neighbour_id, mesh ) );
	R_ASSERT( left_neighbour_id		== bsp_tree_triangle::msc_empty_id || is_triangle_adjacency_correct( left_neighbour_id, mesh ) );

	u32 first_intersection_index	= u32( -1 );
	if ( math::is_similar( first_intersection, peak ) )
		first_intersection_index = peak_index;
	else if ( math::is_similar( first_intersection, first_vertex ) )
		first_intersection_index = first_vertex_index;
	else
	{
		first_intersection_index = mesh.vertices.size();
		mesh.vertices.push_back( first_intersection );
	}
	
	u32 second_intersection_index	= u32( -1 );
	if ( math::is_similar( second_intersection, peak ) )
		second_intersection_index = peak_index;
	else if ( math::is_similar( second_intersection, second_vertex ) )
		second_intersection_index = second_vertex_index;
	else if ( math::is_similar( second_intersection, first_intersection ) )
		second_intersection_index = first_intersection_index;
	else
	{
		second_intersection_index = mesh.vertices.size();
		mesh.vertices.push_back( second_intersection );
	}
	u32 first_triangle_id		= bsp_tree_triangle::msc_empty_id;
	u32 second_triangle_id		= bsp_tree_triangle::msc_empty_id;
	u32 third_triangle_id		= bsp_tree_triangle::msc_empty_id;

	if ( first_triangle_square > c_square_threshold )
	{
		first_triangle_id		= mesh.data.size();
		mesh.indices.push_back( peak_index );
		mesh.indices.push_back( first_intersection_index  );
		mesh.indices.push_back( second_intersection_index );
		bsp_tree_triangle first_triangle( mesh.data[ triangle_id ].plane, __LINE__ );
		first_triangle.divider	= mesh.data[ triangle_id ].divider;
		first_triangle.square = first_triangle_square;
		first_triangle.parent = triangle_id;
		mesh.data.push_back( first_triangle );
		R_ASSERT( mesh.data.back().plane.valid() );
		mesh.data[ triangle_id ].children[ 0 ] = first_triangle_id;
		mesh.data[ first_triangle_id ].neighbours[ 0 ]	= right_neighbour_id;
		mesh.data[ first_triangle_id ].neighbours[ 2 ]	= left_neighbour_id;
		if ( calculate_side( p, first_triangle_id, mesh ) == bsp_tree_triangle::front )
		{
			positive_ids.push_back( first_triangle_id );
		}
		else
		{
			negative_ids.push_back( first_triangle_id  );
		}
	}

	if ( second_triangle_square > c_square_threshold )
	{
		second_triangle_id	= mesh.data.size();
		mesh.indices.push_back( first_intersection_index );
		mesh.indices.push_back( first_vertex_index );
		mesh.indices.push_back( second_intersection_index );
		bsp_tree_triangle second_triangle( mesh.data[ triangle_id ].plane, __LINE__ );
		second_triangle.divider	= mesh.data[ triangle_id ].divider;
		second_triangle.square = second_triangle_square;
		second_triangle.parent = triangle_id;
		mesh.data.push_back( second_triangle );
		R_ASSERT( mesh.data.back().plane.valid() );
		mesh.data[ triangle_id ].children[ 1 ] = second_triangle_id;
		mesh.data[ second_triangle_id ].neighbours[ 0 ]	= right_neighbour_id;
		if ( bottom_neighbour_id != bsp_tree_triangle::msc_empty_id )
		{
			mesh.data[ second_triangle_id ].neighbours[ 1 ]	= bottom_neighbour_id;
			replace_neighbour_of_triangle( bottom_neighbour_id, mesh, triangle_id, second_triangle_id );
		}
		if ( calculate_side( p, second_triangle_id, mesh ) == bsp_tree_triangle::front )
		{
			positive_ids.push_back( second_triangle_id );
		}
		else
		{
			negative_ids.push_back( second_triangle_id );
		}
	}

	if ( third_triangle_square > c_square_threshold )
	{
		third_triangle_id	= mesh.data.size();
		mesh.indices.push_back( second_intersection_index );
		mesh.indices.push_back( first_vertex_index  );
		mesh.indices.push_back( second_vertex_index );
		bsp_tree_triangle third_triangle( mesh.data[ triangle_id ].plane, __LINE__ );
		third_triangle.divider	= mesh.data[ triangle_id ].divider;
		third_triangle.square = third_triangle_square;
		third_triangle.parent = triangle_id;
		mesh.data.push_back( third_triangle );
		R_ASSERT( mesh.data.back().plane.valid() );
		mesh.data[ triangle_id ].children[ 2 ] = third_triangle_id;
		if ( second_triangle_id != bsp_tree_triangle::msc_empty_id )
		{
			mesh.data[ third_triangle_id ].neighbours[ 0 ]	= second_triangle_id;
			mesh.data[ second_triangle_id ].neighbours[ 1 ] = third_triangle_id;
			if ( bottom_neighbour_id != bsp_tree_triangle::msc_empty_id )
			{
				replace_neighbour_of_triangle( bottom_neighbour_id, mesh, second_triangle_id, third_triangle_id );
				mesh.data[ third_triangle_id ].neighbours[ 1 ]	= bottom_neighbour_id;
			}
		}
		else
		{
			//mesh.data[ third_triangle_id ].neighbours[ 0 ] = right_neighbour_id;
			if ( bottom_neighbour_id != bsp_tree_triangle::msc_empty_id )
			{
				replace_neighbour_of_triangle( bottom_neighbour_id, mesh, triangle_id, third_triangle_id );
				mesh.data[ third_triangle_id ].neighbours[ 1 ]	= bottom_neighbour_id;
			}
		}
		mesh.data[ third_triangle_id ].neighbours[ 2 ]	= left_neighbour_id;
		if ( calculate_side( p, third_triangle_id, mesh ) == bsp_tree_triangle::front )
		{
			positive_ids.push_back( third_triangle_id );
		}
		else
		{
			negative_ids.push_back( third_triangle_id );
		}
	}
	if ( second_triangle_id	== bsp_tree_triangle::msc_empty_id &&
		 third_triangle_id	== bsp_tree_triangle::msc_empty_id &&	
		 bottom_neighbour_id != bsp_tree_triangle::msc_empty_id )
	{
		replace_neighbour_of_triangle( bottom_neighbour_id, mesh, triangle_id, bsp_tree_triangle::msc_empty_id );
	}

	R_ASSERT( right_neighbour_id	== bsp_tree_triangle::msc_empty_id || is_triangle_adjacency_correct( right_neighbour_id, mesh ) );
	R_ASSERT( left_neighbour_id		== bsp_tree_triangle::msc_empty_id || is_triangle_adjacency_correct( left_neighbour_id, mesh ) );
	R_ASSERT( bottom_neighbour_id	== bsp_tree_triangle::msc_empty_id || is_triangle_adjacency_correct( bottom_neighbour_id, mesh ) );
	if ( right_neighbour_id != bsp_tree_triangle::msc_empty_id )
	{
		R_ASSERT( first_triangle_id == bsp_tree_triangle::msc_empty_id	|| mesh.data[ first_triangle_id ].is_neighbour_of( right_neighbour_id ) );
		R_ASSERT( std::find( positive_ids.begin(), positive_ids.end(), right_neighbour_id ) ==  positive_ids.end() );
		R_ASSERT( std::find( negative_ids.begin(), negative_ids.end(), right_neighbour_id ) ==  negative_ids.end() );
		if ( second_triangle_id != bsp_tree_triangle::msc_empty_id )
		{
			R_ASSERT( second_triangle_id == bsp_tree_triangle::msc_empty_id	|| mesh.data[ second_triangle_id ].is_neighbour_of( right_neighbour_id ) );
			split_triangle_with_point( right_neighbour_id, mesh, triangle_id,
				first_intersection_index, first_triangle_id, second_triangle_id );
		}
		else if ( first_triangle_id != bsp_tree_triangle::msc_empty_id )
		{
			//R_ASSERT( third_triangle_id == bsp_tree_triangle::msc_empty_id	|| mesh.data[ third_triangle_id ].is_neighbour_of( right_neighbour_id ) );
			split_triangle_with_point( right_neighbour_id, mesh, triangle_id,
				first_intersection_index, first_triangle_id, bsp_tree_triangle::msc_empty_id/*third_triangle_id*/ );
		}
		else
		{
			replace_neighbour_of_triangle( right_neighbour_id, mesh, triangle_id, bsp_tree_triangle::msc_empty_id );
		}
	}

	if ( left_neighbour_id != bsp_tree_triangle::msc_empty_id )
	{
		R_ASSERT( left_neighbour_id		== bsp_tree_triangle::msc_empty_id || is_triangle_adjacency_correct( left_neighbour_id, mesh ) );
		R_ASSERT( first_triangle_id == bsp_tree_triangle::msc_empty_id	|| mesh.data[ first_triangle_id ].is_neighbour_of( left_neighbour_id ) );
		R_ASSERT( std::find( positive_ids.begin(), positive_ids.end(), left_neighbour_id ) ==  positive_ids.end() );
		R_ASSERT( std::find( negative_ids.begin(), negative_ids.end(), left_neighbour_id ) ==  negative_ids.end() );
		if ( third_triangle_id != bsp_tree_triangle::msc_empty_id )
		{
			R_ASSERT( third_triangle_id == bsp_tree_triangle::msc_empty_id	|| mesh.data[ third_triangle_id ].is_neighbour_of( left_neighbour_id ) );
			split_triangle_with_point( left_neighbour_id, mesh,	triangle_id, 
				second_intersection_index, third_triangle_id, first_triangle_id );
		}
		else if ( first_triangle_id != bsp_tree_triangle::msc_empty_id )
		{
			//R_ASSERT( second_triangle_id == bsp_tree_triangle::msc_empty_id	|| mesh.data[ second_triangle_id ].is_neighbour_of( left_neighbour_id ) );
			split_triangle_with_point( left_neighbour_id, mesh, triangle_id,
				second_intersection_index, bsp_tree_triangle::msc_empty_id/*second_triangle_id*/, first_triangle_id );
		}
		else
		{
			replace_neighbour_of_triangle( left_neighbour_id, mesh, triangle_id, bsp_tree_triangle::msc_empty_id );
		}
	}
	clear_triangle_neighbours( triangle_id, mesh );
	R_ASSERT( first_triangle_id		== bsp_tree_triangle::msc_empty_id || is_triangle_adjacency_correct( first_triangle_id, mesh ) );
	R_ASSERT( second_triangle_id	== bsp_tree_triangle::msc_empty_id || is_triangle_adjacency_correct( second_triangle_id, mesh ) );
	R_ASSERT( third_triangle_id		== bsp_tree_triangle::msc_empty_id || is_triangle_adjacency_correct( third_triangle_id, mesh ) );
	R_ASSERT( bottom_neighbour_id	== bsp_tree_triangle::msc_empty_id || is_triangle_adjacency_correct( bottom_neighbour_id, mesh ) );
	R_ASSERT( right_neighbour_id != bsp_tree_triangle::msc_empty_id || first_triangle_id == bsp_tree_triangle::msc_empty_id || mesh.data[ first_triangle_id ].neighbours[ 0 ] == bsp_tree_triangle::msc_empty_id );
	R_ASSERT( right_neighbour_id != bsp_tree_triangle::msc_empty_id || second_triangle_id == bsp_tree_triangle::msc_empty_id || mesh.data[ second_triangle_id ].neighbours[ 0 ] == bsp_tree_triangle::msc_empty_id );
	R_ASSERT( left_neighbour_id	 != bsp_tree_triangle::msc_empty_id || first_triangle_id == bsp_tree_triangle::msc_empty_id || mesh.data[ first_triangle_id ].neighbours[ 2 ] == bsp_tree_triangle::msc_empty_id );
	R_ASSERT( left_neighbour_id	 != bsp_tree_triangle::msc_empty_id || third_triangle_id == bsp_tree_triangle::msc_empty_id || mesh.data[ third_triangle_id ].neighbours[ 2 ] == bsp_tree_triangle::msc_empty_id );
	R_ASSERT( bottom_neighbour_id != bsp_tree_triangle::msc_empty_id || third_triangle_id == bsp_tree_triangle::msc_empty_id || mesh.data[ third_triangle_id ].neighbours[ 1 ] == bsp_tree_triangle::msc_empty_id );

	if ( edges != NULL )
	{

		edge_to_u32_type::iterator edge_it = edges->find( edge( peak_index, first_vertex_index ) );
		if ( edge_it != edges->end() )
		{
			u32 const id = edge_it->second;
			edges->erase( edge_it );
			edges->insert( std::make_pair( edge( peak_index, first_intersection_index ), id ) );
			edges->insert( std::make_pair( edge( first_intersection_index, first_vertex_index ), id ) );
		}
		edge_it = edges->find( edge( peak_index, second_vertex_index ) );
		if ( edge_it != edges->end() )
		{
			u32 const id = edge_it->second;
			edges->erase( edge_it );
			edges->insert( std::make_pair( edge( peak_index, second_intersection_index ), id ) );
			edges->insert( std::make_pair( edge( second_intersection_index, second_vertex_index ), id ) );
		}
	}
	return  true;
}

void split_triangle( math::plane const& p, u32 triangle_id, triangles_mesh_type& mesh, triangle_ids_type& positive_ids, triangle_ids_type& negative_ids, edge_to_u32_type* edges )
{
	R_ASSERT( is_triangle_adjacency_correct( triangle_id, mesh ) );
	R_ASSERT( edges == NULL || is_triangle_adjacency_correct( triangle_id, mesh ) );
	R_ASSERT( mesh.data[ triangle_id ].plane.valid() );
	R_ASSERT( !mesh.data[ triangle_id ].has_children() );
	for ( u32 coord_offset = 0; coord_offset < 3; ++coord_offset )
	{
		u32	const vertex_index			= mesh.indices[ triangle_id * 3 + coord_offset ];
		math::float3 const& vertex		= mesh.vertices[ vertex_index ];
		if ( math::is_zero( p.classify( vertex ), math::epsilon_3 ) )
		{
			if ( triangle_split_to_two( p, triangle_id, coord_offset, mesh, positive_ids, negative_ids, edges ) )
			{
				return;
			}
		}
	}
	for ( u32 coord_offset = 0; coord_offset < 3; ++coord_offset )
	{
		if ( triangle_split_to_three( p, triangle_id, coord_offset, mesh, positive_ids, negative_ids, edges ) )
		{
			clear_triangle_neighbours( triangle_id, mesh );
			return;
		}
	}
	UNREACHABLE_CODE();
}

void split_triangle_with_point	( u32 triangle_id, triangles_mesh_type& mesh, u32 old_neighbour,
								  u32 new_point_index, u32 new_neighbour0, u32 new_neighbour1 )
{
	R_ASSERT( is_triangle_adjacency_correct( triangle_id, mesh ) );
	R_ASSERT( new_neighbour0 != bsp_tree_triangle::msc_empty_id || new_neighbour1 != bsp_tree_triangle::msc_empty_id );
	R_ASSERT( !mesh.data[ triangle_id ].has_children() );
	u32 peak_offset = u32( -1 );
	if ( mesh.data[ triangle_id ].neighbours[ 0 ] == old_neighbour )
		peak_offset = 2;
	else if ( mesh.data[ triangle_id ].neighbours[ 1 ] == old_neighbour )
		peak_offset = 0;
	else if ( mesh.data[ triangle_id ].neighbours[ 2 ] == old_neighbour )
		peak_offset = 1;
	else
		UNREACHABLE_CODE();

	u32 const peak_index			= mesh.indices[ triangle_id * 3 + peak_offset ];
	u32 const first_vertex_index	= mesh.indices[ triangle_id * 3 + ( peak_offset + 1 ) % 3 ];
	u32 const second_vertex_index	= mesh.indices[ triangle_id * 3 + ( peak_offset + 2 ) % 3 ];
	
	math::float3 const& peak			= mesh.vertices[ peak_index ];
	math::float3 const& first_vertex	= mesh.vertices[ first_vertex_index ];
	math::float3 const& second_vertex	= mesh.vertices[ second_vertex_index ];
	math::float3 const& new_point		= mesh.vertices[ new_point_index ];

	u32 first_triangle_id = bsp_tree_triangle::msc_empty_id;
	float const first_triangle_square = triangle_square( peak, first_vertex, new_point );
	if ( first_triangle_square > c_square_threshold )
	{
		first_triangle_id = mesh.data.size();
		mesh.indices.push_back( peak_index );
		mesh.indices.push_back( first_vertex_index );
		mesh.indices.push_back( new_point_index );
		bsp_tree_triangle first_triangle( mesh.data[ triangle_id ].plane, __LINE__ );
		first_triangle.divider	= mesh.data[ triangle_id ].divider;
		first_triangle.square = first_triangle_square;
		first_triangle.parent = triangle_id;
		mesh.data.push_back( first_triangle );
		mesh.data[ triangle_id ].children[0] = first_triangle_id;
	}

	u32 second_triangle_id = bsp_tree_triangle::msc_empty_id;
	float const second_triangle_square = triangle_square( peak, new_point, second_vertex );
	if ( second_triangle_square > c_square_threshold )
	{
		second_triangle_id = mesh.data.size();
		mesh.indices.push_back( peak_index );
		mesh.indices.push_back( new_point_index );
		mesh.indices.push_back( second_vertex_index );
		bsp_tree_triangle second_triangle( mesh.data[ triangle_id ].plane, __LINE__ );
		second_triangle.divider	= mesh.data[ triangle_id ].divider;
		second_triangle.square = second_triangle_square;
		second_triangle.parent = triangle_id;
		mesh.data.push_back( second_triangle );
		mesh.data[ triangle_id ].children[1] = second_triangle_id;
		if ( first_triangle_id != bsp_tree_triangle::msc_empty_id )
		{
			mesh.data[ first_triangle_id ].neighbours[ 2 ]	= second_triangle_id;
			mesh.data[ second_triangle_id ].neighbours[ 0 ]	= first_triangle_id;
		}
	}

	u32 const right_neighbour_id = mesh.data[ triangle_id ].neighbours[ peak_offset ];
	if ( right_neighbour_id != bsp_tree_triangle::msc_empty_id )
	{
		if ( first_triangle_id != bsp_tree_triangle::msc_empty_id )
		{
			mesh.data[ first_triangle_id ].neighbours[ 0 ]	= right_neighbour_id;
			replace_neighbour_of_triangle( right_neighbour_id, mesh, triangle_id, first_triangle_id );
		}
		else
			replace_neighbour_of_triangle( right_neighbour_id, mesh, triangle_id, bsp_tree_triangle::msc_empty_id );
		R_ASSERT( is_triangle_adjacency_correct( right_neighbour_id, mesh ) );
	}
	if ( new_neighbour1 != bsp_tree_triangle::msc_empty_id )
	{
		if ( first_triangle_id != bsp_tree_triangle::msc_empty_id )
			mesh.data[ first_triangle_id ].neighbours[ 1 ] = new_neighbour1;
		replace_neighbour_of_triangle( new_neighbour1, mesh, triangle_id, first_triangle_id );
	}

	u32 const left_neighbour_id = mesh.data[ triangle_id ].neighbours[ ( peak_offset + 2 ) % 3 ];
	if ( left_neighbour_id != bsp_tree_triangle::msc_empty_id )
	{
		if ( second_triangle_id != bsp_tree_triangle::msc_empty_id )
		{
			mesh.data[ second_triangle_id ].neighbours[ 2 ]	= left_neighbour_id;
			replace_neighbour_of_triangle( left_neighbour_id, mesh, triangle_id, second_triangle_id );
		}
		else
			replace_neighbour_of_triangle( left_neighbour_id, mesh, triangle_id, bsp_tree_triangle::msc_empty_id );
		R_ASSERT( is_triangle_adjacency_correct( left_neighbour_id, mesh ) );
	}
	if ( new_neighbour0 != bsp_tree_triangle::msc_empty_id )
	{
		if ( second_triangle_id != bsp_tree_triangle::msc_empty_id )
			mesh.data[ second_triangle_id ].neighbours[ 1 ] = new_neighbour0;
		replace_neighbour_of_triangle( new_neighbour0, mesh, triangle_id, second_triangle_id );
	}
	clear_triangle_neighbours( triangle_id, mesh );
	R_ASSERT( first_triangle_id		== bsp_tree_triangle::msc_empty_id || is_triangle_adjacency_correct( first_triangle_id, mesh ) );
	R_ASSERT( second_triangle_id	== bsp_tree_triangle::msc_empty_id || is_triangle_adjacency_correct( second_triangle_id, mesh ) );
	R_ASSERT( left_neighbour_id		== bsp_tree_triangle::msc_empty_id || is_triangle_adjacency_correct( left_neighbour_id, mesh ) );
	R_ASSERT( right_neighbour_id	== bsp_tree_triangle::msc_empty_id || is_triangle_adjacency_correct( right_neighbour_id, mesh ) );
}

bool is_triangle_adjacency_correct( u32 triangle_id, triangles_mesh_type const& mesh )
{
	bsp_tree_triangle const& current = mesh.data[ triangle_id ];
	for ( u32 i = 0; i < bsp_tree_triangle::msc_neihbours_count; ++i )
	{
		if ( current.neighbours[ i ] != bsp_tree_triangle::msc_empty_id )
		{
			bsp_tree_triangle const& neighbour = mesh.data[ current.neighbours[ i ] ];
			if ( neighbour.neighbours[ 0 ] != triangle_id && 
				 neighbour.neighbours[ 1 ] != triangle_id && 
				 neighbour.neighbours[ 2 ] != triangle_id )
				 return false;
			//if ( neighbour.has_children() )
			//	return false;
		}
	}
	return true;
}

void reset_triangle_adjacency( u32 triangle_id, triangles_mesh_type& mesh )
{
	bsp_tree_triangle& current = mesh.data[ triangle_id ];
	if ( current.neighbours[ 0 ] != bsp_tree_triangle::msc_empty_id )
	{
		replace_neighbour_of_triangle( current.neighbours[ 0 ], mesh, triangle_id, bsp_tree_triangle::msc_empty_id );
	}

	if ( current.neighbours[ 1 ] != bsp_tree_triangle::msc_empty_id )
	{
		replace_neighbour_of_triangle( current.neighbours[ 1 ], mesh, triangle_id, bsp_tree_triangle::msc_empty_id );
	}

	if ( current.neighbours[ 2 ] != bsp_tree_triangle::msc_empty_id )
	{
		replace_neighbour_of_triangle( current.neighbours[ 2 ], mesh, triangle_id, bsp_tree_triangle::msc_empty_id );
	}
	clear_triangle_neighbours( triangle_id, mesh );
}

void clear_triangle_neighbours( u32 triangle_id, triangles_mesh_type& mesh )
{
	bsp_tree_triangle& current = mesh.data[ triangle_id ];
	current.neighbours[ 0 ] = bsp_tree_triangle::msc_empty_id;
	current.neighbours[ 1 ] = bsp_tree_triangle::msc_empty_id;
	current.neighbours[ 2 ] = bsp_tree_triangle::msc_empty_id;
}


}// namespace xray
}//namespace model_editor