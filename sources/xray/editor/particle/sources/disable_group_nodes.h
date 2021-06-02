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
	ref class editor::controls::hypergraph::node;
	namespace particle_editor {
		namespace particle_graph_commands{


			public ref class disable_group_nodes : public xray::editor_base::command
			{
				typedef System::Collections::Generic::List<xray::editor::controls::hypergraph::node^ > nodes;
			public:
				disable_group_nodes	(particle_hypergraph^ h, nodes^ selected_nodes);
				virtual						~disable_group_nodes	()					{};

				virtual bool				commit				() override;
				virtual void				rollback			() override;

			private:
				particle_hypergraph^						m_hypergraph;
				System::Collections::Generic::List<String^>^m_selected_nodes;					
			}; // class add_group

} //namespace particle_graph_commands
} // namespace particle_editor
} // namespace xray

#endif // #ifndef DISABLE_GROUP_NODES_H_INCLUDED