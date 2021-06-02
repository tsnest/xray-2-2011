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
#include <xray/collision/geometry.h>
#include "truncated_sphere_geometry_instance.h"
#include <xray/collision/geometry_instance.h>

namespace xray{
namespace collision	{

void containment_double_dispatcher::dispatch	( box_geometry_instance const& bounding_volume,				capsule_geometry_instance const& testee )
{
	float3 const capsule_start_point		= m_testee.get_matrix( ).c.xyz( ) - m_testee.get_matrix( ).j.xyz( ) * testee.half_length( );
	float3 center_to_point_center			= capsule_start_point - m_bounding_volume.get_matrix( ).c.xyz( );

	float3 const& box_scale					= bounding_volume.get_matrix().get_scale();
	float const testee_radius				= testee.radius();

	float projection_on_x					= math::abs( center_to_point_center | m_bounding_volume.get_matrix( ).i.xyz( ) );
	if ( projection_on_x + testee_radius > box_scale.x )
		return;

	float projection_on_y					= math::abs( center_to_point_center | m_bounding_volume.get_matrix( ).j.xyz( ) );
	if ( projection_on_y + testee_radius > box_scale.y )
		return;

	float projection_on_z					= math::abs( center_to_point_center | m_bounding_volume.get_matrix( ).k.xyz( ) );
	if ( projection_on_z + testee_radius > box_scale.z )
		return;

	float3 const capsule_end_point			= m_testee.get_matrix( ).c.xyz( ) + m_testee.get_matrix( ).j.xyz( ) * testee.half_length( );
	center_to_point_center					= capsule_end_point - m_bounding_volume.get_matrix( ).c.xyz( );

	projection_on_x							= math::abs( center_to_point_center | m_bounding_volume.get_matrix( ).i.xyz( ) );
	if ( projection_on_x + testee.radius( ) > box_scale.x )
		return;

	projection_on_y							= math::abs( center_to_point_center | m_bounding_volume.get_matrix( ).j.xyz( ) );
	if ( projection_on_y + testee.radius( ) > box_scale.y )
		return;

	projection_on_z							= math::abs( center_to_point_center | m_bounding_volume.get_matrix( ).k.xyz( ) );
	if ( projection_on_z + testee.radius( ) > box_scale.z )
		return;

	m_result = true;
}

void containment_double_dispatcher::dispatch	( sphere_geometry_instance const& bounding_volume,			capsule_geometry_instance const& testee )
{
	if( testee.radius( ) > bounding_volume.radius( ) )
		return;

	float3 const capsule_start_point		= m_testee.get_matrix( ).c.xyz( ) - m_testee.get_matrix( ).j.xyz( ) * testee.half_length( );
	if( ( capsule_start_point - m_bounding_volume.get_matrix( ).c.xyz( ) ).squared_length( ) > math::sqr( bounding_volume.radius( ) - testee.radius( ) ) )
		return;

	float3 const capsule_end_point			= m_testee.get_matrix( ).c.xyz( ) + m_testee.get_matrix( ).j.xyz( ) * testee.half_length( );
	if( ( capsule_end_point - m_bounding_volume.get_matrix( ).c.xyz( ) ).squared_length( ) > math::sqr( bounding_volume.radius( ) - testee.radius( ) ) )
		return;

	m_result = true;
}

void containment_double_dispatcher::dispatch	( cylinder_geometry_instance const& bounding_volume,			capsule_geometry_instance const& testee )
{
	XRAY_UNREFERENCED_PARAMETERS( bounding_volume, testee );
}

void containment_double_dispatcher::dispatch	( capsule_geometry_instance const& bounding_volume,				capsule_geometry_instance const& testee )
{
	XRAY_UNREFERENCED_PARAMETERS( bounding_volume, testee );
}

void containment_double_dispatcher::dispatch	( truncated_sphere_geometry_instance const& bounding_volume,	capsule_geometry_instance const& testee )
{
	if( testee.radius( ) > bounding_volume.radius( ) )
		return;

	float3 first_testee_end_point		= m_testee.get_matrix( ).c.xyz( ) + m_testee.get_matrix( ).j.xyz( ) * testee.half_length( );
	float3 second_testee_end_point		= m_testee.get_matrix( ).c.xyz( ) - m_testee.get_matrix( ).j.xyz( ) * testee.half_length( );
	float3 const& bounding_volume_point	= m_bounding_volume.get_matrix( ).c.xyz( );

	if( ( first_testee_end_point - bounding_volume_point ).squared_length( ) > math::sqr( bounding_volume.radius( ) - testee.radius( ) ) )
		return;

	if( ( second_testee_end_point - bounding_volume_point ).squared_length( ) > math::sqr( bounding_volume.radius( ) - testee.radius( ) ) )
		return;

	buffer_vector< float4 > const& planes	= bounding_volume.planes( );
	float3 volume_to_first_testee_point		= first_testee_end_point - bounding_volume_point;
	float3 volume_to_second_testee_point	= second_testee_end_point - bounding_volume_point;
	float4x4 const& sphere_matrix			= m_bounding_volume.get_matrix( );

	u32 count = planes.size( );
	for ( u32 i = 0; i < count; ++i )
	{
		float4  plane					= planes[i];
		sphere_matrix.transform			( plane.xyz( ) );
		float3	plane_point				= plane.xyz( ) * plane.w * bounding_volume.radius( );
		float	first_plane_projection	= ( volume_to_first_testee_point - plane_point ) | plane.xyz( );
		if( first_plane_projection > -testee.radius( ) )
			return;

		float	second_plane_projection	= ( volume_to_second_testee_point - plane_point ) | plane.xyz( );
		if( second_plane_projection > -testee.radius( ) )
			return;
	}

	m_result = true;
}

} // namespace collision
} // namespace xray
