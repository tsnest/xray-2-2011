////////////////////////////////////////////////////////////////////////////
//	Created		: 22.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_PATROL_GRAPH_ADD_EDGE_COMMAND_H_INCLUDED
#define OBJECT_PATROL_GRAPH_ADD_EDGE_COMMAND_H_INCLUDED

namespace xray
{
	namespace editor
	{
		ref class object_patrol_graph;
		ref class object_patrol_graph_node;
		ref class object_patrol_graph_edge;

		typedef object_patrol_graph_node node;
		typedef object_patrol_graph_edge edge;

		ref class object_patrol_graph_add_edge_command: editor_base::command
		{
		public:
			object_patrol_graph_add_edge_command( object_patrol_graph^ patrol_graph, node^ source, node^ destination );

		public:
			virtual bool	commit			( ) override;
			virtual void	rollback		( ) override;

		private:
			object_patrol_graph^		m_patrol_graph;
			u32							m_source_node_index;
			u32							m_destination_node_index;
			u32							m_created_edge_index;
		}; // class object_patrol_graph_add_edge_command


	} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_PATROL_GRAPH_ADD_EDGE_COMMAND_H_INCLUDED