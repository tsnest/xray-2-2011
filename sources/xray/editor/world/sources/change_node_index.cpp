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
	namespace editor {
		namespace particle_graph_commands{

			using	xray::editor::particle_graph_node;


			change_node_index::change_node_index		(particle_graph_node^ parent, particle_graph_node^ node, int index)
			{
				m_parent_node = parent;
				m_node = node;	
				m_index = index;
				m_old_index = m_parent_node->children->IndexOf(m_node);
			}

			bool				change_node_index::commit		()
			{					
				return m_parent_node->children->MoveToIndex(m_node, m_index);
			}

			void				change_node_index::rollback		()
			{
				m_parent_node->children->MoveToIndex(m_node, m_old_index);
			}

		} //namespace particle_graph_commands
	} // namespace editor 
} // namespace xray