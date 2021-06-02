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

static inline float3	nearest_cylinder_edge_center(
							float3 const&	test_cylinder_center,
							float3 const&	test_cylinder_up_axis,
							float3 const&	test_cylinder_alternative_axis,
							float const		test_cylinder_radius,
							float3 const&	test_cylinder_relative_position,
							float3 const&	to_point_vector
							
						)
{
	float3			to_extremums_axis;
	float3			cross_product			= test_cylinder_up_axis ^ to_point_vector;

	if( math::is_zero( cross_product.x + cross_product.y + cross_product.z ) )
	{
		to_extremums_axis = test_cylinder_alternative_axis;
	}
	else
	{
		to_extremums_axis = test_cylinder_up_axis ^ cross_product;
		to_extremums_axis.normalize( );
	}
	
	float3 cylinder_edge_center;

	if( ( test_cylinder_relative_position | to_point_vector ) > 0 )
		cylinder_edge_center			= test_cylinder_center - to_extremums_axis * test_cylinder_radius;
	else
		cylinder_edge_center			= test_cylinder_center + to_extremums_axis * test_cylinder_radius;

	return cylinder_edge_center;
}

static inline bool	is_inside_range(
						float3 const&	test_cylinder_relative_position,
						float3 const&	test_cylinder_center,
						float3 const&	test_cylinder_center_to_cap_vector,
						float3 const&	test_cylinder_up_axis,
						float3 const&	test_cylinder_alternative_axis,
						float const		test_cylinder_radius,
						float3 const&	range_axis,
						float const		range_half_size,
						float3 const&	range_center
#ifndef MASTER_GOLD
						,
						render::debug::renderer &	renderer,
						render::scene_ptr const&	scene,
						math::color					debug_color
#endif
					)
{
	float3			to_extremums_axis;
	float3			cross_product			= test_cylinder_up_axis ^ range_axis;

	if( math::is_zero( cross_product.x + cross_product.y + cross_product.z ) )
	{
		to_extremums_axis = test_cylinder_alternative_axis;
	}
	else
	{
		to_extremums_axis = test_cylinder_up_axis ^ cross_product;
		to_extremums_axis.normalize( );
	}
	
	float3 cylinder_edge_center;

	if( ( test_cylinder_relative_position | range_axis ) > 0 )
		cylinder_edge_center			= test_cylinder_center - to_extremums_axis * test_cylinder_radius;
	else
		cylinder_edge_center			= test_cylinder_center + to_extremums_axis * test_cylinder_radius;

	float3	vertex;
	float	projection_on_axe;

	vertex				= cylinder_edge_center + test_cylinder_center_to_cap_vector;
	projection_on_axe	= ( vertex - range_center ) | range_axis;
	if( math::abs( projection_on_axe ) > range_half_size )
		return					false;

#ifndef MASTER_GOLD
	renderer.draw_sphere_solid	( scene, vertex, 0.2f, debug_color );
#endif

	vertex				= cylinder_edge_center - test_cylinder_center_to_cap_vector;
	projection_on_axe	= ( vertex - range_center ) | range_axis;
	if( math::abs( projection_on_axe ) > range_half_size )
		return					false;

#ifndef MASTER_GOLD
	renderer.draw_sphere_solid	( scene, vertex, 0.2f, debug_color );
#endif

	return true;
}

void containment_double_dispatcher::dispatch	( box_geometry_instance const& bounding_volume,				cylinder_geometry_instance const& testee )
{
	float3 const	test_cylinder_relative_position = m_testee.get_matrix( ).c.xyz( ) - m_bounding_volume.get_matrix( ).c.xyz( );
	float3 const	center_to_cup_vector			= m_testee.get_matrix( ).j.xyz( ) * testee.half_length( );
	
	float3 const& bounding_volume_half_sides	= bounding_volume.get_matrix().get_scale();
	//x axis test
	if( !is_inside_range	(	test_cylinder_relative_position,
							m_testee.get_matrix( ).c.xyz( ),
							center_to_cup_vector,
							m_testee.get_matrix( ).j.xyz( ),
							m_testee.get_matrix( ).i.xyz( ),
							testee.radius( ),
							m_bounding_volume.get_matrix( ).i.xyz( ),
							bounding_volume_half_sides.x,
							m_bounding_volume.get_matrix( ).c.xyz( )
#ifndef MASTER_GOLD
							,
							*m_renderer,
							*m_scene,
							math::color( 200, 0, 0, 255 ) 
#endif //#ifndef MASTER_GOLD
						) )
		return;

	//z axis test
	if( !is_inside_range	(	test_cylinder_relative_position,
							m_testee.get_matrix( ).c.xyz( ),
							center_to_cup_vector,
							m_testee.get_matrix( ).j.xyz( ),
							m_testee.get_matrix( ).i.xyz( ),
							testee.radius( ),
							m_bounding_volume.get_matrix( ).k.xyz( ),
							bounding_volume_half_sides.z,
							m_bounding_volume.get_matrix( ).c.xyz( )
#ifndef MASTER_GOLD
							,
							*m_renderer,
							*m_scene,
							math::color( 0, 0, 200, 255 ) 
#endif //#ifndef MASTER_GOLD
						) )
		return;

	//y axis test
	if( !is_inside_range	(	test_cylinder_relative_position,
							m_testee.get_matrix( ).c.xyz( ),
							center_to_cup_vector,
							m_testee.get_matrix( ).j.xyz( ),
							m_testee.get_matrix( ).i.xyz( ),
							testee.radius( ),
							m_bounding_volume.get_matrix( ).j.xyz( ),
							bounding_volume_half_sides.y,
							m_bounding_volume.get_matrix( ).c.xyz( )
#ifndef MASTER_GOLD
							,
							*m_renderer,
							*m_scene,
							math::color( 0, 128, 0, 255 ) 
#endif //#ifndef MASTER_GOLD
						) )
		return;

	m_result = true;
}

void containment_double_dispatcher::dispatch	( cylinder_geometry_instance const& bounding_volume,			cylinder_geometry_instance const& testee )
{
	float3 const&	testee_center					= m_testee.get_matrix( ).c.xyz( );
	float3 const&	volume_center					= m_bounding_volume.get_matrix( ).c.xyz( );
	float3			center_to_cup_vector			= m_testee.get_matrix( ).j.xyz( ) * testee.half_length( );
	
	float3			testee_edge_center;
	float3			vertex;
	float			projection_on_axe;
	float3			projected_vertex;
	float3			cross_product;

	//vertical test
	if ( !is_inside_range(	testee_center - volume_center,
							testee_center,
							center_to_cup_vector,
							m_testee.get_matrix( ).j.xyz( ),
							m_testee.get_matrix( ).i.xyz( ),
							testee.radius( ),
							m_bounding_volume.get_matrix( ).j.xyz( ),
							bounding_volume.half_length( ),
							volume_center
#ifndef MASTER_GOLD
							,
							*m_renderer,
							*m_scene,
							math::color( 0, 128, 0, 255 )  
#endif //#ifndef MASTER_GOLD
						) )
		return;
	
	//radius test
	float3 const	projected_test_cylinder_center	= volume_center + ( ( testee_center - volume_center ) | m_bounding_volume.get_matrix( ).j.xyz( ) ) * m_bounding_volume.get_matrix( ).j.xyz( );
	float3			cylinder_center_to_center		= projected_test_cylinder_center - testee_center;
					cross_product					= m_testee.get_matrix( ).j.xyz( ) ^ cylinder_center_to_center;
	float3			cylinder_x_axe;
	
	if( math::is_zero( cross_product.x + cross_product.y + cross_product.z ) )
	{
		cylinder_x_axe = m_testee.get_matrix( ).j.xyz( ) ^ m_bounding_volume.get_matrix( ).j.xyz( );
	}
	else
	{
		cylinder_x_axe = m_testee.get_matrix( ).j.xyz( ) ^ cross_product;
		cylinder_x_axe.normalize( );
	}

	testee_edge_center			= testee_center + cylinder_x_axe * testee.radius( );
		
	vertex				= testee_edge_center + center_to_cup_vector;
	projection_on_axe	= ( vertex - volume_center ) | m_bounding_volume.get_matrix( ).j.xyz( );
	projected_vertex	= volume_center + projection_on_axe * m_bounding_volume.get_matrix( ).j.xyz( );
	
	if ( ( vertex - projected_vertex ).squared_length( ) > math::sqr( bounding_volume.radius( ) ) )
		return;

#ifndef MASTER_GOLD
		m_renderer->draw_sphere_solid	( *m_scene, vertex, 0.2f, math::color( 0, 0, 255, 255 ) );
#endif //#ifndef MASTER_GOLD

	vertex				= testee_edge_center - center_to_cup_vector;
	projection_on_axe	= ( vertex - volume_center ) | m_bounding_volume.get_matrix( ).j.xyz( );
	projected_vertex	= volume_center + projection_on_axe * m_bounding_volume.get_matrix( ).j.xyz( );

	if ( ( vertex - projected_vertex ).squared_length( ) > math::sqr( bounding_volume.radius( ) ) )
		return;

#ifndef MASTER_GOLD
		m_renderer->draw_sphere_solid	( *m_scene, vertex, 0.2f, math::color( 0, 0, 255, 255 ) );
#endif //#ifndef MASTER_GOLD

	m_result = true;
}

void containment_double_dispatcher::dispatch	( sphere_geometry_instance const& bounding_volume,			cylinder_geometry_instance const& testee )
{
	float3 const&	test_cylinder_center		= m_testee.get_matrix( ).c.xyz( );
	float3 const&	cylinder_center_to_center	= m_bounding_volume.get_matrix( ).c.xyz( ) - m_testee.get_matrix( ).c.xyz( ); 
	float3 const	cross_product				= m_testee.get_matrix( ).j.xyz( ) ^ cylinder_center_to_center;
	float3			cylinder_x_axe;
	
	if( cross_product.x + cross_product.y + cross_product.z == 0 )
	{
		cylinder_x_axe = m_testee.get_matrix( ).i.xyz( );
	}
	else
	{
		cylinder_x_axe = m_testee.get_matrix( ).j.xyz( ) ^ cross_product;
		cylinder_x_axe.normalize( );
	}

	float3 far_cylinder_edge_center		= test_cylinder_center + cylinder_x_axe * testee.radius( );
	float3 center_to_cup_vector			= m_testee.get_matrix( ).j.xyz( ) * testee.half_length( );
	float3 vertex;

	vertex = far_cylinder_edge_center + center_to_cup_vector;
	if ( ( vertex - m_bounding_volume.get_matrix( ).c.xyz( ) ).squared_length( ) > math::sqr( bounding_volume.radius( ) ) )
		return;

#ifndef MASTER_GOLD
		m_renderer->draw_sphere_solid	( *m_scene, vertex, 0.2f, math::color( 0, 0, 255, 255 ) );
#endif //#ifndef MASTER_GOLD

	vertex = far_cylinder_edge_center - center_to_cup_vector;
	if ( ( vertex - m_bounding_volume.get_matrix( ).c.xyz( ) ).squared_length( ) > math::sqr( bounding_volume.radius( ) ) )
		return;

#ifndef MASTER_GOLD
		m_renderer->draw_sphere_solid	( *m_scene, vertex, 0.2f, math::color( 0, 0, 255, 255 ) );
#endif //#ifndef MASTER_GOLD

	m_result = true;
}

void containment_double_dispatcher::dispatch	( capsule_geometry_instance const& bounding_volume,			cylinder_geometry_instance const& testee )
{
	XRAY_UNREFERENCED_PARAMETERS( bounding_volume, testee );
}

void containment_double_dispatcher::dispatch	( truncated_sphere_geometry_instance const& bounding_volume,	cylinder_geometry_instance const& testee )
{
	float3 const&	testee_center					= m_testee.get_matrix( ).c.xyz( );
	float3 const&	testee_center_to_volume_center	= m_bounding_volume.get_matrix( ).c.xyz( ) - testee_center; 
	float3 const	cross_product					= m_testee.get_matrix( ).j.xyz( ) ^ testee_center_to_volume_center;
	float3			testee_x_axe;
	
	if( cross_product.x + cross_product.y + cross_product.z == 0 )
	{
		testee_x_axe = m_testee.get_matrix( ).i.xyz( );
	}
	else
	{
		testee_x_axe = m_testee.get_matrix( ).j.xyz( ) ^ cross_product;
		testee_x_axe.normalize( );
	}

	float3 far_cylinder_edge_center		= testee_center + testee_x_axe * testee.radius( );
	float3 center_to_cup_vector			= m_testee.get_matrix( ).j.xyz( ) * testee.half_length( );
	float3 vertex;

	vertex = far_cylinder_edge_center + center_to_cup_vector;
	if ( ( vertex - m_bounding_volume.get_matrix( ).c.xyz( ) ).squared_length( ) > math::sqr( bounding_volume.radius( ) ) )
		return;

#ifndef MASTER_GOLD
		m_renderer->draw_sphere_solid	( *m_scene, vertex, 0.2f, math::color( 0, 0, 255, 255 ) );
#endif //#ifndef MASTER_GOLD

	vertex = far_cylinder_edge_center - center_to_cup_vector;
	if ( ( vertex - m_bounding_volume.get_matrix( ).c.xyz( ) ).squared_length( ) > math::sqr( bounding_volume.radius( ) ) )
		return;

#ifndef MASTER_GOLD
		m_renderer->draw_sphere_solid	( *m_scene, vertex, 0.2f, math::color( 0, 0, 255, 255 ) );
#endif //#ifndef MASTER_GOLD

	buffer_vector< float4 > const& planes	= bounding_volume.planes( );
	float4x4 const& sphere_matrix			= m_bounding_volume.get_matrix( );
	u32 count = planes.size( );
	for ( u32 i = 0; i < count; ++i )
	{
		float4  plane					= planes[i];
		sphere_matrix.transform			( plane.xyz( ) );
		float3 plane_position			= m_bounding_volume.get_matrix( ).c.xyz( ) + plane.xyz( ) * plane.w * bounding_volume.radius( );
		float3 nearest_edge_center		= nearest_cylinder_edge_center(
											testee_center,
											m_testee.get_matrix( ).j.xyz( ),
											m_testee.get_matrix( ).i.xyz( ),
											testee.radius( ),
											testee_center - plane_position,
											plane.xyz( )
										);

		vertex							= nearest_edge_center + center_to_cup_vector;
		float	plane_projection		= ( testee_center - plane_position ) | plane.xyz( );
		if( plane_projection > -testee.radius( ) )
			return;

#ifndef MASTER_GOLD
		m_renderer->draw_sphere_solid	( *m_scene, vertex, 0.2f, math::color( 0, 0, 255, 255 ) );
#endif //#ifndef MASTER_GOLD

		vertex							= nearest_edge_center - center_to_cup_vector;
		plane_projection				= ( testee_center - plane_position ) | plane.xyz( );
		if( plane_projection > -testee.radius( ) )
			return;

#ifndef MASTER_GOLD
		m_renderer->draw_sphere_solid	( *m_scene, vertex, 0.2f, math::color( 0, 0, 255, 255 ) );
#endif //#ifndef MASTER_GOLD
	}

	m_result = true;
}

} // namespace collision
} // namespace xray
