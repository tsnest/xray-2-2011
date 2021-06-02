////////////////////////////////////////////////////////////////////////////
//	Created		: 02.02.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "add_delete_node.h"
#include "particle_graph_node.h"
#include "particle_graph_node_collection.h"

namespace xray {
namespace editor {
namespace particle_graph_commands{

using	xray::editor::particle_graph_node;


					add_node::add_node		(particle_graph_node^ src, particle_graph_node^ dest)
{
	m_src = src;
	m_dest = dest;	
}

bool				add_node::commit		()
{

	m_dest->add_children(m_src);
	return true;
}

void				add_node::rollback		()
{
	m_dest->remove_children(m_src);
	m_dest->parent_hypergraph->Controls->Remove(m_src);
	m_dest->parent_hypergraph->all_nodes()->Remove(m_src);
	m_src->parent_hypergraph = nullptr;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////


					remove_node::remove_node(particle_graph_node^ src, particle_graph_node^ dest)
{
	m_src = src;
	m_dest = dest;
}

bool				remove_node::commit		()
{
	m_dest->children->Remove(m_src);
	return true;
}

void				remove_node::rollback	()
{
	m_dest->children->Add(m_src);
}

} //namespace particle_graph_commands
} // namespace editor 
} // namespace xray