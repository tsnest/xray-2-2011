////////////////////////////////////////////////////////////////////////////
//	Created		: 13.03.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "picker.h"

#include "editor_world.h"
#include "collision_object.h"
#include "window_ide.h"

#pragma unmanaged
#include <xray/collision/object.h>
#include <xray/collision/space_partitioning_tree.h>


namespace xray {
namespace editor {

picker::picker( editor_world* world ):
m_world				( world ),
m_chached_pick_xy	( 0, 0 ),
m_cached			( false )
{	
}

bool picker::ray_query( xray::collision::object_type const mask, collision_object const** object, bool check_priority, float3* position )
{
	xray::math::int2 pos = m_world->get_mouse_position();
	
	return ray_query( mask, pos, object, check_priority, position);
}

bool picker::ray_query( xray::collision::object_type const mask, xray::math::int2 screen_xy, collision_object const** object, const bool check_priority, float3* position )
{
	if( m_cached && 
		screen_xy == m_chached_pick_xy && 
		check_priority == m_cached_priority_flag &&
		m_cached_mask == mask	)
	{
		if( m_cached_result	)
		{
			*object = m_cached_obj;

			if( position != NULL )
				*position = m_cached_pos;

			return true;
		}

		*object = NULL;
		return false;
	}

	math::float3 origin;
	math::float3 direction;
	m_world->get_screen_ray( screen_xy, origin, direction );

	bool result = ray_query( mask, origin, direction, object, check_priority, &m_cached_pos );

	if( position != NULL)
		*position = m_cached_pos;

	m_cached			= true;	
	m_chached_pick_xy	= screen_xy;
	m_cached_priority_flag = check_priority;
	m_cached_obj		= *object;
	m_cached_result		= result;
	m_cached_mask		= mask;

	return				result;
}

bool picker::ray_query( xray::collision::object_type const mask, xray::math::float3 origin, xray::math::float3 direction, collision_object const** object, bool check_priority, float3* position )
{
	collision::ray_objects_type			collision_results( g_allocator );

	*object = NULL;
	bool result = false;

	if( !m_world->get_collision_tree() )
		return false;

	m_world->get_collision_tree()->ray_query(
		mask,
		origin, 
		direction, 
		10000.f,  
		collision_results, 
		collision::objects_predicate_type( )
	);

	float distance = 0;
	if( collision_results.size() > 0)
	{
		collision_object const* obj = static_cast_checked<collision_object const*> (collision_results[0].object);
		ASSERT( obj, "The obj must not be NULL.");
		distance = collision_results[0].distance;
		u32 max_priority = obj->get_priority();
		u32 const count = collision_results.size();

		if( check_priority )
			for( u32 i = 1; i< count; ++i )
			{
				collision_object const* tmp_obj = static_cast_checked<collision_object const*> (collision_results[i].object);
				ASSERT( tmp_obj, "The obj must not be NULL.");
				if( tmp_obj->get_priority() < max_priority )
				{
					obj = tmp_obj;
					max_priority = 	obj->get_priority();
					distance = collision_results[i].distance;
				}
			}

		*object = obj;
		result = true;
	}
	else
		result = false;
	
	if( position != NULL )
	{
		*position = origin + direction*distance; 
	}

	return result;
}

bool picker::ray_query( xray::collision::object_type const mask, xray::math::float3 origin, xray::math::float3 direction, collision::ray_objects_type& collision_results )
{
	if( !m_world->get_collision_tree() )
		return false;

	m_world->get_collision_tree()->ray_query(
		mask, //collision::query_mask(mask), 
		origin, 
		direction, 
		1000.f,  
		collision_results, 
		collision::objects_predicate_type( )
	);

	if( collision_results.size() > 0)
		return true;

	return false;
}

bool picker::frustum_query( xray::collision::object_type const mask, xray::math::int2 start_xy, xray::math::int2 end_xy, vector< collision_object const*> &objects )
{
	math::float3 origin;
	math::float3 direction;


	math::float3	near_left_top		= m_world->screen_to_3d_world( math::float2( (float)start_xy.x,	(float)-start_xy.y ),	1.f );
	math::float3	near_right_top		= m_world->screen_to_3d_world( math::float2( (float)end_xy.x,	(float)-start_xy.y ),	1.f );
	math::float3	near_right_bottom	= m_world->screen_to_3d_world( math::float2( (float)end_xy.x,	(float)-end_xy.y ),		1.f );
	math::float3	near_left_bottom	= m_world->screen_to_3d_world( math::float2( (float)start_xy.x,	(float)-end_xy.y ),		1.f );

	math::float3	far_left_top		= m_world->screen_to_3d_world( math::float2( (float)start_xy.x,	(float)-start_xy.y ),	500.f );
	math::float3	far_right_top		= m_world->screen_to_3d_world( math::float2( (float)end_xy.x,	(float)-start_xy.y ),	500.f );
	math::float3	far_right_bottom	= m_world->screen_to_3d_world( math::float2( (float)end_xy.x,	(float)-end_xy.y ),		500.f );
	math::float3	far_left_bottom		= m_world->screen_to_3d_world( math::float2( (float)start_xy.x,	(float)-end_xy.y ),		500.f );

	math::plane		m_planes[ 6 ];

	if ( math::is_zero( get_plane_normal( near_left_top, near_right_top, near_left_bottom ).magnitude() ) )
		return		false;
	
	// near
	m_planes[0]	= math::create_plane( near_left_top,		near_right_top,		near_left_bottom	);
	// far
	m_planes[1]	= math::create_plane( far_left_top,			far_left_bottom,	far_right_top		);
	// up
	m_planes[2]	= math::create_plane( near_left_top,		far_left_top,		far_right_top		);
	// bottom
	m_planes[3]	= math::create_plane( near_left_bottom,		far_right_bottom,	far_left_bottom		);
	// left
	m_planes[4]	= math::create_plane( near_left_bottom,		far_left_bottom,	far_left_top		);
	// right
	m_planes[5]	= math::create_plane( near_right_bottom,	far_right_top,		far_right_bottom	);

	editor::vector<math::float3> tris;


	tris.push_back( near_left_top );
	tris.push_back( near_right_top );
	tris.push_back( near_left_bottom );
					
	tris.push_back( far_left_top );		
	tris.push_back( far_left_bottom );
	tris.push_back( far_right_top );
					
	tris.push_back( near_left_top );
	tris.push_back( far_left_top );	
	tris.push_back( far_right_top );
					
	tris.push_back( near_left_bottom );
	tris.push_back( far_right_bottom );
	tris.push_back( far_left_bottom );
					
	tris.push_back( near_left_bottom );
	tris.push_back( far_left_bottom );
	tris.push_back( far_left_top );
					
	tris.push_back(	near_right_bottom );
	tris.push_back(	far_right_top );
	tris.push_back(	far_right_bottom );


	math::cuboid	cuboid( m_planes );

	collision::objects_type	collision_results( g_allocator );

	bool result = false;

	if( !m_world->get_collision_tree() )
		return false;

	m_world->get_collision_tree()->cuboid_query(
		mask, //collision::query_mask(mask), 
		cuboid,
		collision_results
	);

	objects.clear();

	if( collision_results.size() > 0)
	{
		collision_object const* obj = static_cast_checked<collision_object const*> (collision_results[0]);
		ASSERT( obj, "The obj must not be NULL.");
		objects.push_back( obj );

		u32 const count = collision_results.size();
		for( u32 i = 1; i< count; ++i )
		{
			collision_object const* tmp_obj = static_cast_checked<collision_object const*> (collision_results[i]);
			ASSERT( tmp_obj, "The obj must not be NULL.");
			objects.push_back( tmp_obj );
		}
		result = true;
	}
	else
		result = false;

	return result;
}

void picker::clear_cache()
{
	m_cached = false;
}


} // namespace editor
} // namespace xray

#pragma managed