////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/collision/collision_object.h>
#include <xray/linkage_helper.h>
#include <xray/render/facade/debug_renderer.h>

XRAY_DECLARE_LINKAGE_ID(collision_object)

namespace xray {
namespace collision {

collision_object::collision_object	(
		memory::base_allocator* allocator,
		collision::object_type type_object,
		geometry_instance* geometry,
		pvoid const user_data
	):
	m_geometry_instance			( geometry )
{
	XRAY_UNREFERENCED_PARAMETER	( allocator );
	m_user_data					= user_data;
	set_type					( type_object );
}

collision_object::~collision_object	( )
{
}

math::aabb collision_object::update_aabb	( float4x4 const& local_to_world )
{
	m_geometry_instance->set_matrix	( local_to_world );
	return						m_geometry_instance->get_aabb( );
}

void collision_object::destroy	( memory::base_allocator* allocator )
{
//	XRAY_UNREFERENCED_PARAMETER	( allocator );
	if ( m_geometry_instance->delete_by_collision_object	( ) )
		delete_geometry_instance	( allocator, m_geometry_instance );
}

void collision_object::render( render::scene_ptr const& scene, render::debug::renderer& renderer ) const
{
	m_geometry_instance->render( scene, renderer );
	renderer.draw_aabb( scene, m_aabb.center(), m_aabb.extents(), math::color(255,255,255,255) );
}

bool collision_object::aabb_query( math::aabb const& aabb, triangles_type& results )  const
{
	return m_geometry_instance->aabb_query( this, aabb, results );
}

bool collision_object::cuboid_query( math::cuboid const& cuboid, triangles_type& results )  const
{
	return m_geometry_instance->cuboid_query( this, cuboid, results );
}

bool collision_object::ray_query( math::float3 const& origin, 
										  math::float3 const& direction, 
										  float max_distance, 
										  float& distance,
										  ray_triangles_type& results, 
										  triangles_predicate_type const& predicate ) const
{
	return m_geometry_instance->ray_query( this, origin, direction, max_distance, distance, results, predicate );
}

bool collision_object::aabb_test( math::aabb const& aabb ) const
{
	return m_geometry_instance->aabb_test( aabb );
}

bool collision_object::cuboid_test( math::cuboid const& cuboid ) const
{
	return m_geometry_instance->cuboid_test( cuboid );
}

bool collision_object::ray_test( math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance ) const
{
	return m_geometry_instance->ray_test( origin, direction, max_distance, distance );
}

void collision_object::add_triangles( collision::triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETER	( triangles );
	UNREACHABLE_CODE();
}

void collision_object::set_matrix	( math::float4x4 const& matrix )
{
	m_geometry_instance->set_matrix	( matrix );
}

math::float4x4 const& collision_object::get_matrix( ) const
{
	return m_geometry_instance->get_matrix();
}

math::aabb collision_object::get_aabb		( ) const
{
	return m_geometry_instance->get_aabb( );
}

math::float3 collision_object::get_center	( ) const
{
	return get_aabb().center();
}

math::float3 collision_object::get_extents	( ) const
{
	return get_aabb().extents();
}

void collision_object::dbg_render_nearest_point( render::scene_ptr const& scene, render::debug::renderer& r, float3 const& point ) const
{
	r.draw_cube_solid		( scene, math::create_translation( m_geometry_instance->get_closest_point_to( point ) ), float3( 0.1f, 0.1f, 0.1f ), math::color( 255, 0, 0 ), false );
}

} // namespace collision
} // namespace xray