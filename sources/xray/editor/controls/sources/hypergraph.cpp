////////////////////////////////////////////////////////////////////////////
//	Created		: 15.11.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "hypergraph.h"
#include "hypergraph_node.h"
#include "hypergraph_node_style.h"
#include "node_connection_point.h"
#include "drawing2d_utils.h"
#include "scalable_scroll_bar.h"
#include "scroller_event_args.h"

namespace xray {
namespace editor {
namespace controls {
namespace hypergraph {

void hypergraph_control::in_constructor()
{
	m_need_recalc				= true;
	m_b_rect_selection_mode		= false;
	m_b_drag_mode				= false;
	m_b_graph_move_mode			= false;
	move_mode					= nodes_movability::movable;
	link_style					= nodes_link_style::arrow;
	m_view_mode					= view_mode::full;
	visibility_mode				= nodes_visibility::all;
	node_size_brief				= Drawing::Size(80, 40);
	m_nodes						= gcnew nodes ();
	links_manager				= gcnew links_manager_default(this);
	m_selected_nodes			= gcnew nodes;
	m_selected_links			= gcnew links;
	m_focused_node				= nullptr;
	m_src_conn_point			= nullptr;
	m_src_conn_node				= nullptr;
	m_start_shift_node			= nullptr;
	m_link_pen					= gcnew Drawing::Pen(System::Drawing::SystemColors::WindowText, 1.0f);
	m_selection_rect_pen		= gcnew Drawing::Pen(Color::Gray, 1.0f);
	m_selection_rect_pen->DashStyle	= Drawing::Drawing2D::DashStyle::DashDot;
	m_scale						= 1.0f;
	min_scale					= 0.3f;
	max_scale					= 3.0f;
	scalable					= true;
	show_scrollbars				= true;

	m_inner_to_outer_distance	= 200;
	m_inner_field_rect			= Drawing::Rectangle(0, 0, 100, 100);
	m_outer_field_rect			= Drawing::Rectangle(-200, -200, 300, 300);
	display_rectangle			= Drawing::Rectangle(0, 0, 400, 400);
}

void hypergraph_control::append_node(node^ node)
{
	Controls->Add	(node);
	m_nodes->Add	(node);
	node->set_scale	(m_scale);
	node->on_added	(this);
	node->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &hypergraph_control::on_key_down);
	node->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &hypergraph_control::on_key_up);
	links_manager->on_node_added(node);
}

void hypergraph_control::remove_node(node^ node)
{
	Controls->Remove(node);
	m_nodes->Remove	(node);
	node->on_removed(this);
	links_manager->on_node_removed(node);
}

void hypergraph_control::clear()
{
	Controls->Clear			();
	Controls->Add			(m_v_scroll);
	Controls->Add			(m_h_scroll_bar_container);
	m_nodes->Clear			();
	m_selected_nodes->Clear	();
	m_selected_links->Clear	();
	m_focused_node			= nullptr;
	m_focused_link			= nullptr;
	links_manager->clear	();
}

Void hypergraph_control::hypergraph_area_MouseEnter(System::Object^  sender, System::EventArgs^  e)
{}

Void hypergraph_control::hypergraph_area_MouseLeave(System::Object^  sender, System::EventArgs^  e)
{}

link^ hypergraph_control::get_link_under_cursor()
{
	link^	result = nullptr;

	links^	links = links_manager->visible_links();
	for each(link^ link in links)
	{
		Point local_mouse_pos = PointToClient(MousePosition);
		if(link->pick(local_mouse_pos))
		{
			result = link;
			break;
		}
	}
	return result;
}

links^ hypergraph_control::get_links_under_cursor()
{
	links^	result = gcnew links;
	links^	links = links_manager->visible_links();

	for each(link^ link in links)
	{
		Point local_mouse_pos = PointToClient(MousePosition);
		if(link->pick(local_mouse_pos))
			result->Add(link);
	}
	return result;
}

links^ hypergraph_control::get_links			(Drawing::Rectangle selection_rect)
{
	links^	result = gcnew links;

	if(current_view_mode == view_mode::brief)
		return result;

	links^	links = links_manager->visible_links();

	for each(link^ link in links)
	{
		if(link->intersect(selection_rect))
			result->Add(link);
	}
	return result;
}

node^ hypergraph_control::get_node_under_cursor	()
{
	System::Drawing::Point	pt	= PointToClient(MousePosition);
	Control^ ctl				= GetChildAtPoint(pt, System::Windows::Forms::GetChildAtPointSkip::Invisible);
	node^ n = dynamic_cast<node^>(ctl);
	if (!n)
		return					nullptr;

	return n;
}

nodes^ hypergraph_control::get_nodes			(Drawing::Rectangle selection_rectangle)
{
	nodes^ result = gcnew nodes;
	for each(node^ node in m_nodes)
	{
		if(!node->Visible) 
			continue;

		Drawing::Rectangle node_rectangle	= node->ClientRectangle;
		node_rectangle.Location				= node->Location;
		if(node_rectangle.IntersectsWith	(selection_rectangle))
			result->Add						(node);
	}

	return result;
}

void hypergraph_control::add_selected_node		(nodes^ nodes)
{
	if(nodes)
	{
		m_selected_nodes->AddRange(nodes);
		for each (node^ n in nodes)
		{
			n->BringToFront();
			n->Invalidate();
		}
		m_h_scroll_bar_container->BringToFront();
		m_v_scroll->BringToFront();
	}
	on_selection_changed();
}

void hypergraph_control::add_selected_node		(node^ single_node)
{
	if(m_selected_nodes->Contains(single_node))
		return;

	if(single_node)
	{
		m_selected_nodes->Add(single_node);
		single_node->BringToFront();
		single_node->Invalidate();
		m_h_scroll_bar_container->BringToFront();
		m_v_scroll->BringToFront();
	}
	on_selection_changed();
}

void hypergraph_control::set_selected_node		(nodes^ nodes)
{
	for each (node^ n in m_selected_nodes)
		if(!nodes || !nodes->Contains(n))
			n->Invalidate();

	m_selected_nodes->Clear();
	add_selected_node(nodes);
}

void hypergraph_control::set_selected_node		(node^ single_node)
{
	for each (node^ n in m_selected_nodes)
		if(!single_node || single_node!=n)
			n->Invalidate();

	m_selected_nodes->Clear();
	add_selected_node(single_node);
}

void hypergraph_control::set_focused_node_impl	(node^ node)
{
	if(node==m_focused_node)
		return;

	if(m_focused_node)
		m_focused_node->on_focus(false);
	
	if(node)
		node->on_focus		(true);
	
	m_focused_node		= node;
}

void hypergraph_control::set_view_mode_impl		(view_mode m)
{
	m_view_mode = m;
	for each(node^ node in m_nodes)
		node->set_view_mode(m);

	Invalidate	(true);
}

void hypergraph_control::set_scale_impl			(float new_scale)
{
	m_scale = new_scale;
 	if(m_scale<min_scale)
 		m_scale = min_scale;
 	else if(m_scale>max_scale)
 		m_scale = max_scale;
}

void hypergraph_control::change_scale			(float scale_offset)
{	
	set_scale_impl				(m_scale + scale_offset);
	
	display_rectangle			= Drawing::Rectangle(m_display_rectangle.X, m_display_rectangle.Y, (int)(ClientRectangle.Width / scale), (int)(ClientRectangle.Height / scale));
}

Void hypergraph_control::drag_start				()
{
	m_b_drag_mode					= true;
	m_selection_mode_start_point	= PointToClient(MousePosition);
}

Void hypergraph_control::drag_stop				()
{
	m_b_drag_mode					= false;

	Rectangle	bounding_rect		= Rectangle(m_nodes[0]->position, m_nodes[0]->size);
	Rectangle	node_rect;

	for each (node^ node in m_nodes)
		bounding_rect	= Rectangle::Union(bounding_rect, Rectangle(node->position, node->size));

	m_inner_field_rect = bounding_rect;

	recalc_outer_field			();
	display_rectangle =			display_rectangle;
}

Void hypergraph_control::drag_update			()
{
	Point		pt			= PointToClient(MousePosition);
	Point		offs;
	offs.X					= pt.X - m_selection_mode_start_point.X;
	offs.Y					= pt.Y - m_selection_mode_start_point.Y;

	for each (node^ node in m_selected_nodes)
	{
		Point ctl_location		= node->position;
		ctl_location.X			= ctl_location.X + int(Math::Round(offs.X/m_scale));
		ctl_location.Y			= ctl_location.Y + int(Math::Round(offs.Y/m_scale));
		node->position			= ctl_location;
	}
	m_selection_mode_start_point= pt;
	Invalidate					(false);
}
Void hypergraph_control::move_start()
{
	m_b_graph_move_mode					= true;
	m_selection_mode_start_point		= PointToClient(MousePosition);
}

Void hypergraph_control::move_stop()
{
	m_b_graph_move_mode					= false;
}
Void hypergraph_control::move_update()
{
	Point pt					= PointToClient(MousePosition);
	Point offs;
	offs.X						= -(int)((pt.X - m_selection_mode_start_point.X)/scale);
	offs.Y						= -(int)((pt.Y - m_selection_mode_start_point.Y)/scale);

	if(offs.X != 0 || offs.Y != 0)
	{
		Drawing::Rectangle	rect	= display_rectangle;
		rect.Offset					(offs);
		display_rectangle			= rect;
		m_selection_mode_start_point= pt;
	}
}

Void	hypergraph_control::scale_start						()
{
	m_b_graph_scale_mode			= true;
	m_selection_mode_start_point	= PointToClient(MousePosition);
}

Void	hypergraph_control::scale_stop						()
{
	m_b_graph_scale_mode			= false;
}

Void	hypergraph_control::scale_update					()
{
	Point pt						= PointToClient(MousePosition);
	float scale						= (pt.X - m_selection_mode_start_point.X)/1000.0f;
	if(scale != 0)
	{
		this->scale						+= scale;
		m_selection_mode_start_point	= pt;
	}
}

void			hypergraph_control::display_rectangle::set(Drawing::Rectangle new_display_rectangle)
{
	if(	new_display_rectangle.X > m_inner_field_rect.X || new_display_rectangle.Right < m_inner_field_rect.Right)
	{
		if (show_scrollbars)
			m_h_scroll_bar_container->Visible				= true;
	}
	else
		m_h_scroll_bar_container->Visible				= false;
	
	if(new_display_rectangle.Y > m_inner_field_rect.Y || new_display_rectangle.Bottom < m_inner_field_rect.Bottom)
	{
		if (show_scrollbars){
			m_v_scroll->Visible								= true;
			m_corner_gizmo->Visible							= true;
		}
	}
	else
	{
		m_v_scroll->Visible								= false;
		m_corner_gizmo->Visible							= false;
	}

	m_display_rectangle = new_display_rectangle;
	SuspendLayout	();
	Invalidate		(false);
	for each(node^ nod in m_nodes)
	{
		nod->set_scale	(scale);
		nod->Size		= Drawing::Size((int)(nod->size.Width * scale), (int)(nod->size.Height * scale));
		nod->Location	= Point((int)((nod->position.X - m_display_rectangle.X) * scale), (int)((nod->position.Y - m_display_rectangle.Y) * scale)); 
	}
	ResumeLayout	();
	if(m_need_recalc)
		recalc_scrolls	();
}

void	hypergraph_control::recalc_scrolls					()
{
	Rectangle entire_field_rect = m_display_rectangle;
	entire_field_rect = Rectangle::Union(m_outer_field_rect, entire_field_rect);

	m_h_scroll->set_scrollbar((float)entire_field_rect.X, (float)entire_field_rect.Right, (float)m_display_rectangle.X, (float)m_display_rectangle.Width, false);
	m_v_scroll->set_scrollbar((float)entire_field_rect.Y, (float)entire_field_rect.Bottom, (float)m_display_rectangle.Y, (float)m_display_rectangle.Height, false);
}

void	hypergraph_control::recalc_outer_field				()
{
	m_outer_field_rect = m_inner_field_rect.Inflate(m_inner_field_rect, m_inner_to_outer_distance, m_inner_to_outer_distance);
	recalc_scrolls();
}

void	hypergraph_control::set_node_location				(node^ nod)
{
	nod->Location	= Point((int)((nod->position.X - m_display_rectangle.X) * scale), (int)((nod->position.Y - m_display_rectangle.Y) * scale));
	Rectangle node_rect			= Drawing::Rectangle(nod->position, nod->size);

	if(!m_b_drag_mode && !m_inner_field_rect.Contains(node_rect))
	{
		m_inner_field_rect			= Rectangle::Union(node_rect, m_inner_field_rect);
		recalc_outer_field			();
		display_rectangle			= display_rectangle;
	}
}

void	hypergraph_control::focus_on_node				(node^ nod){
	Point pt					= nod->position;
	Point offs;
	offs.X						= (int)((pt.X - display_rectangle.X)/scale) - 10;
	offs.Y						= (int)((pt.Y - display_rectangle.Y)/scale) - 10;

	if(offs.X != 0 || offs.Y != 0)
	{
		Drawing::Rectangle	rect	= display_rectangle;
		rect.Offset					(offs);
		display_rectangle			= rect;		
	}

}

void	hypergraph_control::center_nodes				()
{
	if(m_nodes->Count==0)
		return;

	Rectangle	bounding_rect		= Rectangle(m_nodes[0]->position, m_nodes[0]->size);
	for each (node^ node in m_nodes)
		bounding_rect	= Rectangle::Union(bounding_rect, Rectangle(node->position, node->size));

	int new_x = bounding_rect.X;
	int new_y = bounding_rect.Y;

	if(bounding_rect.Width < m_display_rectangle.Width)
		new_x = bounding_rect.X - (m_display_rectangle.Width - bounding_rect.Width)/2;
	if(bounding_rect.Height < m_display_rectangle.Height)
		new_y = bounding_rect.Y - (m_display_rectangle.Height - bounding_rect.Height)/2;
	m_display_rectangle.X = new_x;
	m_display_rectangle.Y = new_y;
	display_rectangle = m_display_rectangle;
}

void	hypergraph_control::OnControlAdded					(ControlEventArgs^ e)
{
	if(e->Control == m_h_scroll_bar_container || e->Control == m_h_scroll
		|| e->Control == m_corner_gizmo || e->Control == m_v_scroll)
		return;

	node^ new_node				= safe_cast<node^>(e->Control);

	m_h_scroll->BringToFront	();
	m_v_scroll->BringToFront	();

	Rectangle node_rect			= Drawing::Rectangle(new_node->position, new_node->size);
	if(Controls->Count == 3)
	{
		m_inner_field_rect			= node_rect;
		recalc_outer_field			();
		display_rectangle			= display_rectangle;
	}
	else if(!m_inner_field_rect.Contains(node_rect))
	{
		m_inner_field_rect			= Rectangle::Union(node_rect, m_inner_field_rect);
		recalc_outer_field			();
		display_rectangle			= display_rectangle;
	}
	UserControl::OnControlAdded(e);

	e->Control->Focus();
}

void	hypergraph_control::OnResize						(EventArgs^ e)
{
	if(m_outer_field_rect.Width != 0 && m_outer_field_rect.Height != 0)
	{
		//Boolean is_inner_rect_changed = false;

		display_rectangle = Drawing::Rectangle(
			m_display_rectangle.X,
			m_display_rectangle.Y,
			(int)(ClientRectangle.Width / scale),
			(int)(ClientRectangle.Height / scale));
	}
	UserControl::OnResize(e);
}

void	hypergraph_control::h_scroll_change					(Object^ , xray::editor::controls::scroller_event_args^ e)
{
	m_need_recalc				= false;
	Single new_scale			= ClientRectangle.Width / e->current_width;
	set_scale_impl				(new_scale);
	
	display_rectangle			= Drawing::Rectangle((int)e->current_position, m_display_rectangle.Y, (int)(ClientRectangle.Width / scale), (int)(ClientRectangle.Height / scale));
	m_h_scroll->set_scroller	(m_h_scroll->scroller_position, ClientRectangle.Width / scale, false);
	m_v_scroll->set_scroller	(m_v_scroll->scroller_position, ClientRectangle.Height / scale, false);
	m_need_recalc				= true;
}

void	hypergraph_control::v_scroll_change					(Object^ , xray::editor::controls::scroller_event_args^ e)
{
	m_need_recalc				= false;
	Single new_scale			= ClientRectangle.Height / e->current_width;
	set_scale_impl				(new_scale);

	display_rectangle			= Drawing::Rectangle(m_display_rectangle.X, (int)e->current_position, (int)(ClientRectangle.Width / scale), (int)(ClientRectangle.Height / scale));
	m_h_scroll->set_scroller	(m_h_scroll->scroller_position, ClientRectangle.Width / scale, false);
	m_v_scroll->set_scroller	(m_v_scroll->scroller_position, ClientRectangle.Height / scale, false);
	m_need_recalc				= true;
}

void	hypergraph_control::OnPaint							(PaintEventArgs^ e)
{
// 	Rectangle draw_rect = Rectangle(
// 		(int)((m_outer_field_rect.X - m_display_rectangle.X) * scale),
// 		(int)((m_outer_field_rect.Y - m_display_rectangle.Y) * scale),
// 		(int)(m_outer_field_rect.Width * scale),
// 		(int)(m_outer_field_rect.Height * scale));
// 
// 	e->Graphics->DrawRectangle(gcnew Pen(Color::Black), draw_rect);

	UserControl::OnPaint(e);
}

void	hypergraph_control::hypergraph_area_MouseDown		(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
	if(!ClientRectangle.Contains(e->Location))
		return; // ???

	if(ModifierKeys == Keys::Alt)
	{
		if(e->Button == System::Windows::Forms::MouseButtons::Middle)
			move_start();

		if(e->Button == System::Windows::Forms::MouseButtons::Right && scalable)
			scale_start();

		return;
	}

	node^ current_node							= get_node_under_cursor();
	connection_point^ current_connection_point	= nullptr;

	if(e->Button == System::Windows::Forms::MouseButtons::Left)
	{
		if(ModifierKeys == Keys::Control)
		{
			if(!selected_nodes->Contains(current_node))
				add_selected_node(current_node);
			else
			{
				selected_nodes->Remove(current_node);
				current_node->Invalidate(false);
				on_selection_changed();
				return;
			}
		}
		else
			if(!selected_nodes->Contains(current_node))
				set_selected_node(current_node);

		links^ lnks				= get_links_under_cursor();
		if(ModifierKeys == Keys::Control)
			m_selected_links->AddRange(lnks);
		else
		{
			m_selected_links->Clear();
			m_selected_links->AddRange(lnks);
		}

		m_selected_conn_point	= current_connection_point;

		if(selected_nodes->Count==0 && selected_links->Count==0 && !m_selected_conn_point)
		{ //start selection rect
			m_b_rect_selection_mode			= true;
			m_selection_mode_start_point	= e->Location;
		}
	}
	if(current_node)
	{
		Drawing::Point node_mouse_pos			= current_node->PointToClient			(MousePosition);
		current_connection_point				= current_node->pick_connection_point	(node_mouse_pos);

		switch(e->Button)
		{
		case System::Windows::Forms::MouseButtons::Left:
			{
				if(ModifierKeys == Keys::Shift)
				{
					m_start_shift_node = current_node;
					current_node->select_parents();
				}

				if(move_mode==nodes_movability::movable || (move_mode==nodes_movability::by_nodes_movability && current_node->movable))
					drag_start						();

				break;
			}
		case System::Windows::Forms::MouseButtons::Middle:
			{
				select_src_connection(current_node, current_connection_point);
				break;
			}
		}
	}
	Invalidate(false);
}

void hypergraph_control::hypergraph_area_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
	if(m_b_rect_selection_mode)
	{
		//just redraw selection region
		Invalidate	(false);
		return;
	}
	if(m_b_drag_mode)
	{
		drag_update();
		return;
	}

	if(m_b_graph_move_mode)
	{
		move_update();
		return;
	}

	if(m_b_graph_scale_mode)
	{
		scale_update();
		return;
	}

	node^ current_node		= get_node_under_cursor();
	focused_node			= current_node;

	if(m_src_conn_node)
		Invalidate(false);

	focused_link = get_link_under_cursor();
}

void hypergraph_control::hypergraph_area_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
	if(m_b_graph_move_mode)
		move_stop();

	if(m_b_graph_scale_mode)
		scale_stop();

	if(m_b_rect_selection_mode)
	{
		Drawing::Rectangle selection_rectangle = drawing2d::bound_rectangle(e->Location, m_selection_mode_start_point);
		set_selected_node(get_nodes(selection_rectangle));

		m_selected_links->Clear();
		m_selected_links->AddRange(get_links(selection_rectangle));
		on_selection_changed();

		m_b_rect_selection_mode = false;
		Invalidate				(false);
		return;
	}

	node^ destination_node = get_node_under_cursor();
	if(destination_node)
	{
		Drawing::Point pt						= destination_node->PointToClient			(MousePosition);
		connection_point^ dst_connection_point	= destination_node->pick_connection_point	(pt);

		switch(e->Button)
		{
			case System::Windows::Forms::MouseButtons::Left:
				if(move_mode==nodes_movability::movable)
					drag_stop();
				break;

			case System::Windows::Forms::MouseButtons::Middle:
				if(m_src_conn_node)
				{

					if(	m_src_conn_point && 
						dst_connection_point &&
						can_make_connection		(m_src_conn_point, dst_connection_point))
					{
						make_connection			(m_src_conn_point, dst_connection_point);
					}else
					{// show connection editor here
						if(connection_editor)
							connection_editor->execute(m_src_conn_node, destination_node);
					}
				}
				break;
		}
	}
	if(e->Button == System::Windows::Forms::MouseButtons::Middle)
		select_src_connection	(nullptr, nullptr);//reset

	if(m_b_drag_mode && e->Button == System::Windows::Forms::MouseButtons::Left)
		drag_stop();

	Invalidate				(false);
}

Void hypergraph_control::on_key_down(System::Object^ , System::Windows::Forms::KeyEventArgs^ e)
{
	if(e->KeyCode == Keys::F5)
		Invalidate(true);

	if(e->KeyCode == Keys::Delete)
	{
		if(selected_links->Count)
		{
			destroy_connections	(selected_links);
			m_selected_links->Clear();
		}
		if(selected_nodes->Count)
		{
			destroy_nodes		(selected_nodes);
			set_selected_node	((node^)nullptr);
		}
	}
}

Void hypergraph_control::on_key_up(System::Object^ , System::Windows::Forms::KeyEventArgs^ e)
{
	//if(e->KeyCode == Keys::ControlKey)
	//	

	if(e->KeyCode == Keys::Menu)
		m_b_graph_move_mode = false;
}

void  hypergraph_control::destroy_nodes(nodes^ nodes)
{
	for each(node^ node in nodes)
		destroy_node(node);
}

void  hypergraph_control::destroy_node(node^ node)
{
	remove_node			(node);
	links_manager->on_node_destroyed(node);
	node->on_destroyed	();
	Invalidate			(false);
}

void  hypergraph_control::destroy_connections(links^ links)
{
	for each(link^ link in links)
		destroy_connection(link);
}

void  hypergraph_control::destroy_connection(link^ link)
{
	links_manager->destroy_link(link);
	Invalidate			(false);
}

void  hypergraph_control::make_connection(connection_point^ pt_src, connection_point^ pt_dst)
{
	links_manager->create_link			(pt_src, pt_dst);
}

bool hypergraph_control::can_make_connection(connection_point^ psrc, connection_point^ pdst)
{
	return(	psrc && pdst						&&
			psrc != pdst						&&
			psrc->owner	!= pdst->owner			&&
			psrc->data_type == pdst->data_type	&&
			is_output(psrc->direction)			&&
			is_input(pdst->direction)
			);
}

void hypergraph_control::select_src_connection(node^ node, connection_point^ point)
{
	m_src_conn_point	= point;
	m_src_conn_node		= node;
}

Void hypergraph_control::hypergraph_Resize(System::Object^  sender, System::EventArgs^  e)
{
}

void hypergraph_control::do_layout()
{
	int offset_x	= 10;
	int offset_y	= 10;
	int max_y		= 0;
	Drawing::Point p(offset_x, offset_y);
	for each(node^ node in m_nodes)
	{
		Drawing::Size sz		= node->Size;
		Drawing::Point loc		= p;
		if(loc.X+sz.Width+offset_x > Width)
		{
			loc.X				= offset_x;
			loc.Y				= max_y + offset_y;
		}
		node->position			= loc;
		loc.X					+= sz.Width+offset_x;
		p						= loc;
		max_y					= Math::Max(max_y, node->Bottom);
	}
	Height		= Math::Max(Height, max_y+offset_y);
}

bool xray::editor::controls::hypergraph::is_output(connection_type t)
{
	return t==connection_type::inout || t==connection_type::output;
}

bool xray::editor::controls::hypergraph:: is_input(connection_type t)
{
	return t==connection_type::inout || t==connection_type::input;
}

Void hypergraph_control::onclick(System::Object^  sender, System::EventArgs^  e)
{
}

//-------------------------------------------
using xray::editor::controls::hypergraph::links_manager_default;

links_manager_default::links_manager_default(hypergraph_control^ h)
:m_hypergraph(h)
{
	m_links = gcnew links;
}

void links_manager_default::create_link(connection_point^ pt_src, connection_point^ pt_dst)
{
	link^ lnk				= gcnew link(pt_src, pt_dst);
	m_links->Add			(lnk);
}

void links_manager_default::destroy_link(link^ link)
{
	m_links->Remove			(link);
//	m_links->Remove			(link);
}

void links_manager_default::on_node_added(node^ node)
{}

void links_manager_default::on_node_removed(node^ node)
{}

void links_manager_default::on_node_destroyed(node^ node)
{
	links^ tmp			= gcnew links;
	tmp->AddRange		(m_links);
	for each(link^ link in tmp)
	{
		if(link->is_relative(node))
			m_links->Remove(link);
	}
	tmp->Clear();
}

links^ links_manager_default::visible_links()
{
	return m_links;
}

void links_manager_default::clear()
{
	m_links->Clear();
}

} // namespace hypergraph
} // namespace controls
} // namespace editor 
} // namespace xray