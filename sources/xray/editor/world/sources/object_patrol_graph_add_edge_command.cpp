////////////////////////////////////////////////////////////////////////////
//	Created		: 22.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_patrol_graph_add_edge_command.h"

#include "object_patrol_graph.h"
#include "object_patrol_graph_node.h"
#include "object_patrol_graph_edge.h"

#include "tool_base.h"
#include "level_editor.h"
#include "object_inspector_tab.h"

namespace xray{
namespace editor{

object_patrol_graph_add_edge_command::object_patrol_graph_add_edge_command( object_patrol_graph^ patrol_graph, node^ source, node^ destination )
{
	m_patrol_graph				= patrol_graph;
	m_source_node_index			= source->index;
	m_destination_node_index	= destination->index;
}

bool object_patrol_graph_add_edge_command::commit ( )
{
	edge^ new_edge			= gcnew edge( m_patrol_graph );
	new_edge->attach		( m_patrol_graph->get_node_at( m_source_node_index ), m_patrol_graph->get_node_at( m_destination_node_index ) );
	m_created_edge_index	= new_edge->index;

	return true;
}

void object_patrol_graph_add_edge_command::rollback ( )
{
	edge^ created_edge = m_patrol_graph->get_node_at( m_source_node_index )->outgoing_edges[m_created_edge_index];

	m_patrol_graph->owner_tool( )->get_level_editor( )->get_object_inspector_tab( )->remove_object_from_selection( created_edge );

	if( m_patrol_graph->selected_graph_part == created_edge )
		m_patrol_graph->selected_graph_part = nullptr;

	created_edge->detach( );
	delete created_edge;
}

} // namespace editor
} // namespace xray