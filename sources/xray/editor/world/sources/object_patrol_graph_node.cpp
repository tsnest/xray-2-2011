////////////////////////////////////////////////////////////////////////////
//	Created		: 17.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_patrol_graph.h"
#include "object_patrol_graph_part.h"
#include "object_patrol_graph_node.h"
#include "object_patrol_graph_edge.h"
#include "object_patrol_graph_look_point.h"
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

//////////////////////////////			I N I T I A L I Z E				////////////////////////////////////

object_patrol_graph_node::object_patrol_graph_node( object_patrol_graph^ graph, Float3 position ):
	super( graph, *collision::new_capsule_geometry_instance( g_allocator, math::create_translation( position ), 0.5f, 0.9f ) )
{
	m_position				= position;
	m_outgoing_edges		= gcnew List<edge^>( );
	m_incoming_edges		= gcnew List<edge^>( );
	m_look_points			= gcnew List<look_point^>( );
	m_readonly_out_edges	= gcnew read_only_edges( m_outgoing_edges );
	m_readonly_in_edges		= gcnew read_only_edges( m_incoming_edges );
	m_readonly_look_points	= gcnew read_only_look_points( m_look_points );
	signal					= String::Empty;
}

object_patrol_graph_node::object_patrol_graph_node( object_patrol_graph^ graph, configs::lua_config_value const config_value ):
	super( graph, *collision::new_capsule_geometry_instance( g_allocator, math::create_translation( config_value["position"] ), 0.5f, 0.9f ) )
{
	m_outgoing_edges		= gcnew List<edge^>( );
	m_incoming_edges		= gcnew List<edge^>( );
	m_look_points			= gcnew List<look_point^>( );
	m_readonly_out_edges	= gcnew read_only_edges( m_outgoing_edges );
	m_readonly_in_edges		= gcnew read_only_edges( m_incoming_edges );
	m_readonly_look_points	= gcnew read_only_look_points( m_look_points );
	signal					= String::Empty;

	attach_collision		( );
	load					( config_value );
}

object_patrol_graph_node::~object_patrol_graph_node	( )
{
	for( s32 i = m_outgoing_edges->Count - 1; i >= 0; --i )
		delete m_outgoing_edges[i];

	for( s32 i = m_incoming_edges->Count - 1; i >= 0; --i )
		delete m_incoming_edges[i];

	for( s32 i = m_look_points->Count - 1; i >= 0; --i )
		delete m_look_points[i];
}

//////////////////////////////			P R O P E R T I E S				////////////////////////////////////

u32 object_patrol_graph_node::index::get ( )
{
	return m_parent_graph->nodes->IndexOf( this );
}

Float3 object_patrol_graph_node::position::get ( )
{
	return m_position;
}

void object_patrol_graph_node::position::set ( Float3 value )
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

	for each( edge^ edge in m_outgoing_edges )
		edge->fix_position( );

	for each( edge^ edge in m_incoming_edges )
		edge->fix_position( );
}

//////////////////////////////		 P U B L I C   M E T H O D S		////////////////////////////////////

void object_patrol_graph_node::fix_up_position	( float3 delta_position )
{
	if( is_absolute_position )
		return;

	position = Float3( position + delta_position );
}

void object_patrol_graph_node::select ( )
{
	m_parent_graph->select_graph_part( this );
	m_parent_graph->owner_tool( )->get_level_editor( )->set_transform_object( gcnew object_patrol_graph_part_transform_data( this ) );
}

void object_patrol_graph_node::load ( configs::lua_config_value const config_value )
{
	position					= Float3( (float3) config_value["position"] );

	if( config_value.value_exists( "signal" ) )
		signal						= gcnew String ( (pcstr)config_value["signal"] );

	if( config_value.value_exists( "look_point_selection" ) )
		look_point_selection		= (look_point_selection_type)(u32)config_value["look_point_selection"];

	configs::lua_config_value look_points_value	= config_value["look_points"];
	s32 look_points_count = look_points_value.size( );
	for( s32 i = 0; i < look_points_count; ++i )
		gcnew look_point( this, look_points_value[i] );
}

void object_patrol_graph_node::save ( configs::lua_config_value config_value )
{
	config_value["position"]	= m_position.operator float3( );
	if ( signal != nullptr && signal->Length != 0 )
		config_value["signal"]		= unmanaged_string( signal ).c_str( );

	config_value["look_point_selection"]	= (u32)look_point_selection;

	configs::lua_config_value edges_value	= config_value["edges"];
	int edges_count = m_outgoing_edges->Count;
	for( int i = 0; i < edges_count; ++i )
		m_outgoing_edges[i]->save( edges_value[i] );

	configs::lua_config_value look_points_value	= config_value["look_points"];
	int look_points_count = m_look_points->Count;
	for( int i = 0; i < look_points_count; ++i )
		m_look_points[i]->save( look_points_value[i] );
}

void object_patrol_graph_node::attach ( )
{
	attach( m_parent_graph->nodes->Count );
}

void object_patrol_graph_node::attach ( u32 index )
{
	attach_collision( );
	m_parent_graph->insert_node	( index, this );
}

void object_patrol_graph_node::detach ( )
{
	detach_collision( );

	m_parent_graph->remove_node	( this );

	for( s32 i = m_outgoing_edges->Count - 1; i >= 0; --i )
		m_outgoing_edges[i]->detach( );

	for( s32 i = m_incoming_edges->Count - 1; i >= 0; --i )
		m_incoming_edges[i]->detach( );

	for( s32 i = m_look_points->Count - 1; i >= 0; --i )
		m_look_points[i]->detach( );
}

///////////////////////////		   I N T E R N A L   M E T H O D S			////////////////////////////////

void object_patrol_graph_node::load_edges ( configs::lua_config_value const config_value )
{
	configs::lua_config_value edges_value	= config_value["edges"];
	s32 edges_count = edges_value.size( );
	for( s32 i = 0; i < edges_count; ++i )
		gcnew edge( this, edges_value[i] );
}

void object_patrol_graph_node::add_outgoing_edge ( edge^ edge )
{
	m_outgoing_edges->Add		( edge );
}

void object_patrol_graph_node::add_outgoing_edge ( edge^ edge, u32 index )
{
	m_outgoing_edges->Insert	( index, edge );
}

void object_patrol_graph_node::remove_outgoing_edge ( edge^ edge )
{
	m_outgoing_edges->Remove	( edge );
}

void object_patrol_graph_node::add_incoming_edge ( edge^ edge )
{
	m_incoming_edges->Add		( edge );
}

void object_patrol_graph_node::remove_incoming_edge ( edge^ edge )
{
	m_incoming_edges->Remove	( edge );
}

void object_patrol_graph_node::add_look_point ( look_point^ look_point )
{
	m_look_points->Add			( look_point );
}

void object_patrol_graph_node::add_look_point ( u32 index, look_point^ look_point )
{
	m_look_points->Insert		( index, look_point );
}

void object_patrol_graph_node::remove_look_point ( look_point^ look_point )
{
	m_look_points->Remove		( look_point );
}

float4x4 object_patrol_graph_node::get_matrix ( )
{
	return math::create_translation( position );
}

//////////////////////////////		P R I V A T E   M E T H O D S		////////////////////////////////////

} // namespace editor
} // namespace xray
