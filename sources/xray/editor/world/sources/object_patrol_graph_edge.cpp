////////////////////////////////////////////////////////////////////////////
//	Created		: 17.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_patrol_graph.h"
#include "object_patrol_graph_edge.h"
#include "object_patrol_graph_node.h"
#include "object_patrol_graph_part_collision.h"

#pragma managed ( push, off )
#	include <xray/collision/api.h>
#	include <xray/collision/space_partitioning_tree.h>
#pragma managed ( push, on )

namespace xray{
namespace editor{

object_patrol_graph_edge::object_patrol_graph_edge( object_patrol_graph^ parent ):
	super	( parent, *collision::new_capsule_geometry_instance( g_allocator, math::create_translation( position ), 0.5f, 0.5f ) )
{
	m_probability = -1;
}

object_patrol_graph_edge::object_patrol_graph_edge( node^ source_node, configs::lua_config_value const config_value ):
	super	( source_node->get_parent_graph( ), *collision::new_capsule_geometry_instance( g_allocator, math::create_translation( position ), 0.5f, 0.5f ) )
{
	this->source_node = source_node;

	load( config_value );

	m_source_node->add_outgoing_edge			( this );
	m_destination_node->add_incoming_edge		( this );

	attach_collision	( );
	fix_position		( );
}

object_patrol_graph_edge::~object_patrol_graph_edge	(  )
{
	if( m_source_node != nullptr )
		detach ( );
	
	m_source_node			= nullptr;
	m_destination_node		= nullptr;
}

//////////////////////////////			P R O P E R T I E S				////////////////////////////////////

Single object_patrol_graph_edge::probability::get( )
{
	return m_probability;
}

void object_patrol_graph_edge::probability::set( Single value )
{
	if( m_probability == 100 )
	{
		u32 count			= m_source_node->outgoing_edges->Count;

		if( count == 1 )
			return;

		float new_value		= ( 100.f - value ) / ( count - 1 );

		R_ASSERT( math::valid( new_value ) );
		for ( u32 i = 0; i < count; ++i )
		{
			edge^ edge			= m_source_node->outgoing_edges[i];
			edge->m_probability = new_value;
		}
	}
	else
	{
		float scalar		= ( 100.f - value ) / ( 100.f - m_probability );
		u32 count			= m_source_node->outgoing_edges->Count;

		R_ASSERT( math::valid( scalar ) );
		for ( u32 i = 0; i < count; ++i )
		{
			edge^ edge			= m_source_node->outgoing_edges[i];
			edge->m_probability *= scalar;

			float probability	= edge->m_probability;
			R_ASSERT( math::valid( probability ) );
		}
	}

	m_probability		= value;
}

Int32 object_patrol_graph_edge::index::get( )
{
	return m_source_node->outgoing_edges->IndexOf( this ); 
}

//////////////////////////////		 P U B L I C   M E T H O D S		////////////////////////////////////

void object_patrol_graph_edge::select ( )
{
	m_parent_graph->select_graph_part( this );
}

void object_patrol_graph_edge::load ( configs::lua_config_value const config_value )
{
	destination_node						= m_parent_graph->get_node_at( config_value["destination_node_index"] );
	m_probability							= config_value["probability"];
}

void object_patrol_graph_edge::save ( configs::lua_config_value config_value )
{
	config_value["destination_node_index"]	= destination_node->index;
	config_value["probability"]				= m_probability;
}

void object_patrol_graph_edge::attach ( node^ source, node^ destination )
{
	attach( source, source->outgoing_edges->Count, destination );
}

void object_patrol_graph_edge::attach ( node^ source, u32 index, node^ destination )
{
	attach_collision		( );
	m_source_node			= source;
	m_destination_node		= destination;

	if( m_probability == -1 )
		m_probability = ( source_node->outgoing_edges->Count == 0 ) ? 100 : 100.f / ( source_node->outgoing_edges->Count + 1 );

	float tmp_probability				= m_probability;
	m_probability						= 0;

	m_source_node->add_outgoing_edge			( this, index );
	m_destination_node->add_incoming_edge		( this );

	probability				= tmp_probability;
	fix_position			( );
}

void object_patrol_graph_edge::detach ( )
{
	detach_collision					( );

	float tmp_probability				= m_probability;
	probability							= 0;
	
	m_source_node->remove_outgoing_edge			( this );
	m_destination_node->remove_incoming_edge	( this );

	m_probability						= tmp_probability;

	m_source_node						= nullptr;
	m_destination_node					= nullptr;
}

///////////////////////////		   I N T E R N A L   M E T H O D S			////////////////////////////////

void object_patrol_graph_edge::fix_position ( )
{
	m_parent_graph->m_collision->m_collision_tree->move	(
		m_collision,
		fixed_position( )
	);
}

float4x4 object_patrol_graph_edge::fixed_position ( )
{
	float3 destination	= destination_node->position;
	float3 source		= source_node->position;
	float3 direction	= ( destination - source ).normalize( );

	
	return 	math::create_scale( float3( 1, ( destination - source ).length( ) -2.f, 1 ) ) *
			math::create_rotation_x( -math::pi_d2 ) *
			math::create_rotation( direction, ( direction ^ float3( 0, 1, 0 ) ) ^ direction ) *
			math::create_translation( ( destination + source ) / 2 );
}

float4x4 object_patrol_graph_edge::get_matrix ( )
{
	return fixed_position( );
}

} // namespace editor
} // namespace xray