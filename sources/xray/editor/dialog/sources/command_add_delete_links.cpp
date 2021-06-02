////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2009
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "command_add_delete_links.h"
#include "dialog_hypergraph.h"
#include "dialog_links_manager.h"
#include "dialog_graph_link.h"
#include "dialog_link_precondition.h"
#include "dialog_link_action.h"
#include "dialog_operation.h"
#include "dialog_node_base.h"
#include "dialog_link.h"
#include "dialog_graph_node_base.h"
#include <xray/strings_stream.h>

using xray::dialog_editor::command_add_links;
using xray::dialog_editor::command_remove_links;
using xray::dialog_editor::command_set_actions_and_preconditions;
using xray::dialog_editor::command_change_links_connection_point;
using xray::dialog_editor::dialog_hypergraph;
using xray::dialog_editor::dialog_links_manager;
using xray::editor::controls::hypergraph::link;
using xray::editor::controls::hypergraph::connection_point;
using xray::dialog_editor::dialog_graph_link;
using xray::dialog_editor::dialog_link_action;
using xray::dialog_editor::dialog_link_precondition;
////////////////////////////////////////////////////////////////////////////
//- class command_add_links -----------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
command_add_links::command_add_links(dialog_hypergraph^ h, links^ lnks)
:m_hypergraph(h)
{
	m_links_ids = gcnew links_ids_type();
	configs::lua_config_ptr cfg_ptr = configs::create_lua_config();
	configs::lua_config_value cfg = cfg_ptr->get_root();
	xray::strings::stream stream(g_allocator);

	u32 links_counter = 0;
	for each(dialog_graph_link^ lnk in lnks)
	{
		u32 source_id = safe_cast<dialog_graph_node_base^>(lnk->source_node)->id;
		u32 destination_id = safe_cast<dialog_graph_node_base^>(lnk->destination_node)->id;
		xray::configs::lua_config_value link_cfg = cfg["dialog"]["links"][links_counter];
		link_cfg["src_id"] = source_id;
		link_cfg["dst_id"] = destination_id;
		lnk->m_link->save(link_cfg);
		if(!(m_links_ids->ContainsKey(source_id)))
			m_links_ids->Add(source_id, gcnew nodes_ids_type());

		m_links_ids[source_id]->Add(destination_id);
		++links_counter;
	}

	cfg_ptr->save(stream);
	stream.append("%c", 0);
	m_cfg_string = strings::duplicate(g_allocator, (pcstr)stream.get_buffer());
}

command_add_links::~command_add_links()
{
	delete m_links_ids;
	FREE(m_cfg_string);
}

bool command_add_links::commit()
{
	configs::lua_config_ptr config_ptr = configs::create_lua_config_from_string(m_cfg_string);

	if(!config_ptr || config_ptr->empty()) 
		return false;

	configs::lua_config_value::iterator links_it = config_ptr->get_root()["dialog"]["links"].begin();
	for(; links_it!=config_ptr->get_root()["dialog"]["links"].end(); ++links_it)
	{
		dialog_graph_node_base^ n1 = dialog_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), (*links_it)["src_id"]); 
		dialog_graph_node_base^ n2 = dialog_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), (*links_it)["dst_id"]); 
		if(n1 && n2)
		{
			xray::editor::controls::hypergraph::connection_point^ src_pt = n1->get_connection_point("child");
			xray::editor::controls::hypergraph::connection_point^ dst_pt = n2->get_connection_point("parent");
			if(src_pt==nullptr)
				src_pt = n1->add_connection_point(dialog_graph_node_base::typeid, "child");

			if(dst_pt==nullptr)
				dst_pt = n2->add_connection_point(dialog_graph_node_base::typeid, "parent");

			src_pt->draw_text = false;
			dst_pt->draw_text = false;
			dialog_link* cur_dlg_link = NULL;
			const links_list* lst = n1->object->get_links();
			for(dialog_link* lnk = lst->front(); lnk!=NULL; lnk = lst->get_next_of_object(lnk))
			{
				if(lnk->child()->id()==n2->object->id())
				{
					cur_dlg_link = lnk;
					break;
				}
			}

			if(cur_dlg_link == NULL)
			{
				cur_dlg_link = n1->object->new_link(n2->object);
				cur_dlg_link->load(*links_it);
			}

			dialog_graph_link^ new_link = gcnew dialog_graph_link(src_pt, dst_pt, false);
			new_link->m_link = cur_dlg_link;
			m_hypergraph->links_manager->visible_links()->Add(new_link);
		}		
	}

	m_hypergraph->display_rectangle = m_hypergraph->display_rectangle;
	return true;
}

void command_add_links::rollback()
{
	for each(System::Collections::Generic::KeyValuePair<u32, nodes_ids_type^>^ pr in m_links_ids)
	{
		for each(u32 id2 in pr->Value)
		{
			dialog_graph_link^ lnk = dialog_hypergraph::find_link_by_ids(m_hypergraph->links_manager->visible_links(), pr->Key, id2);
			m_hypergraph->links_manager->visible_links()->Remove(lnk);
			delete lnk;
		}
	}

	m_hypergraph->Invalidate(false);
}
////////////////////////////////////////////////////////////////////////////
//- class command_remove_links --------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
command_remove_links::command_remove_links(dialog_hypergraph^ h, links^ lnks)
:m_hypergraph(h)
{
	m_links_ids = gcnew links_ids_type();
	configs::lua_config_ptr cfg_ptr = configs::create_lua_config();
	configs::lua_config_value cfg = cfg_ptr->get_root();
	xray::strings::stream stream(g_allocator);

	u32 links_counter = 0;
	for each(dialog_graph_link^ lnk in lnks)
	{
		u32 source_id = safe_cast<dialog_graph_node_base^>(lnk->source_node)->id;
		u32 destination_id = safe_cast<dialog_graph_node_base^>(lnk->destination_node)->id;
		xray::configs::lua_config_value link_cfg = cfg["dialog"]["links"][links_counter];
		link_cfg["src_id"] = source_id;
		link_cfg["dst_id"] = destination_id;
		lnk->m_link->save(link_cfg);
		if(!(m_links_ids->ContainsKey(source_id)))
			m_links_ids->Add(source_id, gcnew nodes_ids_type());

		m_links_ids[source_id]->Add(destination_id);
		++links_counter;
	}

	cfg_ptr->save(stream);
	stream.append("%c", 0);
	m_cfg_string = strings::duplicate(g_allocator, (pcstr)stream.get_buffer());
}

command_remove_links::~command_remove_links()
{
	delete m_links_ids;
	FREE(m_cfg_string);
}

bool command_remove_links::commit()
{
	for each(System::Collections::Generic::KeyValuePair<u32, nodes_ids_type^>^ pr in m_links_ids)
	{
		for each(u32 id2 in pr->Value)
		{
			dialog_graph_link^ lnk = dialog_hypergraph::find_link_by_ids(m_hypergraph->links_manager->visible_links(), pr->Key, id2);
			m_hypergraph->links_manager->visible_links()->Remove(lnk);
			delete lnk;
		}
	}

	m_hypergraph->Invalidate(false);
	return true;
}

void command_remove_links::rollback()
{
	configs::lua_config_ptr config_ptr = configs::create_lua_config_from_string(m_cfg_string);

	if(!config_ptr || config_ptr->empty()) 
		return;

	configs::lua_config_value::iterator links_it = config_ptr->get_root()["dialog"]["links"].begin();
	for(; links_it!=config_ptr->get_root()["dialog"]["links"].end(); ++links_it)
	{
		dialog_graph_node_base^ n1 = dialog_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), (*links_it)["src_id"]); 
		dialog_graph_node_base^ n2 = dialog_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), (*links_it)["dst_id"]); 
		if(n1 && n2)
		{
			xray::editor::controls::hypergraph::connection_point^ src_pt = n1->get_connection_point("child");
			xray::editor::controls::hypergraph::connection_point^ dst_pt = n2->get_connection_point("parent");
			if(src_pt==nullptr)
				src_pt = n1->add_connection_point(dialog_graph_node_base::typeid, "child");

			if(dst_pt==nullptr)
				dst_pt = n2->add_connection_point(dialog_graph_node_base::typeid, "parent");

			src_pt->draw_text = false;
			dst_pt->draw_text = false;
			dialog_link* cur_dlg_link = NULL;
			const links_list* lst = n1->object->get_links();
			for(dialog_link* lnk = lst->front(); lnk!=NULL; lnk = lst->get_next_of_object(lnk))
			{
				if(lnk->child()->id()==n2->object->id())
				{
					cur_dlg_link = lnk;
					break;
				}
			}

			if(cur_dlg_link == NULL)
			{
				cur_dlg_link = n1->object->new_link(n2->object);
				cur_dlg_link->load(*links_it);
			}

			dialog_graph_link^ new_link = gcnew dialog_graph_link(src_pt, dst_pt, false);
			new_link->m_link = cur_dlg_link;
			m_hypergraph->links_manager->visible_links()->Add(new_link);
		}		
	}

	m_hypergraph->display_rectangle = m_hypergraph->display_rectangle;
}
////////////////////////////////////////////////////////////////////////////
//- class command_set_actions_and_preconditions ---------------------------------------------
////////////////////////////////////////////////////////////////////////////
xray::dialog_editor::dialog_expression* make_preconditions_from_nodes(dialog_link_precondition^ prec)
{
	xray::dialog_editor::dialog_expression* root = prec->expression_copy();
	for each(dialog_link_precondition^ chld in prec->Nodes)
	{
		xray::dialog_editor::dialog_operation* oper = dynamic_cast<xray::dialog_editor::dialog_operation*>(root);
		oper->add_child(make_preconditions_from_nodes(chld));
	}
	return root;
}

command_set_actions_and_preconditions::command_set_actions_and_preconditions(dialog_hypergraph^ h, dialog_graph_link^ lnk, System::Collections::Generic::List<dialog_link_action^ >^ acts, dialog_link_precondition^ prec_root)
:m_hypergraph(h)
{
	configs::lua_config_ptr cfg_ptr = configs::create_lua_config();
	configs::lua_config_value cfg = cfg_ptr->get_root();
	xray::strings::stream stream(g_allocator);
	
	u32 src_id = safe_cast<dialog_graph_node_base^>(lnk->source_node)->id;
	u32 dst_id = safe_cast<dialog_graph_node_base^>(lnk->destination_node)->id;
	cfg["old_link"]["src_id"] = src_id;
	cfg["old_link"]["dst_id"] = dst_id;
	cfg["new_link"]["src_id"] = src_id;
	cfg["new_link"]["dst_id"] = dst_id;
	lnk->m_link->save(cfg["old_link"]);

	xray::configs::lua_config_value precond_table = cfg["new_link"]["preconditions"];
	if(prec_root!=nullptr)
	{
		dialog_expression* expr = make_preconditions_from_nodes(prec_root);
		expr->save(cfg["new_link"]["preconditions"]);
		expr->destroy();
	}

	xray::configs::lua_config_value action_table = cfg["new_link"]["actions"];
	int i = 0;
	for each(dialog_link_action^ act in acts)
	{
		xray::configs::lua_config_value cur_action = action_table[i];
		dialog_action* action = act->action_copy();
		action->save(cur_action);
		DELETE(action);
		++i;
	}

	cfg_ptr->save(stream);
	stream.append("%c", 0);
	m_cfg_string = strings::duplicate(g_allocator, (pcstr)stream.get_buffer());
}

command_set_actions_and_preconditions::~command_set_actions_and_preconditions()
{
	FREE(m_cfg_string);
}

bool command_set_actions_and_preconditions::commit()
{
	configs::lua_config_ptr config_ptr = configs::create_lua_config_from_string(m_cfg_string);

	if(!config_ptr || config_ptr->empty()) 
		return false;

	configs::lua_config_value new_link_cfg = config_ptr->get_root()["new_link"];
	u32 src_id = new_link_cfg["src_id"];
	u32 dst_id = new_link_cfg["dst_id"];

	dialog_graph_link^ lnk = dialog_hypergraph::find_link_by_ids(m_hypergraph->links_manager->visible_links(), src_id, dst_id);
	R_ASSERT(lnk!=nullptr);

	vector<dialog_action*>::iterator it = lnk->m_link->actions()->begin();
	for(; it!=lnk->m_link->actions()->end(); ++it)
		DELETE(*it);

	lnk->m_link->actions()->clear();

	if(lnk->m_link->root_precondition()!=NULL)
	{
		lnk->m_link->root_precondition()->destroy();
		lnk->m_link->set_root_precondition(NULL);
	}

	lnk->load(new_link_cfg);
	return true;
}

void command_set_actions_and_preconditions::rollback()
{
	configs::lua_config_ptr config_ptr = configs::create_lua_config_from_string(m_cfg_string);

	if(!config_ptr || config_ptr->empty()) 
		return;

	configs::lua_config_value old_link_cfg = config_ptr->get_root()["old_link"];
	u32 src_id = old_link_cfg["src_id"];
	u32 dst_id = old_link_cfg["dst_id"];

	dialog_graph_link^ lnk = dialog_hypergraph::find_link_by_ids(m_hypergraph->links_manager->visible_links(), src_id, dst_id);
	R_ASSERT(lnk!=nullptr);

	vector<dialog_action*>::iterator it = lnk->m_link->actions()->begin();
	for(; it!=lnk->m_link->actions()->end(); ++it)
		DELETE(*it);

	lnk->m_link->actions()->clear();

	if(lnk->m_link->root_precondition()!=NULL)
	{
		lnk->m_link->root_precondition()->destroy();
		lnk->m_link->set_root_precondition(NULL);
	}

	lnk->load(old_link_cfg);
}

////////////////////////////////////////////////////////////////////////////
//- class command_change_links_connection_point ---------------------------------------------
////////////////////////////////////////////////////////////////////////////
command_change_links_connection_point::command_change_links_connection_point(dialog_hypergraph^ h, links^ lnks, xray::editor::controls::hypergraph::node^ n, bool dest)
:m_hypergraph(h),
m_destination(dest)
{
	m_new_links_ids = gcnew links_ids_type();
	m_old_links_ids = gcnew links_ids_type();

	m_new_node_id = safe_cast<dialog_graph_node_base^>(n)->id;
	if(dest)
		m_old_node_id = safe_cast<dialog_graph_node_base^>(lnks[0]->destination_node)->id;
	else
		m_old_node_id = safe_cast<dialog_graph_node_base^>(lnks[0]->source_node)->id;


	for each(dialog_graph_link^ l in lnks)
	{
		dialog_graph_node_base^ src = safe_cast<dialog_graph_node_base^>(l->source_node);
		dialog_graph_node_base^ dst = safe_cast<dialog_graph_node_base^>(l->destination_node);
		m_old_links_ids->Add(links_ids_pair(src->id, dst->id));
		if(dest)
			m_new_links_ids->Add(links_ids_pair(src->id, m_new_node_id));
		else
			m_new_links_ids->Add(links_ids_pair(m_new_node_id, dst->id));
	}
}

command_change_links_connection_point::~command_change_links_connection_point()
{
	delete m_new_links_ids;
	delete m_old_links_ids;
}

bool command_change_links_connection_point::commit()
{
	if(m_new_node_id==m_old_node_id)
		return false;

	for each(links_ids_pair pr in m_old_links_ids)
	{
		dialog_graph_link^ lnk = dialog_hypergraph::find_link_by_ids(m_hypergraph->links_manager->visible_links(), pr.Key, pr.Value);
		dialog_graph_node_base^ new_node = dialog_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), m_new_node_id);
		R_ASSERT(lnk!=nullptr);
		if(m_destination)
		{
			connection_point^ c = new_node->get_connection_point("parent");
			if(c==nullptr)
			{
				c = new_node->add_connection_point(dialog_graph_node_base::typeid, "parent");
				new_node->Invalidate(false);
			}
			
			c->draw_text = false;
			lnk->destination_point = c;
			lnk->destination_node = new_node;
			lnk->m_link->change_child(safe_cast<dialog_graph_node_base^>(new_node)->object);
		}
		else
		{
			dialog_graph_node_base^ old_node = dialog_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), m_old_node_id);
			connection_point^ c = new_node->get_connection_point("child");
			if(c==nullptr)
			{
				c = new_node->add_connection_point(dialog_graph_node_base::typeid, "child");
				new_node->Invalidate(false);
			}

			c->draw_text = false;
			lnk->source_point = c;
			lnk->source_node = new_node;
			safe_cast<dialog_graph_node_base^>(old_node)->object->remove_link(lnk->m_link);
			safe_cast<dialog_graph_node_base^>(new_node)->object->add_link(lnk->m_link);
		}
	}

	m_hypergraph->Invalidate(false);
	return true;
}

void command_change_links_connection_point::rollback()
{
	for each(links_ids_pair pr in m_new_links_ids)
	{
		dialog_graph_link^ lnk = dialog_hypergraph::find_link_by_ids(m_hypergraph->links_manager->visible_links(), pr.Key, pr.Value);
		dialog_graph_node_base^ old_node = dialog_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), m_old_node_id);
		R_ASSERT(lnk!=nullptr);
		if(m_destination)
		{
			connection_point^ c = old_node->get_connection_point("parent");
			if(c==nullptr)
			{
				c = old_node->add_connection_point(dialog_graph_node_base::typeid, "parent");
				old_node->Invalidate(false);
			}

			c->draw_text = false;
			lnk->destination_point = c;
			lnk->destination_node = old_node;
			lnk->m_link->change_child(safe_cast<dialog_graph_node_base^>(old_node)->object);
		}
		else
		{
			dialog_graph_node_base^ new_node = dialog_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), m_new_node_id);
			connection_point^ c = old_node->get_connection_point("child");
			if(c==nullptr)
			{
				c = old_node->add_connection_point(dialog_graph_node_base::typeid, "child");
				old_node->Invalidate(false);
			}

			c->draw_text = false;
			lnk->source_point = c;
			lnk->source_node = old_node;
			safe_cast<dialog_graph_node_base^>(new_node)->object->remove_link(lnk->m_link);
			safe_cast<dialog_graph_node_base^>(old_node)->object->add_link(lnk->m_link);
		}
	}

	m_hypergraph->Invalidate(false);
}
