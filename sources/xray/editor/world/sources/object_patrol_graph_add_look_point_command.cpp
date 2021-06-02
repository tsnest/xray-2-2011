////////////////////////////////////////////////////////////////////////////
//	Created		: 02.12.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_patrol_graph_add_look_point_command.h"
#include "object_patrol_graph.h"
#include "object_patrol_graph_node.h"
#include "object_patrol_graph_look_point.h"

namespace xray{
namespace editor{

object_patrol_graph_add_look_point_command::object_patrol_graph_add_look_point_command( object_patrol_graph^ patrol_graph, node^ parent_node, Float3 position )
{
	m_patrol_graph				= patrol_graph;
	m_parent_node_index			= parent_node->index;
	m_new_look_point_position	= position;
}

bool object_patrol_graph_add_look_point_command::commit ( )
{
	node^			node		= m_patrol_graph->get_node_at( m_parent_node_index );
	look_point^		look_point	= gcnew object_patrol_graph_look_point( m_patrol_graph, m_new_look_point_position );

	look_point->attach( node );
	m_created_look_point_index = look_point->index;

	return true;
}

void object_patrol_graph_add_look_point_command::rollback ( )
{
	node^ node				= m_patrol_graph->get_node_at( m_parent_node_index );
	look_point^ look_point	= node->look_points[m_created_look_point_index];
	
	look_point->detach( );
	delete look_point;
}
	
} // namespace editor
} // namespace xray