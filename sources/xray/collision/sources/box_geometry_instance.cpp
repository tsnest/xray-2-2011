////////////////////////////////////////////////////////////////////////////
//	Created		: 15.09.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "box_geometry_instance.h"
#include <xray/collision/primitives.h>
#include <xray/collision/contact_info.h>
#include <xray/render/facade/debug_renderer.h>
#include <xray/math_randoms_generator.h>
#include "colliders_ray_aabb.h"

namespace xray {
namespace collision {

enum box_plane_types_enum
{
	box_plane_type_front_xy,
	box_plane_type_back_xy,
	box_plane_type_left_zy,
	box_plane_type_right_zy,
	box_plane_type_top_xz,
	box_plane_type_bottom_xz
};

box_geometry_instance::box_geometry_instance	( float4x4 const& matrix ) :
	m_matrix			( matrix ),
	m_inverted_matrix	( math::invert4x3( matrix ) )
{
}

box_geometry_instance::~box_geometry_instance	( )
{
}

bool box_geometry_instance::aabb_query			( object const* object, math::aabb const& aabb, triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETERS	(object, &aabb, &triangles);
	NOT_IMPLEMENTED		( return false ); 
}

bool box_geometry_instance::cuboid_query		( object const* object, math::cuboid const& cuboid, triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETERS	(object, &cuboid, &triangles);
	NOT_IMPLEMENTED		( return false ); 
}

bool box_geometry_instance::ray_query			( object const* object, math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance, ray_triangles_type& triangles, triangles_predicate_type const& predicate ) const
{
	XRAY_UNREFERENCED_PARAMETER	( predicate );
	float3 const new_origin		= m_inverted_matrix.transform_position(origin);
	float3 const new_direction	= m_inverted_matrix.transform_direction(direction);
	float const new_max_distance = length(m_inverted_matrix.transform_position(origin + direction*max_distance) - new_origin);

	bool const ray_test_succeeded	= ray_test( new_origin, new_direction, new_max_distance, distance );
	if ( ray_test_succeeded )
		triangles.push_back		( ray_triangle_result( object, u32(-1), distance ) );

	if ( !ray_test_succeeded )
		return			false;

	return				true;
}

bool box_geometry_instance::aabb_test			( math::aabb const& aabb ) const
{
	return
		math::cuboid( aabb, m_inverted_matrix ).test_inexact	( 
			math::create_aabb_center_radius	(
				float3( 0.f, 0.f, 0.f ),
				float3( 1.f, 1.f, 1.f )
			)
		) != math::intersection_outside;
}

bool box_geometry_instance::cuboid_test			( math::cuboid const& cuboid ) const
{
	return
		math::cuboid( cuboid, m_inverted_matrix ).test_inexact	( 
			math::create_aabb_center_radius	(
				float3( 0.f, 0.f, 0.f ),
				float3( 1.f, 1.f, 1.f )
			)
		) != math::intersection_outside;
}

bool box_geometry_instance::ray_test			( math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance ) const
{
	float3 const new_origin		= m_inverted_matrix.transform_position(origin);
	float3 const new_direction	= m_inverted_matrix.transform_direction(direction);
	float const new_max_distance= length(m_inverted_matrix.transform_position(origin + direction*max_distance) - new_origin);

	colliders::ray_aabb_collider aabb_collider( new_origin, new_direction );//, new_max_distance );

	XRAY_ALIGN(16) colliders::sse::aabb_a16	aabb;
	colliders::sse::construct_aabb_a16( aabb, float3( 0.f, 0.f, 0.f ), float3( 1.f, 1.f, 1.f ) );

	if ( !aabb_collider.intersects_aabb_sse( aabb, distance ) )
		return false;

	distance					*= max_distance/new_max_distance;
	return						true;
}

math::aabb box_geometry_instance::get_aabb		( ) const
{
	return	math::aabb( box_geometry_instance::get_geometry_aabb() ).modify( m_matrix );
}

math::aabb box_geometry_instance::get_geometry_aabb	( ) const
{
	return	math::create_aabb_center_radius( float3( 0.f, 0.f, 0.f ), float3( 1.f, 1.f, 1.f ) );
}

bool box_geometry_instance::is_valid			( ) const
{
	return true;
}

void box_geometry_instance::render				( render::scene_ptr const& scene, render::debug::renderer& renderer ) const
{
	render		( scene, renderer, m_matrix );
}

void box_geometry_instance::render		( render::scene_ptr const& scene, render::debug::renderer& renderer, float4x4 const& transform ) const
{
	renderer.draw_cube	( scene, transform, float3( 1.f, 1.f, 1.f ), math::color( 255u, 255u, 255u, 255u ) );
}

void box_geometry_instance::enumerate_primitives( enumerate_primitives_callback& cb ) const
{
	cb.enumerate( float4x4().identity(), primitive( box( float3( 1.f, 1.f, 1.f ) ) ) );
}

void box_geometry_instance::enumerate_primitives( float4x4 const& transform, enumerate_primitives_callback& cb ) const
{
	cb.enumerate( get_matrix( ) * transform, primitive( box( float3( 1.f, 1.f, 1.f ) ) ) );
}

void box_geometry_instance::add_triangles		( triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETER		( triangles );
	UNREACHABLE_CODE();
}

math::float3 const* box_geometry_instance::vertices	( ) const
{
	NOT_IMPLEMENTED				( return 0 );
}

u32	box_geometry_instance::vertex_count				( ) const
{ 
	NOT_IMPLEMENTED				( return 0 );
}

u32 const* box_geometry_instance::indices			( ) const
{ 
	NOT_IMPLEMENTED				( return 0 );
}

u32 const* box_geometry_instance::indices			( u32 triangle_id ) const
{ 
	XRAY_UNREFERENCED_PARAMETER	( triangle_id );
	UNREACHABLE_CODE			( return 0 );
}

u32 box_geometry_instance::index_count				( ) const
{
	NOT_IMPLEMENTED				( return 0 );
}

float box_geometry_instance::get_surface_area		( ) const
{ 
	float3 const& half_sides	= m_matrix.get_scale();
	return 8 * ( half_sides.x * half_sides.y + half_sides.y * half_sides.z + half_sides.x * half_sides.z );
}

static void random_point_inside_axis_aligned_rectangle	(
		math::random32& randomizer,
		float2 const& center,
		float2 const& half_size,
		float const& x_part,
		float& result_x,
		float& result_y
	)
{
	float2 result(
		x_part * half_size.x * 2.f,
		randomizer.random_f( 2.f * half_size.y )
	);

	result		+= center - half_size;

	result_x	= result.x;
	result_y	= result.y;
}

float3 box_geometry_instance::get_random_surface_point( math::random32& randomizer ) const
{
	float3 const half_sides				= float3( 1.f, 1.f, 1.f );
	float const area1					= 4 * half_sides.x * half_sides.y;
	float const area2					= 4 * half_sides.z * half_sides.y;
	float const area3					= 4 * half_sides.x * half_sides.z;
	float const total_area				= 2 * ( area1 + area2 + area3 ); 
	
	typedef fixed_vector< float, 6 >	box_planes_type;
	box_planes_type						box_planes;

	box_planes.push_back				( area1 );
	box_planes.push_back				( box_planes[0] + area1 );
	box_planes.push_back				( box_planes[1] + area2 );
	box_planes.push_back				( box_planes[2] + area2 );
	box_planes.push_back				( box_planes[3] + area3 );
	box_planes.push_back				( box_planes[4] + area3 );

	float random_area					= randomizer.random_f( total_area );
	box_planes_type::iterator iter		= std::lower_bound( box_planes.begin(), box_planes.end(), random_area, std::less<float>() );
	R_ASSERT							( iter != box_planes.end() );

	typedef std::pair< float3, float3 >	box_plane_coords;
	box_plane_coords const coords[]		=
	{
 		std::make_pair( float3( -half_sides.x,  half_sides.y, -half_sides.z ), float3(  half_sides.x, -half_sides.y, -half_sides.z ) ),
 		std::make_pair( float3( -half_sides.x,  half_sides.y,  half_sides.z ), float3(  half_sides.x, -half_sides.y,  half_sides.z ) ),
 		std::make_pair( float3( -half_sides.x,  half_sides.y,  half_sides.z ), float3( -half_sides.x, -half_sides.y, -half_sides.z ) ), 
 		std::make_pair( float3(  half_sides.x,  half_sides.y,  half_sides.z ), float3(  half_sides.x, -half_sides.y, -half_sides.z ) ),
 		std::make_pair( float3( -half_sides.x,  half_sides.y,  half_sides.z ), float3(  half_sides.x,  half_sides.y, -half_sides.z ) ),
 		std::make_pair( float3( -half_sides.x, -half_sides.y,  half_sides.z ), float3(  half_sides.x, -half_sides.y, -half_sides.z ) )
	};
		
	float const previous_area			= iter == box_planes.begin() ? 0 : (*(iter - 1));
	float const delta_area				= ( random_area - previous_area ) / ( *iter - previous_area );
	R_ASSERT							( delta_area >= 0 );

	box_plane_types_enum const plane_type = box_plane_types_enum( iter - box_planes.begin() );
	box_plane_coords const& plane_coords = coords[plane_type];
	
	float3								result;
	switch ( plane_type )
	{
		case box_plane_type_front_xy:
		case box_plane_type_back_xy:
		{
			result.z					= plane_coords.first.z;
			random_point_inside_axis_aligned_rectangle(
				randomizer,
				float2( 0.f, 0.f ),
				float2( math::abs( plane_coords.first.x ), math::abs( plane_coords.first.y ) ),
				delta_area,
				result.x,
				result.y
			);
		}
		break;

		case box_plane_type_left_zy:
		case box_plane_type_right_zy:
		{
			result.x					= plane_coords.first.x;
			random_point_inside_axis_aligned_rectangle(
				randomizer,
				float2( 0.f, 0.f ),
				float2( math::abs( plane_coords.first.z ), math::abs( plane_coords.first.y ) ),
				delta_area,
				result.z,
				result.y
			);
		}
		break;

		case box_plane_type_top_xz:
		case box_plane_type_bottom_xz:
		{
			result.y					= plane_coords.first.y;
			random_point_inside_axis_aligned_rectangle(
				randomizer,
				float2( 0.f, 0.f ),
				float2( math::abs( plane_coords.first.x ), math::abs( plane_coords.first.z ) ),
				delta_area,
				result.x,
				result.z
			);
		}
		break;
	}
	
	return								result;
}

float3 box_geometry_instance::get_closest_point_to	( float3 const& source, float4x4 const& origin ) const
{
	float4x4 transform			= origin * m_matrix;
	float3 dir					= source - transform.c.xyz( );
	float3 result				= transform.c.xyz( );
	float3 half_sides			= transform.get_scale( );

	for (int i = 0; i < 3; i++) 
	{
		float3 axis				= transform.lines[i].xyz( );
		axis.normalize			( );
		float dist				= dir.dot_product( axis );
		
		if (dist > half_sides[i]) dist	= half_sides[i];
		if (dist < -half_sides[i]) dist	= -half_sides[i];

		result								+= dist * axis;
	}

	return result;
}

} // namespace collision
} // namespace xray