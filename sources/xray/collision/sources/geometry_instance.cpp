////////////////////////////////////////////////////////////////////////////
//	Created		: 17.02.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "geometry_instance.h"
#include <xray/collision/geometry.h>

using xray::collision::detail::geometry_instance;
using xray::collision::geometry;
using xray::collision::object;
using xray::collision::ray_triangles_type;
using xray::collision::triangles_type;
using xray::collision::triangles_predicate_type;
using xray::memory::base_allocator;
using xray::math::float4x4;
using xray::math::float3;
using xray::math::aabb;
using xray::math::cuboid;
using xray::non_null;
using xray::collision::on_contact;

geometry_instance::geometry_instance			( base_allocator* allocator, float4x4 const& matrix, non_null<geometry const>::ptr	geometry ) :
	m_matrix			( matrix ),
	m_inverted_matrix	( math::invert4x3(m_matrix) ),
	m_geometry			( geometry ),
	m_allocator			( allocator )
{
	m_aabb				= m_geometry->get_aabb( m_aabb );
	m_aabb.modify		( matrix );
}

non_null<geometry const>::ptr geometry_instance::get_geometry	( ) const
{
	return				m_geometry;
}

void geometry_instance::set_matrix				( float4x4 const& matrix )
{
	m_matrix			= matrix;
	m_inverted_matrix	= math::invert4x3( m_matrix );
	m_aabb				= m_geometry->get_aabb( m_aabb );
	m_aabb.modify		( matrix );
}

float4x4 const& geometry_instance::get_matrix	( ) const
{
	return				m_matrix;
}

bool geometry_instance::aabb_query				( object const* object, aabb const& aabb, triangles_type& triangles ) const
{
	return				m_geometry->cuboid_query( object, math::cuboid( aabb, m_inverted_matrix ), triangles );
}

bool geometry_instance::cuboid_query			( object const* object, cuboid const& cuboid, triangles_type& triangles ) const
{
	return				m_geometry->cuboid_query( object, math::cuboid( cuboid, m_inverted_matrix ), triangles );
}

class ray_query_helper : private boost::noncopyable {
public:
	inline		ray_query_helper				(
			float const	factor,
			triangles_predicate_type const& predicate
		) :
		m_predicate		( predicate ),
		m_factor		( factor )
	{
	}

	inline bool predicate						( xray::collision::ray_triangle_result const& triangle )
	{
		xray::collision::ray_triangle_result&	raw_triangle = const_cast<xray::collision::ray_triangle_result&>( triangle );
		raw_triangle.distance					*= m_factor;
		return									m_predicate( raw_triangle );
	}

private:
	triangles_predicate_type const& m_predicate;
	float const						m_factor;
}; // class ray_query_helper

bool geometry_instance::ray_query				(
		object const* object,
		float3 const& origin,
		float3 const& direction,
		float max_distance,
		float& distance,
		ray_triangles_type& triangles,
		triangles_predicate_type const& predicate
	) const
{
	float3 const new_origin		= m_inverted_matrix.transform_position(origin);
	float3 const new_direction	= m_inverted_matrix.transform_direction(direction);
	float const new_max_distance = magnitude(m_inverted_matrix.transform_position(origin + direction*max_distance) - new_origin);
	ray_query_helper			helper(max_distance/new_max_distance, predicate );
	if ( !m_geometry->ray_query( object, new_origin, new_direction, new_max_distance, distance, triangles, triangles_predicate_type( &helper, &ray_query_helper::predicate) ) )
		return					false;

	return						true;
}

void geometry_instance::generate_contacts		( on_contact& c, xray::collision::geometry_instance const& oi ) const
{
	get_geometry()->generate_contacts( c,m_matrix, oi.get_matrix(), (*oi.get_geometry()) );
}

bool geometry_instance::aabb_test				( aabb const& aabb ) const
{
	return						m_geometry->cuboid_test( cuboid( aabb, m_inverted_matrix ));
}

bool geometry_instance::cuboid_test				( cuboid const& cuboid ) const
{
	return						m_geometry->cuboid_test( math::cuboid( cuboid, m_inverted_matrix ));
}

bool geometry_instance::ray_test				( float3 const& origin, float3 const& direction, float max_distance, float& distance ) const
{
	float3 const new_origin		= m_inverted_matrix.transform_position(origin);
	float3 const new_direction	= m_inverted_matrix.transform_direction(direction);
	float const new_max_distance= magnitude(m_inverted_matrix.transform_position(origin + direction*max_distance) - new_origin);
	if ( !m_geometry->ray_test( new_origin, new_direction, new_max_distance, distance ) )
		return					false;

	distance					*= max_distance/new_max_distance;
	return						true;
}

void geometry_instance::add_triangles			( triangles_type& triangles ) const
{
	m_geometry->add_triangles	( triangles );
}
bool geometry_instance::is_valid( ) const
{
	return true;
}
xray::math::aabb const& geometry_instance::get_aabb	( ) const
{
	return						m_aabb;
}

void geometry_instance::render					( xray::render::debug::renderer& renderer ) const
{
	m_geometry->render			( renderer, m_matrix );
}