////////////////////////////////////////////////////////////////////////////
//	Created		: 23.08.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_collision_geometry.h"

//debug
#include "game.h"
#include "game_world.h"
#include <xray/render/facade/debug_renderer.h>

namespace stalker2{

////////////////		O B B   T E S T S			////////////////

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

static inline bool is_inside_obb ( float3 const& vertex, float3 const& obb, float4x4 const& obb_transform )
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

bool object_collision_geometry::is_obb_inside_obb ( float4x4 const& obb_transform, float3 const& anti_mesh, float4x4 const& anti_mesh_transform )
{
	render::debug::renderer& renderer	= m_game_world.get_game( ).renderer( ).debug( );
	render::scene_ptr const& scene		= m_game_world.get_game( ).get_render_scene( );

	for ( u32 i=0; i < array_size( cuboid_vertices ); ++i )
	{
		float3 const& vertex	= obb_transform.transform_position( cuboid_vertices[i] );
		if( !is_inside_obb( vertex, anti_mesh, anti_mesh_transform ) )
			return				false;

		renderer.draw_sphere_solid	( scene, vertex, 0.2f, math::color( 0, 0, 255, 255 ) );
	}

	return						true;
}

static inline bool is_inside_cylinder ( float3 const& vertex, float const cylinder_half_height, float const squared_cylinder_radius, float4x4 const& cylinder_transform )
{
	float3 const& center_to_vertex	= vertex - cylinder_transform.c.xyz( );
	float const projection_on_axe	= center_to_vertex | cylinder_transform.j.xyz( );

	if ( math::abs( projection_on_axe ) > cylinder_half_height )
		return					false;

	float3 const& projection_to_vertex	= cylinder_transform.j.xyz( ) * projection_on_axe - center_to_vertex;
	return						projection_to_vertex.square_length( ) <= squared_cylinder_radius;
}

bool object_collision_geometry::is_obb_inside_cylinder ( float4x4 const& obb_transform, float const cylinder_half_height, float const cylinder_radius, float4x4 const& cylinder_transform )
{
	render::debug::renderer& renderer	= m_game_world.get_game( ).renderer( ).debug( );
	render::scene_ptr const& scene		= m_game_world.get_game( ).get_render_scene( );

	float const squared_cylinder_radius = math::sqr( cylinder_radius );

	for ( u32 i=0; i < array_size( cuboid_vertices ); ++i )
	{
		float3 const& vertex		= obb_transform.transform_position( cuboid_vertices[i] );
		if( !is_inside_cylinder( vertex, cylinder_half_height, squared_cylinder_radius, cylinder_transform ) )
			return				false;

		renderer.draw_sphere_solid	( scene, vertex, 0.2f, math::color( 0, 0, 255, 255 ) );
	}

	return						true;
}

static inline bool is_inside_sphere ( float3 const& vertex, float3 const& sphere_center, float const squared_sphere_radius )
{
	return  ( vertex - sphere_center ).square_length( ) <= squared_sphere_radius;
}

bool object_collision_geometry::is_obb_inside_sphere ( float4x4 const& obb_transform, float3 const& sphere_center, float const& sphere_radius )
{
	render::debug::renderer& renderer	= m_game_world.get_game( ).renderer( ).debug( );
	render::scene_ptr const& scene		= m_game_world.get_game( ).get_render_scene( );

	float const squared_sphere_radius = math::sqr( sphere_radius );

	for ( u32 i=0; i < array_size( cuboid_vertices ); ++i )
	{
		float3 const& vertex		= obb_transform.transform_position( cuboid_vertices[i] );
		if( !is_inside_sphere( vertex, sphere_center, squared_sphere_radius ) )
			return				false;

		renderer.draw_sphere_solid	( scene, vertex, 0.2f, math::color( 0, 0, 255, 255 ) );
	}

	return						true;
}

////////////////		S P H E R E   T E S T S			////////////////

bool object_collision_geometry::is_sphere_inside_obb ( float3 const& test_sphere_center, float const test_sphere_radius, float3 const& obb_half_sides, float4x4 const& obb_transform )
{
	float3 const& center_to_sphere_center	= test_sphere_center - obb_transform.c.xyz( );
	float const projection_on_x				= math::abs( center_to_sphere_center | obb_transform.i.xyz( ) );
	if ( projection_on_x + test_sphere_radius > obb_half_sides.x )
		return					false;

	float const projection_on_y				= math::abs( center_to_sphere_center | obb_transform.j.xyz( ) );
	if ( projection_on_y + test_sphere_radius > obb_half_sides.y )
		return					false;

	float const projection_on_z				= math::abs( center_to_sphere_center | obb_transform.k.xyz( ) );
	if ( projection_on_z + test_sphere_radius > obb_half_sides.z )
		return					false;

	return						true;
}

bool object_collision_geometry::is_sphere_inside_cylinder ( float3 const& test_sphere_center, float const test_sphere_radius, float const cylinder_half_height, float const cylinder_radius, float4x4 const& cylinder_transform )
{
	if( test_sphere_radius > cylinder_radius )
		return					false;

	float3 const& center_to_sphere_center	= test_sphere_center - cylinder_transform.c.xyz( );
	float const projection_on_axe			= center_to_sphere_center | cylinder_transform.j.xyz( );

	if ( math::abs( projection_on_axe ) + test_sphere_radius > cylinder_half_height )
		return					false;

	float3 const& projection_to_sphere_center	= cylinder_transform.j.xyz( ) * projection_on_axe - center_to_sphere_center;
	return							projection_to_sphere_center.square_length( ) <= math::sqr( cylinder_radius - test_sphere_radius );
}

bool object_collision_geometry::is_sphere_inside_sphere ( float3 const& test_sphere_center, float const test_sphere_radius, float3 const& sphere_center, float const& sphere_radius )
{
	if( test_sphere_radius > sphere_radius )
		return					false;

	return ( test_sphere_center - sphere_center ).square_length( ) <= math::sqr( sphere_radius - test_sphere_radius );
}

////////////////		C Y L I N D E R   T E S T S			////////////////

static inline bool is_inside_range(	float3 const&	test_cylinder_relative_position,
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

bool object_collision_geometry::is_cylinder_inside_obb ( float4x4 const& test_cylinder_transform, float const test_cylinder_radius, float const test_cylinder_half_height, float3 const& obb_half_sides, float4x4 const& obb_transform )
{
	render::debug::renderer& renderer	= m_game_world.get_game( ).renderer( ).debug( );
	render::scene_ptr const& scene		= m_game_world.get_game( ).get_render_scene( );

	float3 const	test_cylinder_relative_position = test_cylinder_transform.c.xyz( ) - obb_transform.c.xyz( );
	float3 const	center_to_cup_vector			= test_cylinder_transform.j.xyz( ) * test_cylinder_half_height;
	
	//x axis test
	if( !is_inside_range	(	test_cylinder_relative_position,
							test_cylinder_transform.c.xyz( ),
							center_to_cup_vector,
							test_cylinder_transform.j.xyz( ),
							test_cylinder_transform.i.xyz( ),
							test_cylinder_radius,
							obb_transform.i.xyz( ),
							obb_half_sides.x,
							obb_transform.c.xyz( ),
							renderer,
							scene,
							math::color( 200, 0, 0, 255 ) 
						) )
		return					false;

	//z axis test
	if( !is_inside_range	(	test_cylinder_relative_position,
							test_cylinder_transform.c.xyz( ),
							center_to_cup_vector,
							test_cylinder_transform.j.xyz( ),
							test_cylinder_transform.i.xyz( ),
							test_cylinder_radius,
							obb_transform.k.xyz( ),
							obb_half_sides.z,
							obb_transform.c.xyz( ),
							renderer,
							scene,
							math::color( 0, 0, 200, 255 ) 
						) )
		return					false;

	//y axis test
	if( !is_inside_range	(	test_cylinder_relative_position,
							test_cylinder_transform.c.xyz( ),
							center_to_cup_vector,
							test_cylinder_transform.j.xyz( ),
							test_cylinder_transform.i.xyz( ),
							test_cylinder_radius,
							obb_transform.j.xyz( ),
							obb_half_sides.y,
							obb_transform.c.xyz( ),
							renderer,
							scene,
							math::color( 0, 128, 0, 255 ) 
						) )
		return					false;

	return						true;
}

bool object_collision_geometry::is_cylinder_inside_cylinder ( float4x4 const& test_cylinder_transform, float const test_cylinder_radius, float const test_cylinder_half_height, float const cylinder_half_height, float const cylinder_radius, float4x4 const& cylinder_transform )
{
	render::debug::renderer& renderer = m_game_world.get_game( ).renderer( ).debug( );
	render::scene_ptr const& scene = m_game_world.get_game( ).get_render_scene( );

	XRAY_UNREFERENCED_PARAMETERS( renderer, scene );

	float3 const&	test_cylinder_center			= test_cylinder_transform.c.xyz( );
	float3 const&	cylinder_center					= cylinder_transform.c.xyz( );
	float3			center_to_cup_vector			= test_cylinder_transform.j.xyz( ) * test_cylinder_half_height;
	
	float3			cylinder_edge_center;
	float3			vertex;
	float			projection_on_axe;
	float3			projected_vertex;
	float3			cross_product;

	//vertical test
	if ( !is_inside_range(	test_cylinder_transform.c.xyz( ) - cylinder_transform.c.xyz( ),
							test_cylinder_transform.c.xyz( ),
							center_to_cup_vector,
							test_cylinder_transform.j.xyz( ),
							test_cylinder_transform.i.xyz( ),
							test_cylinder_radius,
							cylinder_transform.j.xyz( ),
							cylinder_half_height,
							cylinder_transform.c.xyz( ),
							renderer,
							scene,
							math::color( 0, 128, 0, 255 )  
						) )
		return					false;
	
	//horizontal test
	float3 const	projected_test_cylinder_center	= cylinder_center + ( ( test_cylinder_center - cylinder_center ) | cylinder_transform.j.xyz( ) ) * cylinder_transform.j.xyz( );
	float3			cylinder_center_to_center		= projected_test_cylinder_center - test_cylinder_center;
					cross_product					= test_cylinder_transform.j.xyz( ) ^ cylinder_center_to_center;
	float3			cylinder_x_axe;
	
	if( math::is_zero( cross_product.x + cross_product.y + cross_product.z ) )
	{
		cylinder_x_axe = test_cylinder_transform.j.xyz( ) ^ cylinder_transform.j.xyz( );
	}
	else
	{
		cylinder_x_axe = test_cylinder_transform.j.xyz( ) ^ cross_product;
		cylinder_x_axe.normalize( );
	}

	cylinder_edge_center			= test_cylinder_center + cylinder_x_axe * test_cylinder_radius;
		
	vertex				= cylinder_edge_center + center_to_cup_vector;
	projection_on_axe	= ( vertex - cylinder_center ) | cylinder_transform.j.xyz( );
	projected_vertex	= cylinder_center + projection_on_axe * cylinder_transform.j.xyz( );
	
	if ( ( vertex - projected_vertex ).square_length( ) > math::sqr( cylinder_radius ) )
		return					false;

	renderer.draw_sphere_solid	( scene, vertex, 0.2f, math::color( 255, 0, 100, 255 ) );

	vertex				= cylinder_edge_center - center_to_cup_vector;
	projection_on_axe	= ( vertex - cylinder_center ) | cylinder_transform.j.xyz( );
	projected_vertex	= cylinder_center + projection_on_axe * cylinder_transform.j.xyz( );

	if ( ( vertex - projected_vertex ).square_length( ) > math::sqr( cylinder_radius ) )
		return					false;

	renderer.draw_sphere_solid	( scene, vertex, 0.2f, math::color( 255, 0, 100, 255 ) );

	return						true;
}

bool object_collision_geometry::is_cylinder_inside_sphere ( float4x4 const& test_cylinder_transform, float const test_cylinder_radius, float const test_cylinder_half_height, float3 const& sphere_center, float const& sphere_radius )
{
	render::debug::renderer& renderer = m_game_world.get_game( ).renderer( ).debug( );
	render::scene_ptr const& scene = m_game_world.get_game( ).get_render_scene( );

	XRAY_UNREFERENCED_PARAMETERS( renderer, scene );

	float3 const&	test_cylinder_center		= test_cylinder_transform.c.xyz( );
	float3 const&	cylinder_center_to_center	= sphere_center - test_cylinder_center; 
	float3 const	cross_product				= test_cylinder_transform.j.xyz( ) ^ cylinder_center_to_center;
	float3			cylinder_x_axe;
	
	if( cross_product.x + cross_product.y + cross_product.z == 0 )
	{
		cylinder_x_axe = test_cylinder_transform.i.xyz( );
	}
	else
	{
		cylinder_x_axe = test_cylinder_transform.j.xyz( ) ^ cross_product;
		cylinder_x_axe.normalize( );
	}

	float3 far_cylinder_edge_center		= test_cylinder_center + cylinder_x_axe * test_cylinder_radius;
	float3 center_to_cup_vector			= test_cylinder_transform.j.xyz( ) * test_cylinder_half_height;
	float3 vertex;

	vertex = far_cylinder_edge_center + center_to_cup_vector;
	if ( ( vertex - sphere_center ).square_length( ) > math::sqr( sphere_radius ) )
		return					false;

	renderer.draw_sphere_solid	( scene, vertex, 0.2f, math::color( 0, 255, 0, 255 ) );

	vertex = far_cylinder_edge_center - center_to_cup_vector;
	if ( ( vertex - sphere_center ).square_length( ) > math::sqr( sphere_radius ) )
		return					false;

	renderer.draw_sphere_solid	( scene, vertex, 0.2f, math::color( 0, 255, 0, 255 ) );

	return						true;
}


} // namespace stalker2