////////////////////////////////////////////////////////////////////////////
//	Created		: 05.03.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef CHANGE_NODE_INDEX_H_INCLUDED
#define CHANGE_NODE_INDEX_H_INCLUDED

#include "particle_graph_node.h"

namespace xray {
	namespace particle_editor {
		namespace particle_graph_commands{
			public ref class change_node_index : public xray::editor_base::command
			{

			public:
				change_node_index		(particle_hypergraph^ hypergraph, String^ parent_id, String^ node_id, int index);
				virtual					~change_node_index			()					{};

				virtual bool			commit				() override;
				virtual void			rollback			() override;

			private:
				particle_hypergraph^	m_hypergraph;
				String^					m_parent_id;
				String^					m_node_id;
				int						m_index;
				int						m_old_index;
			}; // class add_delete_node

		}//namespace particle_graph_commands
	} // namespace particle_editor {
} // namespace xray

#endif // #ifndef CHANGE_NODE_INDEX_H_INCLUDED