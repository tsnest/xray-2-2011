////////////////////////////////////////////////////////////////////////////
//	Created		: 15.09.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "composite_geometry_instance.h"
#include "composite_geometry.h"

namespace xray {
namespace collision {

class ray_query_helper : private boost::noncopyable {
public:
	inline		ray_query_helper				(
			float const	factor,
			triangles_predicate_type const& predicate
		) :
		m_predicate		( predicate ),
		m_factor		( factor ),
		m_min_distance	( math::infinity )
	{
	}

	inline bool predicate						( xray::collision::ray_triangle_result const& triangle )
	{
		xray::collision::ray_triangle_result&	raw_triangle = const_cast<xray::collision::ray_triangle_result&>( triangle );
		raw_triangle.distance					*= m_factor;
		m_min_distance							= math::min( m_min_distance, raw_triangle.distance );
		return									m_predicate( raw_triangle );
	}

	inline float min_distance					( ) const
	{
		return			m_min_distance;
	}

private:
	triangles_predicate_type const& m_predicate;
	float const						m_factor;
	float							m_min_distance;
}; // class ray_query_helper

composite_geometry_instance::composite_geometry_instance	( memory::base_allocator* allocator, float4x4 const& matrix, non_null<composite_geometry const>::ptr geometry ) :
	m_matrix			( matrix ),
	m_inverted_matrix	( math::invert4x3( matrix ) ),
	m_geometry			( geometry )
{
	XRAY_UNREFERENCED_PARAMETER	( allocator );
}

composite_geometry_instance::~composite_geometry_instance	( )
{}

void composite_geometry_instance::destroy	( memory::base_allocator* allocator )
{
	XRAY_UNREFERENCED_PARAMETER	( allocator );
}

bool composite_geometry_instance::aabb_query		( object const* object, math::aabb const& aabb, triangles_type& triangles ) const
{
	bool res = false;
	vectora< geometry_instance* >::const_iterator i = m_geometry->begin(), e = m_geometry->end();
	math::cuboid local_cuboid( aabb, m_inverted_matrix );
	for ( ; i != e; ++i )
		res = (*i)->cuboid_query( object, local_cuboid, triangles ) || res;
	
	return res;
}

bool composite_geometry_instance::cuboid_query		( object const* object, math::cuboid const& cuboid, triangles_type& triangles ) const
{
	bool res = false;
	vectora< geometry_instance* >::const_iterator i = m_geometry->begin(), e = m_geometry->end();
	math::cuboid local_cuboid( cuboid, m_inverted_matrix );
	for ( ; i != e; ++i )
		res = (*i)->cuboid_query( object, local_cuboid, triangles ) || res;
	
	return res;
}

bool composite_geometry_instance::ray_query			( object const* object, math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance, ray_triangles_type& triangles, triangles_predicate_type const& predicate ) const
{
	float3 const new_origin		= m_inverted_matrix.transform_position(origin);
	float3 const new_direction	= m_inverted_matrix.transform_direction(direction);
	float new_max_distance		= length(m_inverted_matrix.transform_position(origin + direction*max_distance) - new_origin);
	
	ray_query_helper			helper(max_distance/new_max_distance, predicate );

	bool res = false;
	vectora< geometry_instance* >::const_iterator i = m_geometry->begin(), e = m_geometry->end();
	for ( ; i != e; ++i )
		res = (*i)->ray_query	( object, new_origin, new_direction, max_distance, new_max_distance, triangles, triangles_predicate_type( &helper, &ray_query_helper::predicate) ) || res;
	
	distance					= helper.min_distance();
	return res;
}

bool composite_geometry_instance::aabb_test			( math::aabb const& aabb ) const
{
	vectora< geometry_instance* >::const_iterator i = m_geometry->begin(), e = m_geometry->end();
	for ( ; i != e; ++i )
		if ( (*i)->aabb_test( aabb ) )
			return true;
	
	return false;
}

bool composite_geometry_instance::cuboid_test		( math::cuboid const& cuboid ) const
{
	vectora< geometry_instance* >::const_iterator i = m_geometry->begin(), e = m_geometry->end();

	math::cuboid local_cuboid( cuboid, m_inverted_matrix );
	for ( ; i != e; ++i )
		if ( (*i)->cuboid_test( local_cuboid ) )
			return true;

	return false;
}

bool composite_geometry_instance::ray_test			( math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance ) const
{
	float3 const new_origin		= m_inverted_matrix.transform_position(origin);
	float3 const new_direction	= m_inverted_matrix.transform_direction(direction);
	float new_max_distance		= length(m_inverted_matrix.transform_position(origin + direction*max_distance) - new_origin);
	vectora< geometry_instance* >::const_iterator i = m_geometry->begin(), e = m_geometry->end();
	for ( ; i != e; ++i )
		if ( (*i)->ray_test( new_origin, new_direction, new_max_distance, distance ) ) {
			distance			*= max_distance/new_max_distance;
			return true;
		}
	
	return false;
}

math::aabb composite_geometry_instance::get_aabb	( ) const
{
	math::aabb result = math::create_zero_aabb();
	vectora< geometry_instance* >::const_iterator i = m_geometry->begin(), e = m_geometry->end();
	for ( ; i != e; ++i )
		result.modify( (*i)->get_aabb() );
	
	return math::aabb( result ).modify( m_matrix );
}

math::aabb composite_geometry_instance::get_geometry_aabb	( ) const
{
	math::aabb result = math::create_zero_aabb();
	vectora< geometry_instance* >::const_iterator i = m_geometry->begin(), e = m_geometry->end();
	for ( ; i != e; ++i )
		result.modify( (*i)->get_aabb( ) );
	
	return result;

}

bool composite_geometry_instance::is_valid			( ) const
{
	vectora< geometry_instance* >::const_iterator i = m_geometry->begin(), e = m_geometry->end();
	for ( ; i != e; ++i )
	{
		if ( !(*i)->is_valid( ) )
			return false;
	}
	
	return true;
}

void composite_geometry_instance::render			( render::scene_ptr const& scene, render::debug::renderer& renderer ) const
{
	render			( scene, renderer, m_matrix );
}

void composite_geometry_instance::render	( render::scene_ptr const& scene, render::debug::renderer& renderer, float4x4 const& transform ) const
{
	vectora< geometry_instance* >::const_iterator i = m_geometry->begin(), e = m_geometry->end();
	for ( ; i != e; ++i )
	{
		float4x4 m = (*i)->get_matrix() * transform;
		(*i)->render( scene, renderer, m );
	}
}

void composite_geometry_instance::enumerate_primitives( enumerate_primitives_callback& cb ) const
{
	vectora< geometry_instance const* >::const_iterator i = m_geometry->begin(), e = m_geometry->end();
	for ( ; i != e; ++i )
		(*i)->enumerate_primitives( float4x4().identity(), cb );
}

void composite_geometry_instance::enumerate_primitives( float4x4 const& transform, enumerate_primitives_callback& cb ) const
{
	vectora< geometry_instance const* >::const_iterator i = m_geometry->begin(), e = m_geometry->end();
	for ( ; i != e; ++i )
		(*i)->enumerate_primitives( transform, cb );
}


void composite_geometry_instance::add_triangles		( triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETER (triangles);
}

math::float3 const* composite_geometry_instance::vertices	( ) const
{
	NOT_IMPLEMENTED(return 0);
}

u32		composite_geometry_instance::vertex_count			( ) const
{
	NOT_IMPLEMENTED(return 0);
}

u32 const* composite_geometry_instance::indices				( ) const
{
	NOT_IMPLEMENTED(return 0);
}

u32 const* composite_geometry_instance::indices				( u32 triangle_id ) const
{
	XRAY_UNREFERENCED_PARAMETER		( triangle_id );
	NOT_IMPLEMENTED(return 0);
}

u32	composite_geometry_instance::index_count				( ) const
{
	NOT_IMPLEMENTED(return 0);
}

float composite_geometry_instance::get_surface_area			( ) const
{
	return m_geometry->get_surface_area			( );
}

float3 composite_geometry_instance::get_random_surface_point( math::random32& randomizer ) const
{
	XRAY_UNREFERENCED_PARAMETER		( randomizer );
	UNREACHABLE_CODE				( return float3() );
}

float3 composite_geometry_instance::get_closest_point_to( float3 const& point, float4x4 const& origin ) const
{
	return							m_geometry->get_closest_point_to( point, origin * m_matrix );
}

} // namespace collision
} // namespace xray