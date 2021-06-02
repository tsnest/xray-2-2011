////////////////////////////////////////////////////////////////////////////
//	Created		: 07.04.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef DIALOG_GRAPH_NODE_LAYOUT_H_INCLUDED
#define DIALOG_GRAPH_NODE_LAYOUT_H_INCLUDED

#pragma managed( push, off )

namespace xray {
namespace dialog_editor {
	struct dialog_graph_node_layout
	{
		typedef associative_vector<u32, math::int2, vector> layout_vector;

					dialog_graph_node_layout();
					dialog_graph_node_layout(configs::lua_config_value cfg);
		void		save					(configs::lua_config_value cfg);

		layout_vector	m_layout;
	}; // ref class dialog_graph_node_layout
}; //namespace dialog_editor
}; //namespace xray
#pragma managed( pop )
#endif // #ifndef DIALOG_GRAPH_NODE_LAYOUT_H_INCLUDED
