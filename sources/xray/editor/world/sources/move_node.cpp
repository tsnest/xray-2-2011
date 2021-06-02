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
namespace editor {
namespace particle_graph_commands{

using	xray::editor::particle_graph_node;

					move_node::move_node	(particle_graph_node^ src, particle_graph_node^ dest)
{
	m_src = src;
	m_new_parent = dest;
	m_old_parent = src->parent_node;
}

bool				move_node::commit		()
{
	safe_cast<particle_hypergraph^>(m_src->parent_hypergraph)->change_parent_node(m_src, m_new_parent);
	return true;
}

void				move_node::rollback		()
{
	safe_cast<particle_hypergraph^>(m_src->parent_hypergraph)->change_parent_node(m_src, m_old_parent);
}

} //namespace particle_graph_commands
} // namespace editor 
} // namespace xray