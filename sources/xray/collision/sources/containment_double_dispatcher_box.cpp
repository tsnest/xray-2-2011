////////////////////////////////////////////////////////////////////////////
//	Created		: 06.09.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "containment_double_dispatcher.h"
#include "box_geometry_instance.h"
#include "sphere_geometry_instance.h"
#include "cylinder_geometry_instance.h"
#include "capsule_geometry_instance.h"
#include "truncated_sphere_geometry_instance.h"
#include <xray/collision/geometry.h>
#include <xray/collision/geometry_instance.h>

namespace xray{
namespace collision	{

float3 closest_point_on_segment( float3 const& point, float3 const& segment_origin, float3 const& segment_displacement );

static float3 const cuboid_vertices[] = {
	float3( -1.f, -1.f, -1.f ),
	float3( -1.f, -1.f, +1.f ),
	float3( -1.f, +1.f, -1.f ),
	float3( -1.f, +1.f, +1.f ),
	float3( +1.f, -1.f, -1.f ),
	float3( +1.f, -1.f, +1.f ),
	float3( +1.f, +1.f, -1.f ),
	float3( +1.f, +1.f, +1.f ),
};

static inline bool is_inside_obb		( float3 const& vertex, float3 const& obb, float4x4 const& obb_transform )
{
	float3 const& center_to_vertex	= vertex - obb_transform.c.xyz( );
	float const projection_on_x		= math::abs( center_to_vertex | obb_transform.i.xyz( ) );
	if ( projection_on_x > obb.x )
		return					false;

	float const projection_on_y		= math::abs( center_to_vertex | obb_transform.j.xyz( ) );
	if ( projection_on_y > obb.y )
		return					false;

	float const projection_on_z		= math::abs( center_to_vertex | obb_transform.k.xyz( ) );
	if ( projection_on_z > obb.z )
		return					false;

	return						true;
}

static inline bool is_inside_sphere		( float3 const& vertex, float3 const& sphere_center, float const squared_sphere_radius )
{
	return  ( vertex - sphere_center ).squared_length( ) <= squared_sphere_radius;
}

static inline bool is_inside_cylinder	( float3 const& vertex, float const cylinder_half_height, float const squared_cylinder_radius, float4x4 const& cylinder_transform )
{
	float3 const& center_to_vertex	= vertex - cylinder_transform.c.xyz( );
	float const projection_on_axe	= center_to_vertex | cylinder_transform.j.xyz( );

	if ( math::abs( projection_on_axe ) > cylinder_half_height )
		return					false;

	float3 const& projection_to_vertex	= cylinder_transform.j.xyz( ) * projection_on_axe - center_to_vertex;
	return						projection_to_vertex.squared_length( ) <= squared_cylinder_radius;
}

static inline bool is_inside_capsule	( float3 const& vertex, float3 const& capsule_origin, float3 const& capsule_displacement, float const squared_capsule_radius )
{
	float3 capsule_to_vertex_closest_point = closest_point_on_segment( vertex, capsule_origin, capsule_displacement );
	return  ( vertex - capsule_to_vertex_closest_point ).squared_length( ) <= squared_capsule_radius;
}

void containment_double_dispatcher::dispatch	( box_geometry_instance const& bounding_volume,				box_geometry_instance const& testee )
{
	float4x4 matrix( m_testee.get_matrix( ) );
	float3 const& testee_half_sides	= testee.get_matrix().get_scale();
	matrix.i *= testee_half_sides.x;
	matrix.j *= testee_half_sides.y;
	matrix.k *= testee_half_sides.z;

	for ( u32 i=0; i < array_size( cuboid_vertices ); ++i )
	{
		float3 const& vertex	= matrix.transform_position( cuboid_vertices[i] );
		if( !is_inside_obb( vertex, bounding_volume.get_matrix().get_scale( ), m_bounding_volume.get_matrix( ) ) )
			return;

#ifndef MASTER_GOLD
		m_renderer->draw_sphere_solid	( *m_scene, vertex, 0.2f, math::color( 0, 0, 255, 255 ) );
#endif //#ifndef MASTER_GOLD

	}

	m_result = true;
}

void containment_double_dispatcher::dispatch	( sphere_geometry_instance const& bounding_volume,			box_geometry_instance const& testee )
{
	float4x4 matrix( m_testee.get_matrix( ) );
	float3 const& testee_half_sides	= testee.get_matrix().get_scale();
	matrix.i *= testee_half_sides.x;
	matrix.j *= testee_half_sides.y;
	matrix.k *= testee_half_sides.z;

	float const squared_sphere_radius = math::sqr( bounding_volume.radius( ) );

	for ( u32 i=0; i < array_size( cuboid_vertices ); ++i )
	{
		float3 const& vertex		= matrix.transform_position( cuboid_vertices[i] );
		if( !is_inside_sphere( vertex, m_bounding_volume.get_matrix( ).c.xyz( ), squared_sphere_radius ) )
			return;

#ifndef MASTER_GOLD
		m_renderer->draw_sphere_solid	( *m_scene, vertex, 0.2f, math::color( 0, 0, 255, 255 ) );
#endif //#ifndef MASTER_GOLD
	}

	m_result = true;
}


void containment_double_dispatcher::dispatch	( cylinder_geometry_instance const& bounding_volume,			box_geometry_instance const& testee )
{
	float4x4 matrix( m_testee.get_matrix( ) );
	float3 const& testee_half_sides	= testee.get_matrix().get_scale();
	matrix.i *= testee_half_sides.x;
	matrix.j *= testee_half_sides.y;
	matrix.k *= testee_half_sides.z;

	float const squared_cylinder_radius = math::sqr( bounding_volume.radius( ) );

	for ( u32 i=0; i < array_size( cuboid_vertices ); ++i )
	{
		float3 const& vertex		= matrix.transform_position( cuboid_vertices[i] );
		if( !is_inside_cylinder( vertex, bounding_volume.half_length( ), squared_cylinder_radius, m_bounding_volume.get_matrix( ) ) )
			return;

//#ifndef MASTER_GOLD
//		m_renderer->draw_sphere_solid	( *m_scene, vertex, 0.2f, math::color( 0, 0, 255, 255 ) );
//#endif //#ifndef MASTER_GOLD
	}

	m_result = true;
}

void containment_double_dispatcher::dispatch	( capsule_geometry_instance const& bounding_volume,			box_geometry_instance const& testee )
{
	float4x4 matrix( m_testee.get_matrix( ) );
	float3 const& testee_half_sides	= testee.get_matrix().get_scale();
	matrix.i *= testee_half_sides.x;
	matrix.j *= testee_half_sides.y;
	matrix.k *= testee_half_sides.z;

	float const		squared_capsule_radius	= math::sqr( bounding_volume.radius( ) );
	float3 const	capsule_origin			= m_bounding_volume.get_matrix( ).c.xyz( ) - m_bounding_volume.get_matrix( ).j.xyz( ) * bounding_volume.half_length( );
	float3 const	capsule_displacement	= m_bounding_volume.get_matrix( ).j.xyz( ) * ( bounding_volume.half_length( )* 2 );

	for ( u32 i=0; i < array_size( cuboid_vertices ); ++i )
	{
		float3 const& vertex		= matrix.transform_position( cuboid_vertices[i] );
		if( !is_inside_capsule( vertex, capsule_origin, capsule_displacement, squared_capsule_radius ) )
			return;

//#ifndef MASTER_GOLD
//		m_renderer->draw_sphere_solid	( *m_scene, vertex, 0.2f, math::color( 0, 0, 255, 255 ) );
//#endif //#ifndef MASTER_GOLD
	}

	m_result = true;
}

void containment_double_dispatcher::dispatch	( truncated_sphere_geometry_instance const& bounding_volume,	box_geometry_instance const& testee )
{
	float4x4 matrix( m_testee.get_matrix( ) );
	float3 const& testee_half_sides	= testee.get_matrix().get_scale();
	matrix.i *= testee_half_sides.x;
	matrix.j *= testee_half_sides.y;
	matrix.k *= testee_half_sides.z;

	float const squared_sphere_radius = math::sqr( bounding_volume.radius( ) );

	for ( u32 i=0; i < array_size( cuboid_vertices ); ++i )
	{
		float3 const& vertex		= matrix.transform_position( cuboid_vertices[i] );
		if( !is_inside_sphere( vertex, m_bounding_volume.get_matrix( ).c.xyz( ), squared_sphere_radius ) )
			return;

		float3 volume_to_vertex					= vertex - m_bounding_volume.get_matrix( ).c.xyz( );
		float4x4 const& sphere_matrix			= m_bounding_volume.get_matrix( );
		buffer_vector< float4 > const& planes	= bounding_volume.planes( );

		u32 count = planes.size( );
		for ( u32 i = 0; i < count; ++i )
		{
			float4  plane					= planes[i];
			sphere_matrix.transform			( plane.xyz( ) );
			float	plane_projection		= ( volume_to_vertex - plane.xyz( ) * plane.w * bounding_volume.radius( ) ) | plane.xyz( );
			if( plane_projection > 0 )
				return;
		}

//#ifndef MASTER_GOLD
//		m_renderer->draw_sphere_solid	( *m_scene, vertex, 0.2f, math::color( 0, 0, 255, 255 ) );
//#endif //#ifndef MASTER_GOLD
	}

	m_result = true;
}

} // namespace collision
} // namespace xray
