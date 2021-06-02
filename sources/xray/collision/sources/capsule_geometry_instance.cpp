////////////////////////////////////////////////////////////////////////////
//	Created		: 06.09.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "capsule_geometry_instance.h"
#include <xray/collision/contact_info.h>
#include <xray/math_randoms_generator.h>
#include <xray/render/facade/debug_renderer.h>
#include <xray/collision/primitives.h>

namespace xray {
namespace collision {

bool segment_segment_intersect ( float3 const& p1, float3 const& p2, float3 const& p3, float3 const& p4, float max_distance );

capsule_geometry_instance::capsule_geometry_instance	( float4x4 const& matrix, float const radius, float const half_length ) :
	m_matrix						( matrix ),
	m_inverted_matrix				( math::invert4x3( matrix ) ),
	m_radius						( radius ),
	m_half_length					( half_length ),
	m_true_half_length				( half_length )
{
}

void	capsule_geometry_instance::destroy		( memory::base_allocator* allocator )
{
	XRAY_UNREFERENCED_PARAMETER		( allocator );
}

void	capsule_geometry_instance::add_triangles		( triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETER		(triangles);
	UNREACHABLE_CODE();
}

bool	capsule_geometry_instance::is_valid			( ) const
{
	return true;
}

bool capsule_geometry_instance::aabb_query	( object const* object, math::aabb const& aabb, triangles_type& triangles ) const 
{
	XRAY_UNREFERENCED_PARAMETERS	(object, &aabb, &triangles);
	NOT_IMPLEMENTED					( return false );
}

float3 capsule_geometry_instance::get_random_surface_point( math::random32& randomizer ) const
{
	XRAY_UNREFERENCED_PARAMETERS	( randomizer );
	NOT_IMPLEMENTED					( return float3(0, 0, 0) );
}

float capsule_geometry_instance::get_surface_area	( ) const
{
	NOT_IMPLEMENTED					( return 0 );
}

bool capsule_geometry_instance::cuboid_query	( object const* object, math::cuboid const& cuboid, triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETERS	(object, &cuboid, &triangles);
	NOT_IMPLEMENTED					( return false );
}

bool capsule_geometry_instance::ray_query		( object const* object,
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

bool capsule_geometry_instance::aabb_test	( math::aabb const& aabb ) const
{
	XRAY_UNREFERENCED_PARAMETER		( aabb );
	NOT_IMPLEMENTED					( return false );
}

bool capsule_geometry_instance::cuboid_test	( math::cuboid const& cuboid ) const
{
	XRAY_UNREFERENCED_PARAMETER		( cuboid );
	NOT_IMPLEMENTED					( return false );
}

bool capsule_geometry_instance::ray_test		( math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance ) const
{
	XRAY_UNREFERENCED_PARAMETER( distance );
	return segment_segment_intersect			( 
		origin, 
		origin + direction * max_distance,
		m_matrix.c.xyz( ) - m_matrix.j.xyz( ) * m_half_length,
		m_matrix.c.xyz( ) + m_matrix.j.xyz( ) * m_half_length,
		m_radius
	);
}

void capsule_geometry_instance::render( render::scene_ptr const& scene, render::debug::renderer& renderer ) const 
{
	render				( scene, renderer, m_matrix );
}

void capsule_geometry_instance::render( render::scene_ptr const& scene, render::debug::renderer& renderer, float4x4 const& transform ) const 
{
	renderer.draw_line_capsule	( scene, transform, float3( m_radius, m_half_length, m_radius ), math::color( 255u, 255u, 255u, 255u ) );
}

math::float3 const* capsule_geometry_instance::vertices	( ) const
{
	NOT_IMPLEMENTED(return 0);
}

u32		capsule_geometry_instance::vertex_count				( ) const
{
	NOT_IMPLEMENTED(return 0);
}

u32 const* capsule_geometry_instance::indices				( ) const
{
	NOT_IMPLEMENTED(return 0);
}

u32 const* capsule_geometry_instance::indices				( u32 triangle_id ) const
{
	XRAY_UNREFERENCED_PARAMETER		( triangle_id );
	NOT_IMPLEMENTED(return 0);
}

u32		capsule_geometry_instance::index_count				( ) const
{
	NOT_IMPLEMENTED(return 0);
}

void	capsule_geometry_instance::enumerate_primitives	( enumerate_primitives_callback& cb ) const
{
	cb.enumerate( float4x4().identity(), primitive( capsule( m_half_length, m_radius ) ) );
}

void capsule_geometry_instance::enumerate_primitives ( float4x4 const& transform, enumerate_primitives_callback& cb ) const
{
	cb.enumerate( transform, primitive( capsule( m_half_length, m_radius ) ) );
}

void capsule_geometry_instance::set_matrix ( math::float4x4 const& matrix )
{
	m_half_length		= m_true_half_length * matrix.j.xyz( ).length( );
	m_matrix			= matrix;
	m_matrix.set_scale	( float3( 1, 1, 1 ) );
	m_inverted_matrix	= math::invert4x3( matrix ); 
}

math::aabb capsule_geometry_instance::get_aabb		( ) const
{
	float3 radius_delta					( m_radius, m_radius, m_radius );
	float3 size							= ( m_matrix.j.xyz( ) * half_length( ) ).abs( ) + radius_delta;

	return math::create_aabb_min_max( m_matrix.c.xyz( ) - size, m_matrix.c.xyz( ) + size );
}

math::aabb	capsule_geometry_instance::get_geometry_aabb	( ) const
{
	return	math::create_aabb_min_max(
		float3( -m_radius, -m_half_length - m_radius, -m_radius ),
		float3( +m_radius, +m_half_length + m_radius, +m_radius )
	);
}

float3 capsule_geometry_instance::get_closest_point_to( float3 const& point, float4x4 const& origin ) const
{
	float4x4 transform				= origin * m_matrix;
	float3 center					= transform.c.xyz( );
	float3 y_axis					= transform.j.xyz( );
	float3 top_surface_center		= center + m_half_length * y_axis;
	float3 bottom_surface_center	= center - m_half_length * y_axis;
	float3 height_vector			= bottom_surface_center - top_surface_center;
	float proj_to_y_axis			= ( point - top_surface_center ).dot_product( height_vector ) / height_vector.dot_product( height_vector );
	
	if ( proj_to_y_axis > 0.0f && proj_to_y_axis < 1.0f )
	{
		float3 height_vector_proj_point	= top_surface_center + proj_to_y_axis * height_vector;
		float3 dir						= point - height_vector_proj_point;
		if ( ( dir ).squared_length( ) < m_radius * m_radius )
			return point;

		return							height_vector_proj_point + dir.normalize( ) * m_radius;
	}

	float3 surface_center;
	if ( proj_to_y_axis < 0.0f )
		surface_center			= top_surface_center;
	else
		surface_center			= bottom_surface_center;

	if ( ( point - surface_center ).squared_length( ) < m_radius * m_radius )
		return point;

	return surface_center + ( point - surface_center ).normalize( ) * m_radius;
}

} // namespace collision
} // namespace xray