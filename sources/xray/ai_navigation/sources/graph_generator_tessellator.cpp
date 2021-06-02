////////////////////////////////////////////////////////////////////////////
//	Created		: 27.08.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "graph_generator_tessellator.h"
#include "graph_generator_predicates.h"
#include <xray/collision/geometry.h>
#include <xray/collision/geometry_instance.h>

using xray::ai::navigation::graph_generator_tessellator;
using xray::ai::navigation::triangles_mesh_type;
using xray::ai::navigation::triangles_type;
using xray::math::float3;

graph_generator_tessellator::graph_generator_tessellator(triangles_mesh_type& triangles_mesh, u32 tessellation_max_operation_id ) :
	m_triangles_mesh					( triangles_mesh ),
	m_tessellation_max_operation_id		( tessellation_max_operation_id ),
	m_tessellation_current_operation_id	( 0 )
{
	tessellate_passable_triangles();
}

int tri_tri_intersect_with_isectline(float3 const& V0, float3 const& V1, float3 const& V2,
					 float3 const& U0, float3 const& U1, float3 const& U2, int *coplanar,
					 float3& isectpt1, float3& isectpt2);

void correct_point	( triangles_mesh_type const& triangles_mesh, u32 const triangle_id, float3& point );

u32 select_vertex_index	(
		float3 const& vertex,
		float3 const* (vertices)[3],
		u32 const* const indices
	);

u32 graph_generator_tessellator::untessellated_triangles_count(u32 const triangle_id )
{
	u32 result = 0;
	u32 current_triangle_id = triangle_id;
	while ( current_triangle_id != u32(-1)) {
		if ( !m_triangle_info[ current_triangle_id ].is_processed && !m_triangles_mesh.data[ current_triangle_id ].is_marked )
			result++;
		current_triangle_id = m_triangle_info[ current_triangle_id ].next_triangle;
	}

	return result;
}

void graph_generator_tessellator::collect_untessellated_triangles ( u32 const triangle_id, buffer_indices_type& result ) 
{
	u32 current_triangle_id = triangle_id;
	while ( current_triangle_id != u32(-1)) {
		if ( !m_triangle_info[ current_triangle_id ].is_processed && !m_triangles_mesh.data[ current_triangle_id ].is_marked )
			result.push_back( current_triangle_id );
		current_triangle_id = m_triangle_info[ current_triangle_id ].next_triangle;
	}
}

bool graph_generator_tessellator::tessellate_non_coplanar_triangles (
		u32 const triangle_id0, 
		u32 const triangle_id1, 
		float3 (&segment_vertices)[2]
	)
{

	if ( !process_degenerated_cases( triangle_id1, segment_vertices ) )
		subdivide_triangle ( triangle_id1, segment_vertices );

	if ( !process_degenerated_cases( triangle_id0, segment_vertices ) )
		return subdivide_triangle ( triangle_id0, segment_vertices );

	return false;
}

u32 graph_generator_tessellator::get_similar_edge ( 
		u32 const triangle_id0, 
		u32 const triangle_id1,
		u32* edge_id1
	) 
{
	u32 const* indices0 = m_triangles_mesh.indices.begin() + 3 * triangle_id0;
	u32 const* indices1 = m_triangles_mesh.indices.begin() + 3 * triangle_id1;

	for ( u32 i = 0; i < 3; ++i ) {
		u32 const vertex_index0 = indices0[i];
		u32 const vertex_index1 = indices0[(i+1)%3];

		for ( u32 j = 0; j < 3; ++j ) {
			if ( vertex_index0 == indices1[(j+1)%3] && vertex_index1 == indices1[j] ) {
				if ( edge_id1 )
					*edge_id1 = j;
				return i;
			}
		}
	}

	return u32(-1);
}

bool graph_generator_tessellator::try_tessellate_triangles( u32 const triangle_id0, u32 const triangle_id1 )
{
	u32 const* indices0		= &*m_triangles_mesh.indices.begin() + 3*triangle_id0;
	math::float3 const& v0	= m_triangles_mesh.vertices[ indices0[0] ];
	math::float3 const& v1	= m_triangles_mesh.vertices[ indices0[1] ];
	math::float3 const& v2	= m_triangles_mesh.vertices[ indices0[2] ];

	u32 const* indices1		= &*m_triangles_mesh.indices.begin() + 3*triangle_id1;
	math::float3 const& u0	= m_triangles_mesh.vertices[ indices1[0] ];
	math::float3 const& u1	= m_triangles_mesh.vertices[ indices1[1] ];
	math::float3 const& u2	= m_triangles_mesh.vertices[ indices1[2] ];

	float3 segment_vertices[2];
	int coplanar;
	int const intersection_result	=
		tri_tri_intersect_with_isectline(
			v0, v1, v2, u0, u1, u2, &coplanar, segment_vertices[0], segment_vertices[1]
		);

	if ( intersection_result == 0 )
		return false;

	if ( coplanar ) {
		return tessellate_coplanar_triangles ( triangle_id0, triangle_id1 );
	}

	if ( intersection_result == 1 )
		return false;

	if ( is_similar( segment_vertices[0], segment_vertices[1], math::epsilon_3 ) )
		return false;

	if ( get_similar_edge( triangle_id0, triangle_id1 ) != u32(-1) )
		return false;

	return tessellate_non_coplanar_triangles( triangle_id0, triangle_id1, segment_vertices );
}

inline u32 graph_generator_tessellator::get_next_untessellated_triangle( u32 triangle_id ) 
{
//	R_ASSERT ( triangle_id < m_triangle_list.size() );
	while ( triangle_id != (u32)-1 ) {
		if ( !m_triangle_info[triangle_id].is_processed )
			return triangle_id;
		triangle_id = m_triangle_info[triangle_id].next_triangle;
	}
	return u32(-1);
}

bool graph_generator_tessellator::tessellate_triangle_impl ( u32 triangle_id, buffer_indices_type& intersected_triangles )
{
	u32 success_count = 0;
	buffer_indices_type::iterator i = intersected_triangles.begin();
	buffer_indices_type::iterator const e = intersected_triangles.end();
	for (; i != e; ++i ) {
		u32 current_triangle_id = get_next_untessellated_triangle( triangle_id );

		while ( current_triangle_id != u32(-1) ) {

			u32 const childs_count = untessellated_triangles_count( (*i) );
			buffer_indices_type childs (
				ALLOCA( childs_count * sizeof( buffer_indices_type::value_type ) ),
				childs_count
			);
			collect_untessellated_triangles( (*i), childs );

			buffer_indices_type::iterator k = childs.begin();
			for ( buffer_indices_type::iterator const e = childs.end(); k != e; ++k ) {
				if ( try_tessellate_triangles ( current_triangle_id , (*k) ) ) {
					++success_count;
					break;
				}
			}
			current_triangle_id = get_next_untessellated_triangle( m_triangle_info[ current_triangle_id ].next_triangle );
		}
	}
	
	return (success_count > 0);
}

class filter_non_intersected_predicate {
public:
	inline filter_non_intersected_predicate			( triangles_mesh_type& triangles_mesh, u32 const triangle_id ) :
		m_triangles_mesh		( &triangles_mesh ),
		m_triangle_id			( triangle_id )
	{
	}

	inline bool operator( )							( u32 const triangle_id ) const
	{
		return					!intersect_triangles( *m_triangles_mesh, m_triangle_id, triangle_id, true );
	}

private:
	triangles_mesh_type const*	m_triangles_mesh;
	u32							m_triangle_id;
}; // struct filter_obstructions_predicate

void graph_generator_tessellator::tessellate_triangle ( u32 triangle_id, buffer_indices_type& triangles)
{
	triangles.erase					(
		std::remove_if(
			triangles.begin( ),
			triangles.end( ),
			filter_non_intersected_predicate( m_triangles_mesh, triangle_id )
		),
		triangles.end( )
	);

	if ( triangles.empty() ) {
		m_triangles_mesh.data[ triangle_id ].color.set( 0, 255, 255, 64 );
		return;
	}

	m_triangles_mesh.data[ triangle_id ].color.set	( 255, 255, 0, 64 );
	R_ASSERT										( m_triangles_mesh.data[ triangle_id ].is_passable );

	if ( tessellate_triangle_impl2( triangle_id, triangles ) ) {
		// remove original triangle from the mesh
		// since it has just been tessellated
		m_triangles_mesh.data[ triangle_id ].is_marked = true;
		return;
	}

	m_triangles_mesh.data[ triangle_id ].color.set	( 0, 255, 255, 64 );

}

struct filter_less_id_predicate {
	filter_less_id_predicate (u32 const triangle_id) :
		m_triangle_id( triangle_id )
	{
	}

	inline bool operator() ( xray::collision::triangle_result const& triangle ) {
		return triangle.triangle_id <= m_triangle_id;
	}

private:
	u32		m_triangle_id;

}; // struct filter_less_id_predicate

void graph_generator_tessellator::tessellate_triangle ( u32 triangle_id, triangles_type& triangles )
{
	triangles.erase					(
		std::remove_if(
			triangles.begin( ),
			triangles.end( ),
			filter_non_passable_predicate( m_triangles_mesh )
		),
		triangles.end( )
	);

	triangles.erase					(
		std::remove_if(
			triangles.begin(),
			triangles.end(),
			filter_less_id_predicate( triangle_id )
		),
		triangles.end()
	);

	u32 const triangles_size			= triangles.size( );
	buffer_indices_type buffer_indices	( ALLOCA( triangles_size*sizeof(u32) ), triangles_size, triangles_size );
	buffer_indices_type::iterator i		= buffer_indices.begin( );
	buffer_indices_type::iterator const e = buffer_indices.end( );
	triangles_type::const_iterator j	= triangles.begin( );
	for ( ; i != e; ++i )
		*i								= (*j++).triangle_id;

	tessellate_triangle					( triangle_id, buffer_indices );
}

void graph_generator_tessellator::tessellate_passable_triangles()
{
	triangles_type triangles			( debug::g_mt_allocator );

	u32 const triangle_count			= m_triangles_mesh.data.size( );

	m_triangle_info.resize				( triangle_count );

	for ( u32 i=0; i<triangle_count; ++i ) {
		if ( !m_triangles_mesh.data[i].is_passable )
			continue;

		if ( m_triangles_mesh.data[i].is_marked )
			continue;

		math::aabb						aabb(math::create_invalid_aabb());;
		aabb.modify						( m_triangles_mesh.vertices[m_triangles_mesh.indices[3*i]]		);
		aabb.modify						( m_triangles_mesh.vertices[m_triangles_mesh.indices[3*i + 1]]	);
		aabb.modify						( m_triangles_mesh.vertices[m_triangles_mesh.indices[3*i + 2]]	);

		math::float3 const& center		= aabb.center( );
		math::float3 const& extents		= aabb.extents( );
		float const epsilon				= math::epsilon_3;
		aabb.modify						( center + float3( -epsilon - extents.x, -5.f - extents.y, -epsilon - extents.z ) );
		aabb.modify						( center + float3( +epsilon + extents.x,  5.f + extents.y, +epsilon + extents.z ) );

		triangles.clear();
		m_triangles_mesh.spatial_tree->aabb_query	( 0, aabb, triangles );
		tessellate_triangle				( i, triangles );
	}

	m_triangles_mesh.remove_marked_triangles ( );

}