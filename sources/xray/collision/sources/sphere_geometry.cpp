////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.02.2008
//	Author		: Konstantin Slipchenko
//	Description : geometry: box geometry
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "sphere_geometry.h"
#include "box_geometry.h"
#include "compound_geometry.h"
#include <xray/collision/contact_info.h>
#include <xray/render/base/debug_renderer.h>

using xray::memory::base_allocator;

//void	sphere_geometry::generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const geometry& og )const
//{
//	og.generate_contacts(contacts, anch, *this );
//}

namespace xray {
namespace collision {

sphere_geometry::sphere_geometry		( base_allocator* allocator, float const radius ) :
	m_allocator						( allocator ),
	m_radius						( radius )
{
}

void sphere_geometry::add_triangles		( triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETER		(triangles);
	UNREACHABLE_CODE();
}

void	sphere_geometry::generate_contacts	( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const cylinder_geometry& og ) const 
{
	XRAY_UNREFERENCED_PARAMETERS	(&c, &self_transform, &transform, &og);
}

void	sphere_geometry::generate_contacts	( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const triangle_mesh_base& og ) const 
{
	XRAY_UNREFERENCED_PARAMETERS	(&c, &self_transform, &transform, &og);
}

void	sphere_geometry::generate_contacts	( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const compound_geometry& og )const
{
	XRAY_UNREFERENCED_PARAMETERS	(&c, &self_transform, &transform, &og);
}

void	sphere_geometry::generate_contacts( on_contact& on_c, const float4x4 &self_transform, const float4x4 &pose, const sphere_geometry& og )const
{
	const float3 replacement		= pose.c.xyz() - self_transform.c.xyz();
	
	const float square_distance		= replacement.square_magnitude();
	const float radius_sum			= radius() + og.radius();
	const float square_radius_sum	= radius_sum * radius_sum;
	if( square_distance > square_radius_sum )
		return;

	if( square_distance < xray::math::epsilon_7 )
	{
		contact_info c;
		c.depth = xray::math::max( radius(), og.radius() );
		c.normal.set( 0,1,0);
		c.position =  self_transform.c.xyz();
		on_c( c );
		return;
	}
	
	const float distance			= sqrt( square_distance );

	contact_info c;
	c.depth		= radius_sum - distance ;
	c.normal	= replacement/distance;
	c.position	= self_transform.c.xyz() + c.normal * radius();
	on_c( c );
}


bool sphere_geometry::aabb_query	( object const* object, math::aabb const& aabb, triangles_type& triangles ) const 
{
	XRAY_UNREFERENCED_PARAMETERS	(object, &aabb, &triangles);
	return false; 
}

bool sphere_geometry::cuboid_query	( object const* object, math::cuboid const& cuboid, triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETERS	(object, &cuboid, &triangles);
	return false; 
}

bool sphere_geometry::ray_query		( object const* object,
									  float3 const& origin,
									  float3 const& direction,
									  float max_distance,
									  float& distance,
									  ray_triangles_type& triangles,
									  triangles_predicate_type const& predicate ) const				
{
	XRAY_UNREFERENCED_PARAMETERS	( object, &origin, &direction, max_distance, distance, &triangles, &predicate );
	return false; 
}

bool sphere_geometry::aabb_test		( math::aabb const& aabb ) const
{
	XRAY_UNREFERENCED_PARAMETER		( aabb );
	return true;
}

bool sphere_geometry::cuboid_test	( math::cuboid const& cuboid ) const
{
	XRAY_UNREFERENCED_PARAMETER		( cuboid );
	return true;
}

bool sphere_geometry::ray_test		( math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance ) const
{
	XRAY_UNREFERENCED_PARAMETERS	( &origin, &direction, max_distance, distance );
	return true;
}

//void	sphere_geometry::generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const transformed_geometry& og )const
//{
//
//}
//
//void	sphere_geometry::generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const complex_geometry& og )const
//{
//
//}
//
//
//void	sphere_geometry::generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const box_geometry& og )const
//{
//
//}
//
//
//
//void	sphere_geometry::generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const sphere_geometry& og )const
//{
//
//}
//
//void	sphere_geometry::generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const triangle_mesh_base& og )	const
//{
//
//}

void	sphere_geometry::generate_contacts(  on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const collision::geometry& og )		const
{
	og.generate_contacts( c, transform, self_transform, *this );
}

void	sphere_geometry::generate_contacts	( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const box_geometry& og )				const 
{
	og.generate_contacts( c, transform, self_transform, *this );
}

void	sphere_geometry::render( render::debug::renderer& renderer, float4x4 const& matrix ) const 
{
	renderer.draw_sphere( matrix.c.xyz(), m_radius, math::color( 255u, 255u, 255u, 255u ) );
}

math::aabb& sphere_geometry::get_aabb	( math::aabb& result ) const
{
	result				= 
		math::create_min_max(
			float3( -m_radius, -m_radius, -m_radius ),
			float3( +m_radius, +m_radius, +m_radius )
		);
	return				result;
}

math::float3 const* sphere_geometry::vertices	( ) const
{
	NOT_IMPLEMENTED(return 0);
}

u32 const* sphere_geometry::indices				( u32 triangle_id ) const
{
	XRAY_UNREFERENCED_PARAMETER		( triangle_id );
	NOT_IMPLEMENTED(return 0);
}

} // namespace collision
} // namespace xray