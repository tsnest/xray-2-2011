////////////////////////////////////////////////////////////////////////////
//	Created		: 28.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_patrol_graph_change_edge_direction_command.h"

#include "object_patrol_graph.h"
#include "object_patrol_graph_edge.h"
#include "object_patrol_graph_node.h"

namespace xray{
namespace editor{

object_patrol_graph_change_edge_direction_command::object_patrol_graph_change_edge_direction_command( object_patrol_graph^ patrol_graph, edge^ edge )
{
	m_patrol_graph			= patrol_graph;

	R_ASSERT( edge != nullptr );

	m_edge_index		= edge->index;
	m_node_index		= edge->source_node->index;
	m_node_index_second	= edge->destination_node->index;

	R_ASSERT( m_node_index >= 0 );
	R_ASSERT( m_edge_index >= 0 );
}

bool object_patrol_graph_change_edge_direction_command::commit ( )
{
	edge^ edge = m_patrol_graph->get_node_at( m_node_index )->outgoing_edges[m_edge_index];
	
	float temp_probability	= edge->probability;
	edge->probability				= 0;

	node^ source_node		= edge->source_node;
	node^ destination_node	= edge->destination_node;

	source_node->remove_outgoing_edge		( edge );
	destination_node->remove_incoming_edge	( edge );

	source_node->add_incoming_edge			( edge );
	destination_node->add_outgoing_edge		( edge );

	edge->source_node		= destination_node;
	edge->destination_node	= source_node;

	edge->probability				= temp_probability;
	m_edge_index_second				= edge->index;

	return true;
}

void object_patrol_graph_change_edge_direction_command::rollback ( )
{
	edge^ edge = m_patrol_graph->get_node_at( m_node_index_second )->outgoing_edges[m_edge_index_second];
	
	float temp_probability			= edge->probability;
	edge->probability				= 0;

	node^ source_node		= edge->source_node;
	node^ destination_node	= edge->destination_node;

	source_node->remove_outgoing_edge		( edge );
	destination_node->remove_incoming_edge	( edge );

	source_node->add_incoming_edge			( edge );
	destination_node->add_outgoing_edge		( edge );

	edge->source_node		= destination_node;
	edge->destination_node	= source_node;

	edge->probability				= temp_probability;
}

} // namespace editor
} // namespace xray