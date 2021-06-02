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

/*
   Calculate the line segment PaPb that is the shortest route between
   two lines P1P2 and P3P4. Calculate also the values of mua and mub where
      Pa = P1 + mua (P2 - P1)
      Pb = P3 + mub (P4 - P3)
   Return FALSE if no solution exists.
*/
float3 closest_point_on_segment( float3 const& point, float3 const& segment_origin, float3 const& segment_displacement )
{
	float domen_value	= ( ( point - segment_origin ) | segment_displacement ) / segment_displacement.squared_length( );
	math::clamp			( domen_value, 0.f, 1.f );

	return segment_origin + segment_displacement * domen_value;
}

bool line_line_intersect ( float3 const& p1, float3 const& d1, float3 const& p2, float3 const& d2, float3& pa, float3& pb, float& mua, float& mub )
{
	R_ASSERT( !math::is_zero( d1.x ) || !math::is_zero( d1.y ) || !math::is_zero( d1.z ) );
	R_ASSERT( !math::is_zero( d2.x ) || !math::is_zero( d2.y ) || !math::is_zero( d2.z ) );

	float3 const&	p34		= d2;
	float3 const&	p12		= d1;
	float3 const	p31		= p1 - p2;

	float const		d3412	= p34 | p12;
	float const		d3434	= p34 | p34;
	float const		d1212	= p12 | p12;

	float const		denom	= d1212 * d3434 - d3412 * d3412;
	if ( math::is_zero( denom ) )
	   return false;

	float const		d3134	= p31 | p34;
	float const		d3112	= p31 | p12;

	float const		numer	= d3134 * d3412 - d3112 * d3434;

	mua	= numer / denom;
	mub	= ( d3134 + d3412 * mua ) / d3434;

	pa	= p1 + mua * d1;
	pb	= p2 + mub * d2;

	return true;
}

bool line_line_intersect_non_parallel ( float3 const& p1, float3 const& d1, float3 const& p2, float3 const& d2, float3& pa, float3& pb, float& mua, float& mub )
{
	R_ASSERT( !math::is_zero( d1.x ) || !math::is_zero( d1.y ) || !math::is_zero( d1.z ) );
	R_ASSERT( !math::is_zero( d2.x ) || !math::is_zero( d2.y ) || !math::is_zero( d2.z ) );

	float3 const&	p34		= d2;
	float3 const&	p12		= d1;
	float3 const	p31		= p1 - p2;

	float const		d3412	= p34 | p12;
	float const		d3434	= p34 | p34;
	float const		d1212	= p12 | p12;

	float const		denom	= d1212 * d3434 - d3412 * d3412;
	R_ASSERT( !math::is_zero( denom ) );
	
	float const		d3134	= p31 | p34;
	float const		d3112	= p31 | p12;

	float const		numer	= d3134 * d3412 - d3112 * d3434;

	mua	= numer / denom;
	mub	= ( d3134 + d3412 * mua ) / d3434;

	pa	= p1 + mua * d1;
	pb	= p2 + mub * d2;

	return true;
}
bool segment_segment_intersect ( float3 const& p1, float3 const& p2, float3 const& p3, float3 const& p4, float max_distance )
{
	float max_distance_squared = math::sqr( max_distance );
	float3 const d1		= p2 - p1;
	float3 const d2		= p4 - p3;
	float3 const cross	= d1 ^ d2;

	if( !math::is_zero( cross.x ) || !math::is_zero( cross.y ) || !math::is_zero( cross.z ) )
	{
		float3	pa;
		float3	pb;
		float	mua;
		float	mub;

		line_line_intersect_non_parallel( p1, d1, p3, d2, pa, pb, mua, mub );

		if( ( pa - pb ).squared_length( ) > max_distance_squared )
			return false;

		//segments cross
		if( mua > 0 && mua < 1 && mub > 0 && mub < 1 )
			return true;
	}
	
	if( ( p1 - closest_point_on_segment( p1, p3, d2 ) ).squared_length( ) < max_distance_squared )
		return true;

	if( ( p2 - closest_point_on_segment( p2, p3, d2 ) ).squared_length( ) < max_distance_squared )
		return true;

	if( ( p3 - closest_point_on_segment( p3, p1, d1 ) ).squared_length( ) < max_distance_squared )
		return true;

	if( ( p4 - closest_point_on_segment( p4, p1, d1 ) ).squared_length( ) < max_distance_squared )
		return true;

	return false;
}

void intersection_double_dispatcher::dispatch	( box_geometry_instance const& bounding_volume,				capsule_geometry_instance const& testee )
{
	geometry_instance const* temp	= m_testee;
	m_testee						= m_bounding_volume;
	m_bounding_volume				= temp;
	dispatch						( testee, bounding_volume );
}

void intersection_double_dispatcher::dispatch	( sphere_geometry_instance const& bounding_volume,			capsule_geometry_instance const& testee )
{
	geometry_instance const* temp	= m_testee;
	m_testee						= m_bounding_volume;
	m_bounding_volume				= temp;
	dispatch						( testee, bounding_volume );
}

void intersection_double_dispatcher::dispatch	( cylinder_geometry_instance const& bounding_volume,			capsule_geometry_instance const& testee )
{
	XRAY_UNREFERENCED_PARAMETERS( bounding_volume, testee );
	NOT_IMPLEMENTED( );
}

void intersection_double_dispatcher::dispatch	( capsule_geometry_instance const& bounding_volume,			capsule_geometry_instance const& testee )
{
	m_result		= segment_segment_intersect			( 
		m_bounding_volume->get_matrix( ).c.xyz( ) + m_bounding_volume->get_matrix( ).j.xyz( ) * bounding_volume.half_length( ),
		m_bounding_volume->get_matrix( ).c.xyz( ) - m_bounding_volume->get_matrix( ).j.xyz( ) * bounding_volume.half_length( ),
		m_testee->get_matrix( ).c.xyz( ) + m_testee->get_matrix( ).j.xyz( ) * testee.half_length( ),
		m_testee->get_matrix( ).c.xyz( ) - m_testee->get_matrix( ).j.xyz( ) * testee.half_length( ),
		testee.radius( ) +  bounding_volume.radius( )
	);
}

void intersection_double_dispatcher::dispatch	( truncated_sphere_geometry_instance const& bounding_volume,	capsule_geometry_instance const& testee )
{
	//
	//this test is not accurate, but enough for our purposes
	//

	float3 const& capsule_center		= m_testee->get_matrix( ).c.xyz( );
	float3 const& capsule_orientation	= m_testee->get_matrix( ).j.xyz( );
	float3 const& sphere_center			= m_bounding_volume->get_matrix( ).c.xyz( );

	float3 capsule_start_point			= capsule_center - capsule_orientation * testee.half_length( );
	float3 capsule_end_point			= capsule_center + capsule_orientation * testee.half_length( );
	
	float3 v				= capsule_end_point - capsule_start_point;
	float3 w				= sphere_center - capsule_start_point;
	float t					= ( w | v ) / ( v | v );
	math::clamp				( t, 0.f, 1.f );

	float3 closest_point	= capsule_start_point + v * t;

	if( ( closest_point - sphere_center ).squared_length( ) > math::sqr( testee.radius( ) + bounding_volume.radius( ) ) )
		return;

//#ifndef MASTER_GOLD
//	m_renderer->draw_sphere_solid	( *m_scene, closest_point, 0.2f, math::color( 0, 0, 255, 255 ) );
//#endif //#ifndef MASTER_GOLD

	buffer_vector< float4 > const& planes	= bounding_volume.planes( );
	float3 volume_to_closest_point			= closest_point - m_bounding_volume->get_matrix( ).c.xyz( );
	float4x4 const& sphere_matrix			= m_bounding_volume->get_matrix( );

	u32 count = planes.size( );
	for ( u32 i = 0; i < count; ++i )
	{
		float4  plane					= planes[i];
		sphere_matrix.transform			( plane.xyz( ) );
		float	plane_projection		= volume_to_closest_point | plane.xyz( );

//#ifndef MASTER_GOLD
//		float3 relative_to_plane_point	= sphere_center + plane.xyz( ) * plane_projection;
//		m_renderer->draw_sphere_solid	( *m_scene, relative_to_plane_point, 0.2f, math::color( 0, 0, 255, 255 ) );
//		m_renderer->draw_line			( *m_scene, closest_point, relative_to_plane_point, math::color( 0, 0, 255, 255 ) );
//#endif //#ifndef MASTER_GOLD

		if( plane_projection > testee.radius( ) + plane.w * bounding_volume.radius( ) )
			return;
	}

	m_result = true;
}


} // namespace collision
} // namespace xray
