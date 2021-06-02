//-------------------------------------------------------------------------------------------
//	Created		: 17.12.2009
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2009
//-------------------------------------------------------------------------------------------
#include "pch.h"
#include "dialog_graph_node.h"
#include "dialog_hypergraph.h"
#include "dialog_document.h"
#include "dialog_node_base.h"
#include "dialog_phrase.h"
#include "game_dialog.h"
#include "dialog.h"
#include "dialog_editor.h"
#include "dialog_node_hypergraph.h"
#include "dialog_links_manager.h"
#include "dialog_graph_link.h"
#include <xray/editor/base/managed_delegate.h>

using xray::dialog_editor::dialog_graph_node;
using xray::dialog_editor::dialog_node_base;
using xray::dialog_editor::dialog_phrase;
using xray::dialog_editor::game_dialog;
using xray::dialog_editor::dialog;
using xray::editor::controls::hypergraph::connection_point;
using xray::editor::controls::hypergraph::point_align;
using xray::editor::controls::hypergraph::view_mode;
using xray::editor::controls::hypergraph::node_style;
using xray::dialog_editor::dialog_hypergraph;
using xray::dialog_editor::dialog_document;
using xray::dialog_editor::dialog_editor_impl;
using xray::dialog_editor::dialog_node_hypergraph;
using xray::dialog_editor::dialog_graph_link;

#define VERTICAL_TAB_HEIGHT 8
dialog_graph_node::dialog_graph_node()
:m_parent_dialog(NULL)
{
	initialize();
}

dialog_graph_node::dialog_graph_node(dialog* parent_dialog)
:m_parent_dialog(parent_dialog)
{
	initialize();
}

void dialog_graph_node::initialize()
{
	m_id = u32(-1);
	m_hypergraph = (gcnew dialog_node_hypergraph(this));
	m_object = NEW(dialog_node_base_ptr)();
	m_has_top = false;
	m_has_bottom = false;

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
	button_exp_coll->Click += gcnew System::EventHandler(this, &dialog_graph_node::on_exp_coll_clicked);
	button_exp_coll->Visible = false;
	Controls->Add(button_exp_coll);
	AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
	DoubleClick += gcnew System::EventHandler(this, &dialog_graph_node::on_double_click);
	ResumeLayout(false);
}

dialog_graph_node::~dialog_graph_node()
{
	delete m_hypergraph;

	*m_object = NULL;
	DELETE(m_object);

	if(components)
		delete components;
}

String^	dialog_graph_node::caption()
{
	String^ txt = text;

	if(txt=="" || txt==nullptr)
		return "empty";
	else
		return txt;
}

u32 dialog_graph_node::id()
{
	if(m_id==u32(-1))
		return object->id();
	else
		return m_id;
}

bool dialog_graph_node::is_dialog()
{
	if(m_object==NULL || m_object->c_ptr()==NULL)
		return false;

	return (dynamic_cast<dialog*>(object)) ? true : false;
}

void dialog_graph_node::recalc()
{
	bool is_expanded = (is_dialog() && (button_exp_coll->Text=="-"));
	button_exp_coll->Visible = is_dialog();
	if(is_dialog())
	{
		Point p = Point(0,0);
		p.X = int(Math::Round((m_size.Width)*m_style->scale)) - button_exp_coll->Size.Width;
		button_exp_coll->Location = p;
	}

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

void dialog_graph_node::on_added(xray::editor::controls::hypergraph::hypergraph_control^ parent)
{
	super::on_added(parent);
	parent->display_rectangle = parent->display_rectangle;
	if(m_parent_dialog)
		m_parent_dialog->add_node(*m_object);

	m_hypergraph->nodes_style_manager = parent->nodes_style_manager;
}

void dialog_graph_node::on_removed(xray::editor::controls::hypergraph::hypergraph_control^ parent)
{
	super::on_removed(parent);
	if(m_parent_dialog && m_id==u32(-1))
		m_parent_dialog->remove_node(*m_object);

	parent->display_rectangle = parent->display_rectangle;
}

void dialog_graph_node::draw_caption(System::Windows::Forms::PaintEventArgs^  e)
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
	m_style->brush->Color = Color::Black;
	m_style->border_pen->Width = 1;
//	e->Graphics->DrawLine(m_style->border_pen, rect.Left, rect.Top, rect.Right, rect.Top);
//	e->Graphics->DrawLine(m_style->border_pen, rect.Left, rect.Bottom, rect.Right, rect.Bottom);
	e->Graphics->DrawString(caption(), m_style->font, m_style->brush, rect, m_style->fmt);
}

void dialog_graph_node::draw_frame(System::Windows::Forms::PaintEventArgs^ e)
{
	if(is_dialog())
		m_style->border_pen->Width = 5;
	else
		m_style->border_pen->Width = 1;

	super::draw_frame(e);
}

void dialog_graph_node::on_double_click(System::Object^ , System::EventArgs^ e)
{
	if((ModifierKeys & Keys::Control) == Keys::Control ||
		(ModifierKeys & Keys::Shift) == Keys::Shift ||		
		(ModifierKeys & Keys::Alt) == Keys::Alt)
		return;

	if(safe_cast<System::Windows::Forms::MouseEventArgs^>(e)->Button==System::Windows::Forms::MouseButtons::Left)
	{
		if(dynamic_cast<dialog_hypergraph^>(m_parent))
			safe_cast<dialog_hypergraph^>(m_parent)->on_node_double_click(this);
	}
}

void dialog_graph_node::fill_hypergraph()
{
	dialog* d = dynamic_cast<dialog*>(object);
	const vector<dialog_node_base_ptr>* phrs_lst = d->get_phrases();
	vector<dialog_node_base_ptr>::const_iterator it = phrs_lst->begin();
	for(; it!=phrs_lst->end(); ++it)
	{
		dialog_graph_node^ n = gcnew dialog_graph_node();
		m_hypergraph->nodes_style_manager->assign_style(n, "dialog_node");
		n->object = it->c_ptr();
		m_hypergraph->append_node(n);
	}

	it = phrs_lst->begin();
	for(; it!=phrs_lst->end(); ++it)
		iterate_subnodes(it->c_ptr());
	
	Point cur_pos = Point(0,0);
	for each(dialog_graph_node^ n in m_hypergraph->all_nodes())
	{
		n->position = cur_pos;
		cur_pos.X += n->get_style()->full_size->Width + 10;
		if(cur_pos.X>m_hypergraph->display_rectangle.Right)
		{
			cur_pos.X = 0;
			cur_pos.Y += n->get_style()->full_size->Height + 20;
		}
	}
}

void dialog_graph_node::iterate_subnodes(dialog_node_base* parent)
{
	const links_list* lnks_list = parent->get_links();
	dialog_link* cur_lnk = lnks_list->front();
	while(cur_lnk!=NULL)
	{
		dialog_graph_node^ parent_node = dynamic_cast<dialog_hypergraph^>(m_parent)->find_node_by_id(m_hypergraph->all_nodes(), parent->id());
		dialog_graph_node^ child_node = dynamic_cast<dialog_hypergraph^>(m_parent)->find_node_by_id(m_hypergraph->all_nodes(), cur_lnk->child()->id());
		dialog_graph_link^ lnk = m_hypergraph->create_connection(parent_node, child_node);
		lnk->m_link = cur_lnk;
		m_hypergraph->links_manager->visible_links()->Add(lnk);
		cur_lnk = lnks_list->get_next_of_object(cur_lnk);
	}
}

void dialog_graph_node::on_exp_coll_clicked(System::Object^ , System::EventArgs^ )
{
	if(is_dialog())
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
}

void dialog_graph_node::assign_style(xray::editor::controls::hypergraph::node_style^ style)
{
	m_style	= style;
	m_size = *(style->full_size);
	recalc();
	Invalidate();
}

String^ dialog_graph_node::text::get()
{
	if(object)
		return gcnew String(object->text());
	else
		return "";
}

void dialog_graph_node::string_table::set(String^ new_str_id)
{
	if(object && !is_dialog())
	{
		unmanaged_string str = unmanaged_string(new_str_id);
		dynamic_cast<dialog_phrase*>(object)->set_string_table_id(str.c_str());
	}
}

String^ dialog_graph_node::string_table::get()
{
	if(object && !is_dialog())
		return gcnew String(dynamic_cast<dialog_phrase*>(object)->string_table_id());
	else
		return "";
}

bool dialog_graph_node::is_root::get()
{
	if(m_object)
		return object->is_root();
	else
		return false;
}

void dialog_graph_node::is_root::set(bool val)
{
	object->set_is_root(val);
}

void dialog_graph_node::request_dialog()
{
	dialog_manager::get_dialogs_manager()->set_return_exsisting_dialog(false);
	query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &dialog_graph_node::on_dialog_loaded));
	xray::resources::query_resource(
		object->text(),
		xray::resources::dialog_class,
		boost::bind(&query_result_delegate::callback, rq, _1),
		g_allocator
		);

	dialog_manager::get_dialogs_manager()->set_return_exsisting_dialog(true);
}

void dialog_graph_node::on_dialog_loaded(xray::resources::queries_result& result)
{
	if(result[0].is_success())
	{
		dialog_node_base_ptr sub_dlg = static_cast_resource_ptr<dialog_node_base_ptr>(result[0].get_unmanaged_resource());
		sub_dlg->set_id(object->id());

		links_list* sub_dlg_links_list = const_cast<links_list*>(sub_dlg->get_links());
		links_list* old_dlg_links_list = const_cast<links_list*>(object->get_links());
		sub_dlg_links_list->swap(*old_dlg_links_list);

		const vector<dialog_node_base_ptr>* vec = m_parent_dialog->get_phrases();
		vector<dialog_node_base_ptr>::const_iterator i = vec->begin();
		for(; i!=vec->end(); ++i)
		{
			for(dialog_link* lnk = (i->c_ptr())->get_links()->front(); lnk!=NULL; lnk = (i->c_ptr())->get_links()->get_next_of_object(lnk))
			{
				if(lnk->child()==object)
					lnk->change_child(sub_dlg.c_ptr());
			}
		}

		*m_object = sub_dlg.c_ptr();
		dialog_node_base_ptr* p = m_parent_dialog->get_node_by_id(object->id());
		R_ASSERT(p!=NULL);
		(*p)->set_text("");
		DELETE(p->c_ptr());
		*p = sub_dlg.c_ptr();
	}
}

dialog_graph_node::e_id_category dialog_graph_node::category::get()
{
	if(object && !is_dialog())
	{
		dialog_editor_impl^ ed = safe_cast<dialog_hypergraph^>(m_parent)->get_owner()->get_editor();
		return e_id_category(ed->id_category(string_table));
	}
	else
		return e_id_category::none;
}

void dialog_graph_node::category::set(e_id_category val)
{
	if(object && !is_dialog())
	{
		dialog_editor_impl^ ed = safe_cast<dialog_hypergraph^>(m_parent)->get_owner()->get_editor();
		ed->set_id_category(string_table, u32(val));
	}
}