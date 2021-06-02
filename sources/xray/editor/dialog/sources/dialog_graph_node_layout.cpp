////////////////////////////////////////////////////////////////////////////
//	Created		: 17.12.2009
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_graph_node_layout.h"

#pragma unmanaged

using xray::dialog_editor::dialog_graph_node_layout;

dialog_graph_node_layout::dialog_graph_node_layout()
{
	m_layout = layout_vector();
}

dialog_graph_node_layout::dialog_graph_node_layout(xray::configs::lua_config_value cfg)
{
	m_layout = layout_vector();
	if(cfg.value_exists("layout"))
	{
		configs::lua_config::const_iterator	i = cfg["layout"].begin();
		for(; i!=cfg["layout"].end(); ++i) 
		{
			u32 id = (*i)["id"];
			u32 x = (*i)["x"];
			u32 y = (*i)["y"];
			m_layout.insert(std::pair<u32, math::int2>(id, math::int2(x, y)));
		}
	}
}

void dialog_graph_node_layout::save(xray::configs::lua_config_value cfg)
{
	u32 counter = 0;
	layout_vector::const_iterator i = m_layout.begin();
	for(; i!=m_layout.end(); ++i)
	{
		xray::configs::lua_config_value val = cfg["layout"][counter];
		val["id"] = i->first;
		val["x"] = i->second.x;
		val["y"] = i->second.y;
		++counter;
	}
}