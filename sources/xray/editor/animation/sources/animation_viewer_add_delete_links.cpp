////////////////////////////////////////////////////////////////////////////
//	Created		: 14.09.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_viewer_add_delete_links.h"
#include "animation_viewer_hypergraph.h"
#include "animation_viewer_links_manager.h"
#include "animation_viewer_graph_link.h"
#include "animation_viewer_graph_node_base.h"
#include "viewer_editor.h"
#include "viewer_document.h"
#include <xray/strings_stream.h>

using xray::animation_editor::animation_viewer_add_links;
using xray::animation_editor::animation_viewer_remove_links;
using xray::animation_editor::animation_viewer_hypergraph;
using xray::animation_editor::animation_viewer_links_manager;
using xray::editor::controls::hypergraph::link;
using xray::editor::controls::hypergraph::connection_point;
using xray::animation_editor::animation_viewer_graph_link;
using xray::animation_editor::animation_viewer_graph_link_weighted;
////////////////////////////////////////////////////////////////////////////
//- class animation_viewer_add_links --------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
animation_viewer_add_links::animation_viewer_add_links(animation_viewer_hypergraph^ h, links^ lnks)
:m_hypergraph(h)
{
	m_links_ids = gcnew links_ids_type();
	configs::lua_config_ptr cfg_ptr = configs::create_lua_config();
	xray::strings::stream stream(g_allocator);
	m_hypergraph->save(cfg_ptr->get_root()["scene"], nullptr, lnks);
	cfg_ptr->save(stream);
	stream.append("%c", 0);
	m_cfg_string = strings::duplicate(g_allocator, (pcstr)stream.get_buffer());

	for each(animation_viewer_graph_link^ lnk in lnks)
	{
		u32 source_id = safe_cast<animation_viewer_graph_node_base^>(lnk->source_node)->id;
		u32 destination_id = safe_cast<animation_viewer_graph_node_base^>(lnk->destination_node)->id;
		if(!(m_links_ids->ContainsKey(source_id)))
			m_links_ids->Add(source_id, gcnew nodes_ids_type());

		m_links_ids[source_id]->Add(destination_id);
	}
}

animation_viewer_add_links::~animation_viewer_add_links()
{
	delete m_links_ids;
	FREE(m_cfg_string);
}

bool animation_viewer_add_links::commit()
{
	configs::lua_config_ptr config_ptr = configs::create_lua_config_from_string(m_cfg_string);

	if(!config_ptr || config_ptr->empty()) 
		return false;

	configs::lua_config_value::iterator links_it = config_ptr->get_root()["scene"]["links"].begin();
	for(; links_it!=config_ptr->get_root()["scene"]["links"].end(); ++links_it)
	{
		animation_viewer_graph_node_base^ n1 = animation_viewer_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), (*links_it)["src_id"]); 
		animation_viewer_graph_node_base^ n2 = animation_viewer_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), (*links_it)["dst_id"]); 
		R_ASSERT(n1&&n2);
		animation_viewer_graph_link^ lnk = animation_viewer_hypergraph::create_link(n1, n2); 
		lnk->load(*links_it);
		m_hypergraph->links_manager->visible_links()->Add(lnk);
	}

	m_hypergraph->display_rectangle = m_hypergraph->display_rectangle;
	if(m_hypergraph->get_owner()->target_locked)
		m_hypergraph->get_editor()->change_locked();

	return true;
}

void animation_viewer_add_links::rollback()
{
	for each(System::Collections::Generic::KeyValuePair<u32, nodes_ids_type^>^ pr in m_links_ids)
	{
		for each(u32 id2 in pr->Value)
		{
			animation_viewer_graph_link^ lnk = animation_viewer_hypergraph::find_link_by_ids(m_hypergraph->links_manager->visible_links(), pr->Key, id2);
			m_hypergraph->links_manager->visible_links()->Remove(lnk);
			delete lnk;
		}
	}

	m_hypergraph->Invalidate(false);
	if(m_hypergraph->get_owner()->target_locked)
		m_hypergraph->get_editor()->change_locked();
}
////////////////////////////////////////////////////////////////////////////
//- class animation_viewer_remove_links -----------------------------------------------------
////////////////////////////////////////////////////////////////////////////
animation_viewer_remove_links::animation_viewer_remove_links(animation_viewer_hypergraph^ h, links^ lnks)
:m_hypergraph(h)
{
	m_links_ids = gcnew links_ids_type();
	configs::lua_config_ptr cfg_ptr = configs::create_lua_config();
	xray::strings::stream stream(g_allocator);
	m_hypergraph->save(cfg_ptr->get_root()["scene"], nullptr, lnks);
	cfg_ptr->save(stream);
	stream.append("%c", 0);
	m_cfg_string = strings::duplicate(g_allocator, (pcstr)stream.get_buffer());

	for each(animation_viewer_graph_link^ lnk in lnks)
	{
		u32 source_id = safe_cast<animation_viewer_graph_node_base^>(lnk->source_node)->id;
		u32 destination_id = safe_cast<animation_viewer_graph_node_base^>(lnk->destination_node)->id;
		if(!(m_links_ids->ContainsKey(source_id)))
			m_links_ids->Add(source_id, gcnew nodes_ids_type());

		m_links_ids[source_id]->Add(destination_id);
	}
}

animation_viewer_remove_links::~animation_viewer_remove_links()
{
	delete m_links_ids;
	FREE(m_cfg_string);
}

bool animation_viewer_remove_links::commit()
{
	for each(System::Collections::Generic::KeyValuePair<u32, nodes_ids_type^>^ pr in m_links_ids)
	{
		for each(u32 id2 in pr->Value)
		{
			animation_viewer_graph_link^ lnk = animation_viewer_hypergraph::find_link_by_ids(m_hypergraph->links_manager->visible_links(), pr->Key, id2);
			m_hypergraph->links_manager->visible_links()->Remove(lnk);
			delete lnk;
		}
	}

	m_hypergraph->Invalidate(false);
	if(m_hypergraph->get_owner()->target_locked)
		m_hypergraph->get_editor()->change_locked();

	return true;
}

void animation_viewer_remove_links::rollback()
{
	configs::lua_config_ptr config_ptr = configs::create_lua_config_from_string(m_cfg_string);

	if(!config_ptr || config_ptr->empty()) 
		return;

	configs::lua_config_value::iterator links_it = config_ptr->get_root()["scene"]["links"].begin();
	for(; links_it!=config_ptr->get_root()["scene"]["links"].end(); ++links_it)
	{
		animation_viewer_graph_node_base^ n1 = animation_viewer_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), (*links_it)["src_id"]); 
		animation_viewer_graph_node_base^ n2 = animation_viewer_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), (*links_it)["dst_id"]); 
		R_ASSERT(n1&&n2);
		animation_viewer_graph_link^ lnk = animation_viewer_hypergraph::create_link(n1, n2); 
		lnk->load(*links_it);
		m_hypergraph->links_manager->visible_links()->Add(lnk);
	}

	m_hypergraph->display_rectangle = m_hypergraph->display_rectangle;
	if(m_hypergraph->get_owner()->target_locked)
		m_hypergraph->get_editor()->change_locked();
}