////////////////////////////////////////////////////////////////////////////
//	Created		: 22.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_patrol_graph_add_node_command.h"
#include "object_patrol_graph.h"
#include "object_patrol_graph_node.h"
#include "object_patrol_graph_edge.h"
#include "tool_base.h"
#include "object_inspector_tab.h"
#include "level_editor.h"

namespace xray{
namespace editor{

object_patrol_graph_add_node_command::object_patrol_graph_add_node_command( object_patrol_graph^ patrol_graph, Float3 new_node_position )
{
	m_patrol_graph			= patrol_graph;
	m_new_node_position		= new_node_position;
	m_selected_node_index	= -1;
	m_selected_edge_index	= -1;
	m_new_node_index		= -1;
	m_removed_edge_index	= -1;

	object_patrol_graph_part^ selected_part = m_patrol_graph->selected_graph_part;

	if( selected_part == nullptr )
		return;

	node^ selected_node		= dot_net::as<node^>( selected_part );
	if( selected_node != nullptr )
	{
		m_selected_node_index	= selected_node->index;
	}
	else
	{
		edge^ selected_edge		= safe_cast<edge^>( selected_part );
		m_selected_edge_index	= selected_edge->index;
		m_selected_node_index	= selected_edge->source_node->index;
	}
}

bool object_patrol_graph_add_node_command::commit ( )
{
	node^		new_node	= m_patrol_graph->create_node( m_new_node_position );
	m_new_node_index		= new_node->index;
	
	if( m_selected_node_index >= 0 )
	{
		if( m_selected_edge_index == -1 )
		{
			m_patrol_graph->create_edge	( safe_cast<node^>( m_patrol_graph->get_node_at( m_selected_node_index ) ), new_node )->index;
		}
		else
		{
			node^ source_node			= m_patrol_graph->get_node_at( m_selected_node_index );
			m_removed_edge				= source_node->outgoing_edges[m_selected_edge_index];
			node^ destination_node		= m_removed_edge->destination_node;

			m_removed_edge_index		= m_removed_edge->index;
			m_removed_edge->detach		( );

			m_patrol_graph->create_edge	( source_node, new_node );
			m_patrol_graph->create_edge	( new_node, destination_node );
		}
	}

	new_node->select( );

	return true;
}

void object_patrol_graph_add_node_command::rollback ( )
{
	node^ node = m_patrol_graph->get_node_at( m_new_node_index );

	if( m_removed_edge_index >= 0 )
		m_removed_edge->attach( node->incoming_edges[0]->source_node, m_removed_edge_index, node->outgoing_edges[0]->destination_node );

	node->detach( );
	delete node;

	if( m_patrol_graph->selected_graph_part == node )
		m_patrol_graph->selected_graph_part = nullptr;

	m_patrol_graph->owner_tool( )->get_level_editor( )->get_object_inspector_tab( )->remove_object_from_selection( node );
}

} // namespace editor
} // namespace xray