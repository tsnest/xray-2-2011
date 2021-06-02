////////////////////////////////////////////////////////////////////////////
//	Created		: 16.08.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_collision_geometry.h"
#include "collision_object_types.h"
#include "game_world.h"
#include "game_camera.h"

#include <xray/console_command.h>
#include <xray/collision/api.h>
#include <xray/collision/space_partitioning_tree.h>
#include <xray/collision/geometry_instance.h>
#include <xray/collision/primitives.h>

#ifndef MASTER_GOLD

#include "game.h"
#include <xray/render/facade/debug_renderer.h>

#endif // #ifndef MASTER_GOLD

typedef collision::geometry_instance geometry_instance;
typedef non_null<collision::geometry>::ptr geometry_ptr;
typedef collision::geometry geometry;
typedef buffer_vector< collision::geometry_instance* >::iterator instances_iterator;
typedef buffer_vector< collision::geometry_instance* >::const_iterator instances_const_iterator;
typedef buffer_vector< collision::geometry_instance* >::iterator geometries_iterator;


namespace stalker2{

////////////////			I N I T I A L I Z E				////////////////

object_collision_geometry::object_collision_geometry ( game_world& w ):
	super						( w ),
	m_instances					( 0, 0 ),
	m_anti_instances			( 0, 0 ),
	m_geometries				( 0, 0 ),
	m_mode						( object_collision_geometry_mode_mesh_containment )
{ 
#ifndef MASTER_GOLD

	static xray::console_commands::cc_bool		is_draw_collisions_cc		( "collision_geometry_draw_debug",		m_is_draw_collisions,	true, xray::console_commands::command_type_user_specific );

#endif
}

object_collision_geometry::~object_collision_geometry ( )
{
	if( !m_geometries.empty( ) )
	{
		geometries_iterator	current	= m_geometries.begin( );
		geometries_iterator	end		= m_geometries.end( );

		for ( ; current != end; ++current )
			collision::delete_geometry_instance		( g_allocator, *current );
	}
	void* pointer	= (void*)m_geometries.begin( );
	if( pointer != NULL )
		XRAY_FREE_IMPL	( g_allocator, pointer );

	if( !m_instances.empty( ) )
	{
		instances_iterator current	= m_instances.begin( );
		instances_iterator end		= m_instances.end( );

		for ( ; current != end; ++current )
			collision::delete_geometry_instance	( g_allocator, (*current) );
	}
	pointer			= (void*)m_instances.begin( );
	if( pointer != NULL )
		XRAY_FREE_IMPL	( g_allocator, pointer );

	if( !m_anti_instances.empty( ) )
	{
		instances_iterator current	= m_anti_instances.begin( );
		instances_iterator end		= m_anti_instances.end( );

		for ( ; current != end; ++current )
			collision::delete_geometry_instance	( g_allocator, (*current) );
	}
	pointer			= (void*)m_anti_instances.begin( );
	if( pointer != NULL )
		XRAY_FREE_IMPL	( g_allocator, pointer );
}

////////////////		P U B L I C   M E T H O D S			////////////////

void object_collision_geometry::load ( configs::binary_config_value const& config_value )
{
	super::load							( config_value );

	if( !config_value.value_exists( "meshes" ) )
		return;

	m_mode = (object_collision_geometry_mode)(u32)config_value["mode"];

	u32 geometries_count;

	configs::binary_config_value meshes	= config_value["meshes"];

	//init geometries buffer vector
	{
		geometries_count	= meshes.size( );

		if( config_value.value_exists( "anti_meshes" ) )
			geometries_count += config_value["anti_meshes"].size( );

		pvoid buffer		= XRAY_ALLOC_IMPL( g_allocator, collision::geometry*, geometries_count );
		buffer_vector< collision::geometry_instance* > geometries( buffer, geometries_count );

		m_geometries.swap	( geometries );
	}

	u32 count			= meshes.size( );
	//init instances buffer vector
	{
		pvoid buffer		= XRAY_ALLOC_IMPL( g_allocator, collision::geometry_instance*, count );
		buffer_vector< collision::geometry_instance* > instances( buffer, count );

		m_instances.swap	( instances );
	}

	for ( u32 i = 0; i < count; ++i )
	{
		configs::binary_config_value mesh			= meshes[i];
		collision::primitive_type primitive_type	= (collision::primitive_type)( (int)mesh["type"] );
		float3 position		= mesh["position"];
		float3 rotation		= mesh["rotation"];
		float3 scale		= mesh["scale"];

		if( primitive_type == collision::primitive_sphere )
			scale			= float3( scale.x, scale.x, scale.x );

		collision::primitive primitive;
		primitive.type		= primitive_type;
		primitive.data_		= scale;
		
		float4x4 matrix		= create_rotation( rotation ) * create_translation( position ) * m_transform;

		collision::geometry_instance*	instance;
//		collision::geometry*			geometry;

		switch ( primitive_type )
		{
		case collision::primitive_sphere:
			instance	= &*collision::new_sphere_geometry_instance		( g_allocator, matrix, primitive.sphere( ).radius  );
			break;

		case collision::primitive_box:
			instance	= &*collision::new_box_geometry_instance		( g_allocator, math::create_scale(primitive.box( ).half_side) * matrix );
			break;

		case collision::primitive_cylinder:
			instance	= &*collision::new_cylinder_geometry_instance	( g_allocator, matrix, primitive.cylinder( ).radius, primitive.cylinder( ).half_length );
			break;

		case collision::primitive_capsule:
			instance	= &*collision::new_capsule_geometry_instance	( g_allocator, matrix, primitive.cylinder( ).radius, primitive.cylinder( ).half_length );
			break;
		case collision::primitive_truncated_sphere: 
			{
				configs::binary_config_value planes_value = mesh["planes"];
				u32 count	= planes_value.size( );

				float4* planes = XRAY_ALLOC_IMPL( g_allocator, float4, count );

				for ( u32 i = 0; i < count; ++i )
					planes[i] = planes_value[i];

				instance	= &*collision::new_truncated_sphere_geometry_instance	( g_allocator, matrix, primitive.truncated_sphere( ).radius, planes, count );
				break;
			}
		}

		m_instances.push_back	( instance );
//		m_geometries.push_back	( geometry );
	}

	if( !config_value.value_exists( "anti_meshes" ) )
		return;

	configs::binary_config_value anti_meshes	= config_value["anti_meshes"];

	count			= anti_meshes.size( );
	{
		pvoid buffer		= XRAY_ALLOC_IMPL( g_allocator, collision::geometry_instance*, count );
		buffer_vector< collision::geometry_instance* > anti_instances( buffer, count );

		m_anti_instances.swap	( anti_instances );
	}
	for ( u32 i = 0; i < count; ++i )
	{
		configs::binary_config_value mesh			= anti_meshes[i];
		collision::primitive_type primitive_type	= (collision::primitive_type)( (int)mesh["type"] );
		float3 position		= mesh["position"];
		float3 rotation		= mesh["rotation"];
		float3 scale		= mesh["scale"];

		if( primitive_type == collision::primitive_sphere )
			scale = float3( scale.x, scale.x, scale.x );

		collision::primitive primitive;
		primitive.type		= primitive_type;
		primitive.data_		= scale;

		float4x4 matrix		= create_rotation( rotation ) * create_translation( position ) * m_transform;

		collision::geometry_instance*	instance;

		switch ( primitive_type )
		{
			case collision::primitive_sphere:	instance = &*collision::new_sphere_geometry_instance	( g_allocator, matrix, primitive.sphere( ).radius ); break;
			case collision::primitive_box:		instance = &*collision::new_box_geometry_instance		( g_allocator, math::create_scale(primitive.box( ).half_side) * matrix ); break;
			case collision::primitive_cylinder:	instance = &*collision::new_cylinder_geometry_instance	( g_allocator, matrix, primitive.cylinder( ).radius, primitive.cylinder( ).half_length ); break;
			case collision::primitive_capsule:	instance = &*collision::new_capsule_geometry_instance	( g_allocator, matrix, primitive.capsule( ).radius, primitive.capsule( ).half_length ); break;
			case collision::primitive_truncated_sphere:
			{
				configs::binary_config_value planes_value = mesh["planes"];
				u32 count	= planes_value.size( );

				float4* planes = XRAY_ALLOC_IMPL( g_allocator, float4, count );

				for ( u32 i = 0; i < count; ++i )
					planes[i] = planes_value[i];

				instance	= &*collision::new_truncated_sphere_geometry_instance	 ( g_allocator, matrix, primitive.truncated_sphere( ).radius, planes, count );
				break;
			}
		}

		m_anti_instances.push_back	( instance );
//		m_geometries.push_back		( geometry );
	}

	if( m_mode == object_collision_geometry_mode_mesh_containment )
	{
		if( m_anti_instances.empty( ) )
			return;
	}
	else
	{
		if( m_instances.empty( ) )
			return;
	}
}

void object_collision_geometry::load_contents ( )
{

}

void object_collision_geometry::unload_contents ( )
{

}

bool object_collision_geometry::check_object_inside	( collision::geometry_instance const& testee )
{
	if( m_instances.empty( ) )
		return 0;

#ifndef MASTER_GOLD
		draw_collision( );
#endif //#ifndef MASTER_GOLD

	bool ret_value = false;

	if( m_mode == object_collision_geometry_mode_mesh_containment )
		ret_value = check_object_inside_containment_mode	( testee );
	else
		ret_value = check_object_inside_intersection_mode	( testee );

	return ret_value;
}

/////////////////////////////////////			P R I V A T E   M E T H O D S			/////////////////////////////////////

bool object_collision_geometry::check_object_inside_containment_mode	( collision::geometry_instance const & testee )
{
	bool ret_value = false;

	instances_const_iterator		current = m_instances.begin	( );
	instances_const_iterator		end		= m_instances.end	( );

	for ( ; current != end; ++current )
	{
#ifndef MASTER_GOLD
		render::scene_ptr			scene		= m_game_world.get_game( ).get_render_scene( );
		render::debug::renderer&	renderer	= m_game_world.get_game( ).renderer( ).debug( );

		if( xray::collision::contains( **current, testee, &renderer, &scene ) )
#else
		if( xray::collision::contains( **current, testee ) )
#endif //#ifndef MASTER_GOLD
		{
			ret_value = true;
			break;
		}
	}

	if( ret_value == false || m_anti_instances.empty( ) )
		return ret_value;

	current = m_anti_instances.begin	( );
	end		= m_anti_instances.end		( );

	for ( ; current != end; ++current )
	{
#ifndef MASTER_GOLD
		render::scene_ptr			scene		= m_game_world.get_game( ).get_render_scene( );
		render::debug::renderer&	renderer	= m_game_world.get_game( ).renderer( ).debug( );

		if( xray::collision::intersects( **current, testee, &renderer, &scene ) )
#else
		if( xray::collision::intersects( **current, testee ) )
#endif //#ifndef MASTER_GOLD
		{
			return false;
		}
	}

	return true;
}

bool object_collision_geometry::check_object_inside_intersection_mode	( collision::geometry_instance const & testee )
{
	bool ret_value = false;

	instances_const_iterator	current = m_instances.begin	( );
	instances_const_iterator	end		= m_instances.end	( );

	for ( ; current != end; ++current )
	{
#ifndef MASTER_GOLD
		render::scene_ptr			scene		= m_game_world.get_game( ).get_render_scene( );
		render::debug::renderer&	renderer	= m_game_world.get_game( ).renderer( ).debug( );

		if( xray::collision::intersects( **current, testee, &renderer, &scene ) )
#else
		if( xray::collision::intersects( **current, testee ) )
#endif //#ifndef MASTER_GOLD
		{
			ret_value = true;
			break;
		}
	}

	if( ret_value == false || m_anti_instances.empty( ) )
		return ret_value;
	
	current = m_anti_instances.begin	( );
	end		= m_anti_instances.end		( );

	for ( ; current != end; ++current )
	{
#ifndef MASTER_GOLD
		render::scene_ptr			scene		= m_game_world.get_game( ).get_render_scene( );
		render::debug::renderer&	renderer	= m_game_world.get_game( ).renderer( ).debug( );

		if( xray::collision::contains( **current, testee, &renderer, &scene ) )
#else
		if( xray::collision::contains( **current, testee ) )
#endif //#ifndef MASTER_GOLD
		{
			return false;
		}
	}

	return true;
}

void object_collision_geometry::set_transform ( float4x4 const& transform )	
{
	m_transform = transform;
	
	float4x4 delta = transform * invert4x3( m_transform );
	
	instances_const_iterator		current = m_instances.begin	( );
	instances_const_iterator		end		= m_instances.end	( );
	for ( ; current != end; ++current )
		(*current)->set_matrix( (*current)->get_matrix( ) * delta );
}

/////////////////////////////////////			D E B U G   M E T H O D S			/////////////////////////////////////

#ifndef MASTER_GOLD

void object_collision_geometry::draw_collision ( )
{
	render::scene_ptr			scene		= m_game_world.get_game( ).get_render_scene( );
	render::debug::renderer&	renderer	= m_game_world.get_game( ).renderer( ).debug( );

	if( !m_is_draw_collisions )
		return;

	instances_const_iterator	current		= m_instances.begin( );
	instances_const_iterator	end			= m_instances.end( );

	for ( ; current != end; ++current )
		(*current)->render( scene, renderer );

	current		= m_anti_instances.begin( );
	end			= m_anti_instances.end( );

	for ( ; current != end; ++current )
		(*current)->render( scene, renderer );
}
#endif //#ifndef MASTER_GOLD

} // namespace stalker2
