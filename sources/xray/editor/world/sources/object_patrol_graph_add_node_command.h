////////////////////////////////////////////////////////////////////////////
//	Created		: 22.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_PATROL_GRAPH_ADD_NODE_COMMAND_H_INCLUDED
#define OBJECT_PATROL_GRAPH_ADD_NODE_COMMAND_H_INCLUDED

namespace xray
{
	namespace editor
	{
		ref class object_patrol_graph;
		ref class object_patrol_graph_edge;

		typedef object_patrol_graph_edge edge;

		ref class object_patrol_graph_add_node_command: editor_base::command
		{
		public:
			object_patrol_graph_add_node_command( object_patrol_graph^ patrol_graph, Float3 new_node_position );

		public:
			virtual bool	commit			( ) override;
			virtual void	rollback		( ) override;

		private:
			object_patrol_graph^		m_patrol_graph;

			Float3						m_new_node_position;
			s32							m_new_node_index;
			s32							m_removed_edge_index;
			edge^						m_removed_edge;
			s32							m_selected_node_index;
			s32							m_selected_edge_index;

		}; // class object_patrol_graph_add_node_command

	} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_PATROL_GRAPH_ADD_NODE_COMMAND_H_INCLUDED