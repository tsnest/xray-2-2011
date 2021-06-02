////////////////////////////////////////////////////////////////////////////
//	Created		: 04.11.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "graph_generator.h"
#include "navigation_mesh_types.h"
#include "graph_generator_subdivider.h"
#include <xray/collision/geometry.h>
#include <xray/collision/geometry_instance.h>

using xray::ai::navigation::graph_generator;
using xray::ai::navigation::triangles_mesh_type;

bool is_in_cuboid( graph_generator::cuboid_type& cuboid, xray::math::float3 const& vertex )
{
	xray::math::plane planes[6] = {
		xray::math::create_plane( cuboid[0], cuboid[1], cuboid[3] ),
		xray::math::create_plane( cuboid[7], cuboid[5], cuboid[4] ),
		xray::math::create_plane( cuboid[0], cuboid[4], cuboid[5] ),
		xray::math::create_plane( cuboid[1], cuboid[5], cuboid[6] ),
		xray::math::create_plane( cuboid[2], cuboid[6], cuboid[7] ),
		xray::math::create_plane( cuboid[3], cuboid[7], cuboid[4] ),
	};

	for ( u32 i = 0; i < 6; ++i ) {
		if ( ((vertex | planes[i].normal) + planes[i].d) > 0 )
			return false;
	}
	return true;
}

void graph_generator::remove_areas_in_cuboid( triangles_mesh_type& triangles_mesh, triangles_type& triangles, cuboid_type& cuboid )
{
	//u32 const cuboid_triangle_indices_count = 36;
	//u32 cuboid_triangles_indices[cuboid_triangle_indices_count] = {
	//	0, 1, 3,
	//	1, 2, 3,

	//	7, 5, 4,
	//	7, 6, 5,

	//	0, 4, 5,
	//	5, 1, 0,

	//	1, 5, 6,
	//	6, 2, 1,

	//	2, 6, 7,
	//	7, 3, 2,

	//	3, 7, 4,
	//	4, 0, 3,
	//};

	typedef vector< u32 > triangle_set_type;

	triangle_set_type triangle_set0;
	triangle_set_type triangle_set1;

	triangle_set_type* front = &triangle_set0;
	triangle_set_type* back = &triangle_set1;

	for ( triangles_type::iterator i = triangles.begin(); i != triangles.end(); ++i ) {
		front->push_back( i->triangle_id );
	}

	for ( u32 i = 0; i < 12; ++i ) {
		triangle_set_type::iterator k = front->begin();
		triangle_set_type::iterator e = front->end();
		u32 const triangles_count = triangles_mesh.data.size();
		for ( ;k != e; ++k ) {
			u32 childs_count;
			//graph_generator_subdivider::subdivide( triangles_mesh, *k, cuboid[cuboid_triangles_indices[ i * 3 + 0 ]], cuboid[cuboid_triangles_indices[ i * 3 + 1 ]], cuboid[cuboid_triangles_indices[ i * 3 + 2 ]]  );
			childs_count = triangles_mesh.data.size() - triangles_count;
			if ( childs_count == 0 )
				back->push_back( *k );
			else {
				for ( u32 j = 0; j < childs_count; ++j )
					back->push_back( triangles_count + j );
			}
		}
		
		std::swap( front, back );
		back->clear();
	}

	triangle_set_type::iterator i = front->begin();
	triangle_set_type::iterator e = front->end();
	for ( ; i != e; ++i ) {
		u32* indices = triangles_mesh.indices.begin() + (*i) * 3;
		u32 count = 0;
		for ( u32 j = 0; j < 3; ++j ) {
			if ( is_in_cuboid( cuboid, triangles_mesh.vertices[ indices[j] ] ))
				count++;
		}
		R_ASSERT_CMP(count, <=, 3 );
		if ( count == 3 )
			triangles_mesh.data[ (*i) ].is_marked = true;
	}

}

void graph_generator::remove_restricted_areas( triangles_mesh_type& triangles_mesh, restricted_areas_type& areas )
{
	triangles_type triangles			( debug::g_mt_allocator );
//	u32 const triangle_count			= triangles_mesh.data.size( );

	for ( u32 i = 0, n=areas.size(); i < n; ++i )
	{
		cuboid_type& cuboid = areas[i];
		R_ASSERT_CMP( cuboid.size(), ==, 8 );
		math::aabb aabb(math::create_invalid_aabb());

		for ( u32 j = 0; j < 8; ++j )
			aabb.modify(cuboid[j]);

		triangles.clear();
		triangles_mesh.spatial_tree->aabb_query	( 0, aabb, triangles );

		// Filter non passable
		remove_areas_in_cuboid( triangles_mesh, triangles, cuboid );

	}

	triangles_mesh.remove_marked_triangles ( );

}