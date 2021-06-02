////////////////////////////////////////////////////////////////////////////
//	Created		: 07.09.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_viewer_links_manager.h"

using xray::animation_editor::animation_viewer_links_manager;
using xray::editor::controls::hypergraph::hypergraph_control;

animation_viewer_links_manager::animation_viewer_links_manager(hypergraph_control^ h)
:m_hypergraph(h)
{
	m_links = gcnew links;
}

animation_viewer_links_manager::~animation_viewer_links_manager()
{
	for each(link^ l in m_links)
		delete l;

	delete m_links;
}

void animation_viewer_links_manager::create_link(connection_point^ pt_src, connection_point^ pt_dst)
{
	XRAY_UNREFERENCED_PARAMETER(pt_src);
	XRAY_UNREFERENCED_PARAMETER(pt_dst);
}

void animation_viewer_links_manager::destroy_link(link^ link)
{
	m_links->Remove(link);
}

void animation_viewer_links_manager::on_node_added(node^ node)
{
	XRAY_UNREFERENCED_PARAMETER(node);
}

void animation_viewer_links_manager::on_node_removed(node^ node)
{
	XRAY_UNREFERENCED_PARAMETER(node);
}

void animation_viewer_links_manager::on_node_destroyed(node^ node)
{
	XRAY_UNREFERENCED_PARAMETER(node);
}

animation_viewer_links_manager::links^ animation_viewer_links_manager::visible_links()
{
	return m_links;
}

void animation_viewer_links_manager::clear()
{
	m_links->Clear();
}