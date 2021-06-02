////////////////////////////////////////////////////////////////////////////
//	Created		: 15.09.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_geometry_instance.h"
#include <xray/collision/terrain_collision_utils.h>

namespace xray {
namespace collision {

terrain_geometry_instance::terrain_geometry_instance	( float4x4 const& matrix, float phisical_size, u32 vertex_row_size, const float* heightfield, bool delete_by_collision_object ) :
	geometry_instance				( delete_by_collision_object ),
	m_matrix						( matrix ),
	m_inverted_matrix				( math::invert4x3( matrix ) ),
	m_data							( phisical_size, vertex_row_size, heightfield )
{
}

terrain_geometry_instance::~terrain_geometry_instance	( )
{
}

bool terrain_geometry_instance::aabb_query			( object const* object, math::aabb const& aabb, triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETERS	( object, &aabb, &triangles );
	NOT_IMPLEMENTED					( return false );
}

bool terrain_geometry_instance::cuboid_query		( object const* object, math::cuboid const& cuboid, triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETERS	( object, &cuboid, &triangles );
	NOT_IMPLEMENTED					( return false );
}

bool terrain_geometry_instance::ray_query			( object const* object, math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance, ray_triangles_type& triangles, triangles_predicate_type const& predicate ) const
{
	XRAY_UNREFERENCED_PARAMETERS	( object, &origin, &direction, max_distance, &distance, &triangles, &predicate );
	NOT_IMPLEMENTED					( return false );
}

bool terrain_geometry_instance::aabb_test			( math::aabb const& aabb ) const
{
	XRAY_UNREFERENCED_PARAMETER		( aabb );
	NOT_IMPLEMENTED					( return false );
}

bool terrain_geometry_instance::cuboid_test			( math::cuboid const& cuboid ) const
{
	XRAY_UNREFERENCED_PARAMETER		( cuboid );
	return false;
}

bool terrain_geometry_instance::ray_test			( math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance ) const
{
	float3 const new_origin		= m_inverted_matrix.transform_position(origin);
	float3 const new_direction	= m_inverted_matrix.transform_direction(direction);
	float const new_max_distance= length(m_inverted_matrix.transform_position(origin + direction*max_distance) - new_origin);

	if ( !collision::terrain_ray_test( m_data, new_origin, new_direction, max_distance, distance ) )
		return false;

	distance					*= max_distance/new_max_distance;
	return						true;
}

math::aabb terrain_geometry_instance::get_aabb		( ) const
{
	math::aabb tmp				= collision::terrain_aabb( m_data );
	return						tmp.modify( m_matrix );
}

math::aabb terrain_geometry_instance::get_geometry_aabb	( ) const
{
	return						collision::terrain_aabb( m_data );
}

bool terrain_geometry_instance::is_valid			( ) const
{
	return true;
}

void terrain_geometry_instance::render				( render::scene_ptr const& scene, render::debug::renderer& renderer ) const
{
	render									( scene, renderer, m_matrix );
}

void terrain_geometry_instance::render		( render::scene_ptr const& scene, render::debug::renderer& renderer, float4x4 const& transform ) const
{
	XRAY_UNREFERENCED_PARAMETERS					( &scene, &renderer, &transform );
	NOT_IMPLEMENTED( ); 
}

void terrain_geometry_instance::enumerate_primitives( enumerate_primitives_callback& cb ) const
{
	XRAY_UNREFERENCED_PARAMETER						( cb );
}

void terrain_geometry_instance::enumerate_primitives( float4x4 const& transform, enumerate_primitives_callback& cb ) const
{
	XRAY_UNREFERENCED_PARAMETERS					( &transform, &cb );
}

void terrain_geometry_instance::add_triangles		( triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETER		( triangles );
	UNREACHABLE_CODE();
}

math::float3 const* terrain_geometry_instance::vertices	( ) const
{
	NOT_IMPLEMENTED				( return 0 );
}

u32 terrain_geometry_instance::vertex_count				( ) const
{
	NOT_IMPLEMENTED				( return 0 );
}

u32 const* terrain_geometry_instance::indices			( ) const
{
	NOT_IMPLEMENTED				( return 0 );
}

u32 const* terrain_geometry_instance::indices			( u32 triangle_id ) const
{
	XRAY_UNREFERENCED_PARAMETER ( triangle_id );
	NOT_IMPLEMENTED				( return 0 );
}

u32 terrain_geometry_instance::index_count				( ) const
{
	NOT_IMPLEMENTED				( return 0 );
}

float terrain_geometry_instance::get_surface_area		( ) const
{
	NOT_IMPLEMENTED				( return 0.f );
}

float3 terrain_geometry_instance::get_random_surface_point( math::random32& randomizer ) const
{
	XRAY_UNREFERENCED_PARAMETER		( randomizer );
	NOT_IMPLEMENTED					( return float3( ) );
}

float3 terrain_geometry_instance::get_closest_point_to	( float3 const& point, float4x4 const& origin ) const
{
	XRAY_UNREFERENCED_PARAMETER		( point );

	float4x4 transorm				= origin * m_matrix;
	NOT_IMPLEMENTED					( return transorm.c.xyz( ); );
}

} // namespace collision
} // namespace xray