////////////////////////////////////////////////////////////////////////////
//	Created		: 16.03.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog.h"
#include "dialog_node_base.h"
#include "dialog_phrase.h"
#include "dialog_container.h"
#include "dialog_operation.h"
#include "dialog_precondition.h"
#include "dialog_action.h"
#include "dialogs_manager.h"

#pragma unmanaged
using xray::dialog_editor::dialog;
using xray::dialog_editor::dialog_node_base;
using xray::dialog_editor::dialog_phrase;
using xray::dialog_editor::dialog_action;

dialog::dialog(xray::configs::lua_config_value const& cfg)
{
	m_last_id = 0;
	m_phrases = phrases_list();
	m_phrases.reserve(30*sizeof(dialog_node_base*));
	if(!(cfg.value_exists("dialog") && cfg["dialog"].value_exists("nodes")))
		return;
	
	typedef associative_vector<u32, dialog_node_base*, vector> nodes_vector_type;
	nodes_vector_type vec = nodes_vector_type();
//	loading nodes	
	configs::lua_config::const_iterator i = cfg["dialog"]["nodes"].begin();
	for(; i!=cfg["dialog"]["nodes"].end(); ++i) 
	{
		u32 id = (u32)(*i)["id"];
		if((bool)(*i)["is_dialog"]==true)
		{
			dialog_container* nc = NEW(dialog_container)();
			nc->set_id(id);
			nc->set_is_root((bool)(*i)["is_root"]);
			nc->set_text((pcstr)(*i)["string_table"]);
			nc->request_dialog();
			m_phrases.push_back(nc);
			vec.insert(std::pair<u32, dialog_node_base*>(id, nc));
		}
		else
		{
			dialog_phrase* np = NEW(dialog_phrase)();
			np->set_id(id);
			np->set_string_table_id((pcstr)(*i)["string_table"]);
			np->set_is_root((bool)(*i)["is_root"]);
			m_phrases.push_back(np);
			vec.insert(std::pair<u32, dialog_node_base*>(id, np));
		}
	}
	m_last_id = vec.rbegin()->second->id() + 1;

//	loading links
	if(!(cfg.value_exists("dialog") && cfg["dialog"].value_exists("links")))
		return;

	configs::lua_config::const_iterator	il = cfg["dialog"]["links"].begin();
	for(; il!=cfg["dialog"]["links"].end(); ++il) 
	{
		nodes_vector_type::const_iterator n1 = vec.find((u32)(*il)["src_id"]);
		nodes_vector_type::const_iterator n2 = vec.find((u32)(*il)["dst_id"]);
		if(n1!=vec.end() && n2!=vec.end())
		{
			dialog_link* lnk = n1->second->new_link(n2->second, false);
			lnk->load(*il);
		}
	}
}

void delete_preconditions(xray::dialog_editor::dialog_expression* root)
{
	xray::dialog_editor::dialog_precondition* root_prec = dynamic_cast<xray::dialog_editor::dialog_precondition*>(root);
	if(root_prec)
		DELETE(root_prec);
	else
	{
		xray::dialog_editor::dialog_operation* root_oper = dynamic_cast<xray::dialog_editor::dialog_operation*>(root);
		if(root_oper)
		{
			xray::dialog_editor::vector<xray::dialog_editor::dialog_expression*>::iterator prec_vec_iter = root_oper->childs()->begin();
			for(; prec_vec_iter!=root_oper->childs()->end(); ++prec_vec_iter)
				delete_preconditions(*prec_vec_iter);
			
			DELETE(root_oper);
		}
	}
}

dialog::~dialog()
{
	phrases_list::iterator i = m_phrases.begin();
	for(; i!=m_phrases.end(); ++i)
	{
		const links_list* links_lst = (*i)->get_links();
		dialog_link* cur_lnk = links_lst->front();
		while(cur_lnk!=NULL)
		{
			vector<dialog_action*>::iterator actions_iter = cur_lnk->actions()->begin();
			for(; actions_iter!=cur_lnk->actions()->end(); ++actions_iter)
				DELETE(*actions_iter);

			if(cur_lnk->root_precondition()!=NULL)
				delete_preconditions(cur_lnk->root_precondition());

			dialog_link* tmp_lnk = links_lst->get_next_of_object(cur_lnk);
			DELETE(cur_lnk);
			cur_lnk = tmp_lnk;
		}

		DELETE(*i);
	}
	m_phrases.clear();
	FREE(m_name);
}

void dialog::save(xray::configs::lua_config_value cfg)
{
//	saving nodes
	phrases_list::const_iterator i = m_phrases.begin();
	for(; i!=m_phrases.end(); ++i)
	{
		u32 id = (*i)->id();
		xray::configs::lua_config_value val = cfg["dialog"]["nodes"][id];
		val["id"] = id;
		val["is_root"] = (*i)->is_root();
		dialog_phrase* phrase = dynamic_cast<dialog_phrase*>(*i);
		if(phrase)
		{
			val["is_dialog"] = false;
			val["string_table"] = phrase->string_table_id();
		}
		else
		{
			val["is_dialog"] = true;
			val["string_table"] = (*i)->text();
		}
	}
//	saving links
	u32 link_id = 0;
	i = m_phrases.begin();
	for(; i!=m_phrases.end(); ++i)
	{
		const links_list* lnks = (*i)->get_links();
		dialog_link* cur_lnk = lnks->front();
		while(cur_lnk!=NULL)
		{
//			dialog_node_base* p = get_node(cur_lnk->child()->id());
			phrases_list::const_iterator p = std::find(m_phrases.begin(), m_phrases.end(), cur_lnk->child());
			if(p!=m_phrases.end())
			{
				xray::configs::lua_config_value val = cfg["dialog"]["links"][link_id];
				val["src_id"] = (*i)->id();
				val["dst_id"] = (*p)->id();
				cur_lnk->save(val);
			}
			cur_lnk = lnks->get_next_of_object(cur_lnk);
			++link_id;
		}
	}
}

void dialog::remove_node(xray::dialog_editor::dialog_node_base* n)
{
	m_phrases.erase(std::remove(m_phrases.begin(), m_phrases.end(), n), m_phrases.end());
}

void dialog::add_node(xray::dialog_editor::dialog_node_base* n)
{
	m_phrases.push_back(n);
}

xray::dialog_editor::dialog_node_base* dialog::get_node(u32 id)
{
	phrases_list::const_iterator it = m_phrases.begin();
	for(; it!=m_phrases.end(); ++it)
	{
		if((*it)->id()==id)
			return (*it);
	}

	return NULL;
}

void dialog::set_name(pcstr new_name)
{
	m_name = strings::duplicate(g_allocator, new_name);
}

pcstr dialog::name()
{
	return m_name;
}

void dialog::clear()
{
	m_phrases.clear();
}
