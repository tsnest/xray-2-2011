////////////////////////////////////////////////////////////////////////////
//	Created		: 26.01.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_links_manager.h"

using xray::editor::particle_links_manager;
using xray::editor::controls::hypergraph::hypergraph_control;

particle_links_manager::particle_links_manager(hypergraph_control^ h)
:m_hypergraph(h)
{
	m_links = gcnew links;
}

void particle_links_manager::create_link(connection_point^ pt_src, connection_point^ pt_dst)
{
	link^ lnk				= gcnew link(pt_src, pt_dst);
	m_links->Add			(lnk);
}

void particle_links_manager::create_link(node^ src_node, node^ dst_node)
{
	xray::editor::controls::hypergraph::connection_point^ dst_point = src_node->get_connection_point("parent");
	if(dst_point==nullptr)
		dst_point = src_node->add_connection_point(node::typeid, "parent");

	xray::editor::controls::hypergraph::connection_point^ src_point = dst_node->get_connection_point("child");
	if(src_point==nullptr)
		src_point = dst_node->add_connection_point(node::typeid, "child");


	create_link(dst_point, src_point);
}

void particle_links_manager::destroy_link(link^ link)
{
	m_links->Remove			(link);
}

void particle_links_manager::destroy_link(node^ src_node, node^ dst_node)
{

	xray::editor::controls::hypergraph::connection_point^ pt_src = src_node->get_connection_point("parent");
	xray::editor::controls::hypergraph::connection_point^ pt_dst = dst_node->get_connection_point("child");

	link^ link_to_remove = nullptr;
	for each (link^ lnk in m_links){
		if ((lnk->destination_point == pt_dst)&&(lnk->source_point == pt_src))
			link_to_remove = lnk;
	}
	if (link_to_remove != nullptr)
		destroy_link (link_to_remove);
}

void particle_links_manager::on_node_added(node^ node)
{
	XRAY_UNREFERENCED_PARAMETER(node);
}

void particle_links_manager::on_node_removed(node^ node)
{
	XRAY_UNREFERENCED_PARAMETER(node);
}

void particle_links_manager::on_node_destroyed(node^ node)
{
	links^ tmp			= gcnew links;
	tmp->AddRange		(m_links);
	for each(link^ link in tmp)
	{
		if(link->is_relative(node))
			m_links->Remove(link);
	}
	tmp->Clear();
}

particle_links_manager::links^ particle_links_manager::visible_links()
{
	links^ vis_links  = gcnew links();
	for each(link^ l in m_links){
		if (l->visible())
			vis_links->Add(l);
	}
	return vis_links;
}

void particle_links_manager::clear()
{
	m_links->Clear();
}