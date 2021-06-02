////////////////////////////////////////////////////////////////////////////
//	Created 	: 20.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <xray/collision/api.h>
#include <xray/collision/bone_collision_data.h>
#include <xray/collision/shell_creation_utils.h>
#include <xray/collision/animated_object.h>
#include <xray/collision/geometry.h>
#include <xray/collision/geometry_instance.h>
#include <xray/memory_stack_allocator.h>
#include <xray/animation/skeleton.h>
#include <xray/collision/collision_object.h>
#include "aabb_object.h"
#include "triangle_mesh_buffer.h"
#include "triangle_mesh_resource.h"
#include "loose_oct_tree.h"
#include "composite_geometry.h"
#include "box_geometry_instance.h"
#include "composite_geometry_instance.h"
#include "sphere_geometry_instance.h"
#include "cylinder_geometry_instance.h"
#include "capsule_geometry_instance.h"
#include "terrain_geometry_instance.h"
#include "triangle_mesh_geometry_instance.h"
#include "containment_double_dispatcher.h"
#include "intersection_double_dispatcher.h"
#include "truncated_sphere_geometry_instance.h"

using xray::collision::triangle_mesh_buffer;
using xray::collision::triangle_mesh_resource;
using xray::collision::loose_oct_tree;
using xray::collision::object;
using xray::non_null;
using xray::math::float3;
using xray::math::float4x4;

namespace xray {
namespace collision {

non_null<geometry>::ptr new_triangle_mesh_geometry	(
		memory::base_allocator* allocator,
		float3 const* vertices,
		u32 vertex_count,
		u32 const* indices,
		u32 index_count,
		u32 const* triangle_data,
		u32 triangle_count
	)
{
	return XRAY_NEW_IMPL( allocator, triangle_mesh_buffer )(
			allocator,
			vertices,
			vertex_count,
			indices,
			index_count,
			triangle_data,
			triangle_count
		);
}

non_null<geometry>::ptr new_triangle_mesh_geometry	(
		memory::base_allocator* allocator,
		float3 const* vertices,
		u32 vertex_count,
		u32 const* indices,
		u32 index_count
	)
{
	return new_triangle_mesh_geometry( allocator, vertices, vertex_count, indices, index_count, 0, 0 );
}

non_null<geometry>::ptr new_triangle_mesh_geometry	(
		memory::base_allocator* allocator,
		resources::managed_resource_ptr const resource
	)
{
	return XRAY_NEW_IMPL( allocator, triangle_mesh_resource )( allocator, resource );
}

u32	get_sphere_geometry_instance_size	( )
{
	return						sizeof( sphere_geometry_instance );
}

non_null<geometry_instance>::ptr new_box_geometry_instance		(
												memory::base_allocator* allocator,
												float4x4 const& matrix
											)
{
	return XRAY_NEW_IMPL( allocator, box_geometry_instance ) ( matrix );
}

non_null<geometry_instance>::ptr new_sphere_geometry_instance		(
												memory::base_allocator* allocator,
												float4x4 const& matrix,
												float radius
											)
{
	return XRAY_NEW_IMPL( allocator, sphere_geometry_instance ) ( matrix, radius );
}

non_null<geometry_instance>::ptr new_cylinder_geometry_instance		(
												memory::base_allocator* allocator,
												float4x4 const& matrix,
												float radius, 
												float half_length
											)
{
	return XRAY_NEW_IMPL( allocator, cylinder_geometry_instance ) ( matrix, radius, half_length );
}

non_null<geometry_instance>::ptr new_terrain_geometry_instance	( 
												memory::base_allocator* allocator,
												float4x4 const& matrix,
												float physical_size,
												u32 vertex_row_size,
												const float* heightfield,
												bool delete_by_collision_object
											)
{
	return XRAY_NEW_IMPL( allocator, terrain_geometry_instance ) ( matrix, physical_size, vertex_row_size, heightfield, delete_by_collision_object );
}

u32	get_box_geometry_instance_size		( )
{
	return						sizeof( box_geometry_instance );
}

u32	get_cylinder_geometry_instance_size	( )
{
	return						sizeof( cylinder_geometry_instance );
}

non_null<geometry_instance>::ptr new_capsule_geometry_instance	(
		memory::base_allocator* allocator,
		float4x4 const& matrix,
		float radius,
		float half_length
	)
{
	return XRAY_NEW_IMPL( allocator, capsule_geometry_instance )( matrix, radius, half_length );
}

u32	get_capsule_geometry_instance_size	( )
{
	return						sizeof( capsule_geometry_instance );
}

non_null<geometry>::ptr	new_composite_geometry		(
		memory::base_allocator* allocator,
		buffer_vector<geometry_instance*> const& instances
	)
{
	return XRAY_NEW_IMPL( allocator, composite_geometry )( allocator, instances );
}

non_null<geometry_instance>::ptr	new_composite_geometry_instance		(
		memory::base_allocator* allocator,
		float4x4 const& matrix,
		non_null<geometry const>::ptr comp_geom
	)
{
	composite_geometry const* geom		= static_cast_checked<composite_geometry const*>( &*comp_geom );
	return XRAY_NEW_IMPL( allocator, composite_geometry_instance )( allocator, matrix, geom );
}

non_null<geometry_instance>::ptr		new_triangle_mesh_geometry_instance	(
												memory::base_allocator* allocator,
												float4x4 const& matrix,
												non_null<geometry const>::ptr geometry
											)
{
	triangle_mesh_geometry const* mesh_geom		= static_cast_checked<triangle_mesh_geometry const*>( &*geometry );
	return XRAY_NEW_IMPL( allocator, triangle_mesh_geometry_instance )( matrix, mesh_geom );
}

u32 get_composite_geometry_size	( )
{
	return						sizeof( composite_geometry );
}

non_null<geometry_instance>::ptr new_truncated_sphere_geometry_instance	( memory::base_allocator* allocator, float4x4 const& matrix, float radius, float4* planes, u32 planes_count )
{
	return XRAY_NEW_IMPL( allocator, truncated_sphere_geometry_instance )( matrix, radius, planes, planes_count );
}

u32 get_truncated_sphere_geometry_instance_size	( )
{
	return						sizeof( truncated_sphere_geometry_instance );
}

void delete_geometry		( memory::base_allocator* allocator, geometry* geometry )
{
	if ( !allocator || !geometry )
		return;

	geometry->destroy		( allocator );
	XRAY_DELETE_IMPL		( allocator, geometry );
}

non_null<space_partitioning_tree>::ptr new_space_partitioning_tree	(
		memory::base_allocator* allocator,
		float min_aabb_radius,
		u32 reserve_node_count
	)
{
	return XRAY_NEW_IMPL( allocator, loose_oct_tree )( allocator, min_aabb_radius, reserve_node_count );
}

void delete_space_partitioning_tree	( space_partitioning_tree* tree )
{
	if ( !tree )
		return;

	loose_oct_tree* temp = static_cast_checked<loose_oct_tree*>( tree );
	XRAY_DELETE_IMPL( &temp->get_allocator(), temp );
}

non_null<object>::ptr new_collision_object	(
										memory::base_allocator* allocator,
										object_type object_type,
										non_null<geometry_instance>::ptr geometry,
										pvoid user_data
									)
{
	return XRAY_NEW_IMPL( allocator, collision_object ) ( allocator, object_type, &*geometry, user_data );
}

non_null<object>::ptr	new_aabb_object		(
		memory::base_allocator* allocator,
		object_type object_type,
		float3 const& position,
		math::float3 const& epsilon,
		pvoid user_data
	)
{
	return XRAY_NEW_IMPL( allocator, aabb_object ) ( object_type, create_aabb_center_radius(position, epsilon), user_data );
}

void delete_object			( memory::base_allocator* allocator, object* object )
{
	if ( !object )
		return;

	XRAY_DELETE_IMPL		( allocator, object );
}

u32	get_geometry_instance_size	( )
{
	return						sizeof( geometry_instance );
}

void delete_geometry_instance	( memory::base_allocator* allocator, geometry_instance* object )
{
	if ( !object )
		return;

	object->destroy				( allocator );

	XRAY_DELETE_IMPL			( allocator, object );
}

animated_object* new_animated_object(
		configs::binary_config_value const& config,
		animation::skeleton_ptr const& model_skeleton,
		memory::base_allocator* allocator )
{
	u32 total_object_size			= calculate_composite_geometry_size_from_physics_shell_config( config ) + sizeof( animated_object );			
	pcstr description				= "collision::animated_object memory";
	pvoid arena						= XRAY_MALLOC_IMPL( allocator, total_object_size, description );
	
	memory::stack_allocator			stack_allocator;
	stack_allocator.initialize		( arena, total_object_size, description );
	
	u32 const bones_count			= get_bones_count_from_physics_shell_config( config );
	animated_object* new_object		= XRAY_NEW_IMPL( stack_allocator, animated_object )( config, model_skeleton, bones_count, &stack_allocator );
	R_ASSERT						( stack_allocator.allocated_size() == total_object_size );
	return							new_object;
}

void delete_animated_object	( animated_object* object_to_be_deleted, memory::base_allocator* allocator )
{
	memory::stack_allocator					stack_allocator;
	object_to_be_deleted->destroy			( &stack_allocator );
	object_to_be_deleted->~animated_object	( );
	XRAY_FREE_IMPL							( allocator, object_to_be_deleted );
}

#ifdef MASTER_GOLD

bool contains	(	geometry_instance const&	bounding_volume,
					geometry_instance const&	testee
				)
{
	containment_double_dispatcher dispatcher	( bounding_volume, testee );
	return dispatcher.check_contains			( );
}

#else

bool contains	(	geometry_instance const&	bounding_volume,
					geometry_instance const&	testee,
					render::debug::renderer*	renderer,
					render::scene_ptr const*	scene
				)
{
	containment_double_dispatcher dispatcher	( bounding_volume, testee, renderer, scene );
	return dispatcher.check_contains			( );
}

#endif //#ifndef MASTER_GOLD

#ifdef MASTER_GOLD

bool intersects	(	geometry_instance const&	bounding_volume,
					geometry_instance const&	testee
				)
{
	intersection_double_dispatcher dispatcher	( &bounding_volume, &testee );
	return dispatcher.check_intersects			( );
}

#else

bool intersects	(	geometry_instance const&	bounding_volume,
					geometry_instance const&	testee,
					render::debug::renderer*	renderer,
					render::scene_ptr const*	scene
				)
{
	intersection_double_dispatcher dispatcher	( &bounding_volume, &testee, renderer, scene );
	return dispatcher.check_intersects			( );
}

#endif //#ifndef MASTER_GOLD

} // namespace collision
} // namespace xray
