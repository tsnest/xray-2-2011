////////////////////////////////////////////////////////////////////////////
//	Created		: 29.09.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "graph_generator.h"

#include "delaunay_triangulator.h"

using xray::ai::navigation::graph_generator;
using xray::ai::navigation::triangles_mesh_type;

void graph_generator::test_delaunay( )
{
	clear_geometry					( );

	debug::vector<float3> vertices;
	
	vertices.push_back( float3(-8.0000000, 0.00000000, -6.0000000) );
	vertices.push_back( float3(-8.0000000, 0.00000000, -5.5000000) );
	vertices.push_back( float3(-7.4000001f, 0.00000000, -7.4000001f) );
	vertices.push_back( float3(-8.0000000, 0.00000000, -7.4375000) );
	vertices.push_back( float3(-8.0000000, 0.00000000, -3.0000000) );
	vertices.push_back( float3(-3.0000000, 0.00000000, -3.0000000) );
	vertices.push_back( float3(-8.0000000, 0.00000000, -8.0000000) );

	u32 const max_indices_count = 3 * delaunay_triangulator::get_maximum_triangle_count_in_triangulation( vertices.size() );
	delaunay_triangulator::indices_type indices(
		ALLOCA( max_indices_count * sizeof( delaunay_triangulator::indices_type::value_type ) ),
		max_indices_count
	);

	delaunay_triangulator(
		vertices,
		indices,
		float3( 0.0, 1.0, 0.0 )
	);


	for ( u32 i = 0; i < vertices.size(); ++i )
		m_input_triangles.vertices.push_back( vertices[i] );

	u32 const triangles_count = indices.size() / 3;
	for ( u32 i = 0; i < triangles_count; ++i ) {
		m_input_triangles.indices.push_back( indices[i*3+0] );
		m_input_triangles.indices.push_back( indices[i*3+1] );
		m_input_triangles.indices.push_back( indices[i*3+2] );

		m_input_triangles.data.push_back( navigation_triangle() );

	}

	m_input_triangles.build_spatial_tree ( );
}
