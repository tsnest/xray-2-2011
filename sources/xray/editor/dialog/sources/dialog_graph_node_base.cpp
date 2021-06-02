////////////////////////////////////////////////////////////////////////////
//	Created		: 25.06.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_graph_node_base.h"

using xray::dialog_editor::dialog_graph_node_base;

void dialog_graph_node_base::initialize()
{
	m_has_top = false;
	m_has_bottom = false;
	DoubleClick += gcnew System::EventHandler(this, &dialog_graph_node_base::on_double_click);
}

void dialog_graph_node_base::assign_style(xray::editor::controls::hypergraph::node_style^ style)
{
	m_style	= style;
	m_size = *(style->full_size);
	recalc();
	Invalidate();
}

void dialog_graph_node_base::draw_caption(System::Windows::Forms::PaintEventArgs^  e)
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
	e->Graphics->DrawString(caption, m_style->font, m_style->brush, rect, m_style->fmt);
}

void dialog_graph_node_base::draw_frame(System::Windows::Forms::PaintEventArgs^ e)
{
	if(is_dialog())
		m_style->border_pen->Width = 5;
	else
		m_style->border_pen->Width = 1;

	super::draw_frame(e);
}

void dialog_graph_node_base::on_added(xray::editor::controls::hypergraph::hypergraph_control^ parent)
{
	super::on_added(parent);
	parent->display_rectangle = parent->display_rectangle;
}

void dialog_graph_node_base::on_removed(xray::editor::controls::hypergraph::hypergraph_control^ parent)
{
	super::on_removed(parent);
	parent->display_rectangle = parent->display_rectangle;
}