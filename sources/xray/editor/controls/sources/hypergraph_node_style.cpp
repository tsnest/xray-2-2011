////////////////////////////////////////////////////////////////////////////
//	Created		: 7.12.2009
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "hypergraph_node_style.h"

using xray::editor::controls::hypergraph::node_style;
using xray::editor::controls::hypergraph::connection_point_style;
////////////////////////////////////////////////////////////////////////////
//- class connection_point_style ------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
connection_point_style::connection_point_style()
{
	name = "default_connection_point";
	m_rect = Rectangle(0,30,10,10);
	m_base_rect = Rectangle(0,30,10,10);
	dir = connection_type::input;
	al = point_align::left;
	active_color = Color::White;
	highlight_color = Color::White;
	inactive_color = Color::Transparent;
	m_scale = 1.0f;
}

void connection_point_style::set_scale_impl(float scl)
{
	m_scale = scl;
	m_rect.X = int((m_base_rect.X)*m_scale);
	m_rect.Y = int((m_base_rect.Y)*m_scale);
	m_rect.Width = int(m_base_rect.Width*m_scale);
	m_rect.Height = int(m_base_rect.Height*m_scale);
}

void connection_point_style::set_rect_impl(System::Drawing::Rectangle r)
{
	m_rect.X = int((r.X)*m_scale);
	m_rect.Y = int((r.Y)*m_scale);
	m_rect.Width = int(r.Width*m_scale);
	m_rect.Height = int(r.Height*m_scale);
}

void connection_point_style::clone(connection_point_style^ st)
{
	name = st->name;
	m_scale = st->scale;
	m_base_rect = st->base_rect;
	set_rect_impl(m_base_rect);
	dir = st->dir;
	al = st->al;
	active_color = st->active_color;
	highlight_color = st->highlight_color;
	inactive_color = st->inactive_color;
}
////////////////////////////////////////////////////////////////////////////
//- class node_style ------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
node_style::node_style()
{
	name				= "default_node";
	font				= gcnew Drawing::Font(L"Microsoft Sans Serif", 
							9, 
							Drawing::FontStyle::Regular, 
							Drawing::GraphicsUnit::Point, 
							static_cast<System::Byte>(204));
	connection_points_font= gcnew Drawing::Font(L"Microsoft Sans Serif", 
							9, 
							Drawing::FontStyle::Regular, 
							Drawing::GraphicsUnit::Point, 
							static_cast<System::Byte>(204));
	border_pen			= gcnew Drawing::Pen(Color::Black);
	brush				= gcnew Drawing::SolidBrush(Color::Transparent);
	mode				= view_mode::brief;	
	fmt					= gcnew System::Drawing::StringFormat();
	fmt->LineAlignment	= System::Drawing::StringAlignment::Near;
	fmt->Alignment		= System::Drawing::StringAlignment::Near;
	full_size			= gcnew System::Drawing::Size(120, 35);
	brief_size			= gcnew System::Drawing::Size(80, 40);

	m_color_node_active			= Color::White;
	m_color_node_highlighted	= Color::White;
	m_color_node_inactive		= Color::Transparent;
	m_color_link_active			= Color::Black;
	m_color_link_highlighted	= Color::Black;
	m_color_link_inactive		= Color::Black;
	m_max_point_size			= System::Drawing::Size(0,0);
	m_point_styles				= gcnew System::Collections::ArrayList();

	m_point_cached		= true;
	m_scale				= 1.0f;
}

Color node_style::get_node_color(e_states st)
{
	switch (st)
	{
		case e_states::active: return m_color_node_active;
		case e_states::highlihted: return m_color_node_highlighted;
		case e_states::inactive: return m_color_node_inactive;
	}

	return Color::Black;
}

Color node_style::get_link_color(e_states st)
{
	switch (st)
	{
	case e_states::active: return m_color_link_active;
	case e_states::highlihted: return m_color_link_highlighted;
	case e_states::inactive: return m_color_link_inactive;
	}

	return Color::Black;
}

Color node_style::get_point_color(System::String^ n, e_states st)
{
	connection_point_style^ res = nullptr;
	for each(connection_point_style^ st in m_point_styles)
	{
		if(st->name==n)
			res = st;
	}

	switch (st)
	{
		case e_states::active: return res->active_color;
		case e_states::highlihted: return res->highlight_color;
		case e_states::inactive: return res->inactive_color;
	}

	return Color::Black;
}

void node_style::set_brush_color(e_states st)
{
	brush->Color = get_node_color(st);
}

System::Drawing::Size node_style::get_max_point_size()
{
	if(m_point_cached)
		return m_max_point_size;

	m_max_point_size = System::Drawing::Size(0,0);
	for each(connection_point_style^ st in m_point_styles)
	{
		if(st->rect.Height>m_max_point_size.Height*m_scale)
			m_max_point_size.Height = int(st->rect.Height);

		if(st->rect.Width>m_max_point_size.Width*m_scale)
			m_max_point_size.Width = int(st->rect.Width);
	}

	m_point_cached = true;
	return m_max_point_size;
}

void node_style::add_new_style(connection_point_style^ st)
{
	m_point_cached = false;
	st->scale = m_scale;
	m_point_styles->Add(st);
}

connection_point_style^ node_style::get_point_style(System::String^ style)
{
	for each(connection_point_style^ st in m_point_styles)
	{
		if(st->name==style)
			return st;
	}
	return nullptr;
}

void node_style::set_scale_impl(float scl)
{
	m_point_cached = false;
	m_scale = scl;
}