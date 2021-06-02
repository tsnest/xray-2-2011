////////////////////////////////////////////////////////////////////////////
//	Created		: 14.09.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_viewer_move_nodes.h"
#include "animation_viewer_graph_node_base.h"
#include "animation_viewer_hypergraph.h"

using xray::animation_editor::animation_viewer_move_nodes;
using xray::editor::controls::hypergraph::node;
using xray::animation_editor::animation_viewer_hypergraph;

animation_viewer_move_nodes::animation_viewer_move_nodes(animation_viewer_hypergraph^ h, nodes^ n, System::Drawing::Point offset)
:m_hypergraph(h)
{
	m_nodes_ids = gcnew nodes_ids();
	for each(animation_viewer_graph_node_base^ node in n)
	{
		m_nodes_ids->Add(node->id);
		u32 x = (u32)node->position.X;
		u32 y = (u32)node->position.Y;
		m_rollback_positions.Add(System::Drawing::Point(x-offset.X, y-offset.Y));
		m_cur_positions.Add(node->position);
	}
}

bool animation_viewer_move_nodes::commit()
{
	u32 i = 0;
	for each(u32 node_id in m_nodes_ids)
	{
		animation_viewer_graph_node_base^ n = animation_viewer_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), node_id);
		n->position = m_cur_positions[i];
		i++;
	}
	return true;
}

void animation_viewer_move_nodes::rollback()
{
	u32 i = 0;
	for each(u32 node_id in m_nodes_ids)
	{
		animation_viewer_graph_node_base^ n = animation_viewer_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), node_id);
		n->position = m_rollback_positions[i];
		i++;
	}
}