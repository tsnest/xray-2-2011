////////////////////////////////////////////////////////////////////////////
//	Created		: 08.09.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_viewer_graph_node_operator.h"

using xray::animation_editor::animation_viewer_graph_node_operator;
using xray::animation_editor::animation_viewer_hypergraph;

void animation_viewer_graph_node_operator::initialize()
{
	super::initialize();
}

animation_viewer_graph_node_operator::~animation_viewer_graph_node_operator()
{
}

System::String^ animation_viewer_graph_node_operator::get_text()
{
	return "operator +";
}

void animation_viewer_graph_node_operator::on_double_click(System::Object^ , System::EventArgs^ )
{
}

void animation_viewer_graph_node_operator::save(xray::configs::lua_config_value cfg)
{
	super::save(cfg);
	cfg["type"] = 2;
}

void animation_viewer_graph_node_operator::load(xray::configs::lua_config_value const& cfg, animation_viewer_hypergraph^ h)
{
	super::load(cfg, h);
}
