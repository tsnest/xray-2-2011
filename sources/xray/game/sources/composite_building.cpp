////////////////////////////////////////////////////////////////////////////
//	Created		: 27.12.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "composite_building.h"
#include <xray/collision/api.h>
#include "collision_object_types.h"
#include <xray/collision/geometry.h>
#include <xray/collision/geometry_instance.h>
#include <xray/collision/space_partitioning_tree.h>
#include <xray/render/facade/debug_renderer.h>
#include <xray/render/facade/game_renderer.h>
#include <xray/math_randoms_generator.h>
#include "ai_collision_object.h"

namespace stalker2 {

composite_building::composite_building(
		pcstr name,
		u32 id,
		math::color debug_draw_color,
		collision::space_partitioning_tree& spatial_tree,
		render::scene_ptr const& scene,
		render::debug::renderer& renderer,
		float cylinder_radius,
		float half_length,
		float sphere_radius,
		math::float3 const& position
	) :
	m_name							( name ),
	m_id							( id ),
	m_debug_draw_color				( debug_draw_color ), 
	m_spatial_tree					( spatial_tree ),
	m_scene							( scene ),
	m_renderer						( renderer ),
	m_position						( position ),
	m_visibility_parameters			( 0.75f ),
	m_geometry_cylinder				( &*collision::new_cylinder_geometry_instance	( g_allocator, float4x4().identity(), cylinder_radius, half_length ) ),
	m_geometry_sphere				( &*collision::new_sphere_geometry_instance		( g_allocator, float4x4().identity(), sphere_radius ) )
{
	float4x4 const transform		= math::create_translation				( float3( 0.f, half_length, 0.f ) );
	float4x4 const global_transform	= math::create_translation				( position );

	m_geometry_cylinder->set_matrix	( global_transform );
	m_geometry_sphere->set_matrix	( transform );

	buffer_vector< collision::geometry_instance* > geometry_instances		( ALLOCA( 2 * sizeof( collision::geometry_instance* ) ), 2 );
	geometry_instances.push_back	( m_geometry_cylinder );
	geometry_instances.push_back	( m_geometry_sphere );
	m_geometry_composite			= &*collision::new_composite_geometry	( g_allocator, geometry_instances );
	
	m_collision_object				= &*ai_collision_object::new_ai_geometry_object(
										g_allocator,
										this,
										collision_object_type_ai,
										transform,
										m_geometry_composite,
										&m_visibility_parameters
									);
	
 	m_spatial_tree.insert			( m_collision_object, global_transform );
}

composite_building::~composite_building	( )
{
	m_spatial_tree.erase				( m_collision_object );
	
	R_ASSERT							( m_collision_object );
	ai_collision_object::delete_ai_collision_object	( g_allocator, m_collision_object );
	m_collision_object					= 0;
	
	R_ASSERT							( m_geometry_composite );
	collision::delete_geometry			( g_allocator, m_geometry_composite );
	m_geometry_composite					= 0;

	R_ASSERT							( m_geometry_sphere );
	collision::delete_geometry_instance	( g_allocator, m_geometry_sphere );
	m_geometry_sphere					= 0;

	R_ASSERT							( m_geometry_cylinder );
	collision::delete_geometry_instance	( g_allocator, m_geometry_cylinder );
	m_geometry_cylinder					= 0;
}

math::aabb composite_building::get_aabb	( ) const
{	
	return								m_geometry_composite->get_aabb( );
}

float4x4 composite_building::local_to_cell( float3 const& requester ) const
{
	XRAY_UNREFERENCED_PARAMETER			( requester );	
	return								create_translation( m_position );
}

float3 composite_building::get_random_surface_point( u32 const current_time ) const
{
	math::random32						randomizer( current_time );
	return								m_geometry_cylinder->get_random_surface_point( randomizer );
}

ai::collision_object* composite_building::get_collision_object( ) const
{
	return m_collision_object;
}

void composite_building::draw	( ) const
{
	m_renderer.draw_cylinder_solid	(
		m_scene,
		local_to_cell( float3( 0, 0, 0 ) ),
		m_geometry_cylinder->get_aabb().extents(),
		m_debug_draw_color
	);
	
	m_renderer.draw_sphere_solid	(
		m_scene,
		float3( m_collision_object->get_aabb().center().x, m_geometry_sphere->get_aabb().center().y, m_collision_object->get_aabb().center().z ),
		m_geometry_sphere->get_aabb().extents().x,
		m_debug_draw_color
	);
}

} // namespace stalker2
