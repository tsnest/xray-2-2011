////////////////////////////////////////////////////////////////////////////
//	Created		: 05.02.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ADD_DELETE_GROUP_NODES_H_INCLUDED
#define ADD_DELETE_GROUP_NODES_H_INCLUDED

#include "particle_hypergraph.h"
#include "particle_graph_node.h"

using namespace System::Collections::Generic;

namespace xray {
	ref class editor::controls::hypergraph::node;
namespace particle_editor {
namespace particle_graph_commands{


public ref class add_group : public xray::editor_base::command
{
	typedef System::Collections::Generic::List<xray::editor::controls::hypergraph::node^ > nodes;
public:
	add_group	(particle_hypergraph^ h, configs::lua_config_ptr config,String^ selected_node_id);
	virtual						~add_group	()					{
		DELETE(m_config_ptr);
	};

	virtual bool				commit				() override;
	virtual void				rollback			() override;

private:

	particle_hypergraph^		m_hypergraph;
	String^						m_selected_node_id;
	configs::lua_config_ptr*	m_config_ptr;
	List<String^>^				m_added_nodes;


}; // class add_group

public ref class remove_group : public xray::editor_base::command
{
	typedef System::Collections::Generic::List<xray::editor::controls::hypergraph::node^ > nodes;
public:
	remove_group	(particle_hypergraph^ h, List<xray::editor::controls::hypergraph::node^ >^ nds);
	virtual						~remove_group	()					
	{
		DELETE(m_config);
	};

	virtual bool				commit				() override;
	virtual void				rollback			() override;

private:
	particle_hypergraph^ 				m_hypergraph;
	List<String^>^						m_nodes_to_delete;
	configs::lua_config_value*			m_config;
	List<String^>^						m_parent_ids;
	List<Int32>^						m_node_indexes;
};// class remove_group

} //namespace particle_graph_commands
} // namespace particle_editor
} // namespace xray
#endif // #ifndef ADD_DELETE_GROUP_NODES_H_INCLUDED