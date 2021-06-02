////////////////////////////////////////////////////////////////////////////
//	Created		: 02.02.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MOVE_NODE_H_INCLUDED
#define MOVE_NODE_H_INCLUDED

#include "particle_graph_node.h"

namespace xray {
namespace editor {
namespace particle_graph_commands{

public ref class move_node : public xray::editor_base::command
{
public:
							move_node			(particle_graph_node^ src, particle_graph_node^ dest);
	virtual					~move_node			()					{};

	virtual bool			commit				() override;
	virtual void			rollback			() override;

private:
	particle_graph_node^	m_src;
	particle_graph_node^	m_new_parent;
	particle_graph_node^	m_old_parent;
}; // class move_node

}//namespace particle_graph_commands
} // namespace editor 
} // namespace xray

#endif // #ifndef MOVE_NODE_H_INCLUDED