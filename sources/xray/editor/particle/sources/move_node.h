////////////////////////////////////////////////////////////////////////////
//	Created		: 02.02.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MOVE_NODE_H_INCLUDED
#define MOVE_NODE_H_INCLUDED

#include "particle_graph_node.h"

namespace xray {
namespace particle_editor {
namespace particle_graph_commands{

public ref class move_node : public xray::editor_base::command
{
public:
							move_node			(particle_graph_node^ src, particle_graph_node^ dest);
	virtual					~move_node			()					{};

	virtual bool			commit				() override;
	virtual void			rollback			() override;

private:
	particle_hypergraph^	m_hypergraph;
	String^					m_node_id;
	String^					m_new_parent_id;
	String^					m_old_parent_id;
}; // class move_node

}//namespace particle_graph_commands
} // namespace particle_editor
} // namespace xray

#endif // #ifndef MOVE_NODE_H_INCLUDED