////////////////////////////////////////////////////////////////////////////
//	Created		: 02.12.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_patrol_graph_remove_look_point_command.h"
#include "object_patrol_graph.h"
#include "object_patrol_graph_node.h"
#include "object_patrol_graph_look_point.h"

namespace xray{
namespace editor{

object_patrol_graph_remove_look_point_command::object_patrol_graph_remove_look_point_command( object_patrol_graph^ patrol_graph, look_point^ look_point )
{
	m_patrol_graph			= patrol_graph;
	m_parent_node_index		= look_point->parent_node->index;
	m_look_point_index		= look_point->index;
}

object_patrol_graph_remove_look_point_command::~object_patrol_graph_remove_look_point_command( )
{
	if( m_is_commited )
		delete m_look_point;
}

bool object_patrol_graph_remove_look_point_command::commit ( )
{
	m_look_point				= m_patrol_graph->get_node_at( m_parent_node_index )->look_points[m_look_point_index];
	m_look_point->detach		( );

	m_is_commited = true;
	return true;
}

void object_patrol_graph_remove_look_point_command::rollback ( )
{
	m_is_commited = false;
	m_look_point->attach		( m_look_point_index, m_patrol_graph->get_node_at ( m_parent_node_index ) );
}

} // namespace editor
} // namespace xray