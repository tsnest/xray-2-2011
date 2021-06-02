////////////////////////////////////////////////////////////////////////////
//	Created		: 25.06.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_graph_node_dialog.h"
#include "dialog_container.h"
#include "dialog_hypergraph.h"
#include "dialog_node_hypergraph.h"
#include "dialog_document.h"
#include "dialog_editor.h"
#include "dialog_links_manager.h"
#include "dialog.h"
#include "dialog_graph_node_phrase.h"
#include "dialog_graph_link.h"
#include "dialog_graph_node_layout.h"

using xray::dialog_editor::dialog_graph_node_dialog;
using namespace System::Windows::Forms;
using xray::dialog_editor::dialog_node_base;
using xray::dialog_editor::dialog_container;
using xray::dialog_editor::dialog;
using xray::dialog_editor::dialog_graph_node_layout;
using xray::editor::controls::hypergraph::view_mode;
using xray::editor::controls::hypergraph::connection_point;
using xray::editor::controls::hypergraph::point_align;

void dialog_graph_node_dialog::initialize()
{
	super::initialize();

	m_object = NULL;
	m_hypergraph = (gcnew dialog_node_hypergraph(this));

	SuspendLayout();

	// m_hypergraph
	m_hypergraph->AllowDrop = false;
	m_hypergraph->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
		| System::Windows::Forms::AnchorStyles::Left) 
		| System::Windows::Forms::AnchorStyles::Right));
	m_hypergraph->BackColor = System::Drawing::SystemColors::Window;
	m_hypergraph->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
	m_hypergraph->current_view_mode = xray::editor::controls::hypergraph::view_mode::full;
	m_hypergraph->focused_link = nullptr;
	m_hypergraph->focused_node = nullptr;
	m_hypergraph->Location = System::Drawing::Point(10, 10);
	m_hypergraph->max_scale = 5.0F;
	m_hypergraph->min_scale = 0.8F;
	m_hypergraph->move_mode = xray::editor::controls::hypergraph::nodes_movability::fixed;
	m_hypergraph->Name = L"m_hypergraph";
	m_hypergraph->node_size_brief = System::Drawing::Size(20, 10);
	m_hypergraph->scale = 1.0F;					
	m_hypergraph->Size = System::Drawing::Size(100, 15);
	m_hypergraph->visibility_mode = xray::editor::controls::hypergraph::nodes_visibility::all;
	m_hypergraph->links_manager	= gcnew	dialog_links_manager(m_hypergraph);
	m_hypergraph->clear();
	m_hypergraph->Visible = false;
	Controls->Add(m_hypergraph);

	button_exp_coll = gcnew System::Windows::Forms::Button();
	button_exp_coll->Name = L"button_exp_coll";
	button_exp_coll->Size = System::Drawing::Size(18, 18);
	button_exp_coll->Text = L"+";
	button_exp_coll->UseVisualStyleBackColor = true;
	button_exp_coll->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
	button_exp_coll->Click += gcnew System::EventHandler(this, &dialog_graph_node_dialog::on_exp_coll_clicked);
	button_exp_coll->Visible = true;
	Controls->Add(button_exp_coll);
	ResumeLayout(false);
}

dialog_graph_node_dialog::~dialog_graph_node_dialog()
{
	m_object = NULL;
}

void dialog_graph_node_dialog::recalc()
{
	bool is_expanded = (button_exp_coll->Text=="-");
	m_has_top = false;
	m_has_bottom = false;
	if(m_style->mode==view_mode::full)
	{
		int h = (is_expanded)?m_size.Height:m_style->full_size->Height;
		if(!is_expanded)
		{
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
		}

		for each(connection_point^ p in m_connection_points)
		{
			System::Drawing::Rectangle rect = p->base_rect();
			if(is_expanded)
				rect.X = m_size.Width/2;

			if(!(p->align==point_align::top))
				rect.Y = h - (p->align==point_align::bottom?10:17);

			p->set_rect(rect);
		}
		if(!is_expanded)
			m_size = System::Drawing::Size(m_style->full_size->Width, h);
	}
}

String^ dialog_graph_node_dialog::get_text()
{
	return gcnew System::String(m_object->text());
}

void dialog_graph_node_dialog::on_exp_coll_clicked(System::Object^ , System::EventArgs^ )
{
	if(button_exp_coll->Text=="+")
	{
		button_exp_coll->Text = "-";
		m_hypergraph->Visible = true;
		button_exp_coll->BringToFront();
		m_size = System::Drawing::Size(400, 400);
		m_hypergraph->clear();
		fill_hypergraph();
	}
	else
	{
		button_exp_coll->Text = "+";
		m_hypergraph->Visible = false;
		m_size = *(m_style->full_size);
	}
	m_parent->display_rectangle = m_parent->display_rectangle;
}

bool dialog_graph_node_dialog::is_root::get()
{
	return m_object->is_root();
}

void dialog_graph_node_dialog::is_root::set(bool rt)
{
	m_object->set_is_root(rt);
}

void dialog_graph_node_dialog::on_double_click(System::Object^ , System::EventArgs^ e)
{
	if((ModifierKeys & Keys::Control) == Keys::Control ||
		(ModifierKeys & Keys::Shift) == Keys::Shift ||		
		(ModifierKeys & Keys::Alt) == Keys::Alt)
		return;

	if(safe_cast<System::Windows::Forms::MouseEventArgs^>(e)->Button==System::Windows::Forms::MouseButtons::Left)
	{
		dialog_hypergraph^ h = dynamic_cast<dialog_hypergraph^>(m_parent);
		dialog_editor_impl^ ed = nullptr;
		if(h)
			ed = h->get_owner()->get_editor();
		else
			ed = dynamic_cast<dialog_node_hypergraph^>(m_parent)->get_owner()->get_editor();
		
		ed->creating_new_document = true;
		ed->multidocument_base->load_document(get_text());
		ed->creating_new_document = false;
	}
}

dialog_node_base* dialog_graph_node_dialog::object::get()
{
	return m_object;
}


void dialog_graph_node_dialog::object::set(dialog_node_base* n)
{
	m_object = dynamic_cast<dialog_container*>(n);
	R_ASSERT(m_object);
}

void dialog_graph_node_dialog::fill_hypergraph()
{
	if(m_hypergraph->nodes_style_manager==nullptr)
		m_hypergraph->nodes_style_manager = m_parent->nodes_style_manager;

	dialog* d = m_object->get_resource()->c_ptr()->get_dialog();
	const vector<dialog_node_base*>* phrs_lst = d->get_phrases();
	vector<dialog_node_base*>::const_iterator it = phrs_lst->begin();
	for(; it!=phrs_lst->end(); ++it)
	{
		dialog_graph_node_base^ n = nullptr;
		if(dynamic_cast<dialog_container*>(*it))
			n = gcnew dialog_graph_node_dialog();
		else
			n = gcnew dialog_graph_node_phrase();
		m_hypergraph->nodes_style_manager->assign_style(n, "dialog_node");
		n->object = *it;
		m_hypergraph->append_node(n);
	}

	it = phrs_lst->begin();
	for(; it!=phrs_lst->end(); ++it)
	{
		dialog_node_base* parent = *it;
		const links_list* lnks_list = parent->get_links();
		dialog_link* cur_lnk = lnks_list->front();
		while(cur_lnk!=NULL)
		{
			dialog_graph_node_base^ parent_node = dialog_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), parent->id());
			dialog_graph_node_base^ child_node = dialog_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), cur_lnk->child()->id());
			dialog_graph_link^ lnk = m_hypergraph->create_connection(parent_node, child_node);
			lnk->m_link = cur_lnk;
			m_hypergraph->links_manager->visible_links()->Add(lnk);
			cur_lnk = lnks_list->get_next_of_object(cur_lnk);
		}
	}

	dialog_graph_node_layout* l = m_object->get_resource()->c_ptr()->m_layout;
	if(l)
	{
		for each(dialog_graph_node_base^ n in m_hypergraph->all_nodes())
		{
			dialog_graph_node_layout::layout_vector::const_iterator it_pos = l->m_layout.find(n->id);
			System::Drawing::Point p = System::Drawing::Point(it_pos->second.x, it_pos->second.y);
			n->position = p;
		}
	}
	else
	{
		System::Drawing::Point p = System::Drawing::Point(0,0);
		for each(dialog_graph_node_base^ n in m_hypergraph->all_nodes())
		{
			n->position = p;
			p.X += n->get_style()->full_size->Width + 10;
			if(p.X>m_hypergraph->display_rectangle.Right)
			{
				p.X = 0;
				p.Y += n->get_style()->full_size->Height + 20;
			}
		}
	}
	m_hypergraph->center_nodes();
}

void dialog_graph_node_dialog::set_expanded(bool val)
{
	button_exp_coll->Text = (val) ? "+" : "-";
	on_exp_coll_clicked(nullptr, System::EventArgs::Empty);
}
