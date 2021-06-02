////////////////////////////////////////////////////////////////////////////
//	Created		: 22.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_PATROL_GRAPH_REMOVE_NODE_COMMAND_H_INCLUDED
#define OBJECT_PATROL_GRAPH_REMOVE_NODE_COMMAND_H_INCLUDED

using namespace System;
using namespace System::Collections::Generic;

namespace xray
{
	namespace editor
	{
		ref class object_patrol_graph;
		ref class object_patrol_graph_node;
		ref class object_patrol_graph_edge;
		ref class object_patrol_graph_look_point;

		typedef object_patrol_graph_node		node;
		typedef object_patrol_graph_edge		edge;
		typedef object_patrol_graph_look_point	look_point;

		ref class object_patrol_graph_remove_node_command: editor_base::command
		{
		public:
			object_patrol_graph_remove_node_command( object_patrol_graph^ patrol_graph );
			~object_patrol_graph_remove_node_command( );

		public:
			virtual bool	commit		( ) override;
			virtual void	rollback	( ) override;

		private:
			value struct edge_data
			{
			public:
				edge_data( node^ n, edge^ e ){ node = n; edge = e; }

				node^ node;
				edge^ edge;
			}; // value struct edge_data

		private:
			object_patrol_graph^		m_patrol_graph;
			u32							m_node_index;
			Float3						m_node_position;
			node^						m_deleted_node;
			array<edge_data>^			m_outgoing_edges;
			array<edge_data>^			m_incoming_edges;
			array<u32>^					m_incoming_edges_indices;
			array<look_point^>^			m_look_points;
			Boolean						m_is_commited;

		}; // class object_patrol_graph_remove_node_command

	} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_PATROL_GRAPH_ADD_REMOVE_NODE_COMMAND_H_INCLUDED