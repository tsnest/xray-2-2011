////////////////////////////////////////////////////////////////////////////
//	Created		: 28.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_PATROL_GRAPH_CHANGE_EDGE_DIRECTION_ACTION_H_INCLUDED
#define OBJECT_PATROL_GRAPH_CHANGE_EDGE_DIRECTION_ACTION_H_INCLUDED

#include "object_patrol_graph_action.h"

using namespace System;

namespace xray
{
	namespace editor
	{
		ref class object_patrol_graph;

		ref class object_patrol_graph_change_edge_direction_action: object_patrol_graph_action
		{
		public:
			object_patrol_graph_change_edge_direction_action	( String^ name, editor_base::scene_view_panel^ value );

		private:
			editor_base::scene_view_panel^	m_window_view;

		public:
			virtual bool		do_it						( ) override;

		private:
			typedef object_patrol_graph_action	super;

		}; // class object_patrol_graph_change_link_direction_action

	} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_PATROL_GRAPH_CHANGE_EDGE_DIRECTION_ACTION_H_INCLUDED