#include "pch.h"
#include "hypergraph.h"
#include "hypergraph_node.h"
#include "node_connection_point.h"
#include "drawing2d_utils.h"

using xray::editor::controls::hypergraph::hypergraph_control;
using xray::editor::controls::hypergraph::node;
using xray::editor::controls::hypergraph::connection_point;
using xray::editor::controls::hypergraph::link;
using xray::editor::controls::hypergraph::connection_type;
using xray::editor::controls::hypergraph::view_mode;

Void hypergraph_control::hypergraph_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e)
{
	//e->Graphics->DrawLine(m_selection_rect_pen, 168,80,252,218);
	//e->Graphics->DrawLine(m_selection_rect_pen, 184,140,240,155);
//	e->Graphics->ScaleTransform(2, 2);
	if(m_view_mode==view_mode::full)
		draw_links_full(e);
	else
		draw_links_brief(e);
	
	if(m_src_conn_node)
		draw_rt_connection(e);

	if(m_b_rect_selection_mode)
	{
		Drawing::Point mouse_point		= PointToClient(MousePosition);
		//ControlPaint::DrawReversibleFrame(drawing2d::bound_rectangle(mouse_point, m_selection_mode_start_point), 
		//	Color::Black, 
		//	FrameStyle::Dashed);
		e->Graphics->DrawRectangle(m_selection_rect_pen, drawing2d::bound_rectangle(mouse_point, m_selection_mode_start_point));
	}
}
void hypergraph_control::draw_arrow(Point pt_start, Point pt_end, Pen^ pen, Graphics^ g)
{
	drawing2d::draw_arrow(pt_start, pt_end, pen, g);
}

Point get_rect_center (Rectangle r)
{
	return Point(r.X+r.Width/2, r.Y+r.Height/2);
}

void hypergraph_control::draw_rt_connection(System::Windows::Forms::PaintEventArgs^  e)
{
	Point pt_start;
	if(m_src_conn_point)
		pt_start = m_src_conn_node->PointToScreen(m_src_conn_point->get_point());
	else
		pt_start = m_src_conn_node->PointToScreen(get_rect_center(m_src_conn_node->ClientRectangle));

	pt_start		= PointToClient(pt_start);
	Point pt_end	= PointToClient(MousePosition);
	Color bk		= m_link_pen->Color;
	
	m_link_pen->Color	= System::Drawing::SystemColors::AppWorkspace;
	draw_arrow(pt_start, pt_end, m_link_pen, e->Graphics);
	m_link_pen->Color	= bk;
}

void hypergraph_control::draw_links_full(System::Windows::Forms::PaintEventArgs^  e)
{
	links^	links = links_manager->visible_links();

	for each(link^ link in links)
	{
		Color bk_color		= m_link_pen->Color;
		
		if(selected_links->Contains(link))
			m_link_pen->Color = link->source_node->get_style()->get_link_color(node_style::e_states::active);
		else if(link==focused_link)
			m_link_pen->Color = link->source_node->get_style()->get_link_color(node_style::e_states::highlihted);
		else
			m_link_pen->Color = link->source_node->get_style()->get_link_color(node_style::e_states::inactive);

		
		link->draw			(e->Graphics, m_link_pen, link_style);
		m_link_pen->Color	= bk_color;
	}
}

ref struct _pair
{
	_pair	(link^ l):m_link(l){}
	link^	m_link;
	virtual bool Equals( Object^ obj ) override
	{
		link^ lnk = safe_cast<link^>(obj);
		return m_link->endpoints_equal(lnk);
	}
};

void hypergraph_control::draw_links_brief(System::Windows::Forms::PaintEventArgs^  e)
{
	typedef System::Collections::ArrayList pairs_list;
	pairs_list^	connected_nodes			= gcnew pairs_list();

	links^	links = links_manager->visible_links();

	for each(link^ link in links)
	{
		if(!connected_nodes->Contains(link))
			connected_nodes->Add(gcnew _pair(link));
	}
	for each (_pair^ p in connected_nodes)
	{
		Color bk_color			= m_link_pen->Color;
		
		if(selected_links->Contains(p->m_link))
			m_link_pen->Color = p->m_link->source_node->get_style()->get_node_color(node_style::e_states::active);
		else if(p->m_link==focused_link)
			m_link_pen->Color = p->m_link->source_node->get_style()->get_node_color(node_style::e_states::highlihted);
		else
			m_link_pen->Color = p->m_link->source_node->get_style()->get_node_color(node_style::e_states::inactive);
		
		p->m_link->draw		(e->Graphics, m_link_pen, link_style);
		m_link_pen->Color	= bk_color;
	}
}

