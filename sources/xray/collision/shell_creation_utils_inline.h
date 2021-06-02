////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "shell_creation_utils.h"
#include <xray/collision/api.h>
#include <xray/collision/bone_collision_data.h>
#include <xray/collision/geometry_instance.h>
#include <xray/memory_stack_allocator.h>

namespace xray {
namespace collision {

template < class ConfigValueType >
bool read_transform	( ConfigValueType const& table, float4x4& result_transform, float3& result_scale )
{
	float3 position			= table["position"];
	float3 rotation			= table["rotation"];

	result_scale			= table["scale"];
						
	float4x4 x_rotation		= math::create_rotation_x( rotation.x );
	float4x4 y_rotation		= math::create_rotation_y( rotation.y );
	float4x4 z_rotation		= math::create_rotation_z( rotation.z );

	float4x4 new_rotation	= x_rotation * y_rotation * z_rotation;
	float4x4 translation	= math::create_translation( position );

	result_transform		= new_rotation * translation;
	return					true;
}

template < class ConfigValueType >
static non_null< geometry_instance >::ptr new_sphere_geometry	(
		ConfigValueType const& config,
		float4x4 const& transform,
		float3 const& scale,
		memory::base_allocator* allocator
	)
{
	XRAY_UNREFERENCED_PARAMETER	( config );
	return						new_sphere_geometry_instance( allocator, transform, scale.x / 2.f );
}

template < class ConfigValueType >
static non_null< geometry_instance >::ptr new_box_geometry		(
		ConfigValueType	const& config,
		float4x4 const& transform,
		memory::base_allocator* allocator
	)
{
	XRAY_UNREFERENCED_PARAMETER	( config );
	return						new_box_geometry_instance( allocator, transform );
}

template < class ConfigValueType >
static non_null< geometry_instance >::ptr new_cylinder_geometry	(
		ConfigValueType	const& config,
		float4x4 const& transform,
		float3 const& scale,
		memory::base_allocator* allocator
	)
{
	XRAY_UNREFERENCED_PARAMETER	( config );
	return						new_cylinder_geometry_instance( allocator, transform, scale.x / 2.f, scale.y / 2.f );
}

template < class ConfigValueType >
static non_null< geometry_instance >::ptr new_geometry	(
		ConfigValueType const& config,
		float4x4 const& transform,
		float3 const& scale,
		memory::base_allocator* allocator
	)
{
	if ( strings::equal( config["type"], "sphere" ) )
		return				new_sphere_geometry( config, transform, scale, allocator );
	
	else if ( strings::equal( config["type"], "box" ) )
		return				new_box_geometry( config, create_scale(scale)*transform, allocator );
	
	else if ( strings::equal( config["type"], "cylinder" ) )
		return				new_cylinder_geometry( config, transform, scale, allocator );

	UNREACHABLE_CODE		( return new_sphere_geometry( config, transform, scale, allocator ) );
}

template < class ConfigValueType >
static u32 calculate_geometry_size	( ConfigValueType const& config )
{
	if ( strings::equal( config["type"], "sphere" ) )
		return				get_sphere_geometry_instance_size();
	
	else if ( strings::equal( config["type"], "box" ) )
		return				get_box_geometry_instance_size();
	
	else if ( strings::equal( config["type"], "cylinder" ) )
		return				get_cylinder_geometry_instance_size();

	UNREACHABLE_CODE		( return get_sphere_geometry_instance_size() );
}

template < class ConfigValueType >
static non_null< geometry_instance >::ptr new_collision_transformed	(
		ConfigValueType	const& config,
		float4x4 const& transform,
		float3 const& scale,
		memory::base_allocator* allocator
	)
{
	non_null< geometry_instance >::ptr collision_geometry	= new_geometry( config, transform, scale, allocator );
	return				collision_geometry;
}

template < class ConfigValueType >
static non_null< geometry_instance >::ptr new_collision	(
		ConfigValueType	const& config,
		memory::base_allocator* allocator
	)
{
	float4x4				transform; 
	float3					scale;
	read_transform			( config, transform, scale );
	return					new_collision_transformed( config, transform, scale, allocator );
}

template < class ConfigValueType >
static u32 calculate_collision_size	( ConfigValueType const& config )
{
	return							/*get_geometry_instance_size() + */calculate_geometry_size( config );
}

template < class ConfigValueType >
non_null< geometry_instance >::ptr new_element_collision	(
		ConfigValueType const& collision_table, 
		float4x4 const& transform, 
		memory::base_allocator* allocator
	)
{
	u32 const collisions_count			= collision_table.size();
	R_ASSERT							( collisions_count );

	size_t const buffer_size			= collisions_count * sizeof( geometry_instance* );

	buffer_vector< geometry_instance* >	instances( ALLOCA( buffer_size ), collisions_count );
	for ( u32 i = 0; i < collisions_count; ++i )
		instances.push_back				( &*new_collision( collision_table[i], allocator ) );
	
	non_null< geometry >::ptr new_geometry = new_composite_geometry( allocator, instances );
	
	non_null< geometry_instance >::ptr new_instance	= new_composite_geometry_instance( allocator, transform, new_geometry ); 
	return								new_instance;
}

template < class ConfigValueType >
static non_null< geometry_instance >::ptr new_element_collision	(
		ConfigValueType const& table,
		memory::base_allocator* allocator
	)
{
	float4x4 transform; 
	float3 scale;
	read_transform							( table, transform, scale );
	ConfigValueType const collision_table	= table["collision"];
	return									new_element_collision(
												collision_table,
												transform,
												allocator
											);
}

template < class ConfigValueType >
static u32 calculate_element_collision_size	( ConfigValueType const& config )
{
	ConfigValueType const collision_table	= config["collision"];
	u32 const collisions_count				= collision_table.size();

	u32 result								= get_composite_geometry_size() + collisions_count * sizeof( geometry_instance* ) + sizeof(composite_geometry_instance);// get_geometry_instance_size();

	for ( u32 i = 0; i < collisions_count; ++i )
		result								+= calculate_collision_size( collision_table[i] );
	
	return									result;
}

template < class ConfigValueType >
non_null< geometry >::ptr	new_composite_geometry_from_physics_shell_config	(
		ConfigValueType	const& config, 
		buffer_vector< bone_collision_data >& geometries_data,
		memory::base_allocator* allocator
	)
{
	ConfigValueType const bodies_table	= config["bodies"];
	u32 const bodies_count				= bodies_table.size();

	R_ASSERT							( bodies_count > 0, "Bad physics shell settings: no bodies!" );
	R_ASSERT							( allocator );

 	size_t const buffer_size			= bodies_count * sizeof( geometry_instance* );

	buffer_vector< geometry_instance* >	instances( ALLOCA( buffer_size ), bodies_count );
	for ( u32 bi = 0; bi < bodies_count; ++bi )
	{
		geometry_instance& instance		= *new_element_collision( bodies_table[bi], allocator );
		
		bone_collision_data				data( bodies_table[bi]["anim_joint"], &instance );
		geometries_data.push_back		( data );
		instances.push_back				( &instance );
	}
	
	non_null<geometry>::ptr new_geometry		= new_composite_geometry( allocator, instances );
	//non_null<geometry_instance>::ptr instance	= new_composite_geometry_instance( allocator, float4x4().identity(), new_geometry );
	return								new_geometry;
}

template < class ConfigValueType >
u32	calculate_composite_geometry_size_from_physics_shell_config	( ConfigValueType const& config )
{
	ConfigValueType const bodies_table	= config["bodies"];
	u32 const bodies_count				= bodies_table.size();
	
	u32 result							= get_composite_geometry_size() + bodies_count *
										( sizeof( geometry_instance* ) + sizeof( bone_collision_data ) );// + sizeof(composite_geometry);
	
	for ( u32 bi = 0; bi < bodies_count; ++bi )
		result							+= calculate_element_collision_size( bodies_table[bi] );

	return								result;
}

template < class ConfigValueType >
u32	get_bones_count_from_physics_shell_config	( ConfigValueType const& config )
{
	ConfigValueType const bodies_table			= config["bodies"];
	return										bodies_table.size();
}

} // namespace collision
} // namespace xray