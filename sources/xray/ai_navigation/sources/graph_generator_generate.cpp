////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if XRAY_DEBUG_ALLOCATOR

#include "graph_generator.h"
#include "graph_generator_subdivider.h"
#include "graph_generator_tessellator.h"
#include "graph_generator_extruder.h"
#include "graph_generator_merger.h"
#include "graph_generator_fuser.h"
#include "graph_generator_predicates.h"
#include "graph_generator_t_junction_remover.h"
#include "graph_generator_adjacency_builder.h"
#include "graph_generator_disconnected_regions_remover.h"
#include "constrained_delaunay_triangulator.h"
#include "navigation_mesh_functions.h"

#include <xray/collision/geometry_instance.h>
#include <xray/collision/geometry.h>
#include <xray/math_randoms_generator.h>
#include <xray/fs/file_type_pointer.h>

using xray::ai::navigation::graph_generator;
using xray::ai::navigation::triangles_mesh_type;
using xray::ai::navigation::vertices_type;
using xray::math::float3;
using xray::math::float4x4;

void graph_generator::fill_triangles			( xray::collision::geometry_instance* geometry, triangles_mesh_type& result, float4x4 transform )
{

	u32 const index_count_old			= result.indices.size();
	u32 const index_count				= index_count_old + geometry->index_count( );
	R_ASSERT_CMP						( index_count % 3, ==, 0, "index count is not dividable by 3: %d", index_count );
	u32 const triangle_count			= index_count / 3;
	result.data.reserve					( 8*triangle_count );
	result.data.resize					( triangle_count );

	u32 const* const indices			= geometry->indices( );
	for (u32 i=0; i<triangle_count; ++i)
		R_ASSERT_CMP					( (geometry->indices(i) - indices) % 3, ==, 0 );

	result.indices.reserve				( 8*index_count );

	u32 const vertex_count_old			= result.vertices.size();
	u32 const vertex_count				= vertex_count_old + geometry->vertex_count();
	result.vertices.reserve				( 2*vertex_count );

	for ( u32 i = index_count_old; i < index_count; ++i )
		result.indices.push_back( indices[i-index_count_old] + vertex_count_old );

	transform			= geometry->get_matrix( ) * transform;

	float3 const* vertices = geometry->vertices();
	for ( u32 i = vertex_count_old; i < vertex_count; ++i ) {
		result.vertices.push_back( transform.transform_position( vertices[i-vertex_count_old] ) );
	}
}

void graph_generator::remove_invalid_triangles	( triangles_mesh_type& result )
{
	
	u32 const triangles_count	= result.data.size();
	for ( u32 i = 0; i < triangles_count; ++i ) {
		float3 const& triangle_vertex0	= result.vertices[ result.indices[ i*3+0 ] ];
		float3 const& triangle_vertex1	= result.vertices[ result.indices[ i*3+1 ] ];
		float3 const& triangle_vertex2	= result.vertices[ result.indices[ i*3+2 ] ];

		if ( is_similar( triangle_vertex0, triangle_vertex1 )
			|| is_similar( triangle_vertex0, triangle_vertex2 )
			|| is_similar( triangle_vertex1, triangle_vertex2 ) )
		{
			result.data[i].is_marked = true;
			continue;
		}

		if ( is_null_square( triangle_vertex0, triangle_vertex1, triangle_vertex2 ) )
			result.data[ i ].is_marked = true;
	}

	result.remove_marked_triangles();

	R_ASSERT_CMP ( result.indices.size() % 3, ==, 0);

}

void graph_generator::mark_passable_triangles	( triangles_mesh_type& result )
{
	float3 const up( 0.f, 1.f, 0.f );

	triangles_mesh_type::data_type::iterator i			= result.data.begin( );
	triangles_mesh_type::data_type::iterator const e	= result.data.end( );
	for (u32 j=0; i != e; ++i, ++j ) {
		(*i).plane						= math::create_plane( result.vertices[result.indices[3*j]], result.vertices[result.indices[3*j + 1]], result.vertices[result.indices[3*j + 2]] );
		(*i).is_passable				= acos((*i).plane.normal | up) < math::pi_d8;
	}
}

float project_vertex (float3 const& v0, float3 const& v1, float3 const& p)
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

static inline float3 project_vertex_on_segment (float3 const& vertex, float3 const& segment_vertex0, float3 const& segment_vertex1 )
{
	float3 v = normalize( segment_vertex1 - segment_vertex0 );
	float k = ( vertex - segment_vertex0 ) | v;
	return segment_vertex0 + v * k;
}

bool is_on_segment	(
		xray::math::float3 const& testee,				// t
		xray::math::float3 const& first_segment_vertex,	// v0
		xray::math::float3 const& second_segment_vertex	// v1
	);

/*
struct triangle_reindexer {
	triangle_reindexer ( triangles_mesh_type const triangles_mesh ) :
		m_triangles_mesh ( &triangles_mesh )
	{
	}

	void operator() ( indices_type const& unique vertices_indices )
	{

	}
private:
	triangles_mesh_type*		m_triangles_mesh;
}; // struct triangle_reindexer
*/

void graph_generator::remove_duplicates ( triangles_mesh_type& triangles_mesh )
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
				std::swap ( triangles_mesh.data[i].obstructions[0],  triangles_mesh.data[i].obstructions[2]);
				std::swap ( triangles_mesh.data[i].obstructions[0],  triangles_mesh.data[i].obstructions[1]);
				break;
			}
			case 2: {
				std::swap ( indices[0], indices[2] );
				std::swap ( indices[1], indices[2] );
				std::swap ( triangles_mesh.data[i].obstructions[0],  triangles_mesh.data[i].obstructions[1]);
				std::swap ( triangles_mesh.data[i].obstructions[0],  triangles_mesh.data[i].obstructions[2]);
				break;
			}
			default: NODEFAULT( );
		}
	}

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

		float3* vertices[] = {
			&triangles_mesh.vertices[ triangles_mesh.indices[ 3*(*i) + 0 ] ],
			&triangles_mesh.vertices[ triangles_mesh.indices[ 3*(*i) + 1 ] ],
			&triangles_mesh.vertices[ triangles_mesh.indices[ 3*(*i) + 2 ] ]
		};

		if ( is_zero_square( *vertices[0], *vertices[1], *vertices[2] ) )
			equal = true;

		if ( !equal ) {
			for ( u32 j = 0; j < 3; ++j ) {
				if ( ::is_on_segment( *vertices[(j+2)%3], *vertices[(j+0)%3], *vertices[(j+1)%3] ) ) {
					(*vertices[(j+2)%3]) = project_vertex_on_segment( *vertices[(j+2)%3], *vertices[(j+0)%3], *vertices[(j+1)%3] );
					equal = true;
					break;
				}
			}
		}

		if ( equal )
			triangles_mesh.data[ *i ].is_marked = true;
	}
	triangles_mesh.remove_marked_triangles ( );
	R_ASSERT_CMP ( triangles_mesh.indices.size() % 3, ==, 0);
}

void graph_generator::remove_non_passable_triangles ( triangles_mesh_type& result )
{
	u32 const triangles_count = result.data.size();
	for ( u32 i = 0; i < triangles_count; ++i )
		if ( !result.data[ i ].is_passable )
			result.data[ i ].is_marked = true;

	result.remove_marked_triangles( );
}

void fill_sample_triangles					( triangles_mesh_type& triangles_mesh )
{
	triangles_mesh.data.clear();
	triangles_mesh.indices.clear();
	triangles_mesh.vertices.clear();

	u32	const width		= 11;
	u32 const height	= 11;

	triangles_mesh.vertices.reserve ( width * height );
	triangles_mesh.data.resize (2*(width-1)*(height-1));

	for (u32 i = 0; i < width; ++i )
		for ( u32 j = 0; j < height; ++j ) {
			triangles_mesh.vertices.push_back( xray::math::float3( (float)i, 0.f, (float)j ) );
		}

	for (u32 i = 0; i < width - 1; ++i )
		for ( u32 j = 0; j < height - 1; ++j ) {
			triangles_mesh.indices.push_back ( (j)*width+i );
			triangles_mesh.indices.push_back ( (j)*width+i+1 );
			triangles_mesh.indices.push_back ( (j+1)*width+i+1 );

			triangles_mesh.indices.push_back ( (j+1)*width+i+1 );
			triangles_mesh.indices.push_back ( (j+1)*width+i );
			triangles_mesh.indices.push_back ( (j)*width+i );

		}
}

void push_triangle ( triangles_mesh_type& triangles_mesh, u32 v0, u32 v1, u32 v2) {
	triangles_mesh.indices.push_back( v0 );
	triangles_mesh.indices.push_back( v1 );
	triangles_mesh.indices.push_back( v2 );
	triangles_mesh.data.push_back ( xray::ai::navigation::navigation_triangle() );
}

void load_obj_file ( const char* fname, triangles_mesh_type& triangles_mesh )
{
	char buffer[1024];

	using namespace xray;
	using namespace xray::fs_new;
	synchronous_device_interface const & device	=	resources::get_synchronous_device();
	file_type_pointer	file(fname, device, file_mode::open_existing, file_access::read);
		
	if ( !file ) {
		LOG_TRACE("Error Loading test geometry ");
		return;
	}

	while ( device->read( file, buffer, 1024) ) {
		if ( buffer[0] == 'v' ) {
			float3 vertex;
			XRAY_SSCANF	( buffer, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z );
			triangles_mesh.vertices.push_back( vertex );
			continue;
		}

		if ( buffer[0] == 'f' ) {
			u32 indices[3];
			XRAY_SSCANF		( buffer, "f %d %d %d",  indices, indices+1, indices+2 );

			triangles_mesh.indices.push_back( indices[0] - 1);
			triangles_mesh.indices.push_back( indices[1] - 1);
			triangles_mesh.indices.push_back( indices[2] - 1);

			triangles_mesh.data.push_back	( xray::ai::navigation::navigation_triangle() );
		}
	}
}

void make_bridge( triangles_mesh_type& triangles_mesh )
{
	float const	height				= 0.87215608f;

	//float const	height				= 0;
	u32 		vertices_offset		= triangles_mesh.vertices.size();

	triangles_mesh.vertices.push_back( float3( 4,  height,  12) );
	triangles_mesh.vertices.push_back( float3(10,  height,  12) );
	triangles_mesh.vertices.push_back( float3(10,  height,  7) );
	triangles_mesh.vertices.push_back( float3( 4,  height,  7) );

	push_triangle(triangles_mesh, vertices_offset, vertices_offset+1, vertices_offset+2 );
	push_triangle(triangles_mesh, vertices_offset+2, vertices_offset+3, vertices_offset );

	vertices_offset		= triangles_mesh.vertices.size();
	triangles_mesh.vertices.push_back( float3( 4.8f,  3+height,  42) );
	triangles_mesh.vertices.push_back( float3( 9.5f,  3+height,  42) );
	triangles_mesh.vertices.push_back( float3( 9.5f,  3+height,  37) );
	triangles_mesh.vertices.push_back( float3( 4.8f,  3+height,  37) );

	push_triangle(triangles_mesh, vertices_offset, vertices_offset+1, vertices_offset+2 );
	push_triangle(triangles_mesh, vertices_offset+2, vertices_offset+3, vertices_offset );

}

void make_coplanar_triangles_tests( triangles_mesh_type& result )
{

	// Simple Case
	u32 vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(-8, 0, -3) );
	result.vertices.push_back( float3(-3, 0, -3) );
	result.vertices.push_back( float3(-8, 0, -8) );
	push_triangle(result, vertices_offset, vertices_offset+1, vertices_offset+2 );

	vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(-6.5, 0, -4.5) );
	result.vertices.push_back( float3(-9, 0, -7) );
	result.vertices.push_back( float3(-7, 0, -4) );
	push_triangle(result, vertices_offset, vertices_offset+1, vertices_offset+2 );

	// Hierarchical intersect
	vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(-10, 0, -9) );
	result.vertices.push_back( float3(-9, 0, -7.5) );
	result.vertices.push_back( float3(-1, 0, -7) );
	push_triangle(result, vertices_offset, vertices_offset+1, vertices_offset+2 );

	// David Star
	vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(-10, 0, -20) );
	result.vertices.push_back( float3(-20, 0, -20) );
	result.vertices.push_back( float3(-15, 0, -10) );
	push_triangle(result, vertices_offset, vertices_offset+1, vertices_offset+2 );

	vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(-10, 0, -10) );
	result.vertices.push_back( float3(-15, 0, -25) );
	result.vertices.push_back( float3(-20, 0, -15) );
	push_triangle(result, vertices_offset, vertices_offset+1, vertices_offset+2 );

	// Single edge case #1
	vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(-15, 0, -35) );
	result.vertices.push_back( float3(-10, 0, -45) );
	result.vertices.push_back( float3(-20, 0, -45) );
	push_triangle(result, vertices_offset, vertices_offset+1, vertices_offset+2 );

	vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(-15, 0, -30) );
	result.vertices.push_back( float3(-10, 0, -40) );
	result.vertices.push_back( float3(-20, 0, -40) );
	push_triangle(result, vertices_offset, vertices_offset+1, vertices_offset+2 );

	// Single edge case #2
	vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(-15, 0, -55) );
	result.vertices.push_back( float3(-10, 0, -65) );
	result.vertices.push_back( float3(-20, 0, -65) );
	push_triangle(result, vertices_offset, vertices_offset+1, vertices_offset+2 );

	vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(-10, 0, -50) );
	result.vertices.push_back( float3(-15, 0, -60) );
	result.vertices.push_back( float3(-20, 0, -50) );
	push_triangle(result, vertices_offset, vertices_offset+1, vertices_offset+2 );
	
	// Single edge case #3
	vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(-15, 0, -75) );
	result.vertices.push_back( float3(-10, 0, -85) );
	result.vertices.push_back( float3(-20, 0, -85) );
	push_triangle(result, vertices_offset, vertices_offset+1, vertices_offset+2 );

	vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(-15, 0, -90) );
	result.vertices.push_back( float3(-30, 0, -80) );
	result.vertices.push_back( float3(  0, 0, -80) );
	push_triangle(result, vertices_offset, vertices_offset+1, vertices_offset+2 );

	// Single edge case #4
	vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(-35, 0, -35) );
	result.vertices.push_back( float3(-40, 0, -45) );
	result.vertices.push_back( float3(-50, 0, -45) );
	push_triangle(result, vertices_offset, vertices_offset+1, vertices_offset+2 );

	vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(-35, 0, -40) );
	result.vertices.push_back( float3(-30, 0, -50) );
	result.vertices.push_back( float3(-60, 0, -50) );
	push_triangle(result, vertices_offset, vertices_offset+1, vertices_offset+2 );

	// Single edge case #5
	vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(-55, 0, -55) );
	result.vertices.push_back( float3(-55, 0, -65) );
	result.vertices.push_back( float3(-70, 0, -70) );
	push_triangle(result, vertices_offset, vertices_offset+1, vertices_offset+2 );

	vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(-55, 0, -55) );
	result.vertices.push_back( float3(-55, 0, -65) );
	result.vertices.push_back( float3(-70, 0, -65) );
	push_triangle(result, vertices_offset, vertices_offset+1, vertices_offset+2 );

	// Single edge case #6
	vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(10, 0, -65) );
	result.vertices.push_back( float3(15, 0, -55) );
	result.vertices.push_back( float3(20, 0, -65) );
	push_triangle(result, vertices_offset, vertices_offset+1, vertices_offset+2 );

	vertices_offset = result.vertices.size();
	result.vertices.push_back( float3( 0, 0, -70) );
	result.vertices.push_back( float3(15, 0, -60) );
	result.vertices.push_back( float3(30, 0, -70) );
	push_triangle(result, vertices_offset, vertices_offset+1, vertices_offset+2 );

	// Duplicated triangles
	vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(-65, 0, -35) );
	result.vertices.push_back( float3(-70, 0, -45) );
	result.vertices.push_back( float3(-80, 0, -45) );
	push_triangle(result, vertices_offset, vertices_offset+1, vertices_offset+2 );
	push_triangle(result, vertices_offset, vertices_offset+1, vertices_offset+2 );

	vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(-65, 0, -35) );
	result.vertices.push_back( float3(-70, 0, -45) );
	result.vertices.push_back( float3(-80, 0, -45) );
	push_triangle(result, vertices_offset, vertices_offset+1, vertices_offset+2 );

}

void make_non_coplanar_triangles_tests( triangles_mesh_type& result ) 
{
	u32 vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(30, 0, -45) );
	result.vertices.push_back( float3(35, 0, -35) );
	result.vertices.push_back( float3(40, 0, -45) );
	push_triangle(result, vertices_offset, vertices_offset+1, vertices_offset+2 );
	
	vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(30, 3, -50) );
	result.vertices.push_back( float3(35,-1, -40) );
	result.vertices.push_back( float3(40, 3, -50) );
	push_triangle(result, vertices_offset, vertices_offset+1, vertices_offset+2 );

	vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(25, 2, -60) );
	result.vertices.push_back( float3(33, 2, -40) );
	result.vertices.push_back( float3(50, 2, -60) );
	push_triangle(result, vertices_offset, vertices_offset+1, vertices_offset+2 );

	/*
	// Slope
	vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(55, 3, -60) );
	result.vertices.push_back( float3(70, 3, -60) );
	result.vertices.push_back( float3(55, 3, -40) );
	result.vertices.push_back( float3(70, 3, -40) );

	push_triangle(result, vertices_offset, vertices_offset+2, vertices_offset+1 );
	push_triangle(result, vertices_offset+1, vertices_offset+2, vertices_offset+3 );

	vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(55, 3, -39.9f) );
	result.vertices.push_back( float3(70, 3, -39.9f) );
	result.vertices.push_back( float3(55, 0, -20.1f) );
	result.vertices.push_back( float3(70, 0, -20.1f) );

	push_triangle(result, vertices_offset, vertices_offset+2, vertices_offset+1 );
	push_triangle(result, vertices_offset+1, vertices_offset+2, vertices_offset+3 );

	vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(55, 0, -20) );
	result.vertices.push_back( float3(70, 0, -20) );
	result.vertices.push_back( float3(55, 0,   0) );
	result.vertices.push_back( float3(70, 0,   0) );

	push_triangle(result, vertices_offset, vertices_offset+2, vertices_offset+1 );
	push_triangle(result, vertices_offset+1, vertices_offset+2, vertices_offset+3 );
	*/

}

void make_fuse_tests ( triangles_mesh_type& result)
{
	u32 vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(120, 3, -60) );
	result.vertices.push_back( float3(105, 3, -60) );
	result.vertices.push_back( float3(105, 3, -70) );

	push_triangle(result, vertices_offset, vertices_offset+2, vertices_offset+1 );

	vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(115, 3.0f, -59.9f) );
	result.vertices.push_back( float3(105, 3.0f, -30) );
	result.vertices.push_back( float3(100, 3.0f, -59.9f) );

	push_triangle(result, vertices_offset, vertices_offset+2, vertices_offset+1 );

	vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(80, 3, -60) );
	result.vertices.push_back( float3(104.9f, 3, -60) );
	result.vertices.push_back( float3(104.80f, 3, -67) );

	push_triangle(result, vertices_offset, vertices_offset+1, vertices_offset+2 );

}

void make_t_junctions_remover_tests ( triangles_mesh_type& result )
{
	u32 vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(135, 3, -60) );
	result.vertices.push_back( float3(130, 3, -60) );
	result.vertices.push_back( float3(135, 3, -70) );

	push_triangle(result, vertices_offset, vertices_offset+2, vertices_offset+1 );

	vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(130, 3, -60) );
	result.vertices.push_back( float3(132, 3, -60) );
	result.vertices.push_back( float3(132, 3, -55) );

	push_triangle(result, vertices_offset, vertices_offset+2, vertices_offset+1 );

	vertices_offset = result.vertices.size();
	result.vertices.push_back( float3(135, 3, -65) );
	result.vertices.push_back( float3(135, 3, -70) );
	result.vertices.push_back( float3(137, 3, -65) );
	push_triangle(result, vertices_offset, vertices_offset+2, vertices_offset+1 );

}

void make_random_obstructions ( triangles_mesh_type& triangles_mesh )
{
	xray::math::random32 random(10);
	u32 const max_constraints = 20;
	u32 const triangles_count = triangles_mesh.data.size();
	for ( u32 i = 0; i < max_constraints; ++i )
		triangles_mesh.data[ random( triangles_count ) ].obstructions[ random(3) ] = 1;
}

void filter_triangles( triangles_mesh_type& triangles_mesh )
{
	u32 const triangles_count = triangles_mesh.data.size();
	for ( u32 i = 0; i < triangles_count; ++i ) {
		switch ( i ) {
		case 0:
		case 1517:
		case 1518:
		case 2410:
		case 2703:
			break;
		default:
			triangles_mesh.data[i].is_marked = true;
		}
	}
	triangles_mesh.remove_marked_triangles();
}

typedef xray::buffer_vector< u32 > vertices_indices_type;

struct remove_vertices_if_unused {
	remove_vertices_if_unused( vertices_indices_type& vertices_indices, triangles_mesh_type::vertices_type::iterator begin ) :
		m_begin( begin ),
		m_vertices_indices( &vertices_indices )
	{
	}

	bool operator() ( triangles_mesh_type::vertices_type::value_type const& v )
	{
		u32 const index = &v - m_begin;
		return (*m_vertices_indices)[ index ] == u32(-1);
	}
private:
	triangles_mesh_type::vertices_type::iterator	m_begin;
	vertices_indices_type*							m_vertices_indices;

}; // struct remove_vertices_if_unused

void graph_generator::remove_unused_vertices( triangles_mesh_type& triangles_mesh )
{
	u32 const vertices_count = triangles_mesh.vertices.size();


	vertices_indices_type vertices_indices (
		ALLOCA( vertices_count * sizeof( u32 ) ),
		vertices_count,
		vertices_count,
		u32(-1)
	);


	u32 const indices_count = triangles_mesh.indices.size();
	for ( u32 i = 0; i < indices_count; ++i ) {
		vertices_indices[ triangles_mesh.indices[i] ] = 1;	
	}
	
	for ( u32 j = 0, i = 0; i < vertices_count; ++i ) {
		if ( vertices_indices[i] != u32(-1) ) {
			vertices_indices[i] = j;
			++j;
		}
	}

	for ( u32 i = 0; i < indices_count; ++i )
		triangles_mesh.indices[i] = vertices_indices[ triangles_mesh.indices[i] ];

	triangles_mesh.vertices.erase(
		std::remove_if(
			triangles_mesh.vertices.begin(),
			triangles_mesh.vertices.end(),
			remove_vertices_if_unused( vertices_indices, triangles_mesh.vertices.begin() )
		),
		triangles_mesh.vertices.end()
	);


}

void graph_generator::clear_geometry		( )
{
	m_input_triangles.vertices.clear				( );
	m_input_triangles.indices.clear					( );
	m_input_triangles.data.clear					( );
	m_input_triangles.destroy_spatial_tree			( );
}

void graph_generator::generate				( )
{
//	m_input_triangles.vertices.clear				( );
//	m_input_triangles.indices.clear					( );
//	m_input_triangles.data.clear					( );
	m_input_triangles.set_save_marked				( false );

#if 0
	load_obj_file									( "C:/dungeon.obj", m_input_triangles );
#else
//	fill_triangles									( m_geometry, m_input_triangles, xray::math::create_translation( float3( 0, 0, 0)) );
//	fill_triangles									( m_geometry, m_input_triangles, xray::math::create_translation( float3( 15, 0, 0)) );
//	fill_triangles									( m_geometry, m_input_triangles, xray::math::create_translation( float3(  0, 3, 30)) );
//	fill_triangles									( m_geometry, m_input_triangles, xray::math::create_translation( float3( 15, 3, 30)) );
//	make_bridge										( m_input_triangles );
//	make_fuse_tests									( m_input_triangles );
//	make_coplanar_triangles_tests					( m_input_triangles );
//	make_non_coplanar_triangles_tests				( m_input_triangles );
//	fill_sample_triangles							( m_input_triangles );
//	make_t_junctions_remover_tests					( m_input_triangles );
#endif // #if 1

	remove_invalid_triangles						( m_input_triangles );
	mark_passable_triangles							( m_input_triangles );							// inplace!

	m_input_triangles.build_spatial_tree			( );
	remove_restricted_areas							( m_input_triangles, m_restricted_areas );
	m_input_triangles.destroy_spatial_tree			( );

	if ( m_input_triangles.data.empty() )
		return;

	/* Debug */
	//filter_triangles								( m_input_triangles );
	//m_input_triangles.set_save_marked				( true );
	/**/
	
	/*
	m_input_triangles.build_spatial_tree();
	return;
	*/

	//remove_duplicates								( m_input_triangles );

	m_input_triangles.build_spatial_tree			( );
	//graph_generator_tessellator						( m_input_triangles, m_tessellation_max_operation_id );
	m_input_triangles.destroy_spatial_tree			( );
	
	remove_duplicates								( m_input_triangles );
	
	graph_generator_adjacency_builder (
		m_input_triangles.indices,
		m_input_triangles.data,
		m_input_triangles.data,
		math::epsilon_3
	);

	m_input_triangles.build_spatial_tree			( );
	//graph_generator_fuser							( m_input_triangles, m_fuser_max_operation_id );
	m_input_triangles.destroy_spatial_tree			( );
	
	/* Debug */
	m_debug_edges.clear();
	/**/
	
	m_input_triangles.build_spatial_tree			( );
	graph_generator_merger							( m_input_triangles, m_merger_max_operation_id, 0.01f, 0.001f, m_merge_delaunay_max_operation_id, m_debug_edges );
	m_input_triangles.destroy_spatial_tree			( );

	remove_non_passable_triangles					( m_input_triangles );

	remove_duplicates								( m_input_triangles );
	
	graph_generator_adjacency_builder (
		m_input_triangles.indices,
		m_input_triangles.data,
		m_input_triangles.data,
		math::epsilon_3
	);
	
	//graph_generator_disconnected_regions_remover	( m_input_triangles, m_min_agent_radius, m_min_agent_height );
	graph_generator_adjacency_builder (
		m_input_triangles.indices,
		m_input_triangles.data
	);
	
	/* Debug Render */
	m_input_triangles.build_spatial_tree();
	/**/

	remove_unused_vertices				( m_input_triangles );
/*
	* remove invalid triangles
	* mark passable
	* tessellate
	* remove duplicate vertices
	* check duplicated triangles
	* build adjacency tree
	* fuse
	* rebuild adjacency tree
	* construct regions
	* remove non passable triangles
	* remove disconnected regions
	* rebuild adjacency tree
	* serialize
*/

}

#endif // #if XRAY_DEBUG_ALLOCATOR
