////////////////////////////////////////////////////////////////////////////
//	Created		: 17.12.2009
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_graph_node_phrase.h"
#include "dialog_phrase.h"
#include "dialog_hypergraph.h"
#include "dialog_document.h"
#include "dialog_editor.h"

using xray::dialog_editor::dialog_graph_node_phrase;
using xray::editor::controls::hypergraph::view_mode;
using xray::editor::controls::hypergraph::connection_point;
using xray::editor::controls::hypergraph::point_align;
using namespace System::Windows::Forms;
using xray::dialog_editor::dialog_node_base;

void dialog_graph_node_phrase::initialize()
{
	super::initialize();
	m_object = NULL;
}

dialog_graph_node_phrase::~dialog_graph_node_phrase()
{
	m_object = NULL;
}

String^	dialog_graph_node_phrase::get_text()
{
	pcstr const txt = m_object->text();
	if(txt)
	{
		System::String^ t = gcnew System::String(txt);
		if(t!="")
			return t;
	}

	return "empty";
}

void dialog_graph_node_phrase::recalc()
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
			rect.X = m_size.Width/2;

			if(!(p->align==point_align::top))
				rect.Y = h - (p->align==point_align::bottom?10:17);

			p->set_rect(rect);
		}
		m_size = System::Drawing::Size(m_style->full_size->Width, h);
	}
}

void dialog_graph_node_phrase::on_double_click(System::Object^ , System::EventArgs^ e)
{
	if((ModifierKeys & Keys::Control) == Keys::Control ||
		(ModifierKeys & Keys::Shift) == Keys::Shift ||
		(ModifierKeys & Keys::Alt) == Keys::Alt)
		return;

	if(safe_cast<System::Windows::Forms::MouseEventArgs^>(e)->Button==System::Windows::Forms::MouseButtons::Left)
	{
		dialog_hypergraph^ h = dynamic_cast<dialog_hypergraph^>(m_parent);
		if(h)
		{
			dialog_document^ d = safe_cast<dialog_document^>(h->get_owner());
			d->get_editor()->show_text_editor(this);
		}
	}
}

void dialog_graph_node_phrase::string_table::set(String^ new_str_id)
{
	unmanaged_string str = unmanaged_string(new_str_id);
	m_object->set_string_table_id(str.c_str());
}

String^ dialog_graph_node_phrase::string_table::get()
{
	if(m_object->string_table_id())
		return gcnew String(m_object->string_table_id());
	else
		return "empty";
}

bool dialog_graph_node_phrase::is_root::get()
{
	return m_object->is_root();
}

void dialog_graph_node_phrase::is_root::set(bool rt)
{
	m_object->set_is_root(rt);
}

dialog_graph_node_phrase::e_id_category dialog_graph_node_phrase::category::get()
{
	dialog_hypergraph^ h = dynamic_cast<dialog_hypergraph^>(m_parent);
	if(h && string_table!="empty")
	{
		dialog_editor_impl^ ed = h->get_owner()->get_editor();
		return e_id_category(ed->id_category(string_table));
	}
	return e_id_category::none;
}

void dialog_graph_node_phrase::category::set(e_id_category val)
{
	dialog_hypergraph^ h = dynamic_cast<dialog_hypergraph^>(m_parent);
	if(h)
	{
		dialog_editor_impl^ ed = h->get_owner()->get_editor();
		ed->set_id_category(string_table, u32(val));
	}
}

dialog_node_base* dialog_graph_node_phrase::object::get()
{
	return m_object;
}

void dialog_graph_node_phrase::object::set(dialog_node_base* n)
{
	m_object = dynamic_cast<dialog_phrase*>(n);
	R_ASSERT(m_object);
}
