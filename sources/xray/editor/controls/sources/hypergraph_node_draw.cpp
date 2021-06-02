#include "pch.h"
#include "hypergraph_node.h"
#include "hypergraph.h"
#include "node_connection_point.h"
#include "hypergraph_node_style.h"

using xray::editor::controls::hypergraph::node;
using xray::editor::controls::hypergraph::node_style;
using xray::editor::controls::hypergraph::connection_point;
using xray::editor::controls::hypergraph::point_align;
using xray::editor::controls::hypergraph::view_mode;
using System::Math;

void node::recalc()
{
	if(m_style->mode==view_mode::full)
	{
		int h = m_style->full_size->Height;
		for each(connection_point^ p in m_connection_points)
		{
			if(p->get_rect().Bottom > h-m_style->get_max_point_size().Height*m_style->scale/2)
			{
				if(p->align==point_align::bottom)
					h = p->get_rect().Bottom+1;
				else
					h = int(Math::Round(p->get_rect().Bottom+m_style->get_max_point_size().Height*m_style->scale/2+2));
			} 
		}
		m_size = System::Drawing::Size(m_style->full_size->Width, (int)(h/m_style->scale));
	}
	else if(m_style->mode==view_mode::brief)
	{
		m_size = System::Drawing::Size(m_style->brief_size->Width, m_style->brief_size->Height);
	}
}

void node::on_paint(System::Windows::Forms::PaintEventArgs^ e)
{
	if(DesignMode)
		return;
	
	draw_frame					(e);
	draw_caption				(e);
	
	if(m_style->mode==view_mode::full)
		draw_connection_points	(e);
}

void node::draw_caption(System::Windows::Forms::PaintEventArgs^  e)
{
	System::Drawing::Size sz	= m_style->get_max_point_size();
	sz.Height					= int(Math::Round(sz.Height*m_style->scale));
	sz.Width					= int(Math::Round(sz.Width*m_style->scale));
	Rectangle rect				= Rectangle::Inflate(ClientRectangle, -sz.Width/2, -sz.Height/2);
	m_style->brush->Color		= Color::Black;
	e->Graphics->DrawString		(caption(), m_style->font, m_style->brush, rect, m_style->fmt);
}

void node::draw_frame(System::Windows::Forms::PaintEventArgs^  e)
{
	System::Drawing::Size sz	= m_style->get_max_point_size();
	sz.Height					= int(Math::Round(sz.Height*m_style->scale));
	sz.Width					= int(Math::Round(sz.Width*m_style->scale));
	Rectangle rect				= Rectangle::Inflate(ClientRectangle, -sz.Width/2, -sz.Height/2);

	e->Graphics->DrawRectangle	(m_style->border_pen, rect);

	if(m_parent->selected_nodes->Contains(this))
		m_style->set_brush_color(node_style::e_states::active);
	else if(m_parent->focused_node==this)
		m_style->set_brush_color(node_style::e_states::highlihted);
	else
		m_style->set_brush_color(node_style::e_states::inactive);

	int w = int(Math::Round(m_style->border_pen->Width/2));
	e->Graphics->FillRectangle(m_style->brush, rect.X+w+1, rect.Y+w+1, rect.Width-w*2-1, rect.Height-w*2-1);
}

System::Drawing::Brush^ node::select_brush(connection_point^ p, Point mouse_pos)
{
	Color col;
	if(p->get_rect().Contains(mouse_pos))
		col = m_style->get_point_color(p->name, node_style::e_states::highlihted);
	else
		col = m_style->get_point_color(p->name, node_style::e_states::inactive);
	
	m_style->brush->Color = col;
	return m_style->brush;
}

void node::draw_connection_points(System::Windows::Forms::PaintEventArgs^ e)
{
	Point client_mouse_pos = PointToClient(MousePosition);

	for each (connection_point^ p in m_connection_points)
	{
		System::Drawing::Brush^ br	= select_brush(p, client_mouse_pos);
		System::Drawing::Rectangle rect = p->get_rect();
		e->Graphics->FillEllipse	(br, rect);
		e->Graphics->DrawEllipse	(m_style->border_pen, rect);
		
		PointF text_pt				= rect.Location;

		if(!(p->draw_text))
			continue;

		switch(p->align)
		{
			case point_align::left:
				text_pt.X			+= rect.Width;
				text_pt.Y			+= (rect.Height - e->Graphics->MeasureString(p->name, m_style->connection_points_font).Height)/2;
				break;
			case point_align::right:
				text_pt.X			-= e->Graphics->MeasureString(p->name, m_style->connection_points_font).Width;
				text_pt.Y			+= (rect.Height - e->Graphics->MeasureString(p->name, m_style->connection_points_font).Height)/2;
				break;
			case point_align::top:
				text_pt.X			-= (e->Graphics->MeasureString(p->name, m_style->connection_points_font).Width-rect.Width)/2;
				text_pt.Y			+= rect.Height-2;
				break;
			case point_align::bottom:
				text_pt.X			-= (e->Graphics->MeasureString(p->name, m_style->connection_points_font).Width-rect.Width)/2;
				text_pt.Y			-= m_style->connection_points_font->Height;
				break;
		}

		m_style->brush->Color = Color::Black;
		e->Graphics->DrawString	(p->name, m_style->connection_points_font, m_style->brush, text_pt);
	}
}
