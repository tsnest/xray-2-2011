////////////////////////////////////////////////////////////////////////////
//	Created		: 19.09.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "intersection_double_dispatcher.h"
#include "box_geometry_instance.h"
#include "sphere_geometry_instance.h"
#include "cylinder_geometry_instance.h"
#include "capsule_geometry_instance.h"
#include "truncated_sphere_geometry_instance.h"
#include <xray/collision/geometry.h>
#include <xray/collision/geometry_instance.h>

namespace xray{
namespace collision{

void intersection_double_dispatcher::dispatch	( box_geometry_instance const& bounding_volume,				sphere_geometry_instance const& testee )
{
	geometry_instance const* temp	= m_testee;
	m_testee						= m_bounding_volume;
	m_bounding_volume				= temp;
	dispatch						( testee, bounding_volume );
}

void intersection_double_dispatcher::dispatch	( sphere_geometry_instance const& bounding_volume,			sphere_geometry_instance const& testee )
{
	if( ( m_testee->get_matrix( ).c.xyz( ) - m_bounding_volume->get_matrix( ).c.xyz( ) ).squared_length( ) > math::sqr( bounding_volume.radius( ) + testee.radius( ) ) )
		return;

	m_result = true;
}

void intersection_double_dispatcher::dispatch	( cylinder_geometry_instance const& bounding_volume,			sphere_geometry_instance const& testee )
{
	XRAY_UNREFERENCED_PARAMETERS( bounding_volume, testee );
	NOT_IMPLEMENTED( );

	//float3 const& bouding_volume_center		= m_bounding_volume->get_matrix( ).c.xyz( );
	//float3 const& testee_center				= m_testee->get_matrix( ).c.xyz( );
	//float3 const& bouding_volume_up			= m_testee->get_matrix( ).j.xyz( );

	//float3 bounding_volume_to_testee		= testee_center - bouding_volume_center;
	//float projection						= bounding_volume_to_testee | bouding_volume_up;

	//if( math::abs( projection ) > bounding_volume.half_length( ) + testee.radius( ) )
	//	return;

	//if( ( projection * bouding_volume_up - testee_center ).squared_length( ) > math::sqr( bounding_volume.radius( ) + testee.radius( ) ) )
	//	return;

	//float3 cross = bouding_volume_up ^ bounding_volume_to_testee;
	//if( cross.x == 0 && cross.y == 0 && cross.z == 0 )
	//{
	//	m_result = true;
	//	return;
	//}
	//	
	//float3 closest_edge_center	= ( bouding_volume_up ^ cross ) * bounding_volume.radius( );
	//float squared_testee_radius	= math::sqr( testee.radius( ) ); 

	//float3 point				= closest_edge_center + bouding_volume_up * bounding_volume.half_length( );
	//if( ( testee_center - point ).squared_length( ) < squared_testee_radius )
	//{
	//	m_result = true;
	//	return;
	//}

	//point						= closest_edge_center - bouding_volume_up * bounding_volume.half_length( );
	//if( ( testee_center - point ).squared_length( ) < squared_testee_radius )
	//{
	//	m_result = true;
	//	return;
	//}
}

void intersection_double_dispatcher::dispatch	( capsule_geometry_instance const& bounding_volume,			sphere_geometry_instance const& testee )
{
	float3 const& capsule_center		= m_bounding_volume->get_matrix( ).c.xyz( );
	float3 const& capsule_orientation	= m_bounding_volume->get_matrix( ).j.xyz( );
	float3 const& sphere_center			= m_testee->get_matrix( ).c.xyz( );

	float3 capsule_start_point			= capsule_center - capsule_orientation * bounding_volume.half_length( );
	float3 capsule_end_point			= capsule_center + capsule_orientation * bounding_volume.half_length( );
	
	float3 v				= capsule_end_point - capsule_start_point;
	float3 w				= sphere_center - capsule_start_point;
	float t					= ( w | v ) / ( v | v );
	math::clamp				( t, 0.f, 1.f );

	float3 closest_point	= capsule_start_point + v * t;

//#ifndef MASTER_GOLD
//	m_renderer->draw_line			( *m_scene, capsule_start_point, capsule_end_point, math::color( 0, 0, 255, 255 ) );
//	m_renderer->draw_line			( *m_scene, closest_point, sphere_center, math::color( 0, 0, 255, 255 ) );
//	m_renderer->draw_sphere_solid	( *m_scene, closest_point, 0.2f, math::color( 0, 0, 255, 255 ) );
//	m_renderer->draw_sphere_solid	( *m_scene, sphere_center, 0.2f, math::color( 0, 0, 255, 255 ) );
//#endif //#ifndef MASTER_GOLD

	if( ( closest_point - sphere_center ).squared_length( ) > math::sqr( bounding_volume.radius( ) + testee.radius( ) ) )
		return;

	m_result = true;
}

void intersection_double_dispatcher::dispatch	( truncated_sphere_geometry_instance const& bounding_volume,	sphere_geometry_instance const& testee )
{
	if( ( m_testee->get_matrix( ).c.xyz( ) - m_bounding_volume->get_matrix( ).c.xyz( ) ).squared_length( ) > math::sqr( bounding_volume.radius( ) + testee.radius( ) ) )
		return;

	buffer_vector< float4 > const& planes	= bounding_volume.planes( );
	float3 volume_to_testee_position		= m_testee->get_matrix( ).c.xyz( ) - m_bounding_volume->get_matrix( ).c.xyz( );
	float4x4 const& sphere_matrix			= m_bounding_volume->get_matrix( );

	u32 count = planes.size( );
	for ( u32 i = 0; i < count; ++i )
	{
		float4  plane					= planes[i];
		sphere_matrix.transform			( plane.xyz( ) );
		float	plane_projection		= volume_to_testee_position | plane.xyz( );
		if( plane_projection > testee.radius( ) + plane.w * bounding_volume.radius( ) )
			return;
	}

	m_result = true;
}


} // namespace collision
} // namespace xray