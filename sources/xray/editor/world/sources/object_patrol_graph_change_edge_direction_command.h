////////////////////////////////////////////////////////////////////////////
//	Created		: 28.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_PATROL_GRAPH_CHANGE_EDGE_DIRECTION_COMMAND_H_INCLUDED
#define OBJECT_PATROL_GRAPH_CHANGE_EDGE_DIRECTION_COMMAND_H_INCLUDED

namespace xray
{
	namespace editor
	{
		ref class object_patrol_graph;
		ref class object_patrol_graph_edge;

		ref class object_patrol_graph_change_edge_direction_command: editor_base::command
		{
		public:
			object_patrol_graph_change_edge_direction_command( object_patrol_graph^ patrol_graph, object_patrol_graph_edge^ edge );

		public:
			virtual bool	commit			( ) override;
			virtual void	rollback		( ) override;

		private:
			object_patrol_graph^		m_patrol_graph;

			s32							m_node_index;
			s32							m_edge_index;
			s32							m_node_index_second;
			s32							m_edge_index_second;

		}; // class object_patrol_graph_change_edge_direction_command

	} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_PATROL_GRAPH_CHANGE_EDGE_DIRECTION_COMMAND_H_INCLUDED