////////////////////////////////////////////////////////////////////////////
//	Created		: 02.12.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_PATROL_GRAPH_REMOVE_LOOK_POINT_COMMAND_H_INCLUDED
#define OBJECT_PATROL_GRAPH_REMOVE_LOOK_POINT_COMMAND_H_INCLUDED

namespace xray
{
	namespace editor
	{
		ref class	object_patrol_graph;
		ref class	object_patrol_graph_look_point;

		typedef		object_patrol_graph_look_point look_point;

		ref class object_patrol_graph_remove_look_point_command: editor_base::command
		{
		public:
			object_patrol_graph_remove_look_point_command( object_patrol_graph^ patrol_graph, look_point^ look_point );
			~object_patrol_graph_remove_look_point_command( );

		public:
			virtual bool	commit			( ) override;
			virtual void	rollback		( ) override;

		private:
			object_patrol_graph^		m_patrol_graph;
			u32							m_parent_node_index;
			u32							m_look_point_index;
			float						m_look_point_probability;
			look_point^					m_look_point;
			bool						m_is_commited;

		}; // class object_patrol_graph_remove_look_point_command

	} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_PATROL_GRAPH_REMOVE_LOOK_POINT_COMMAND_H_INCLUDED