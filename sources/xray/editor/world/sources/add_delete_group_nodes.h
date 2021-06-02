////////////////////////////////////////////////////////////////////////////
//	Created		: 05.02.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ADD_DELETE_GROUP_NODES_H_INCLUDED
#define ADD_DELETE_GROUP_NODES_H_INCLUDED

#include "particle_hypergraph.h"
#include "particle_graph_node.h"

namespace xray {
namespace editor {
namespace particle_graph_commands{

ref class controls::hypergraph::node;

public ref class add_group : public xray::editor_base::command
{
	typedef System::Collections::Generic::List<controls::hypergraph::node^ > nodes;
public:
	add_group	(particle_hypergraph^ h, configs::lua_config_ptr config, particle_graph_node^ selected_node);
	virtual						~add_group	()					{};

	virtual bool				commit				() override;
	virtual void				rollback			() override;

private:
	particle_hypergraph^		m_hypergraph;
	nodes^						m_all_nodes;
	nodes^						m_all_nodes_parents;
	bool						m_first_commit;
	particle_graph_node^		m_selected_node;


}; // class add_group

public ref class remove_group : public xray::editor_base::command
{
	typedef System::Collections::Generic::List<controls::hypergraph::node^ > nodes;
	typedef System::Collections::Generic::List<int>							indexes;
								
	
public:
	remove_group	(particle_hypergraph^ h, nodes^ nds);
	virtual						~remove_group	()					{};

	virtual bool				commit				() override;
	virtual void				rollback			() override;

private:
	particle_hypergraph^ 				m_hypergraph;
	nodes^ 								m_nodes;
	nodes^ 								m_parent_nodes;
	indexes^							m_selected_nodes_indexes;
	nodes^ 								m_selected_nodes;
	particle_graph_node^				m_selected_node;
};// class remove_group

} //namespace particle_graph_commands
} // namespace editor
} // namespace xray
#endif // #ifndef ADD_DELETE_GROUP_NODES_H_INCLUDED