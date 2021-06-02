////////////////////////////////////////////////////////////////////////////
//	Created		: 15.09.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "triangle_mesh_geometry_instance.h"
#include "triangle_mesh_geometry.h"

namespace xray {
namespace collision {

triangle_mesh_geometry_instance::triangle_mesh_geometry_instance	( float4x4 const& matrix, non_null<triangle_mesh_geometry const>::ptr triangle_mesh ) :
	m_matrix			( matrix ),
	m_inverted_matrix	( math::invert4x3( matrix ) ),
	m_triangle_mesh		( triangle_mesh )
{
}

triangle_mesh_geometry_instance::~triangle_mesh_geometry_instance( )
{
}

bool triangle_mesh_geometry_instance::aabb_query			( object const* object, math::aabb const& aabb, triangles_type& triangles ) const
{
	return m_triangle_mesh->cuboid_query( object, math::cuboid( aabb, m_inverted_matrix ), triangles );
}

bool triangle_mesh_geometry_instance::cuboid_query			( object const* object, math::cuboid const& cuboid, triangles_type& triangles ) const
{
	return m_triangle_mesh->cuboid_query( object, math::cuboid( cuboid, m_inverted_matrix ), triangles );
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

bool triangle_mesh_geometry_instance::ray_query				(
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
	float const new_max_distance = length(m_inverted_matrix.transform_position(origin + direction*max_distance) - new_origin);
	ray_query_helper			helper(max_distance/new_max_distance, predicate );
	if ( !m_triangle_mesh->ray_query( object, new_origin, new_direction, new_max_distance, distance, triangles, triangles_predicate_type( &helper, &ray_query_helper::predicate) ) )
		return					false;

	return						true;
}

bool triangle_mesh_geometry_instance::aabb_test				( math::aabb const& aabb ) const
{
	return	m_triangle_mesh->cuboid_test( math::cuboid( aabb, m_inverted_matrix ));
}

bool triangle_mesh_geometry_instance::cuboid_test			( math::cuboid const& cuboid ) const
{
	return m_triangle_mesh->cuboid_test( math::cuboid( cuboid, m_inverted_matrix ));
}

bool triangle_mesh_geometry_instance::ray_test				( float3 const& origin, float3 const& direction, float max_distance, float& distance ) const
{
	float3 const new_origin		= m_inverted_matrix.transform_position(origin);
	float3 const new_direction	= m_inverted_matrix.transform_direction(direction);
	float const new_max_distance= length(m_inverted_matrix.transform_position(origin + direction*max_distance) - new_origin);
	if ( !m_triangle_mesh->ray_test( new_origin, new_direction, new_max_distance, distance ) )
		return					false;

	distance					*= max_distance/new_max_distance;
	return						true;
}

void triangle_mesh_geometry_instance::add_triangles			( triangles_type& triangles ) const
{
	m_triangle_mesh->add_triangles	( triangles );
}

bool triangle_mesh_geometry_instance::is_valid				( ) const
{
	return						true;
}

xray::math::aabb triangle_mesh_geometry_instance::get_aabb	( ) const
{
	return						math::aabb( m_triangle_mesh->get_aabb( ) ).modify( m_matrix );
}

math::aabb triangle_mesh_geometry_instance::get_geometry_aabb	( ) const
{
	return						m_triangle_mesh->get_aabb( );
}

void triangle_mesh_geometry_instance::enumerate_primitives	( enumerate_primitives_callback& cb ) const
{
	m_triangle_mesh->enumerate_primitives( get_matrix( ), cb );
}

void triangle_mesh_geometry_instance::enumerate_primitives	( float4x4 const& transform, enumerate_primitives_callback& cb ) const
{
	m_triangle_mesh->enumerate_primitives( get_matrix( ) * transform, cb );
}

void triangle_mesh_geometry_instance::render				( xray::render::scene_ptr const& scene, xray::render::debug::renderer& renderer ) const
{
	render					( scene, renderer, m_matrix );
}

void triangle_mesh_geometry_instance::render		( render::scene_ptr const& scene, render::debug::renderer& renderer, float4x4 const& transform ) const
{
	m_triangle_mesh->render			( scene, renderer, transform );
}

void triangle_mesh_geometry_instance::destroy( memory::base_allocator* allocator )
{
	XRAY_UNREFERENCED_PARAMETER	( allocator );
}

math::float3 const*	triangle_mesh_geometry_instance::vertices	( ) const
{
	return m_triangle_mesh->vertices				( );
}

u32 triangle_mesh_geometry_instance::vertex_count				( ) const
{
	return m_triangle_mesh->vertex_count			( );
}

u32 const* triangle_mesh_geometry_instance::indices				( ) const
{
	return m_triangle_mesh->indices					( );
}

u32 const* triangle_mesh_geometry_instance::indices				( u32 triangle_id ) const
{
	return m_triangle_mesh->indices					( triangle_id );
}

u32 triangle_mesh_geometry_instance::index_count				( )	const
{
	return m_triangle_mesh->index_count				( );
}

float triangle_mesh_geometry_instance::get_surface_area			( ) const
{
	return m_triangle_mesh->get_surface_area		( );
}

float3 triangle_mesh_geometry_instance::get_random_surface_point( math::random32& randomizer ) const
{
	XRAY_UNREFERENCED_PARAMETER		( randomizer );
	NOT_IMPLEMENTED					( return float3() );
}

float3 triangle_mesh_geometry_instance::get_closest_point_to( float3 const& point, float4x4 const& origin ) const
{
	XRAY_UNREFERENCED_PARAMETER		( point );
	float4x4 transform				= origin * m_matrix;
	NOT_IMPLEMENTED					( return transform.c.xyz( ); );
}

} // namespace collision
} // namespace xray