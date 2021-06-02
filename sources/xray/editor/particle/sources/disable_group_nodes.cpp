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
namespace particle_editor {
namespace particle_graph_commands{

disable_group_nodes::disable_group_nodes(particle_hypergraph^ h, nodes^ selected_nodes)
:m_hypergraph(h)
{
	m_selected_nodes = gcnew System::Collections::Generic::List<String^>();
	for each(particle_graph_node^ n in selected_nodes)
	{
		m_selected_nodes->Add(n->id);
	}
}

bool disable_group_nodes::commit()
{
	if (m_selected_nodes->Count == 0)
		return false;

	if (m_selected_nodes->Count == 1 && m_hypergraph->get_node_by_id(m_selected_nodes[0]) == m_hypergraph->root_node){
		MessageBox::Show("You can not disable 'root' node!", 
			"Information", MessageBoxButtons::OK, MessageBoxIcon::Information);
	}
		

	for each (String^ id in m_selected_nodes){
		particle_graph_node^ particle_node = m_hypergraph->get_node_by_id(id);
		if (particle_node == m_hypergraph->root_node){
			MessageBox::Show("You can not disable 'root' node!", 
				"Information", MessageBoxButtons::OK, MessageBoxIcon::Information);
			continue;
		}
		particle_node->enabled = !particle_node->enabled;
	}
	m_hypergraph->Invalidate(false);
	return true;
}

void disable_group_nodes::rollback()
{
	commit();
}

};// namespace particle_graph_commands
} // namespace particle_editor
} // namespace xray