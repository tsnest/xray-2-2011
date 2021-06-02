////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "building_object.h"
#include <xray/collision/geometry_instance.h>
#include "collision_object_types.h"
#include <xray/collision/space_partitioning_tree.h>
#include <xray/render/facade/debug_renderer.h>
#include <xray/render/facade/game_renderer.h>
#include <xray/math_randoms_generator.h>

namespace stalker2 {

building_object::building_object(
		pcstr name,
		u32 id,
		math::color debug_draw_color,
		collision::space_partitioning_tree& spatial_tree,
		render::scene_ptr const& scene,
		render::debug::renderer& renderer,
		math::float3 const& half_sides,
		math::float3 const& position
	) :
	m_name						( name ),
	m_id						( id ),
	m_debug_draw_color			( debug_draw_color ), 
	m_spatial_tree				( spatial_tree ),
	m_scene						( scene ),
	m_renderer					( renderer ),
	m_position					( position ),
	m_visibility_parameters		( 0.5f ),
	m_geometry_box				( &*collision::new_box_geometry_instance( g_allocator, math::create_scale(half_sides) ) )
{
	m_collision_object			= &*ai_collision_object::new_ai_geometry_object(
									g_allocator,
									this,
									collision_object_type_ai,
									m_geometry_box,
									&m_visibility_parameters
								);
	
 	m_spatial_tree.insert		( m_collision_object, math::create_translation( position ) );
}

building_object::~building_object	( )
{
	m_spatial_tree.erase				( m_collision_object );
	
	R_ASSERT							( m_collision_object );
	ai_collision_object::delete_ai_collision_object( g_allocator, m_collision_object );
	m_collision_object					= 0;
	
	R_ASSERT							( m_geometry_box );
	collision::delete_geometry_instance	( g_allocator, m_geometry_box );
	m_geometry_box						= 0;
}

math::aabb building_object::get_aabb( ) const
{	
	return								m_geometry_box->get_aabb( );
}

float4x4 building_object::local_to_cell	( float3 const& requester ) const
{
	XRAY_UNREFERENCED_PARAMETER			( requester );	
	return								create_translation( m_position );
}

float3 building_object::get_random_surface_point( u32 const current_time ) const
{
	math::random32								randomizer( current_time );
	return										m_geometry_box->get_random_surface_point( randomizer );
}

ai::collision_object* building_object::get_collision_object	( ) const
{
	return m_collision_object;
}

void building_object::draw	( ) const
{
	m_renderer.draw_aabb( m_scene, m_collision_object->get_aabb().center(), get_aabb().extents(), m_debug_draw_color );
}

} // namespace stalker2
