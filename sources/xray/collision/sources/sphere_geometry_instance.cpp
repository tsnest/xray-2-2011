////////////////////////////////////////////////////////////////////////////
//	Created		: 14.09.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sphere_geometry_instance.h"
#include <xray/collision/primitives.h>
#include <xray/collision/contact_info.h>
#include <xray/render/facade/debug_renderer.h>
#include <xray/math_randoms_generator.h>

namespace xray {
namespace collision {

sphere_geometry_instance::sphere_geometry_instance	( float4x4 const& matrix, float radius ) : 
	m_matrix			( matrix ),
	m_radius			( radius )
{ }

sphere_geometry_instance::~sphere_geometry_instance	( )
{ }

bool sphere_geometry_instance::aabb_query			( object const* object, math::aabb const& aabb, triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETERS	(object, &aabb, &triangles);
	return false; 
}

bool sphere_geometry_instance::cuboid_query			( object const* object, math::cuboid const& cuboid, triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETERS	(object, &cuboid, &triangles);
	return false; 
}

bool sphere_geometry_instance::ray_query			( object const* object, math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance, ray_triangles_type& triangles, triangles_predicate_type const& predicate ) const
{
	XRAY_UNREFERENCED_PARAMETERS	( object, &origin, &direction, max_distance, distance, &triangles, &predicate );

	return ray_test( origin, direction, max_distance, distance );
}

bool sphere_geometry_instance::aabb_test			( math::aabb const& aabb ) const
{
	XRAY_UNREFERENCED_PARAMETER		( aabb );
	return true;
}

bool sphere_geometry_instance::cuboid_test			( math::cuboid const& cuboid ) const
{
	XRAY_UNREFERENCED_PARAMETER		( cuboid );
	return true;
}

bool sphere_geometry_instance::ray_test				( math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance ) const 
{
	float3 const& to_sphere			= m_matrix.c.xyz( ) - origin;
	float const projection_squared_length	= math::sqr( to_sphere | direction );
	float const squared_distance	= to_sphere.squared_length( ) - projection_squared_length;
	float const squared_radius		= math::sqr(m_radius);
	if ( squared_distance > squared_radius )
		return						false;

	distance						= math::max( math::sqrt( projection_squared_length ) - math::sqrt( squared_radius - squared_distance ), 0.f );
	return							distance <= max_distance;
}

math::aabb sphere_geometry_instance::get_aabb			( ) const
{
	math::aabb tmp				= math::create_aabb_center_radius	
								( 
									float3( 0.f, 0.f, 0.f ),
									float3( m_radius, m_radius, m_radius )
								);
	return						tmp.modify( m_matrix );
}

math::aabb sphere_geometry_instance::get_geometry_aabb	( ) const
{
	return						math::create_aabb_center_radius	
								( 
									float3( 0.f, 0.f, 0.f ),
									float3( m_radius, m_radius, m_radius )
								);
}

bool sphere_geometry_instance::is_valid					( ) const
{
	return						true;
}

void sphere_geometry_instance::render					( render::scene_ptr const& scene, render::debug::renderer& renderer ) const
{
	render		( scene, renderer, m_matrix );
}

void sphere_geometry_instance::render		( render::scene_ptr const& scene, render::debug::renderer& renderer, float4x4 const& transform ) const
{
	renderer.draw_sphere( scene, transform.c.xyz(), m_radius, math::color( 255u, 255u, 255u, 255u ) );
}

void sphere_geometry_instance::enumerate_primitives		( enumerate_primitives_callback& cb ) const
{
	cb.enumerate( float4x4().identity(), primitive( sphere( m_radius ) ) );
}

void sphere_geometry_instance::enumerate_primitives		( float4x4 const& transform, enumerate_primitives_callback& cb ) const
{
	cb.enumerate( transform, primitive( sphere( m_radius ) ) );
}

void sphere_geometry_instance::add_triangles		( triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETER		( triangles );
	UNREACHABLE_CODE				( );
}

math::float3 const* sphere_geometry_instance::vertices	( ) const
{
	NOT_IMPLEMENTED				( return 0 );
}

u32 sphere_geometry_instance::vertex_count				( ) const
{
	NOT_IMPLEMENTED				( return 0 );
}

u32 const* sphere_geometry_instance::indices			( ) const
{
	NOT_IMPLEMENTED				( return 0 );
}

u32 const* sphere_geometry_instance::indices			( u32 triangle_id ) const
{
	XRAY_UNREFERENCED_PARAMETER ( triangle_id );
	NOT_IMPLEMENTED				( return 0 );
}

u32 sphere_geometry_instance::index_count				( ) const
{
	NOT_IMPLEMENTED				( return 0 );
}

float sphere_geometry_instance::get_surface_area		( ) const
{
	return 4 * math::pi * math::sqr( m_radius );
}

float3 sphere_geometry_instance::get_random_surface_point( math::random32& randomizer ) const
{
	float const z_random		= -m_radius + randomizer.random_f( 2.f * m_radius );
	float const teta_angle		= randomizer.random_f( math::pi_x2 );
	float const r_coefficient	= math::sqrt( math::sqr( m_radius ) - math::sqr( z_random ) );
	float const x_random		= r_coefficient * math::cos( teta_angle );
 	float const y_random		= r_coefficient * math::sin( teta_angle );

	return						float3( x_random, y_random, z_random );
}

float3 sphere_geometry_instance::get_closest_point_to	( float3 const& point, float4x4 const& origin ) const
{
	float4x4 transform			= origin * m_matrix;
	float3 position				= transform.c.xyz();
	float3 direction			= point - position;

	if ( direction.squared_length( ) < m_radius*m_radius )
		return point;

	float3 result				= direction.normalize();
	result						*= m_radius;
	result						+= position;
	return						result;
}

} // namespace collision
} // namespace xray