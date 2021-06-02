////////////////////////////////////////////////////////////////////////////
//	Created		: 26.05.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "composite_geometry.h"
#include <xray/collision/contact_info.h>
#include <xray/collision/api.h>

namespace xray {
namespace collision {

composite_geometry::composite_geometry	(
		memory::base_allocator* allocator, 
		geometry_instances_type const& instances
	) :
	m_geometry_instances				(
		XRAY_MALLOC_IMPL( allocator, instances.size() * sizeof( geometry_instances_type::value_type ), "geometry instances vector" ),
		instances.size(),
		instances.begin(),
		instances.end()
	)
{
}

void composite_geometry::destroy	( memory::base_allocator* allocator )
{
	vectora< geometry_instance* >::const_iterator i = m_geometry_instances.begin(), e = m_geometry_instances.end();
	for ( ; i != e; ++i )
	{
		(*i)->destroy				( allocator );
//		delete_geometry_instance	( allocator, *i );
	}

	geometry_instance** begin		= m_geometry_instances.begin();
	XRAY_FREE_IMPL					( allocator, begin );
}

math::aabb composite_geometry::get_aabb( ) const
{
	math::aabb result = math::create_zero_aabb();
	vectora< geometry_instance* >::const_iterator i = m_geometry_instances.begin(), e = m_geometry_instances.end();
	for ( ; i != e; ++i )
		result.modify( (*i)->get_aabb() );
	
	return result;
}

float3 composite_geometry::get_random_surface_point	( math::random32& randomizer ) const
{
	XRAY_UNREFERENCED_PARAMETER		( randomizer );
	UNREACHABLE_CODE				( return float3() );
}

float composite_geometry::get_surface_area	( ) const
{
	float areas_sum							= 0.f;
	vectora< geometry_instance* >::const_iterator i = m_geometry_instances.begin(), e = m_geometry_instances.end();
	for ( ; i != e; ++i )
		areas_sum							+= (*i)->get_surface_area();
	
	return									areas_sum;
}

void composite_geometry::render( render::scene_ptr const& scene, render::debug::renderer& renderer, float4x4 const& matrix ) const
{
	vectora< geometry_instance* >::const_iterator i = m_geometry_instances.begin(), e = m_geometry_instances.end();
	for ( ; i != e; ++i )
		(*i)->render	( scene, renderer, (*i)->get_matrix() * matrix );
}
math::float3 const* composite_geometry::vertices	( ) const 
{ 
	NOT_IMPLEMENTED(return 0);
}

u32 composite_geometry::vertex_count		( ) const
{
	NOT_IMPLEMENTED(return 0);
}

u32 const*	composite_geometry::indices		( ) const 
{
	NOT_IMPLEMENTED(return 0);
}

u32 const*	composite_geometry::indices		( u32 triangle_id ) const 
{
	XRAY_UNREFERENCED_PARAMETER (triangle_id);
	NOT_IMPLEMENTED(return 0);
}

u32 composite_geometry::index_count				( ) const
{
	NOT_IMPLEMENTED(return 0);
}

bool composite_geometry::aabb_query	( object const* object, math::aabb const& aabb, triangles_type& triangles ) const
{ 
	bool res = false;
	vectora< geometry_instance* >::const_iterator i = m_geometry_instances.begin(), e = m_geometry_instances.end();
	for ( ; i != e; ++i )
		res = (*i)->aabb_query( object, aabb, triangles ) || res;
	
	return res;
}

bool composite_geometry::cuboid_query( object const* object, math::cuboid const& cuboid, triangles_type& triangles ) const
{ 
	bool res = false;
	vectora< geometry_instance* >::const_iterator i = m_geometry_instances.begin(), e = m_geometry_instances.end();
	for ( ; i != e; ++i )
		res = (*i)->cuboid_query( object, cuboid, triangles ) || res;
	
	return res;
};

bool composite_geometry::ray_query(
						object const* object,
						float3 const& origin,
						float3 const& direction,
						float max_distance,
						float& distance,
						ray_triangles_type& triangles,
						triangles_predicate_type const& predicate
					) const				
{ 
	bool res = false;
	vectora< geometry_instance* >::const_iterator i = m_geometry_instances.begin(), e = m_geometry_instances.end();
	for ( ; i != e; ++i )
		res = (*i)->ray_query( object, origin, direction, max_distance, distance, triangles, predicate ) || res;
	return res; 
}

bool composite_geometry::aabb_test( math::aabb const& aabb ) const
{
	vectora< geometry_instance* >::const_iterator i = m_geometry_instances.begin(), e = m_geometry_instances.end();
	for ( ; i != e; ++i )
		if ( (*i)->aabb_test( aabb ) )
			return true;
	
	return false;
}
bool composite_geometry::cuboid_test( math::cuboid const& cuboid ) const
{
	vectora< geometry_instance* >::const_iterator i = m_geometry_instances.begin(), e = m_geometry_instances.end();
	for ( ; i != e; ++i )
		if ( (*i)->cuboid_test( cuboid ) )
			return true;

	return false;
}
bool composite_geometry::ray_test( math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance ) const
{
	vectora< geometry_instance* >::const_iterator i = m_geometry_instances.begin(), e = m_geometry_instances.end();
	for ( ; i != e; ++i )
		if ( (*i)->ray_test( origin, direction, max_distance, distance ) )
			return true;
	
	return false;
}

void composite_geometry::add_triangles		( triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETER (triangles);
}

//math::float3 const* composite_geometry::vertices	( ) const
//{
//	NOT_IMPLEMENTED(return 0);
//}
//
//u32 const* composite_geometry::indices			( u32 triangle_id ) const
//{
//	NOT_IMPLEMENTED(return 0);
//}

void	composite_geometry::enumerate_primitives( enumerate_primitives_callback& cb ) const
{
	
	vectora< geometry_instance const* >::const_iterator i = m_geometry_instances.begin(), e = m_geometry_instances.end();
	for ( ; i != e; ++i )
		(*i)->enumerate_primitives( float4x4().identity(), cb );

}

void	composite_geometry::enumerate_primitives( float4x4 const& transform, enumerate_primitives_callback& cb ) const
{
	vectora< geometry_instance const* >::const_iterator i = m_geometry_instances.begin(), e = m_geometry_instances.end();
	for ( ; i != e; ++i )
		(*i)->enumerate_primitives( transform, cb );
}

bool composite_geometry::is_inside				( float3 const& position ) const
{
	XRAY_UNREFERENCED_PARAMETER	( position );
	//vectora< geometry_instance const* >::const_iterator i = m_geometry_instances.begin(), e = m_geometry_instances.end();
	//for ( ; i != e; ++i ) {
	//	if ( (*i)->is_inside() )
	//		return	true;
	//}

	return			false;
}

u32	composite_geometry::get_triangles_count	( math::convex const& bounding_convex ) const
{
	XRAY_UNREFERENCED_PARAMETER		( bounding_convex );
	NOT_IMPLEMENTED					(return 0);
}

float3 composite_geometry::get_closest_point_to( float3 const& point, float4x4 const& origin ) const
{
	if ( m_geometry_instances.empty( ) )
		return origin.c.xyz( );

	vectora< geometry_instance const* >::const_iterator i = m_geometry_instances.begin(), e = m_geometry_instances.end();

	float3 min_closest_point				= (*i++)->get_closest_point_to( point, origin );
	float min_squared_distance				= ( point - min_closest_point ).squared_length( );

	for ( ; i != e; ++i )
	{
		float3 closest_point				= (*i)->get_closest_point_to( point, origin );
		float squared_distance				= ( point - closest_point ).squared_length( );
		if ( squared_distance < min_squared_distance )
		{
			min_squared_distance			= squared_distance;
			min_closest_point				= closest_point;
		}
	}
	return min_closest_point;
}

} // namespace collision
} // namespace xray
