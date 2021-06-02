////////////////////////////////////////////////////////////////////////////
//	Created		: 05.03.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "disable_group_nodes.h"
#include "particle_hypergraph.h"
#include "particle_graph_node.h"

using xray::editor::controls::hypergraph::node;

namespace xray {
namespace editor {
namespace particle_graph_commands{

disable_group_nodes::disable_group_nodes(particle_hypergraph^ h, nodes^ selected_nodes)
:m_hypergraph(h)
{
	m_selected_nodes = selected_nodes;
}

bool disable_group_nodes::commit()
{
	if (m_selected_nodes->Count == 0)
		return false;

	if (m_selected_nodes->Count == 1 && m_selected_nodes[0] == safe_cast<particle_hypergraph^>(m_hypergraph)->root_node){
		MessageBox::Show("You can not disable 'root' node!", 
			"Information", MessageBoxButtons::OK, MessageBoxIcon::Information);
	}
		

	for each (node^ n in m_selected_nodes){
		particle_graph_node^ particle_node = safe_cast<particle_graph_node^>(n);
		if (particle_node == safe_cast<particle_hypergraph^>(m_hypergraph)->root_node){
			MessageBox::Show("You can not disable 'root' node!", 
				"Information", MessageBoxButtons::OK, MessageBoxIcon::Information);
			continue;
		}
		particle_node->set_enabled(!particle_node->enabled());
	}
	m_hypergraph->Invalidate(false);
	return true;
}

void disable_group_nodes::rollback()
{
	commit();
}

};// namespace particle_graph_commands
} // namespace editor
} // namespace xray