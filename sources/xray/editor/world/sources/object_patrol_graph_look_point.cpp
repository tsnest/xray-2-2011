////////////////////////////////////////////////////////////////////////////
//	Created		: 01.12.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_patrol_graph_look_point.h"
#include "object_patrol_graph.h"
#include "object_patrol_graph_node.h"
#include "object_patrol_graph_part_transform_data.h"
#include "object_patrol_graph_part_collision.h"
#include <xray/editor/base/collision_object_types.h>

#include "tool_base.h"
#include "level_editor.h"

#pragma managed ( push, off )
#	include <xray/collision/api.h>
#	include <xray/collision/collision_object.h>
#	include <xray/collision/space_partitioning_tree.h>
#pragma managed ( push, on )

namespace xray{
namespace editor{

object_patrol_graph_look_point::object_patrol_graph_look_point( object_patrol_graph^ parent, Float3 position ):
	super	( parent, *collision::new_sphere_geometry_instance( g_allocator, math::create_translation( position ), 0.5f ) )
{
	m_position			= position;
	animation			= "";
	signal_on_begin		= "";
	signal_on_end		= "";
	m_probability		= -1;
}

object_patrol_graph_look_point::object_patrol_graph_look_point( node^ parent_node, configs::lua_config_value const config_value ):
	super	( parent_node->get_parent_graph( ), *collision::new_sphere_geometry_instance( g_allocator, math::create_translation( position ), 0.5f ) )
{
	m_parent_node		= parent_node;
	m_parent_node->add_look_point( this );
	animation			= "";
	signal_on_begin		= "";
	signal_on_end		= "";

	attach_collision	( );
	load				( config_value );
}

object_patrol_graph_look_point::~object_patrol_graph_look_point	(  )
{
	m_parent_node		= nullptr;
}

//////////////////////////////			P R O P E R T I E S				////////////////////////////////////

u32 object_patrol_graph_look_point::index::get ( )
{
	return m_parent_node->look_points->IndexOf( this ); 
}

Float3 object_patrol_graph_look_point::position::get ( )
{
	return m_position;
}

void object_patrol_graph_look_point::position::set ( Float3 value )
{
	collision::object const* object		= NULL;
	math::float3 position;

	float3 origin	= value;
	origin.y		+= 1;

	if( m_parent_graph->owner_tool( )->get_level_editor( )->view_window( )->ray_query( editor_base::collision_object_type_dynamic | editor_base::collision_object_type_terrain, origin, float3( 0, -1, 0 ), &object, &position ) )
	{
		R_ASSERT	( object->get_type( ) & ( editor_base::collision_object_type_dynamic | editor_base::collision_object_type_terrain ) );
		position.y	+= 0.9f;
	}
	else
		position	= value;
	
	m_position		= Float3( position );
	
	m_parent_graph->m_collision->m_collision_tree->move( m_collision, math::create_translation( position ) );
}

Single object_patrol_graph_look_point::probability::get( )
{
	return m_probability;
}

void object_patrol_graph_look_point::probability::set( Single value )
{
	if( m_probability == 100 )
	{
		u32 count			= m_parent_node->look_points->Count;

		if( count == 1 )
			return;

		float new_value		= ( 100.f - value ) / ( count - 1 );

		R_ASSERT( math::valid( new_value ) );
		for ( u32 i = 0; i < count; ++i )
		{
			look_point^ look_point		= m_parent_node->look_points[i];
			look_point->m_probability	= new_value;
		}
	}
	else
	{
		float scalar		= ( 100.f - value ) / ( 100.f - m_probability );
		u32 count			= m_parent_node->look_points->Count;

		R_ASSERT( math::valid( scalar ) );
		for ( u32 i = 0; i < count; ++i )
		{
			look_point^ look_point		= m_parent_node->look_points[i];
			look_point->m_probability	*= scalar;

			float probability	= look_point->m_probability;
			R_ASSERT( math::valid( probability ) );
		}
	}

	m_probability		= value;
}

//////////////////////////////		 P U B L I C   M E T H O D S		////////////////////////////////////

void object_patrol_graph_look_point::select ( )
{
	m_parent_graph->select_graph_part( this );
	m_parent_graph->owner_tool( )->get_level_editor( )->set_transform_object( gcnew object_patrol_graph_part_transform_data( this ) );
}

void object_patrol_graph_look_point::load ( configs::lua_config_value const config_value )
{
	duration								= config_value["duration"];
	if( config_value.value_exists("signal_on_begin") )
	{
		signal_on_begin							= gcnew String( (pcstr)config_value["signal_on_begin"] );
		signal_on_end							= gcnew String( (pcstr)config_value["signal_on_end"] );
	}
	else
		signal_on_begin							= gcnew String( (pcstr)config_value["signal"] );
	
	animation								= gcnew String( (pcstr)config_value["animation"] );
	m_probability							= config_value["probability"];
	position								= Float3( (float3)config_value["position"] );
}

void object_patrol_graph_look_point::save ( configs::lua_config_value config_value )
{
	config_value["duration"]				= duration;
	config_value["signal_on_begin"]			= unmanaged_string( signal_on_begin ).c_str( );
	config_value["signal_on_end"]			= unmanaged_string( signal_on_end ).c_str( );
	config_value["animation"]				= unmanaged_string( animation ).c_str( );
	config_value["probability"]				= m_probability;
	config_value["position"]				= m_position.operator float3( );
}

void object_patrol_graph_look_point::attach ( node^ parent )
{
	m_parent_node = parent;
	attach( m_parent_node->look_points->Count, parent );
}

void object_patrol_graph_look_point::attach ( u32 index, node^ parent )
{
	attach_collision					( );
	m_parent_node						= parent;

	if( m_probability == -1 )
		m_probability = ( m_parent_node->look_points->Count == 0 ) ? 100 : 100.f / ( m_parent_node->look_points->Count + 1 );

	float tmp_probability				= m_probability;
	m_probability						= 0;
	m_parent_node->add_look_point		( index, this );
	probability							= tmp_probability;
	position							= m_position;
}

void object_patrol_graph_look_point::detach	( )
{
	detach_collision					( );
	float tmp_probability				= m_probability;
	probability							= 0;
	m_parent_node->remove_look_point	( this );
	m_probability						= tmp_probability;
	m_parent_node						= nullptr;
}

///////////////////////////		   I N T E R N A L   M E T H O D S			////////////////////////////////

float4x4 object_patrol_graph_look_point::get_matrix ( )
{
	return math::create_translation( position );
}

} // namespace editor
} // namespace xray