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

void containment_double_dispatcher::dispatch	( box_geometry_instance const& bounding_volume,				sphere_geometry_instance const& testee )
{
	float3 const& center_to_sphere_center	= m_testee.get_matrix( ).c.xyz( ) - m_bounding_volume.get_matrix( ).c.xyz( );
	float const projection_on_x				= math::abs( center_to_sphere_center | m_bounding_volume.get_matrix( ).i.xyz( ) );
	float3 const& bounding_volume_half_sides = bounding_volume.get_matrix().get_scale();

	if ( projection_on_x + testee.radius( ) > bounding_volume_half_sides.x )
		return;

	float const projection_on_y				= math::abs( center_to_sphere_center | m_bounding_volume.get_matrix( ).j.xyz( ) );
	if ( projection_on_y + testee.radius( ) > bounding_volume_half_sides.y )
		return;

	float const projection_on_z				= math::abs( center_to_sphere_center | m_bounding_volume.get_matrix( ).k.xyz( ) );
	if ( projection_on_z + testee.radius( ) > bounding_volume_half_sides.z )
		return;

	m_result = true;
}

void containment_double_dispatcher::dispatch	( sphere_geometry_instance const& bounding_volume,			sphere_geometry_instance const& testee )
{
	if( testee.radius( ) > bounding_volume.radius( ) )
		return;

	m_result = ( m_testee.get_matrix( ).c.xyz( ) - m_bounding_volume.get_matrix( ).c.xyz( ) ).squared_length( ) <= math::sqr( bounding_volume.radius( ) - testee.radius( ) );
}
void containment_double_dispatcher::dispatch	( cylinder_geometry_instance const& bounding_volume,			sphere_geometry_instance const& testee )
{
	if( testee.radius( ) > bounding_volume.radius( ) )
		return;

	float3 const& center_to_sphere_center	= m_testee.get_matrix( ).c.xyz( ) - m_bounding_volume.get_matrix( ).c.xyz( );
	float const projection_on_axe			= center_to_sphere_center | m_bounding_volume.get_matrix( ).j.xyz( );

	if ( math::abs( projection_on_axe ) + testee.radius( ) > bounding_volume.half_length( ) )
		return;

	float3 const& projection_to_sphere_center	= m_bounding_volume.get_matrix( ).j.xyz( ) * projection_on_axe - center_to_sphere_center;
	m_result =	projection_to_sphere_center.squared_length( ) <= math::sqr( bounding_volume.radius( ) - testee.radius( ) );
}

void containment_double_dispatcher::dispatch	( capsule_geometry_instance const& bounding_volume,			sphere_geometry_instance const& testee )
{
	if( testee.radius( ) > bounding_volume.radius( ) )
		return;

//	float const		squared_capsule_radius	= math::sqr( bounding_volume.radius( ) );
	float3 const	capsule_origin			= m_bounding_volume.get_matrix( ).c.xyz( ) - m_bounding_volume.get_matrix( ).j.xyz( ) * bounding_volume.half_length( );
	float3 const	capsule_displacement	= m_bounding_volume.get_matrix( ).j.xyz( ) * ( bounding_volume.half_length( )* 2 );

	float3 const	closest_point			= closest_point_on_segment( m_testee.get_matrix( ).c.xyz( ), capsule_origin, capsule_displacement );

	if( ( m_testee.get_matrix( ).c.xyz( ) - closest_point ).squared_length( ) > math::sqr( bounding_volume.radius( ) - testee.radius( ) ) )
		return;

	m_result = true;
}

void containment_double_dispatcher::dispatch	( truncated_sphere_geometry_instance const& bounding_volume, sphere_geometry_instance const& testee )
{
	if( testee.radius( ) > bounding_volume.radius( ) )
		return;

	if( ( m_testee.get_matrix( ).c.xyz( ) - m_bounding_volume.get_matrix( ).c.xyz( ) ).squared_length( ) > math::sqr( bounding_volume.radius( ) - testee.radius( ) ) )
		return;

	buffer_vector< float4 > const& planes	= bounding_volume.planes( );
	float3 volume_ot_testee_position		= m_testee.get_matrix( ).c.xyz( ) - m_bounding_volume.get_matrix( ).c.xyz( );
	float4x4 const& sphere_matrix			= m_bounding_volume.get_matrix( );

	u32 count = planes.size( );
	for ( u32 i = 0; i < count; ++i )
	{
		float4  plane					= planes[i];
		sphere_matrix.transform			( plane.xyz( ) );
		float	plane_projection		= ( volume_ot_testee_position - ( plane.xyz( ) * plane.w * bounding_volume.radius( ) ) ) | plane.xyz( );
		if( plane_projection > -testee.radius( ) )
			return;
	}

	m_result = true;
}

} // namespace collision
} // namespace xray
