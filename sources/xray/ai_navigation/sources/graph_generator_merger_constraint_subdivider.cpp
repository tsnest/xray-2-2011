////////////////////////////////////////////////////////////////////////////
//	Created		: 25.08.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "graph_generator_merger.h"
#include <xray/collision/api.h>
#include <xray/collision/space_partitioning_tree.h>
#include <xray/collision/object.h>
using xray::ai::navigation::graph_generator_merger;
using xray::ai::navigation::triangles_mesh_type;
using xray::ai::navigation::constrained_delaunay_triangulator;
using xray::collision::objects_type;
using xray::math::float3;

//! Should subdivide using
//! http://learn.tsinghua.edu.cn:8080/2008210718/%E6%96%87%E7%8C%AE.pdf
//! Algorithm #2
//! http://softsurfer.com/Archive/algorithm_0108/algorithm_0108.htm
void graph_generator_merger::try_subdivide_constraint_edge (
		u32 const (&coordinate_indices)[2],
		u32 const constraint_id,
		float3 const& v0,
		float3 const& v1,
		buffer_indices_type& constraint_edge_indices
	)
{
	if ( xray::math::is_similar( v0[ coordinate_indices[0] ], v1[ coordinate_indices[0] ] ) &&
		xray::math::is_similar( v0[ coordinate_indices[1] ], v1[ coordinate_indices[1] ] ) )
	{
		return;
	}

	u32 const constraint_edge_indices_count = constraint_edge_indices.size();
	buffer_indices_type new_constraint_edge_indices(
		ALLOCA( constraint_edge_indices_count * sizeof( buffer_indices_type::value_type ) ),
		constraint_edge_indices_count,
		constraint_edge_indices.begin(),
		constraint_edge_indices.end()
	);

	if ( !subdivide_constraint_edge( coordinate_indices, constraint_id, v0, v1, new_constraint_edge_indices ) ) {
		u32 constraint_indices[2] = { u32(-1), u32(-1) };
		float3 const* parent_vertices[] = {
			&m_triangles_mesh.vertices[ m_constraint_edges[ constraint_id ].vertex_index0 ],
			&m_triangles_mesh.vertices[ m_constraint_edges[ constraint_id ].vertex_index1 ]
		};

		if ( is_similar( v0, *parent_vertices[0] ) )
			constraint_indices[0] = m_constraint_edges[ constraint_id ].vertex_index0;
		if ( is_similar( v0, *parent_vertices[1] ) )
			constraint_indices[0] = m_constraint_edges[ constraint_id ].vertex_index1;

		if ( is_similar( v1, *parent_vertices[0] ) )
			constraint_indices[1] = m_constraint_edges[ constraint_id ].vertex_index0;
		if ( is_similar( v1, *parent_vertices[1] ) )
			constraint_indices[1] = m_constraint_edges[ constraint_id ].vertex_index1;

		if ( constraint_indices[0] == u32(-1) ) {
			constraint_indices[0] = m_triangles_mesh.vertices.size();
			m_triangles_mesh.vertices.push_back( v0 );
		}
		if ( constraint_indices[1] == u32(-1) ) {
			constraint_indices[1] = m_triangles_mesh.vertices.size();
			m_triangles_mesh.vertices.push_back( v1 );
		}
		
		if ( constraint_indices[1] < constraint_indices[0] )
			std::swap( constraint_indices[0], constraint_indices[1] );

		m_constraint_edges.push_back ( 
			constraint_edge (
				constraint_indices[0],
				constraint_indices[1],
				m_constraint_edges[ constraint_id ].is_region_edge,
				m_constraint_edges[ constraint_id ].edge_direction,
				m_constraint_edges[ constraint_id ].obstruction_height
			)
		);
	}
}

bool is_on_segment	(
		xray::math::float3 const& testee,				// t
		xray::math::float3 const& first_segment_vertex,	// v0
		xray::math::float3 const& second_segment_vertex	// v1
	);
bool is_on_segment	(
		u32 const (&coordinate_indices)[2],
		xray::math::float3 const& testee,				// t
		xray::math::float3 const& first_segment_vertex,	// v0
		xray::math::float3 const& second_segment_vertex,// v1
		float epsilon
	);

bool graph_generator_merger::subdivide_constraint_edge (
		u32 const (&coordinate_indices)[2],
		u32 const constraint_id,
		float3 const& v0,
		float3 const& v1,
		buffer_indices_type& constraint_edge_indices
	)
{
	while ( !constraint_edge_indices.empty() ) {
		buffer_indices_type::iterator i = constraint_edge_indices.end() - 1;
		
		if ( constraint_id == (*i) ) {
			constraint_edge_indices.erase ( i );
			continue;
		}
		
		u32 const intersector_id = *i;

		float3 const u0 = m_triangles_mesh.vertices[ m_constraint_edges[(*i)].vertex_index0 ];
		float3 const u1 = m_triangles_mesh.vertices[ m_constraint_edges[(*i)].vertex_index1 ];

		bool are_on_same_line;
		float3 result_vertices[2];
		int intersections_count = segment_intersects_segment(
			coordinate_indices,
			v0, v1,
			u0, u1,
			result_vertices[0],
			result_vertices[1],
			are_on_same_line
		);

		constraint_edge_indices.erase ( i );

		if ( intersections_count == 0 ) {
			R_ASSERT( !is_on_segment( coordinate_indices, u0, v0, v1 ) );
			R_ASSERT( !is_on_segment( coordinate_indices, u1, v0, v1 ) );
			continue;
		}

		if ( intersections_count == 1 ) {
			u32 colinear_test = ::is_on_segment( u0, v0, v1 ) ? 1 : 0;
			colinear_test += ::is_on_segment( u1, v0, v1 ) ? 1 : 0;

//			R_ASSERT ( colinear_test != 2 );

			if ( is_similar( v0, result_vertices[0] ) || is_similar( v1, result_vertices[0] ) )
				continue;

			try_subdivide_constraint_edge(coordinate_indices, constraint_id, v0, result_vertices[0], constraint_edge_indices );
			try_subdivide_constraint_edge(coordinate_indices, constraint_id, result_vertices[0], v1, constraint_edge_indices );
			return true;
		} else {
			R_ASSERT_CMP( intersections_count, ==, 2 );

			float const squared_length0 = squared_length( result_vertices[0] - v0 );
			float const squared_length1 = squared_length( result_vertices[1] - v0 );
			if ( squared_length1 < squared_length0 )
				std::swap( result_vertices[0], result_vertices[1] );

			if ( m_constraint_edges[ intersector_id ].is_region_edge ) {
				m_constraint_edges[ constraint_id ].is_region_edge = true;
				m_constraint_edges[ constraint_id ].edge_direction = m_constraint_edges[ intersector_id ].edge_direction;
			}

			if ( is_similar( v0, result_vertices[0] ) && is_similar( v1, result_vertices[1] ) )
				continue;

			try_subdivide_constraint_edge(coordinate_indices, constraint_id, v0, result_vertices[0], constraint_edge_indices );
			try_subdivide_constraint_edge(coordinate_indices, constraint_id, result_vertices[0], result_vertices[1], constraint_edge_indices );
			try_subdivide_constraint_edge(coordinate_indices, constraint_id, result_vertices[1], v1, constraint_edge_indices );
			return true;
		}
	}
	return false;
}

typedef xray::buffer_vector< bool > markers_type;

void graph_generator_merger::subdivide_constraint_edge ( 
		u32 const (&coordinate_indices)[2],
		u32 const constraint_id, 
		objects_type& objects, 
		markers_type& markers
	)
{
	u32 const constraint_count = objects.size();
	buffer_indices_type constraint_edges (
		ALLOCA( constraint_count * sizeof( buffer_indices_type::value_type ) ),
		constraint_count
	);
	
	objects_type::iterator i = objects.begin();
	objects_type::iterator const e = objects.end();
	for ( ; i != e; ++i ) 
		constraint_edges.push_back( reinterpret_cast<u32>((*i)->user_data() ) );


	u32 vertex_index0 = m_constraint_edges[ constraint_id ].vertex_index0;
	u32 vertex_index1 = m_constraint_edges[ constraint_id ].vertex_index1;

	float3 const v0 = m_triangles_mesh.vertices[ vertex_index0 ];
	float3 const v1 = m_triangles_mesh.vertices[ vertex_index1 ];
	if ( subdivide_constraint_edge ( coordinate_indices, constraint_id, v0, v1, constraint_edges ) )
		markers[ constraint_id ] = true;
}

void graph_generator_merger::subdivide_constraint_edges ( float3 const& normal )
{
	u32 coordinate_indices[2];
	fill_coordinate_indices( normal, coordinate_indices );

	non_null<collision::space_partitioning_tree>::ptr	spatial_tree = collision::new_space_partitioning_tree (
		&debug::g_mt_allocator,
		1.0,
		1
	);

	u32 const constraint_edges_count = m_constraint_edges.size();

	typedef buffer_vector< collision::object* > object_list_type;
	object_list_type object_list (
		ALLOCA( constraint_edges_count * sizeof( object_list_type::value_type ) ),
		constraint_edges_count
	);

	for ( u32 i = 0; i < constraint_edges_count; ++i ) {
		float3 const* segment_vertices[] = {
			&m_triangles_mesh.vertices[ m_constraint_edges[i].vertex_index0 ],
			&m_triangles_mesh.vertices[ m_constraint_edges[i].vertex_index1 ]
		};

		float3 const& segment_center	= (*segment_vertices[0] + *segment_vertices[1]) * .5f;
		float3 extents = math::abs( *segment_vertices[0] - segment_center ) + float3( 1.5f, 1.5f, 1.5f );

		non_null<collision::object>::ptr object	= collision::new_aabb_object (
			&debug::g_mt_allocator,
			1,
			segment_center,
			extents,
			reinterpret_cast<pvoid>(i)
		);

		spatial_tree->insert	( object, float4x4().identity() );
		object_list.push_back	( &*object );
	}

	markers_type markers (
		ALLOCA( constraint_edges_count * sizeof( markers_type::value_type ) ),
		constraint_edges_count,
		constraint_edges_count,
		false
	);

	/*
+		min	{x=-4.7553029 y=-5.2685590 z=3.7029507}	xray::math::float3
+		max	{x=-4.6747975 y=4.7314410 z=4.3493414}	xray::math::float3

+		min	{x=-4.7553029 y=-5.2685590 z=4.2735095}	xray::math::float3
+		max	{x=-4.7443194 y=4.7314410 z=4.3493409}	xray::math::float3
	*/

	objects_type objects( debug::g_mt_allocator );
	for ( u32 i = 0; i < constraint_edges_count; ++i ) {
		float3 const* segment_vertices[] = {
			&m_triangles_mesh.vertices[ m_constraint_edges[i].vertex_index0 ],
			&m_triangles_mesh.vertices[ m_constraint_edges[i].vertex_index1 ],
		};

		math::aabb aabb(math::create_invalid_aabb());
		aabb.modify			( *segment_vertices[0] );
		aabb.modify			( *segment_vertices[1] );

		math::float3 const& center		= aabb.center( );
		math::float3 const& extents		= aabb.extents( );
		float const epsilon				= math::epsilon_3;
		aabb.modify						( center + float3( -epsilon - extents.x, -5.f - extents.y, -epsilon - extents.z ) );
		aabb.modify						( center + float3( +epsilon + extents.x,  5.f + extents.y, +epsilon + extents.z ) );

		objects.clear();
		spatial_tree->aabb_query(1, aabb, objects );

		/* Debug
		u32 const parent_triangle = m_constraint_edges[ i ].triangle_id;
		*/

		subdivide_constraint_edge ( coordinate_indices, i, objects, markers );
	}

	m_constraint_edges.erase(
		std::remove_if (
			m_constraint_edges.begin(),
			m_constraint_edges.end(),
			is_marked_constraint_predicate( m_constraint_edges, markers )
		),
		m_constraint_edges.end()
	);
	
	object_list_type::iterator i = object_list.begin();
	object_list_type::iterator const e = object_list.end();
	for ( ; i != e; ++i ) {
		spatial_tree->erase( *i );
		collision::delete_object( &debug::g_mt_allocator, *i );
	}
	collision::delete_space_partitioning_tree( &*spatial_tree );
}
