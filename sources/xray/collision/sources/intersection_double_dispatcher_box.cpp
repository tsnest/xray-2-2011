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

static void closest_point_to_point ( float3& result, float4x4 const& box_matrix, float3 const& box_half_sides, float3 const& point )
{
	float3 const		box_to_point	= point - box_matrix.c.xyz( );

	float x				= box_to_point | box_matrix.i.xyz( );
	math::clamp			( x, -box_half_sides.x, +box_half_sides.x );

	float y				= box_to_point | box_matrix.j.xyz( );
	math::clamp			( y, -box_half_sides.y, +box_half_sides.y );

	float z				= box_to_point | box_matrix.k.xyz( );
	math::clamp			( z, -box_half_sides.z, +box_half_sides.z );

	result				= box_matrix.c.xyz( ) + ( box_matrix.i.xyz( ) * x + box_matrix.j.xyz( ) * y + box_matrix.k.xyz( ) * z );
}

void intersection_double_dispatcher::dispatch	( box_geometry_instance const& bounding_volume,				box_geometry_instance const& testee )
{
	XRAY_UNREFERENCED_PARAMETERS( bounding_volume, testee );
}

void intersection_double_dispatcher::dispatch	( sphere_geometry_instance const& bounding_volume,			box_geometry_instance const& testee )
{
	float3 const&		bounding_position			= m_bounding_volume->get_matrix( ).c.xyz( );

//#ifndef MASTER_GOLD
//	m_renderer->draw_sphere_solid	( *m_scene, bounding_position, 0.2f, math::color( 0, 0, 255, 255 ) );
//	m_renderer->draw_sphere_solid	( *m_scene, m_testee->get_matrix( ).c.xyz( ), 0.2f, math::color( 255, 0, 0, 255 ) );
//#endif //#ifndef MASTER_GOLD

	float3				closest_point;
	closest_point_to_point( closest_point, m_testee->get_matrix( ), testee.get_matrix().get_scale( ), bounding_position );

//#ifndef MASTER_GOLD
//	m_renderer->draw_sphere_solid	( *m_scene, closest_point, 0.2f, math::color( 255, 0, 0, 255 ) );
//	m_renderer->draw_line			( *m_scene, closest_point, m_testee->get_matrix( ).c.xyz( ), math::color( 255, 0, 0, 255 ) );
//#endif //#ifndef MASTER_GOLD

	m_result			= ( closest_point - bounding_position ).squared_length( ) < math::sqr( bounding_volume.radius( ) );
}

void intersection_double_dispatcher::dispatch	( cylinder_geometry_instance const& bounding_volume,			box_geometry_instance const& testee )
{
	XRAY_UNREFERENCED_PARAMETERS( bounding_volume, testee );
	NOT_IMPLEMENTED( );
}

void intersection_double_dispatcher::dispatch	( capsule_geometry_instance const& bounding_volume,			box_geometry_instance const& testee )
{
	float4x4 inverted_matrix	= math::invert4x3( m_testee->get_matrix( ) );

	float3 const&	l			= inverted_matrix.transform_direction( m_bounding_volume->get_matrix( ).j.xyz( ) );
	float3 const&	mid			= inverted_matrix.transform_position( m_bounding_volume->get_matrix( ).c.xyz( ) );
	float3 const&	bE			= testee.get_matrix( ).get_scale( );
	float const		hl			= bounding_volume.half_length( );
	float const		radius		= bounding_volume.radius( );

	//
	// http://www.vr-online.ru/content/i-vot-oni-stolknulis%E2%80%A6-634
	//

    /* ALGORITHM: Use the separating axis
    theorem to see if the line segment
    and the box overlap. A line
    segment is a degenerate OBB. */

    float3 const T = mid;
    float3	v;
    float	r;

    //do any of the principal axes
    //form a separating axis?

	if( math::abs( T.x ) > bE.x + hl * math::abs( l.x ) + radius )
		return;

    if( math::abs( T.y)  > bE.y + hl * math::abs( l.y ) + radius )
		return;

    if( math::abs( T.z ) > bE.z + hl * math::abs( l.z ) + radius )
		return;

    /* NOTE: Since the separating axis is
    perpendicular to the line in these
    last four cases, the line does not
    contribute to the projection. */

    //l.cross(x-axis)?

    r = bE.y * math::abs( l.z ) + bE.z * math::abs( l.y );

    if( math::abs( T.y * l.z - T.z * l.y ) > r + radius )
		return;

    //l.cross(y-axis)?

    r = bE.x * math::abs( l.z ) + bE.z * math::abs( l.x );

    if( math::abs( T.z * l.x - T.x * l.z ) > r + radius )
		return;

    //l.cross(z-axis)?

    r = bE.x * math::abs( l.y ) + bE.y * math::abs( l.x );

    if( math::abs( T.x * l.y - T.y * l.x ) > r + radius )
		return;

	m_result = true;
}

void intersection_double_dispatcher::dispatch	( truncated_sphere_geometry_instance const& bounding_volume,	box_geometry_instance const& testee )
{
	//
	//this test is not accurate, but enough for our purposes
	//

	float3 const&		bounding_position			= m_bounding_volume->get_matrix( ).c.xyz( );

#ifndef MASTER_GOLD
	m_renderer->draw_sphere_solid	( *m_scene, bounding_position, 0.2f, math::color( 0, 0, 255, 255 ) );
	m_renderer->draw_sphere_solid	( *m_scene, m_testee->get_matrix( ).c.xyz( ), 0.2f, math::color( 255, 0, 0, 255 ) );
#endif //#ifndef MASTER_GOLD

	float3				closest_point;
	closest_point_to_point( closest_point, m_testee->get_matrix( ), testee.get_matrix().get_scale( ), bounding_position );

#ifndef MASTER_GOLD
	m_renderer->draw_sphere_solid	( *m_scene, closest_point, 0.2f, math::color( 255, 0, 0, 255 ) );
	m_renderer->draw_line			( *m_scene, closest_point, m_testee->get_matrix( ).c.xyz( ), math::color( 255, 0, 0, 255 ) );
#endif //#ifndef MASTER_GOLD

	if( ( closest_point - bounding_position ).squared_length( ) > math::sqr( bounding_volume.radius( ) ) )
		return;

	//planes
	buffer_vector< float4 > const& planes	= bounding_volume.planes( );
	float3 volume_to_closest_point			= closest_point - bounding_position;
	float4x4 const& sphere_matrix			= m_bounding_volume->get_matrix( );

	u32 count = planes.size( );
	for ( u32 i = 0; i < count; ++i )
	{
		float4  plane					= planes[i];
		sphere_matrix.transform			( plane.xyz( ) );
		float	plane_projection		= volume_to_closest_point | plane.xyz( );
		if( plane_projection > plane.w * bounding_volume.radius( ) )
			return;
	}

	m_result = true;
}

} // namespace collision
} // namespace xray
