////////////////////////////////////////////////////////////////////////////
//	Created		: 16.08.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_proximity_trigger.h"
#include "object_collision_geometry.h"
#include "game_world.h"

#include <xray/collision/api.h>
#include <xray/collision/geometry.h>
#include <xray/collision/geometry_instance.h>

typedef collision::geometry				geometry;
typedef collision::geometry_instance	geometry_instance;
typedef collision::primitive			primitive;

namespace stalker2{

////////////////			I N I T I A L I Z E				////////////////

object_proximity_trigger::object_proximity_trigger	( game_world& w )
	:super					( w ),
	m_inside_objects		( NULL, 0 ),
	m_collision_geometry	( NULL )
{

}
object_proximity_trigger::~object_proximity_trigger	( )
{
	void* pointer	= (void*)m_inside_objects.begin( );
	if( pointer != NULL )
		XRAY_FREE_IMPL	( g_allocator, pointer );

	m_collision_geometry = NULL;
}

////////////////		P U B L I C   M E T H O D S			////////////////

u32 object_proximity_trigger::get_inside_objects_count ( )
{
	return m_inside_objects.size( );
}

void object_proximity_trigger::load ( configs::binary_config_value const& config_value )
{
	super::load( config_value );

	pcstr	collision_geometry_name		= config_value["collision_geometry"];
	u32		max_tracked_objects_count	= config_value["max_tracked_objects_count"];

	//init inside objects buffer vector
	{
		pvoid buffer		= XRAY_ALLOC_IMPL( g_allocator, collision::geometry_instance*, max_tracked_objects_count );
		instances_vector inside_objects( buffer, max_tracked_objects_count );

		m_inside_objects.swap( inside_objects );
	}	

	m_game_world.query_object_by_name(
		collision_geometry_name,
		object_loaded_callback_type( this, &object_proximity_trigger::collision_object_loaded )
	);
}

void object_proximity_trigger::collision_object_loaded ( game_object_ptr_ const& object )
{
	m_collision_geometry		= static_cast_checked<object_collision_geometry*>( object.c_ptr( ) );
	R_ASSERT					( m_collision_geometry );
}

struct sort_predicate
{
	u32 operator( )( geometry_instance* left, geometry_instance* right )
	{
		return left > right;
	}
};

bool object_proximity_trigger::is_object_inside ( geometry_instance* testee )
{
	instances_vector::iterator begin	= m_inside_objects.begin( );
	instances_vector::iterator end		= m_inside_objects.end( );

	std::sort( begin, end, sort_predicate( ) );
	instances_vector::iterator iterator =	std::find( begin, end, testee );

	return iterator != end;
}

proximity_trigger_testee_status object_proximity_trigger::test_object ( geometry_instance* testee )
{
	proximity_trigger_testee_status	result = proximity_trigger_testee_status_none;

	instances_vector::iterator begin	= m_inside_objects.begin( );
	instances_vector::iterator end		= m_inside_objects.end( );

	std::sort( begin, end, sort_predicate( ) );

	bool is_object_iside_collision = m_collision_geometry->check_object_inside( *testee );

	if( is_object_iside_collision )
	{
		result = proximity_trigger_testee_status_inside;
		instances_vector::iterator iterator =	std::find( begin, end, testee );
		if( iterator == end )
		{
			m_inside_objects.push_back	( testee );
			//enter						( **iterator );
			result						= proximity_trigger_testee_status_enter;
		}
	}
	else
	{
		result = proximity_trigger_testee_status_outside;
		instances_vector::iterator iterator =	std::find( begin, end, testee );
		if( iterator != end )
		{
			m_inside_objects.erase		( iterator );
			//leave						( **iterator );
			result						= proximity_trigger_testee_status_leave;
		}
	}

	return result;
}

//void object_proximity_trigger::test_objects ( instances_vector testees, instances_vector entered, instances_vector gone, instances_vector inside  )
//{
//
//}

} // namespace stalker2
