////////////////////////////////////////////////////////////////////////////
//	Created		: 05.03.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef DISABLE_GROUP_NODES_H_INCLUDED
#define DISABLE_GROUP_NODES_H_INCLUDED

#include "particle_hypergraph.h"
#include "particle_graph_node.h"

namespace xray {
	namespace editor {
		namespace particle_graph_commands{

			ref class controls::hypergraph::node;

			public ref class disable_group_nodes : public xray::editor_base::command
			{
				typedef System::Collections::Generic::List<controls::hypergraph::node^ > nodes;
			public:
				disable_group_nodes	(particle_hypergraph^ h, nodes^ selected_nodes);
				virtual						~disable_group_nodes	()					{};

				virtual bool				commit				() override;
				virtual void				rollback			() override;

			private:
				particle_hypergraph^		m_hypergraph;
				nodes^						m_selected_nodes;				
			}; // class add_group

} //namespace particle_graph_commands
} // namespace editor
} // namespace xray

#endif // #ifndef DISABLE_GROUP_NODES_H_INCLUDED