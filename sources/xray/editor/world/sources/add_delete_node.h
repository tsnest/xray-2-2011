////////////////////////////////////////////////////////////////////////////
//	Created		: 02.02.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ADD_DELETE_NODE_H_INCLUDED
#define ADD_DELETE_NODE_H_INCLUDED

#include "particle_graph_node.h"

namespace xray {
namespace editor {
namespace particle_graph_commands{

	typedef System::Collections::Generic::List<particle_graph_node^ > nodes;

	public ref class add_node : public xray::editor_base::command
	{

	public:
								add_node			(particle_graph_node^ src, particle_graph_node^ dest);
		virtual					~add_node			()					{};

		virtual bool			commit				() override;
		virtual void			rollback			() override;

	private:
		particle_graph_node^	m_src;
		particle_graph_node^	m_dest;
		nodes^					m_nodes;
	}; // class add_delete_node

	public ref class remove_node : public xray::editor_base::command
	{
	public:
								remove_node			(particle_graph_node^ src, particle_graph_node^ dest);
		virtual					~remove_node		()					{};

		virtual bool			commit				() override;
		virtual void			rollback			() override;

	private:
		particle_graph_node^	m_src;
		particle_graph_node^	m_dest;
		nodes^					m_nodes;

	}; // class add_delete_node

}//namespace particle_graph_commands
} // namespace editor 
} // namespace xray

#endif // #ifndef ADD_DELETE_NODE_H_INCLUDED