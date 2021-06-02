////////////////////////////////////////////////////////////////////////////
//	Created		: 07.09.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_viewer_graph_node_base.h"
#include "animation_viewer_hypergraph.h"
#include "animation_viewer_graph_node_animation.h"
#include "animation_viewer_graph_node_operator.h"
#include "animation_viewer_graph_node_root.h"
#include "animation_viewer_graph_link.h"
#include "animation_viewer_graph_link_weighted.h"

using xray::animation_editor::animation_viewer_graph_node_base;
using xray::editor::controls::hypergraph::view_mode;
using xray::editor::controls::hypergraph::connection_point;
using xray::editor::controls::hypergraph::point_align;
using xray::animation_editor::animation_viewer_hypergraph;

void animation_viewer_graph_node_base::initialize()
{
	m_has_top = false;
	m_has_bottom = false;
	childs = gcnew nodes();
	parent = nullptr;
	DoubleClick += gcnew System::EventHandler(this, &animation_viewer_graph_node_base::on_double_click);
}

void animation_viewer_graph_node_base::assign_style(xray::editor::controls::hypergraph::node_style^ style)
{
	m_style	= style;
	m_size = *(style->full_size);
	recalc();
	Invalidate();
}

void animation_viewer_graph_node_base::recalc()
{
	m_has_top = false;
	m_has_bottom = false;
	if(m_style->mode==view_mode::full)
	{
		int h = m_style->full_size->Height;
		for each(connection_point^ p in m_connection_points)
		{
			if(!m_has_top && p->align==point_align::top)
			{
				h += VERTICAL_TAB_HEIGHT;
				m_has_top = true;
			}

			if(!m_has_bottom && (p->align==point_align::bottom ||
				p->align==point_align::left || p->align==point_align::right))
			{
				h += VERTICAL_TAB_HEIGHT;
				m_has_bottom = true;
			}
		}

		for each(connection_point^ p in m_connection_points)
		{
			System::Drawing::Rectangle rect = p->base_rect();
			if(p->align==point_align::right)
				rect.X = m_size.Width - rect.Width;
			else if(!(p->align==point_align::left))
				rect.X = m_size.Width/2;

			if(!(p->align==point_align::top))
			{
				if(p->align==point_align::bottom)
					rect.Y = h - 10;
				else
					rect.Y = (h - 10)/2;
			}

			p->set_rect(rect);
		}
		m_size = System::Drawing::Size(m_style->full_size->Width, h);
	}
}

void animation_viewer_graph_node_base::draw_caption(System::Windows::Forms::PaintEventArgs^  e)
{
	System::Drawing::Size sz = m_style->get_max_point_size();
	sz.Height = int(Math::Round(sz.Height*m_style->scale));
	sz.Width = int(Math::Round(sz.Width*m_style->scale));
	System::Drawing::Rectangle client = ClientRectangle;
	if(m_has_top)
	{
		client.Height -= int(Math::Round(VERTICAL_TAB_HEIGHT*m_style->scale));
		client.Y += int(Math::Round(VERTICAL_TAB_HEIGHT*m_style->scale));
	}

	if(m_has_bottom)
		client.Height -= int(Math::Round(VERTICAL_TAB_HEIGHT*m_style->scale));

	System::Drawing::Rectangle rect = Rectangle::Inflate(client, -sz.Width/2, -sz.Height/2);
	m_style->brush->Color = Drawing::Color::Black;
	m_style->border_pen->Width = 1;
	e->Graphics->DrawString(get_text(), m_style->font, m_style->brush, rect, m_style->fmt);
}

void animation_viewer_graph_node_base::draw_frame(System::Windows::Forms::PaintEventArgs^ e)
{
	super::draw_frame(e);
}

void animation_viewer_graph_node_base::on_added(xray::editor::controls::hypergraph::hypergraph_control^ h)
{
	super::on_added(h);
	h->display_rectangle = h->display_rectangle;
}

void animation_viewer_graph_node_base::on_removed(xray::editor::controls::hypergraph::hypergraph_control^ h)
{
	super::on_removed(h);
	h->display_rectangle = h->display_rectangle;
}

void animation_viewer_graph_node_base::save(xray::configs::lua_config_value cfg)
{
	cfg["id"] = m_id;
	cfg["position"] = float2((float)position.X, (float)position.Y);
}

void animation_viewer_graph_node_base::load(xray::configs::lua_config_value const& cfg, animation_viewer_hypergraph^ h)
{
	if(cfg.value_exists("id"))
		m_id = (u32)cfg["id"];

	System::Drawing::Point p(0, 0);
	if(cfg.value_exists("position"))
	{
		float2 pos = cfg["position"];
		p.X = (u32)pos.x;
		p.Y = (u32)pos.y;
	}

	position = p;

	if(cfg.value_exists("childs"))
	{
		xray::configs::lua_config_iterator it = cfg["childs"].begin();
		for(; it!=cfg["childs"].end(); ++it)
		{
			u32 type = (u32)(*it)["type"];
			animation_viewer_graph_node_base^ child = nullptr;
			if(type==1) // animation
			{
				child = gcnew animation_viewer_graph_node_animation();
				h->nodes_style_manager->assign_style(child, "animation_node");
			}
			else if(type==2) // addition
			{
				child = gcnew animation_viewer_graph_node_operator();
				h->nodes_style_manager->assign_style(child, "operation_node");
			}
			else if(type==3) // weight
			{
				if(this->parent!=nullptr && this->parent->childs->Count>0)
				{
					animation_viewer_graph_link^ lnk = animation_viewer_hypergraph::find_link_by_ids(h->links_manager->visible_links(), this->parent->childs[this->parent->childs->Count-1]->id, this->parent->id); 
					R_ASSERT(lnk!=nullptr);
					lnk->load(*it);
				}
				else
				{
					animation_viewer_graph_link^ lnk = animation_viewer_hypergraph::create_link(this, this->parent); 
					lnk->load(*it);
					h->links_manager->visible_links()->Add(lnk);
				}
			}
			else if(type==4) // substraction
				NOT_IMPLEMENTED();
			else if(type==5) // multiplication
			{
				child = gcnew animation_viewer_graph_node_base();
				h->nodes_style_manager->assign_style(child, "operation_node");
				child->initialize();
				child->parent = this;
				child->load(*it, h);
				delete child;
				child = nullptr;
			}
			else
				NODEFAULT();

			if(child!=nullptr)
			{
				child->set_scale(h->scale);
				child->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(h, &animation_viewer_hypergraph::on_key_down);
				child->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(h, &animation_viewer_hypergraph::on_key_up);
				child->load(*it, h);
				h->assign_node_id(child, u32(-1));
				h->all_nodes()->Add(child);
				h->links_manager->on_node_added(child);
				h->Controls->Add(child);
				child->on_added(h);

				animation_viewer_graph_link^ lnk = nullptr;
				if(dynamic_cast<animation_viewer_graph_node_root^>(this))
					lnk = animation_viewer_hypergraph::create_link(child, this); 
				else if(this->parent!=nullptr)
				{
					lnk = animation_viewer_hypergraph::find_link_by_ids(h->links_manager->visible_links(), this->id, this->parent->id); 
					if(lnk==nullptr)
						lnk = animation_viewer_hypergraph::create_link(child, this->parent); 
					else
					{
						lnk->source_node = child;
						xray::editor::controls::hypergraph::connection_point^ src_pt = child->add_connection_point(animation_viewer_graph_node_base::typeid, "child");
						src_pt->draw_text = false;
						lnk->source_point = src_pt;
						this->parent->childs->Remove(this);
						R_ASSERT(!this->parent->childs->Contains(child));
						this->parent->childs->Add(child);
					}
				}
				h->links_manager->visible_links()->Add(lnk);
			}
		}
	}
}

animation_viewer_graph_node_base^ animation_viewer_graph_node_base::root()
{
	if(parent!=nullptr)
		return parent->root();

	return nullptr;
}
