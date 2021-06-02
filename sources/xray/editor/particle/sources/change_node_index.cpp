////////////////////////////////////////////////////////////////////////////
//	Created		: 05.03.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "change_node_index.h"
#include "particle_graph_node.h"
#include "particle_graph_node_collection.h"

namespace xray {
	namespace particle_editor {
		namespace particle_graph_commands{

			using	xray::particle_editor::particle_graph_node;


			change_node_index::change_node_index		(particle_hypergraph^ hypergraph, String^ parent_id, String^ node_id, int index)
			{
				m_hypergraph = hypergraph;
				m_parent_id = parent_id;
				m_node_id = node_id;	
				m_index = index;
				m_old_index = m_hypergraph->get_node_by_id(parent_id)->children->IndexOf(node_id);
			}

			bool				change_node_index::commit		()
			{			
				particle_graph_node^ parent_node = m_hypergraph->get_node_by_id(m_parent_id);
				particle_graph_node^ node = m_hypergraph->get_node_by_id(m_node_id);
				parent_node->on_change_node_index(m_node_id, parent_node->children[m_index]);
				return parent_node->children->MoveToIndex(node, m_index);
			}

			void				change_node_index::rollback		()
			{
				particle_graph_node^ parent_node = m_hypergraph->get_node_by_id(m_parent_id);
				particle_graph_node^ node = m_hypergraph->get_node_by_id(m_node_id);
				parent_node->on_change_node_index(m_node_id, parent_node->children[m_old_index]);
				parent_node->children->MoveToIndex(node, m_old_index);
			}

		} //namespace particle_graph_commands
	} // namespace particle_editor
} // namespace xray