////////////////////////////////////////////////////////////////////////////
//	Created		: 14.04.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_node_hypergraph.h"
#include "dialog_links_manager.h"
#include "dialog_graph_link.h"
#include "dialog_graph_node_base.h"
#include "dialog_graph_node_dialog.h"
#include "dialog_hypergraph.h"

using xray::dialog_editor::dialog_node_hypergraph;
using xray::dialog_editor::dialog_graph_link;
using xray::dialog_editor::dialog_document;

dialog_node_hypergraph::dialog_node_hypergraph(xray::dialog_editor::dialog_graph_node_dialog^ owner)
:m_owner(owner)
{
}

dialog_node_hypergraph::~dialog_node_hypergraph()
{
}

void dialog_node_hypergraph::append_node(node^ node)
{
	Controls->Add(node);
	m_nodes->Add(node);
	node->on_added(this);
	node->set_scale(scale);
	links_manager->on_node_added(node);
}

void dialog_node_hypergraph::append_node(nodes^ nds)
{
	for each(xray::editor::controls::hypergraph::node^ n in nds)
		append_node(n);

	Invalidate(false);
}

void dialog_node_hypergraph::remove_node(node^ node)
{
	Controls->Remove(node);
	m_nodes->Remove(node);
	node->on_removed(this);
	links_manager->on_node_removed(node);
}

void dialog_node_hypergraph::remove_node(nodes^ nds)
{
	for each(xray::editor::controls::hypergraph::node^ n in nds)
		remove_node(n);

	Invalidate(false);
}

void dialog_node_hypergraph::make_connection(connection_point^ pt_src, connection_point^ pt_dst)
{
	XRAY_UNREFERENCED_PARAMETER(pt_src);
	XRAY_UNREFERENCED_PARAMETER(pt_dst);
}

dialog_graph_link^ dialog_node_hypergraph::create_connection(node^ source, node^ destination)
{
	dialog_graph_node_base^ src	= safe_cast<dialog_graph_node_base^>(source);
	dialog_graph_node_base^ dst	= safe_cast<dialog_graph_node_base^>(destination);
	xray::editor::controls::hypergraph::connection_point^ src_pt = src->get_connection_point("child");
	xray::editor::controls::hypergraph::connection_point^ dst_pt = dst->get_connection_point("parent");
	if(src_pt==nullptr)
		src_pt = src->add_connection_point(dialog_graph_node_base::typeid, "child");

	if(dst_pt==nullptr)
		dst_pt = dst->add_connection_point(dialog_graph_node_base::typeid, "parent");

	src_pt->draw_text = false;
	dst_pt->draw_text = false;
	display_rectangle = display_rectangle;

	return gcnew dialog_graph_link(src_pt, dst_pt, false);
}

void dialog_node_hypergraph::hypergraph_area_MouseDown(Object^ , System::Windows::Forms::MouseEventArgs^ e)
{
	if((ModifierKeys & Keys::Alt)==Keys::Alt)
	{
		if(e->Button==System::Windows::Forms::MouseButtons::Left)
			move_start();

		return;
	}
	super::hypergraph_area_MouseDown(nullptr, e);
}

void dialog_node_hypergraph::hypergraph_area_MouseUp(Object^ , System::Windows::Forms::MouseEventArgs^ e)
{
	if(e->Button==System::Windows::Forms::MouseButtons::Left)
		move_stop();

	super::hypergraph_area_MouseUp(nullptr, e);
}

xray::dialog_editor::dialog_document^ dialog_node_hypergraph::get_owner()
{
	dialog_hypergraph^ h = dynamic_cast<dialog_hypergraph^>(m_owner->get_parent());
	if(h)
		return h->get_owner();
	else
		return dynamic_cast<dialog_node_hypergraph^>(m_owner->get_parent())->get_owner();
}
