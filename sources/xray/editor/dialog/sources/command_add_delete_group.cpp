////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2009
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "command_add_delete_group.h"
#include "dialog_graph_node_phrase.h"
#include "dialog_graph_node_dialog.h"
#include "dialog_graph_node_layout.h"
#include "dialog.h"
#include "dialog_phrase.h"
#include "dialog_hypergraph.h"
#include "dialog_document.h"
#include "dialog_editor.h"
#include "dialog_graph_link.h"
#include <xray/strings_stream.h>

using xray::dialog_editor::command_add_group;
using xray::dialog_editor::command_remove_group;
using xray::dialog_editor::dialog_hypergraph;

using xray::dialog_editor::dialog_phrase;
using xray::dialog_editor::dialog_container;
////////////////////////////////////////////////////////////////////////////
//- class command_add_group -----------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
command_add_group::command_add_group(dialog_hypergraph^ h, 
									 pcstr config_string, 
									 System::Drawing::Point offset, 
									 xray::editor::controls::hypergraph::node^ n)
:m_hypergraph(h),
m_offset(offset)
{
	m_cfg_string = strings::duplicate(g_allocator, config_string);
	m_nodes_ids = gcnew ids_type();
	m_links_ids = gcnew links_ids_type();
	m_string_ids = gcnew string_ids_type();
	if(n!=nullptr)
		m_node_to_connect_id = safe_cast<dialog_graph_node_base^>(n)->id;
	else
		m_node_to_connect_id = u32(-1);

	m_first_run = true;
}

command_add_group::~command_add_group()
{
	delete m_nodes_ids;
	delete m_links_ids;
	delete m_string_ids;
	FREE(m_cfg_string);
}

bool command_add_group::commit()
{
	configs::lua_config_ptr config_ptr = configs::create_lua_config_from_string(m_cfg_string);

	if(!config_ptr || config_ptr->empty()) 
		return false;

	dialog* tmp_dialog = NEW(dialog)(config_ptr->get_root());
	tmp_dialog->set_name("tmp_dialog");
	dialog_graph_node_layout* tmp_layout = NEW(dialog_graph_node_layout)(config_ptr->get_root());

	dialog* parent_dialog = m_hypergraph->get_owner()->get_dialog();
	m_hypergraph->set_selected_node((xray::editor::controls::hypergraph::node^)nullptr);
	nodes^ nds = gcnew nodes();
	const vector<dialog_node_base*>* nodes_vec = tmp_dialog->get_phrases();
	vector<dialog_node_base*>::const_iterator i = nodes_vec->begin();
	for(; i!=nodes_vec->end(); ++i)
	{
		if(dynamic_cast<dialog_phrase*>(*i))
		{
			dialog_graph_node_phrase^ n = gcnew dialog_graph_node_phrase();
			n->object = *i;
			dialog_graph_node_layout::layout_vector::const_iterator it_pos = tmp_layout->m_layout.find(n->id);
			System::Drawing::Point p = System::Drawing::Point(it_pos->second.x, it_pos->second.y);
			init_node(n, p);
			if(m_first_run)
			{
				u32 old_id = n->id;
				if(parent_dialog->get_node((*i)->id())!=NULL)
					(*i)->set_id(parent_dialog->m_last_id);

				++(parent_dialog->m_last_id);
				n->on_added(m_hypergraph);
				m_nodes_ids[old_id] = n->id;

				if(n->string_table=="")
					m_hypergraph->get_owner()->generate_node_string_table_id(n);

				m_string_ids->Add(n->id, n->string_table);
			}
			else
			{
				(*i)->set_id(m_nodes_ids[(*i)->id()]);
				n->string_table = m_string_ids[n->id];
				n->on_added(m_hypergraph);
			}

			m_hypergraph->get_owner()->get_editor()->change_references_count(n->string_table, nullptr);
			nds->Add(n);
		}
		else
		{
			dialog_graph_node_dialog^ n = gcnew dialog_graph_node_dialog();
			n->object = *i;
			dialog_graph_node_layout::layout_vector::const_iterator it_pos = tmp_layout->m_layout.find(n->id);
			System::Drawing::Point p = System::Drawing::Point(it_pos->second.x, it_pos->second.y);
			init_node(n, p);
			if(m_first_run)
			{
				u32 old_id = n->id;
				if(parent_dialog->get_node((*i)->id())!=NULL)
					(*i)->set_id(parent_dialog->m_last_id);

				++(parent_dialog->m_last_id);
				n->on_added(m_hypergraph);
				m_nodes_ids[old_id] = n->id;
			}
			else
			{
				(*i)->set_id(m_nodes_ids[(*i)->id()]);
				n->on_added(m_hypergraph);
			}
			nds->Add(n);
		}
		parent_dialog->add_node(*i);
	}

	m_hypergraph->add_selected_node(nds);

	dialog_graph_node_base^ node_to_connect = dialog_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), m_node_to_connect_id);
	i = nodes_vec->begin();
	for(; i!=nodes_vec->end(); ++i)
	{
		dialog_graph_node_base^ n1 = dialog_hypergraph::find_node_by_id(nds, (*i)->id()); 
		R_ASSERT(n1!=nullptr);
		for(dialog_link* cur_dlg_link = (*i)->get_links()->front(); cur_dlg_link!=NULL; cur_dlg_link=(*i)->get_links()->get_next_of_object(cur_dlg_link))
		{
			dialog_graph_node_base^ n2 = dialog_hypergraph::find_node_by_id(nds, cur_dlg_link->child()->id()); 
			R_ASSERT(n2!=nullptr);
			xray::editor::controls::hypergraph::connection_point^ src_pt = n1->get_connection_point("child");
			xray::editor::controls::hypergraph::connection_point^ dst_pt = n2->get_connection_point("parent");
			if(src_pt==nullptr)
				src_pt = n1->add_connection_point(dialog_graph_node_base::typeid, "child");

			if(dst_pt==nullptr)
				dst_pt = n2->add_connection_point(dialog_graph_node_base::typeid, "parent");

			src_pt->draw_text = false;
			dst_pt->draw_text = false;
			dialog_graph_link^ new_link = gcnew dialog_graph_link(src_pt, dst_pt, false);
			new_link->m_link = cur_dlg_link;
			m_hypergraph->links_manager->visible_links()->Add(new_link);
			if(m_first_run)
			{
				if(!(m_links_ids->ContainsKey(n1->id)))
					m_links_ids->Add(n1->id, gcnew nodes_ids_type());

				m_links_ids[n1->id]->Add(n2->id);
			}
		}

		if(node_to_connect!=nullptr && n1->is_root)
		{
			dialog_graph_link^ new_link = m_hypergraph->get_owner()->create_connection(node_to_connect, n1, true);
			m_hypergraph->links_manager->visible_links()->Add(new_link);
			if(m_first_run)
			{
				if(!(m_links_ids->ContainsKey(node_to_connect->id)))
					m_links_ids->Add(node_to_connect->id, gcnew nodes_ids_type());

				m_links_ids[node_to_connect->id]->Add(n1->id);
			}
		}
	}

	m_hypergraph->display_rectangle = m_hypergraph->display_rectangle;
	m_first_run = false;
	tmp_dialog->clear();
	DELETE(tmp_dialog);
	DELETE(tmp_layout);
	return true;
}

void command_add_group::init_node(xray::dialog_editor::dialog_graph_node_base^ n, System::Drawing::Point pnt)
{
	m_hypergraph->nodes_style_manager->assign_style(n, "dialog_node");
	n->set_scale(m_hypergraph->scale);
	n->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(m_hypergraph, &dialog_hypergraph::on_key_down);
	n->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(m_hypergraph, &dialog_hypergraph::on_key_up);
	m_hypergraph->Controls->Add(n);
	m_hypergraph->all_nodes()->Add(n);
	m_hypergraph->links_manager->on_node_added(n);

	Point p = Point(m_offset.X + (int)Math::Round(pnt.X * m_hypergraph->scale), 
					m_offset.Y + (int)Math::Round(pnt.Y * m_hypergraph->scale));
	p.X = int(Math::Round(p.X/m_hypergraph->scale));
	p.Y = int(Math::Round(p.Y/m_hypergraph->scale));
	p.X += m_hypergraph->display_rectangle.X;
	p.Y += m_hypergraph->display_rectangle.Y;
	n->position = p;
}

void command_add_group::rollback()
{
	m_first_run = false;

	for each(System::Collections::Generic::KeyValuePair<u32, nodes_ids_type^>^ pr in m_links_ids)
	{
		for each(u32 id2 in pr->Value)
		{
			dialog_graph_link^ lnk = dialog_hypergraph::find_link_by_ids(m_hypergraph->links_manager->visible_links(), pr->Key, id2);
			R_ASSERT(lnk!=nullptr);
			m_hypergraph->links_manager->visible_links()->Remove(lnk);
			delete lnk;
		}
	}

	for each(System::Collections::Generic::KeyValuePair<u32, u32>^ pr in m_nodes_ids)
	{
		dialog_graph_node_base^ n = dialog_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), pr->Value);
		m_hypergraph->get_owner()->get_dialog()->remove_node(n->object);
		m_hypergraph->Controls->Remove(n);
		m_hypergraph->all_nodes()->Remove(n);
		n->on_removed(m_hypergraph);
		m_hypergraph->links_manager->on_node_removed(n);
		if(!(n->is_dialog()))
		{
			dialog_graph_node_phrase^ ph = safe_cast<dialog_graph_node_phrase^>(n);
			m_hypergraph->get_owner()->get_editor()->change_references_count(nullptr, ph->string_table);
		}
		DELETE(n->object);
		delete n;
	}

	m_hypergraph->set_selected_node((xray::editor::controls::hypergraph::node^)nullptr);
	m_hypergraph->Invalidate(false);
}
////////////////////////////////////////////////////////////////////////////
//- class command_remove_group --------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
command_remove_group::command_remove_group(dialog_hypergraph^ h, nodes^ nds, links^ lnks)
:m_hypergraph(h)
{
	m_nodes_ids = gcnew nodes_ids_type();
	m_links_ids = gcnew links_ids_type();

	configs::lua_config_ptr cfg_ptr = configs::create_lua_config();
	configs::lua_config_value cfg = cfg_ptr->get_root();
	xray::strings::stream stream(g_allocator);

	for each(dialog_graph_node_base^ n in nds)
	{
		u32 id = n->id;
		configs::lua_config_value nd_cfg = cfg["dialog"]["nodes"][id];
		nd_cfg["id"] = id;
		nd_cfg["is_root"] = n->is_root;
		nd_cfg["is_dialog"] = n->is_dialog();
		if(n->is_dialog())
			nd_cfg["string_table"] = unmanaged_string(n->get_text()).c_str();
		else
		{
			dialog_graph_node_phrase^ ph = safe_cast<dialog_graph_node_phrase^>(n);
			nd_cfg["string_table"] = unmanaged_string(ph->string_table).c_str();
		}

		m_nodes_ids->Add(id);
		configs::lua_config_value lt_cfg = cfg["layout"][id];
		lt_cfg["id"] = id;
		lt_cfg["x"] = n->position.X;
		lt_cfg["y"] = n->position.Y;
	}

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

command_remove_group::~command_remove_group()
{
	delete m_nodes_ids;
	delete m_links_ids;
	FREE(m_cfg_string);
}

bool command_remove_group::commit()
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

	for each(u32 node_id in m_nodes_ids)
	{
		dialog_graph_node_base^ n = dialog_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), node_id);
		m_hypergraph->get_owner()->get_dialog()->remove_node(n->object);
		m_hypergraph->Controls->Remove(n);
		m_hypergraph->all_nodes()->Remove(n);
		n->on_removed(m_hypergraph);
		m_hypergraph->links_manager->on_node_removed(n);
		if(!(n->is_dialog()))
		{
			dialog_graph_node_phrase^ ph = safe_cast<dialog_graph_node_phrase^>(n);
			m_hypergraph->get_owner()->get_editor()->change_references_count(nullptr, ph->string_table);
		}
		DELETE(n->object);
		delete n;
	}

	m_hypergraph->set_selected_node((xray::editor::controls::hypergraph::node^)nullptr);
	m_hypergraph->Invalidate(false);
	return true;
}

void command_remove_group::rollback()
{
	configs::lua_config_ptr config_ptr = configs::create_lua_config_from_string(m_cfg_string);

	if(!config_ptr || config_ptr->empty()) 
		return;

	dialog* tmp_dialog = NEW(dialog)(config_ptr->get_root());
	tmp_dialog->set_name("tmp_dialog");
	dialog_graph_node_layout* tmp_layout = NEW(dialog_graph_node_layout)(config_ptr->get_root());

	const vector<dialog_node_base*>* nodes_vec = tmp_dialog->get_phrases();
	vector<dialog_node_base*>::const_iterator i = nodes_vec->begin();
	for(; i!=nodes_vec->end(); ++i)
	{
		if(dynamic_cast<dialog_phrase*>(*i))
		{
			dialog_graph_node_phrase^ n = gcnew dialog_graph_node_phrase();
			n->object = *i;

			m_hypergraph->nodes_style_manager->assign_style(n, "dialog_node");
			m_hypergraph->Controls->Add(n);
			m_hypergraph->all_nodes()->Add(n);
			n->on_added(m_hypergraph);
			m_hypergraph->links_manager->on_node_added(n);
			m_hypergraph->get_owner()->get_editor()->change_references_count(n->string_table, nullptr);

			n->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(m_hypergraph, &dialog_hypergraph::on_key_down);
			n->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(m_hypergraph, &dialog_hypergraph::on_key_up);

			dialog_graph_node_layout::layout_vector::const_iterator it_pos = tmp_layout->m_layout.find(n->id);
			n->position = System::Drawing::Point(it_pos->second.x, it_pos->second.y);
			n->set_scale(m_hypergraph->scale);
		}
		else
		{
			dialog_graph_node_dialog^ n = gcnew dialog_graph_node_dialog();
			n->object = *i;

			m_hypergraph->nodes_style_manager->assign_style(n, "dialog_node");
			m_hypergraph->Controls->Add(n);
			m_hypergraph->all_nodes()->Add(n);
			n->on_added(m_hypergraph);
			m_hypergraph->links_manager->on_node_added(n);

			n->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(m_hypergraph, &dialog_hypergraph::on_key_down);
			n->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(m_hypergraph, &dialog_hypergraph::on_key_up);

			dialog_graph_node_layout::layout_vector::const_iterator it_pos = tmp_layout->m_layout.find(n->id);
			n->position = System::Drawing::Point(it_pos->second.x, it_pos->second.y);
			n->set_scale(m_hypergraph->scale);
		}
		m_hypergraph->get_owner()->get_dialog()->add_node(*i);
	}

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
	m_first_run = false;
	tmp_dialog->clear();
	DELETE(tmp_dialog);
	DELETE(tmp_layout);
}