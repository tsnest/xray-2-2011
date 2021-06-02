////////////////////////////////////////////////////////////////////////////
//	Created		: 02.02.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "move_node.h"
#include "particle_graph_node.h"
#include "particle_graph_node_collection.h"

namespace xray {
namespace particle_editor {
namespace particle_graph_commands{

using	xray::particle_editor::particle_graph_node;

					move_node::move_node	(particle_graph_node^ src, particle_graph_node^ dest)
{
	m_hypergraph	= src->parent_graph;
	m_node_id		= src->id;
	m_new_parent_id = dest->id;
	m_old_parent_id = src->parent_node->id;
}

bool				move_node::commit		()
{

	m_hypergraph->change_parent_node(m_hypergraph->get_node_by_id(m_node_id), m_hypergraph->get_node_by_id(m_new_parent_id));
	return true;
}

void				move_node::rollback		()
{
	m_hypergraph->change_parent_node(m_hypergraph->get_node_by_id(m_node_id), m_hypergraph->get_node_by_id(m_old_parent_id));
}

} //namespace particle_graph_commands
} // namespace particle_editor
} // namespace xray