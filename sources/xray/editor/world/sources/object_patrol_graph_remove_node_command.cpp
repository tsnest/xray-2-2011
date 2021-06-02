////////////////////////////////////////////////////////////////////////////
//	Created		: 22.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_patrol_graph_remove_node_command.h"
#include "object_patrol_graph.h"
#include "object_patrol_graph_node.h"
#include "object_patrol_graph_edge.h"
#include "object_patrol_graph_look_point.h"

#include "tool_base.h"
#include "level_editor.h"
#include "object_inspector_tab.h"

namespace xray{
namespace editor{

object_patrol_graph_remove_node_command::object_patrol_graph_remove_node_command ( object_patrol_graph^ patrol_graph )
{
	R_ASSERT( dot_net::is<node^>( patrol_graph->selected_graph_part ) );

	m_patrol_graph		= patrol_graph;
	node^ rem_node		= safe_cast<node^>( m_patrol_graph->selected_graph_part );
	m_node_index		= rem_node->index;
	m_node_position		= rem_node->position;
	
	u32 count			= rem_node->outgoing_edges->Count;
	m_outgoing_edges	= gcnew array<edge_data>( count );

	for ( u32 i = 0; i < count; ++i )
		m_outgoing_edges[i] = edge_data( rem_node->outgoing_edges[i]->destination_node, rem_node->outgoing_edges[i] );

	count				= rem_node->incoming_edges->Count;
	m_incoming_edges	= gcnew array<edge_data>( count );
	m_incoming_edges_indices = gcnew array<u32>( count );

	for ( u32 i = 0; i < count; ++i )
	{
		m_incoming_edges[i]			= edge_data( rem_node->incoming_edges[i]->source_node, rem_node->incoming_edges[i] );
		m_incoming_edges_indices[i]	= rem_node->incoming_edges[i]->index;
	}

	count				= rem_node->look_points->Count;
	m_look_points		= gcnew array<look_point^>( count );

	for ( u32 i = 0; i < count; ++i )
		m_look_points[i]			= ( rem_node->look_points[i] );
}

object_patrol_graph_remove_node_command::~object_patrol_graph_remove_node_command ( )
{
	if( m_is_commited )
		delete m_deleted_node;
}

bool object_patrol_graph_remove_node_command::commit ( )
{
	m_deleted_node				= m_patrol_graph->get_node_at( m_node_index );
	m_deleted_node->detach		( );
	
	if( m_patrol_graph->selected_graph_part == m_deleted_node )
		m_patrol_graph->selected_graph_part = nullptr;

	m_patrol_graph->owner_tool( )->get_level_editor( )->get_object_inspector_tab( )->remove_object_from_selection( m_deleted_node );
	
	m_is_commited = true;
	return true;
}

void object_patrol_graph_remove_node_command::rollback ( )
{
	m_deleted_node->attach( m_node_index );

	u32 count		= m_outgoing_edges->Length;
	for ( u32 i = 0; i < count; ++i )
		m_outgoing_edges[i].edge->attach( m_deleted_node, m_outgoing_edges[i].node );

	count			= m_incoming_edges->Length;
	for ( u32 i = 0; i < count; ++i )
		m_incoming_edges[i].edge->attach( m_incoming_edges[i].node, m_incoming_edges_indices[i], m_deleted_node );

	count			= m_look_points->Length;
	for ( u32 i = 0; i < count; ++i )
		m_look_points[i]->attach( m_deleted_node );

	m_is_commited = false;
}

} // namespace editor
} // namespace xray