////////////////////////////////////////////////////////////////////////////
//	Created		: 07.09.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "truncated_sphere_geometry_instance.h"
#include <xray/collision/contact_info.h>
#include <xray/math_randoms_generator.h>
#include <xray/render/facade/debug_renderer.h>
#include <xray/geometry_primitives.h>
#include <xray/memory_stack_allocator.h>
#include <xray/collision/primitives.h>

namespace xray {
namespace collision {


truncated_sphere_geometry_instance::truncated_sphere_geometry_instance	( float4x4 const& matrix, float const radius, float4* planes, u32 planes_count ) :
	m_matrix			( matrix ),
	m_inverted_matrix	( math::invert4x3( matrix ) ),
	m_radius			( radius ),
	m_planes			( planes, planes_count, planes_count )
{
}

void	truncated_sphere_geometry_instance::destroy		( memory::base_allocator* allocator )
{
	if( !m_planes.empty( ) )
	{
		void* pointer	= m_planes.begin( );
		XRAY_FREE_IMPL	( allocator, pointer );
	}
	XRAY_UNREFERENCED_PARAMETER		( allocator );
}

void	truncated_sphere_geometry_instance::add_triangles		( triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETER		(triangles);
	UNREACHABLE_CODE();
}

bool truncated_sphere_geometry_instance::aabb_query	( object const* object, math::aabb const& aabb, triangles_type& triangles ) const 
{
	XRAY_UNREFERENCED_PARAMETERS	(object, &aabb, &triangles);
	NOT_IMPLEMENTED					( return false );
}

float3 truncated_sphere_geometry_instance::get_random_surface_point( math::random32& randomizer ) const
{
	XRAY_UNREFERENCED_PARAMETERS	( randomizer );
	NOT_IMPLEMENTED					( return float3(0, 0, 0) );
}

float truncated_sphere_geometry_instance::get_surface_area	( ) const
{
	NOT_IMPLEMENTED					( return 0 );
}

bool truncated_sphere_geometry_instance::cuboid_query	( object const* object, math::cuboid const& cuboid, triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETERS	(object, &cuboid, &triangles);
	NOT_IMPLEMENTED					( return false );
}

bool truncated_sphere_geometry_instance::ray_query		( object const* object,
									  float3 const& origin,
									  float3 const& direction,
									  float max_distance,
									  float& distance,
									  ray_triangles_type& triangles,
									  triangles_predicate_type const& predicate ) const				
{
	XRAY_UNREFERENCED_PARAMETERS	( object, &origin, &direction, max_distance, distance, &triangles, &predicate );
	NOT_IMPLEMENTED					( return false );
}

bool truncated_sphere_geometry_instance::aabb_test	( math::aabb const& aabb ) const
{
	XRAY_UNREFERENCED_PARAMETER		( aabb );
	NOT_IMPLEMENTED					( return false );
}

bool truncated_sphere_geometry_instance::cuboid_test	( math::cuboid const& cuboid ) const
{
	XRAY_UNREFERENCED_PARAMETER		( cuboid );
	NOT_IMPLEMENTED					( return false );
}

bool truncated_sphere_geometry_instance::ray_test		( math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance ) const
{
	XRAY_UNREFERENCED_PARAMETERS	( origin, max_distance, direction, distance );
	NOT_IMPLEMENTED					( return false );
}

void	truncated_sphere_geometry_instance::render( render::scene_ptr const& scene, render::debug::renderer& renderer ) const 
{
	render					( scene, renderer, m_matrix );
}

void truncated_sphere_geometry_instance::render	( render::scene_ptr const& scene, render::debug::renderer& renderer, float4x4 const& transform ) const
{
	float4x4 local_matrix = math::create_scale( float3( m_radius, m_radius, m_radius ) ) * transform;

	u8 indices_buffer [ sizeof( u16 ) * 336 * 6 ];
	u8 vertices_buffer [ sizeof( render::vertex_colored ) * 114 * 3 ];

	xray::memory::stack_allocator indices_allocator;
	indices_allocator.initialize( (void*)indices_buffer, sizeof( u16 ) * 336 * 6, "indices" );

	xray::memory::stack_allocator vertices_allocator;
	vertices_allocator.initialize( (void*)vertices_buffer, sizeof( render::vertex_colored ) * 114 * 3, "vertices" );

	render::debug_indices_type		indices( indices_allocator );
	indices.assign( geometry_utils::ellipsoid::pairs, geometry_utils::ellipsoid::pairs + ( geometry_utils::ellipsoid::pair_count * 2 ) );

	render::debug_vertices_type		vertices( vertices_allocator );
	vertices.resize( geometry_utils::ellipsoid::vertex_count );

	render::debug_vertices_type::iterator		i = vertices.begin( );
	render::debug_vertices_type::iterator		e = vertices.end( );
	for ( u32 j = 0; i != e; ++i, j += 3 )
	{
		float3 vertex( geometry_utils::ellipsoid::vertices[ j ], geometry_utils::ellipsoid::vertices[ j + 1 ], geometry_utils::ellipsoid::vertices[ j + 2 ] );
		( *i ) = render::vertex_colored( vertex, math::color( 255u, 255u, 255u, 255u ) );
	}

	u32 planes_count = m_planes.size( );
	for( u32 k = 0; k < planes_count; ++k )
	{
		float4	plane					= m_planes[k];
		float	plane_radius_squared	= 1.0f - math::sqr( plane.w );
		float	plane_radius			= math::sqrt( plane_radius_squared );
		float3	plane_position			= plane.xyz( ) * plane.w;

		render::debug_vertices_type::iterator		i = vertices.begin( );
		render::debug_vertices_type::iterator		e = vertices.end( );
		for ( ; i != e; ++i )
		{
			float3 vertex = ( *i ).position;
			
			float pos = ( vertex - plane.xyz( ) * plane.w ) | plane.xyz( );
			if( pos > 0 )
			{
				vertex -= plane.xyz( ) * pos;
				float3 plane_to_vertex = vertex - plane_position;
				if( plane_to_vertex.squared_length( ) > plane_radius_squared )
				{
					float	plane_to_vertex_distance	= plane_to_vertex.length( );
					float3	plane_to_vertex_normal		= plane_to_vertex / plane_to_vertex_distance;
					vertex	-= plane_to_vertex_normal * ( plane_to_vertex_distance - plane_radius );
				}
			}
			
			( *i ).position = vertex;
		}
	}

	i = vertices.begin( );
	for ( ; i != e; ++i )
		( *i ).position = ( *i ).position * local_matrix;

	renderer.draw_lines	( scene, vertices, indices, false );
}

math::aabb& truncated_sphere_geometry_instance::get_aabb	( math::aabb& result ) const
{
	result				= 
		math::create_aabb_min_max(
			float3( -m_radius, -m_radius, -m_radius ),
			float3( +m_radius, +m_radius, +m_radius )
		);
	return				result;
}

void		truncated_sphere_geometry_instance::get_aabb	( math::aabb& bbox, float4x4 const& self_transform ) const
{
	bbox = math::create_aabb_min_max(
			self_transform.transform( float3( -m_radius, -m_radius, -m_radius ) ),
			self_transform.transform( float3( +m_radius, +m_radius, +m_radius ) )
		);
}

math::float3 const* truncated_sphere_geometry_instance::vertices	( ) const
{
	NOT_IMPLEMENTED(return 0);
}

u32		truncated_sphere_geometry_instance::vertex_count				( ) const
{
	NOT_IMPLEMENTED(return 0);
}

u32 const* truncated_sphere_geometry_instance::indices				( ) const
{
	NOT_IMPLEMENTED(return 0);
}

u32 const* truncated_sphere_geometry_instance::indices				( u32 triangle_id ) const
{
	XRAY_UNREFERENCED_PARAMETER		( triangle_id );
	NOT_IMPLEMENTED(return 0);
}

u32		truncated_sphere_geometry_instance::index_count				( ) const
{
	NOT_IMPLEMENTED(return 0);
}

void	truncated_sphere_geometry_instance::enumerate_primitives	( enumerate_primitives_callback& cb ) const
{
	cb.enumerate( float4x4().identity(), primitive( sphere( m_radius ) ) );
}

void	truncated_sphere_geometry_instance::enumerate_primitives	( float4x4 const& transform, enumerate_primitives_callback& cb ) const
{
	cb.enumerate( transform, primitive( sphere( m_radius ) ) );
}

math::aabb	truncated_sphere_geometry_instance::get_aabb		( ) const
{
	return	create_aabb_min_max(
			m_matrix.transform( float3( -m_radius, -m_radius, -m_radius ) ),
			m_matrix.transform( float3( +m_radius, +m_radius, +m_radius ) )
		);
}

math::aabb	truncated_sphere_geometry_instance::get_geometry_aabb	( ) const
{
	return	math::create_aabb_min_max(
			float3( -m_radius, -m_radius, -m_radius ),
			float3( +m_radius, +m_radius, +m_radius )
		);
}

float3 truncated_sphere_geometry_instance::get_closest_point_to( float3 const& point, float4x4 const& origin ) const
{
	XRAY_UNREFERENCED_PARAMETER		( point );

	float4x4 transform				= origin * m_matrix;
	NOT_IMPLEMENTED					( return transform.c.xyz( ); );
}

} // namespace collision
} // namespace xray