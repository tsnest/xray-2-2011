////////////////////////////////////////////////////////////////////////////
//	Created		: 23.11.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "scene_graph_tab.h"
#include "scene_graph_node.h"
#include "scene_graph_node_style.h"
#include "property_connection_editor.h"
#include "level_editor.h"
#include "project.h"
#include "attribute.h"
#include "object_base.h"
#include "command_select.h"

using xray::editor::scene_graph_tab;
using xray::editor::scene_graph_node;
using xray::editor::object_base;

using xray::editor::scene_links_manager;
using xray::editor::scene_graph_nodes_style_mgr;
using xray::editor::controls::hypergraph::connection_point;
using xray::editor::controls::hypergraph::hypergraph_control;
using xray::editor::controls::hypergraph::link;

void scene_graph_tab::in_constructor()
{
	m_hypergraph->connection_editor		= m_level_editor->get_connection_editor();
	m_hypergraph->nodes_style_manager	= gcnew xray::editor::scene_graph_nodes_style_mgr();
}

void scene_graph_tab::in_destructor()
{
	delete m_hypergraph->nodes_style_manager;
}

void scene_graph_tab::initialize()
{
	m_hypergraph->clear					();

	object_base_list^ object_list		= m_level_editor->selected_objects();
	m_hypergraph->links_manager			= gcnew	scene_links_manager(m_hypergraph, m_level_editor->get_project()->get_link_storage());
	
	for each(object_base^ o in object_list)
	{
		scene_graph_node^ n			= gcnew scene_graph_node;
		m_hypergraph->nodes_style_manager->assign_style(n, "scene_graph_node");
		n->object					= o;
		m_hypergraph->append_node	(n);
	}
	m_hypergraph->do_layout();
}

void scene_graph_tab::reset()
{}

void  scene_graph_tab::m_hypergraph_on_selection_changed()
{
	scene_links_manager::nodes^ selected_nodes = m_hypergraph->selected_nodes;

	item_id_list^ list = gcnew item_id_list;

	for each(scene_graph_node^ node in selected_nodes)
		list->Add(node->object->id());

	m_level_editor->get_command_engine()->run( 
		gcnew xray::editor::command_select(	m_level_editor->get_project(), 
											list, 
											enum_selection_method_set )
											);
}

//---------------------------------------------------------
scene_links_manager::scene_links_manager(hypergraph_control^ h, xray::editor::link_storage^ s)
:m_hypergraph(h),
m_link_storage(s)
{
	m_links	= gcnew links;
}

void scene_links_manager::create_link(connection_point^ pt_src, connection_point^ pt_dst)
{
	link^ lnk		= gcnew link(pt_src, pt_dst);
	m_links->Add	(lnk);
	object_base^ 	src_object = safe_cast<scene_graph_node^>(pt_src->owner)->object;
	object_base^ 	dst_object = safe_cast<scene_graph_node^>(pt_dst->owner)->object;
	m_link_storage->create_link(src_object, pt_src->name, dst_object, pt_dst->name);
}

void scene_links_manager::destroy_link(link^ link)
{
	m_links->Remove	(link);

	scene_graph_node^	source_node			= safe_cast<scene_graph_node^>(link->source_node);
	scene_graph_node^	destination_node	= safe_cast<scene_graph_node^>(link->destination_node);
	m_link_storage->remove_link(source_node->object, 
								link->source_point->name, 
								destination_node->object, 
								link->destination_point->name);
}

void scene_links_manager::on_node_added(node^ node)
{
	scene_graph_node^ scene_node = safe_cast<scene_graph_node^>(node);

	// add SRC links
	System::Collections::ArrayList^ list = m_link_storage->get_links(scene_node->object, nullptr, nullptr, nullptr);
	for each (property_link^ property_link in list)
	{
		scene_graph_node^ destination_node = find_node(property_link->m_dst_object);
		if(!destination_node)
			continue;

		connection_point^ pt_src = node->get_connection_point(property_link->m_src_property_name);
		connection_point^ pt_dst = destination_node->get_connection_point(property_link->m_dst_property_name);

		link^ lnk		= gcnew link(pt_src, pt_dst);
		m_links->Add	(lnk);
	}

	// add DST links
	list = m_link_storage->get_links(nullptr, nullptr, scene_node->object, nullptr);
	for each (property_link^ property_link in list)
	{
		scene_graph_node^ source_node = find_node(property_link->m_src_object);
		if(!source_node)
			continue;

		connection_point^ pt_src = source_node->get_connection_point(property_link->m_src_property_name);

		connection_point^ pt_dst = node->get_connection_point(property_link->m_dst_property_name);

		link^ lnk		= gcnew link(pt_src, pt_dst);
		m_links->Add	(lnk);
	}
}

scene_graph_node^ scene_links_manager::find_node(object_base^ o)
{
	nodes^ nodes = m_hypergraph->all_nodes();
	for each(scene_graph_node^ node in nodes)
		if(node->object==o)
			return node;

	return nullptr;
}

void scene_links_manager::on_node_removed(node^)
{}

void scene_links_manager::on_node_destroyed(node^ node)
{
	links^ tmp			= gcnew links;
	tmp->AddRange		(m_links);
	for each(link^ link in tmp)
		if(link->is_relative(node))
			m_links->Remove(link);
}

scene_links_manager::links^ scene_links_manager::visible_links()
{
	return m_links;
}

void scene_links_manager::clear()
{}