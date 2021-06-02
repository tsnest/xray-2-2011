////////////////////////////////////////////////////////////////////////////
//	Created		: 10.09.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_viewer_graph_node_root.h"
#include "animation_viewer_graph_link.h"
#include "animation_viewer_graph_link_timed.h"
#include "animation_viewer_hypergraph.h"

using xray::animation_editor::animation_viewer_graph_node_root;
using xray::animation_editor::animation_viewer_hypergraph;

void animation_viewer_graph_node_root::initialize()
{
	super::initialize();
}

animation_viewer_graph_node_root::~animation_viewer_graph_node_root()
{
}

System::String^ animation_viewer_graph_node_root::get_text()
{
	if(m_text=="TEMP")
		generate_caption();

	return m_text;
}

void animation_viewer_graph_node_root::on_double_click(System::Object^ , System::EventArgs^ )
{
}

void animation_viewer_graph_node_root::save(xray::configs::lua_config_value cfg)
{
	super::save(cfg);
	cfg["type"] = 0;
	unmanaged_string str = unmanaged_string(m_text);
	cfg["caption"] = str.c_str();
}

void animation_viewer_graph_node_root::load(xray::configs::lua_config_value const& cfg, animation_viewer_hypergraph^ h)
{
	super::load(cfg, h);
	if(cfg.value_exists("caption"))
		m_text = gcnew System::String(cfg["caption"]);
}

animation_viewer_graph_node_root^ animation_viewer_graph_node_root::transition()
{
	for each(animation_viewer_graph_link^ lnk in m_parent->links_manager->visible_links())
	{
		if(dynamic_cast<animation_viewer_graph_link_timed^>(lnk) && lnk->source_node==this)
			return safe_cast<animation_viewer_graph_node_root^>(lnk->destination_node);
	}

	return nullptr;
}

System::String^ animation_viewer_graph_node_root::caption::get()
{
	return m_text;
}

bool node_with_name_exists(xray::animation_editor::animation_viewer_graph_node_root^ r, System::String^ name)
{
	for each(xray::animation_editor::animation_viewer_graph_node_base^ n in r->get_parent()->all_nodes())
	{
		if(r!=n && dynamic_cast<xray::animation_editor::animation_viewer_graph_node_root^>(n)
			&& dynamic_cast<xray::animation_editor::animation_viewer_graph_node_root^>(n)->caption==name)
			return true;
	}

	return false;
}

void animation_viewer_graph_node_root::generate_caption()
{
	System::String^ str = "TARGET_";
	u32 i = 0;
	while(node_with_name_exists(this, str+i))
		++i;

	m_text = str+i;
}
