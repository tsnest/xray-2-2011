////////////////////////////////////////////////////////////////////////////
//	Created		: 10.09.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_viewer_add_delete_group.h"
#include "animation_viewer_graph_node_base.h"
#include "animation_viewer_graph_node_animation.h"
#include "animation_viewer_graph_node_operator.h"
#include "animation_viewer_graph_node_root.h"
#include "animation_viewer_hypergraph.h"
#include "animation_viewer_graph_link.h"
#include "animation_viewer_graph_link_weighted.h"
#include "animation_viewer_graph_link_timed.h"
#include "time_instance.h"
#include "viewer_editor.h"
#include "viewer_document.h"
#include <xray/strings_stream.h>

using xray::animation_editor::animation_viewer_add_group;
using xray::animation_editor::animation_viewer_remove_group;
using xray::animation_editor::animation_viewer_hypergraph;
////////////////////////////////////////////////////////////////////////////
//- class animation_viewer_add_group -----------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
animation_viewer_add_group::animation_viewer_add_group(animation_viewer_hypergraph^ h, 
									 pcstr config_string, 
									 System::Drawing::Point offset)
:m_hypergraph(h),
m_offset(offset)
{
	m_cfg_string = strings::duplicate(g_allocator, config_string);
	m_nodes_ids = gcnew ids_type();
	m_links_ids = gcnew links_ids_type();
	m_first_run = true;
}

animation_viewer_add_group::~animation_viewer_add_group()
{
	delete m_nodes_ids;
	delete m_links_ids;
	FREE(m_cfg_string);
}

bool animation_viewer_add_group::commit()
{
	configs::lua_config_ptr config_ptr = configs::create_lua_config_from_string(m_cfg_string);

	if(!config_ptr || config_ptr->empty()) 
		return false;

	nodes^ nds = gcnew nodes();
	xray::configs::lua_config_iterator nb = config_ptr->get_root()["scene"]["nodes"].begin();
	xray::configs::lua_config_iterator ne = config_ptr->get_root()["scene"]["nodes"].end();
	for(; nb!=ne; ++nb)
	{
		animation_viewer_graph_node_base^ n = nullptr;
		u32 type = (u32)(*nb)["type"];
		if(type==0)
		{
			n = gcnew animation_viewer_graph_node_root("TEMP");
			m_hypergraph->nodes_style_manager->assign_style(n, "root_node");
		}
		else if(type==1)
		{
			n = gcnew animation_viewer_graph_node_animation();
			m_hypergraph->nodes_style_manager->assign_style(n, "animation_node");
		}
		else if(type==2)
		{
			n = gcnew animation_viewer_graph_node_operator();
			m_hypergraph->nodes_style_manager->assign_style(n, "operation_node");
		}
		else
			NODEFAULT();

		n->set_scale(m_hypergraph->scale);
		n->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(m_hypergraph, &animation_viewer_hypergraph::on_key_down);
		n->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(m_hypergraph, &animation_viewer_hypergraph::on_key_up);
		n->load(*nb, m_hypergraph);
		if(m_first_run)
		{
			u32 old_id = n->id;
			u32 new_id = m_hypergraph->assign_node_id(n, u32(-1));
			m_nodes_ids->Add(old_id, new_id);
		}
		else
			m_hypergraph->assign_node_id(n, m_nodes_ids[n->id]);

		m_hypergraph->all_nodes()->Add(n);
		m_hypergraph->links_manager->on_node_added(n);
		m_hypergraph->Controls->Add(n);
		n->on_added(m_hypergraph);

		Point p = Point(m_offset.X + (int)Math::Round((n->position.X - n->size.Width/2) * m_hypergraph->scale), 
						m_offset.Y + (int)Math::Round((n->position.Y - n->size.Height/2) * m_hypergraph->scale));
		p.X = int(Math::Round(p.X/m_hypergraph->scale));
		p.Y = int(Math::Round(p.Y/m_hypergraph->scale));
		p.X += m_hypergraph->display_rectangle.X;
		p.Y += m_hypergraph->display_rectangle.Y;
		n->position = p;
		if(type==0 && (*nb).value_exists("playing_time") && nds->Count>0)
		{
			animation_viewer_graph_node_base^ n2 = safe_cast<animation_viewer_graph_node_base^>(nds[nds->Count-1]);
			animation_viewer_graph_link^ l = animation_viewer_hypergraph::create_link(n2, n); 
			animation_viewer_graph_link_timed^ lnk = dynamic_cast<animation_viewer_graph_link_timed^>(l); 
			time_instance^ ti = dynamic_cast<time_instance^>(lnk->playing_time->owner);
			ti->time_s = (float)(*nb)["playing_time"];
			m_hypergraph->links_manager->visible_links()->Add(lnk);
		}

		nds->Add(n);
	}

	m_hypergraph->set_selected_node(nds);

	xray::configs::lua_config_iterator lb = config_ptr->get_root()["scene"]["links"].begin();
	xray::configs::lua_config_iterator le = config_ptr->get_root()["scene"]["links"].end();
	for(; lb!=le; ++lb)
	{
		animation_viewer_graph_node_base^ n1 = animation_viewer_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), m_nodes_ids[(u32)(*lb)["src_id"]]); 
		animation_viewer_graph_node_base^ n2 = animation_viewer_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), m_nodes_ids[(u32)(*lb)["dst_id"]]); 
		animation_viewer_graph_link^ lnk = animation_viewer_hypergraph::create_link(n1, n2); 
		lnk->load(*lb);
		m_hypergraph->links_manager->visible_links()->Add(lnk);
		if(m_first_run)
		{
			if(!(m_links_ids->ContainsKey(n1->id)))
				m_links_ids->Add(n1->id, gcnew nodes_ids_type());

			m_links_ids[n1->id]->Add(n2->id);
		}
	}

	m_hypergraph->display_rectangle = m_hypergraph->display_rectangle;
	m_first_run = false;
	if(m_hypergraph->get_owner()->target_locked)
		m_hypergraph->get_editor()->change_locked();

	return true;
}

void animation_viewer_add_group::rollback()
{
	m_first_run = false;

	for each(System::Collections::Generic::KeyValuePair<u32, nodes_ids_type^>^ pr in m_links_ids)
	{
		for each(u32 id2 in pr->Value)
		{
			animation_viewer_graph_link^ lnk = animation_viewer_hypergraph::find_link_by_ids(m_hypergraph->links_manager->visible_links(), pr->Key, id2);
			R_ASSERT(lnk!=nullptr);
			m_hypergraph->links_manager->visible_links()->Remove(lnk);
			delete lnk;
		}
	}

	for each(System::Collections::Generic::KeyValuePair<u32, u32>^ pr in m_nodes_ids)
	{
		animation_viewer_graph_node_base^ n = animation_viewer_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), pr->Value);
		m_hypergraph->remove_node_id(n);
		m_hypergraph->Controls->Remove(n);
		m_hypergraph->all_nodes()->Remove(n);
		n->on_removed(m_hypergraph);
		m_hypergraph->links_manager->on_node_removed(n);
		delete n;
	}

	if(m_nodes_ids->Count!=0)
		m_hypergraph->set_selected_node((xray::editor::controls::hypergraph::node^)nullptr);

	m_hypergraph->Invalidate(false);
	if(m_hypergraph->get_owner()->target_locked)
		m_hypergraph->get_editor()->change_locked();
}
////////////////////////////////////////////////////////////////////////////
//- class animation_viewer_remove_group --------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
animation_viewer_remove_group::animation_viewer_remove_group(animation_viewer_hypergraph^ h, nodes^ nds, links^ lnks)
:m_hypergraph(h)
{
	m_nodes_ids = gcnew nodes_ids_type();
	m_links_ids = gcnew links_ids_type();
	m_link_values = gcnew link_values_type();

	configs::lua_config_ptr cfg_ptr = configs::create_lua_config();
	xray::strings::stream stream(g_allocator);

	m_hypergraph->save(cfg_ptr->get_root()["scene"], nds, lnks);
	for each(animation_viewer_graph_node_base^ n in nds)
		m_nodes_ids->Add(n->id);

	for each(animation_viewer_graph_link^ lnk in lnks)
	{
		u32 source_id = safe_cast<animation_viewer_graph_node_base^>(lnk->source_node)->id;
		u32 destination_id = safe_cast<animation_viewer_graph_node_base^>(lnk->destination_node)->id;
		if(!(m_links_ids->ContainsKey(source_id)))
			m_links_ids->Add(source_id, gcnew nodes_ids_type());

		m_links_ids[source_id]->Add(destination_id);

		animation_viewer_graph_link_weighted^ w_lnk = dynamic_cast<animation_viewer_graph_link_weighted^>(lnk);
		if(w_lnk)
			init_weight_values(w_lnk, lnks);
	}

	cfg_ptr->save(stream);
	stream.append("%c", 0);
	m_cfg_string = strings::duplicate(g_allocator, (pcstr)stream.get_buffer());
}

animation_viewer_remove_group::~animation_viewer_remove_group()
{
	delete m_nodes_ids;
	delete m_links_ids;
	FREE(m_cfg_string);
}

void animation_viewer_remove_group::init_weight_values(xray::animation_editor::animation_viewer_graph_link_weighted^ initial_link, links^ deleting_links)
{
	animation_viewer_graph_node_animation^ anim = dynamic_cast<animation_viewer_graph_node_animation^>(initial_link->source_node);
	if(anim && anim->additive())
		return;

	animation_viewer_graph_node_base^ parent = safe_cast<animation_viewer_graph_node_base^>(initial_link->destination_node);
	float sum = 0.0f;
	links^ lnks = gcnew links();
	for each(animation_viewer_graph_node_base^ child in parent->childs)
	{
		if(child==initial_link->source_node)
			continue;

		animation_viewer_graph_node_animation^ child_anim = dynamic_cast<animation_viewer_graph_node_animation^>(child);
		if(!child_anim || !(child_anim->additive()))
		{
			animation_viewer_graph_link^ lnk = animation_viewer_hypergraph::find_link_by_ids(m_hypergraph->links_manager->visible_links(), child->id, parent->id);
			if(deleting_links->Contains(lnk))
				continue;

			animation_viewer_graph_link_weighted^ l = dynamic_cast<animation_viewer_graph_link_weighted^>(lnk);
			sum += l->weight;
			lnks->Add(l);
		}
	}

	for each(animation_viewer_graph_link_weighted^ l in lnks)
	{
		float new_val = 0.0f;
		if(!xray::math::is_similar(sum, 0.0f))
			new_val = l->weight + initial_link->weight*(l->weight/sum);
		else
			new_val = initial_link->weight/lnks->Count;

		new_val = (float)System::Math::Round(xray::math::clamp_r(new_val, 0.0f, 1.0f), 3);
		u32 src_id = safe_cast<animation_viewer_graph_node_base^>(l->source_node)->id;
		u32 dst_id = safe_cast<animation_viewer_graph_node_base^>(l->destination_node)->id;
		link_weight_values^ v = nullptr;
		for each(link_weight_values^ val in m_link_values)
		{
			if(val->src_id==src_id && val->dst_id==dst_id)
			{
				v = val;
				break;
			}
		}

		if(v==nullptr)
			m_link_values->Add(gcnew link_weight_values(src_id, dst_id, l->weight, new_val));
		else
			v->new_val += new_val - l->weight;
	}
}

bool animation_viewer_remove_group::commit()
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

	for each(u32 node_id in m_nodes_ids)
	{
		animation_viewer_graph_node_base^ n = animation_viewer_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), node_id);
		m_hypergraph->remove_node_id(n);
		m_hypergraph->Controls->Remove(n);
		m_hypergraph->all_nodes()->Remove(n);
		n->on_removed(m_hypergraph);
		m_hypergraph->links_manager->on_node_removed(n);
		delete n;
	}

	for each(link_weight_values^ val in m_link_values)
	{
		animation_viewer_graph_link^ lnk = animation_viewer_hypergraph::find_link_by_ids(m_hypergraph->links_manager->visible_links(), val->src_id, val->dst_id);
		animation_viewer_graph_link_weighted^ w_lnk = safe_cast<animation_viewer_graph_link_weighted^>(lnk);
		w_lnk->weight = val->new_val;
	}

	if(m_nodes_ids->Count!=0)
		m_hypergraph->set_selected_node((xray::editor::controls::hypergraph::node^)nullptr);

	m_hypergraph->Invalidate(false);
	if(m_hypergraph->get_owner()->target_locked)
		m_hypergraph->get_editor()->change_locked();

	return true;
}

void animation_viewer_remove_group::rollback()
{
	configs::lua_config_ptr config_ptr = configs::create_lua_config_from_string(m_cfg_string);

	if(!config_ptr || config_ptr->empty()) 
		return;

	xray::configs::lua_config_iterator nb = config_ptr->get_root()["scene"]["nodes"].begin();
	xray::configs::lua_config_iterator ne = config_ptr->get_root()["scene"]["nodes"].end();
	for(; nb!=ne; ++nb)
	{
		animation_viewer_graph_node_base^ n = nullptr;
		u32 type = (u32)(*nb)["type"];
		if(type==0)
		{
			n = gcnew animation_viewer_graph_node_root("TEMP");
			m_hypergraph->nodes_style_manager->assign_style(n, "root_node");
		}
		else if(type==1)
		{
			n = gcnew animation_viewer_graph_node_animation();
			m_hypergraph->nodes_style_manager->assign_style(n, "animation_node");
		}
		else if(type==2)
		{
			n = gcnew animation_viewer_graph_node_operator();
			m_hypergraph->nodes_style_manager->assign_style(n, "operation_node");
		}
		else
			NODEFAULT();

		n->set_scale(m_hypergraph->scale);
		n->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(m_hypergraph, &animation_viewer_hypergraph::on_key_down);
		n->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(m_hypergraph, &animation_viewer_hypergraph::on_key_up);
		n->load(*nb, m_hypergraph);
		m_hypergraph->assign_node_id(n, n->id);
		m_hypergraph->all_nodes()->Add(n);
		m_hypergraph->links_manager->on_node_added(n);
		m_hypergraph->Controls->Add(n);
		n->on_added(m_hypergraph);
	}

	xray::configs::lua_config_iterator lb = config_ptr->get_root()["scene"]["links"].begin();
	xray::configs::lua_config_iterator le = config_ptr->get_root()["scene"]["links"].end();
	for(; lb!=le; ++lb)
	{
		animation_viewer_graph_node_base^ n1 = animation_viewer_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), (u32)(*lb)["src_id"]); 
		animation_viewer_graph_node_base^ n2 = animation_viewer_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), (u32)(*lb)["dst_id"]); 
		R_ASSERT(n1&&n2);
		animation_viewer_graph_link^ lnk = animation_viewer_hypergraph::create_link(n1, n2); 
		lnk->load(*lb);
		m_hypergraph->links_manager->visible_links()->Add(lnk);
	}

	for each(link_weight_values^ val in m_link_values)
	{
		animation_viewer_graph_link^ lnk = animation_viewer_hypergraph::find_link_by_ids(m_hypergraph->links_manager->visible_links(), val->src_id, val->dst_id);
		animation_viewer_graph_link_weighted^ w_lnk = safe_cast<animation_viewer_graph_link_weighted^>(lnk);
		w_lnk->weight = val->old_val;
	}

	m_hypergraph->display_rectangle = m_hypergraph->display_rectangle;
	if(m_hypergraph->get_owner()->target_locked)
		m_hypergraph->get_editor()->change_locked();
}