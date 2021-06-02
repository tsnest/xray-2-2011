////////////////////////////////////////////////////////////////////////////
//	Created		: 07.09.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_viewer_graph_link.h"
#include "animation_viewer_graph_node_base.h"
#include "animation_viewer_graph_node_animation.h"
#include "animation_viewer_graph_node_root.h"

using xray::animation_editor::animation_viewer_graph_link;
using namespace System::Collections;

animation_viewer_graph_link::animation_viewer_graph_link(connection_point^ src, connection_point^ dst)
:super(src, dst)
{
	src->draw_text = false;
	dst->draw_text = false;
	m_container = gcnew wpf_property_container();
}

animation_viewer_graph_link::~animation_viewer_graph_link()
{
	animation_viewer_graph_node_animation^ dst_an = dynamic_cast<animation_viewer_graph_node_animation^>(destination_node);
	animation_viewer_graph_node_animation^ src_an = dynamic_cast<animation_viewer_graph_node_animation^>(source_node);
	if(dst_an && src_an)
		src_an->init_properties(nullptr);
	else
	{
		if(!dynamic_cast<animation_viewer_graph_node_root^>(source_node))
		{
			animation_viewer_graph_node_base^ n1 = safe_cast<animation_viewer_graph_node_base^>(source_node);
			animation_viewer_graph_node_base^ n2 = safe_cast<animation_viewer_graph_node_base^>(destination_node);
			n1->parent = nullptr;
			n2->childs->Remove(n1);
		}
	}
}

void animation_viewer_graph_link::load(xray::configs::lua_config_value const& cfg)
{
	XRAY_UNREFERENCED_PARAMETER(cfg);
}

void animation_viewer_graph_link::save(xray::configs::lua_config_value cfg)
{
	cfg["src_id"] = safe_cast<animation_viewer_graph_node_base^>(source_node)->id;
	cfg["dst_id"] = safe_cast<animation_viewer_graph_node_base^>(destination_node)->id;
}