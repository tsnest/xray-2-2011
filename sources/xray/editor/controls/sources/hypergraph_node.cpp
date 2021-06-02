#include "pch.h"
#include "hypergraph_node.h"
#include "hypergraph_node_style.h"
#include "hypergraph.h"
#include "node_connection_point.h"

using xray::editor::controls::hypergraph::node;
using xray::editor::controls::hypergraph::node_style;
using xray::editor::controls::hypergraph::connection_point_style;
using xray::editor::controls::hypergraph::hypergraph_control;
using xray::editor::controls::hypergraph::connection_point;
using xray::editor::controls::hypergraph::point_align;
using xray::editor::controls::hypergraph::connection_type;
using xray::editor::controls::hypergraph::nodes_movability;
using xray::editor::controls::hypergraph::view_mode;
using System::Math;

void node::in_constructor()
{
	m_parent					= nullptr;
	m_focused_connection_point	= nullptr;
	m_connection_points			= gcnew Collections::ArrayList ();
	m_style						= nullptr;
	m_position					= Point(0,0);
	m_size						= Drawing::Size(80, 40);
	movable						= false;
}

void node::on_added(hypergraph_control^ parent)
{
	m_parent		= parent;
	set_view_mode	(m_parent->current_view_mode);
}

void node::assign_style(node_style^ style)
{
	m_style	= style;
	recalc();
	Invalidate();
}

void node::on_removed(hypergraph_control^ parent)
{
	m_parent = nullptr;
}

void node::on_focus(bool b_enter)
{
	if(m_parent!=nullptr && !(m_parent->selected_nodes->Contains(this)))
		Invalidate();
}

void node::set_focused_connection_point_impl(connection_point^ p)
{
	if(p==m_focused_connection_point)
		return;
	
	m_focused_connection_point = p;
	Invalidate				();
}

Void node::hypergraph_node_MouseEnter(Object^ sender, System::EventArgs^ e)
{}

Void node::hypergraph_node_MouseDown(Object^ sender, MouseEventArgs^ e)
{
	Point p = Point(e->X+Location.X, e->Y+Location.Y);
	MouseEventArgs^ new_e = gcnew MouseEventArgs(e->Button, e->Clicks, p.X, p.Y, e->Delta);
	m_parent->hypergraph_area_MouseDown(sender, new_e);
}

Void node::hypergraph_node_MouseUp(Object^ sender, MouseEventArgs^ e)
{
	Point p = Point(e->X+Location.X, e->Y+Location.Y);
	MouseEventArgs^ new_e = gcnew MouseEventArgs(e->Button, e->Clicks, p.X, p.Y, e->Delta);
	m_parent->hypergraph_area_MouseUp(sender, new_e);
}

Void node::hypergraph_node_MouseMove(Object^ sender, MouseEventArgs^ e)
{
	Point pt = PointToClient(MousePosition);
	focused_connection_point = pick_connection_point(pt);

	Point p = Point(e->X+Location.X, e->Y+Location.Y);
	MouseEventArgs^ new_e = gcnew MouseEventArgs(e->Button, e->Clicks, p.X, p.Y, e->Delta);
	m_parent->hypergraph_area_MouseMove(sender, new_e);
}

connection_point^ node::add_connection_point(System::Type^ t, System::String^ name)
{
	connection_point_style^ result = gcnew connection_point_style();
	for each(connection_point_style^ st in m_style->get_point_styles())
	{
		if(name==st->name)
			result->clone(st);
	}
	if(result==nullptr)
	{
		result->name = name;
		m_style->add_new_style(result);
	}

	connection_point^ p				= gcnew connection_point(this, t, result);
	p->set_scale(m_style->scale);
	m_connection_points->Add		(p);
	recalc();
	Invalidate(false);
	return p;
}

void node::set_view_mode(view_mode m)
{
	m_style->mode = m;
	recalc();
	Invalidate();
}

void node::set_scale(float scl)
{
	m_style->scale = scl;
	for each(connection_point^ p in m_connection_points)
		p->set_scale(scl);

	recalc();
};


connection_point^ node::get_connection_point(String^ name)
{
	for each(connection_point^ p in m_connection_points)
		if(p->name==name)
			return p;

	return nullptr;
}

connection_point^ node::pick_connection_point(Point pt)
{
	connection_point^ result = nullptr;
	if(m_parent->current_view_mode == view_mode::brief)
		return result;

	for each(connection_point^ p in m_connection_points)
	{
		if(p->get_rect().Contains(pt))
		{
			result			= p;
			break;
		}
	}
	return result;
}

System::Drawing::Point node::get_point_location(connection_point^ pt)
{
	if(m_parent->current_view_mode==view_mode::full)
		return PointToScreen(pt->get_point());
	else
		return get_point_location(pt->direction);
}

System::Drawing::Point node::get_point_location(connection_type t)
{
	Drawing::Size				offset = m_style->get_max_point_size();
	switch (t)
	{
	case connection_type::input :
		return PointToScreen(Point(0+offset.Width/2, Height/2));
	case connection_type::output :
	case connection_type::inout :
		return PointToScreen(Point(Width-offset.Width/2, Height/2));
	default:
		NODEFAULT();
	}
}

void node::select_parents()
{
	for each(link^ lnk in m_parent->links_manager->visible_links())
	{
		if(lnk->destination_node==this && !m_parent->is_start_shift_node(lnk->source_node)
			&& !m_parent->selected_nodes->Contains(lnk->source_node))
		{
			m_parent->add_selected_node(lnk->source_node);
			lnk->source_node->select_parents();
		}
	}
}

void node::size::set(Drawing::Size value)
{
	m_size			= value;
	value.Width		= int(Math::Round(value.Width*m_style->scale));
	value.Height	= int(Math::Round(value.Height*m_style->scale));
	Size			= value;
}

void node::set_position_impl(Point p)
{
	if (m_position == p) 
		return;

	m_position		= p;

	if(m_parent != nullptr)
	{
		m_parent->set_node_location(this);

		m_on_position_changed(this);
	}
}