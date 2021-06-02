////////////////////////////////////////////////////////////////////////////
//	Created		: 19.10.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_viewer_graph_link_timed.h"
#include "time_instance.h"

using xray::animation_editor::animation_viewer_graph_link_timed;
using namespace System::Drawing;

animation_viewer_graph_link_timed::animation_viewer_graph_link_timed(connection_point^ src, connection_point^ dst)
:super(src, dst)
{
	m_time = gcnew time_instance(this);
	m_container->owner = this;
	m_container->properties->add_from_obj(this);
}

animation_viewer_graph_link_timed::~animation_viewer_graph_link_timed()
{
	delete m_time;
	delete m_container;
}

void animation_viewer_graph_link_timed::load(xray::configs::lua_config_value const& cfg)
{
	super::load(cfg);
	m_time->time_s = (float)cfg["playing_time"];
}

void animation_viewer_graph_link_timed::save(xray::configs::lua_config_value cfg)
{
	super::save(cfg);
	cfg["playing_time"] = m_time->time_s;
}

void animation_viewer_graph_link_timed::draw(System::Drawing::Graphics^ g, System::Drawing::Pen^ pen, xray::editor::controls::hypergraph::nodes_link_style link_style)
{
	super::draw(g, pen, link_style);
	SolidBrush^ bk_brush = gcnew SolidBrush(Color::White);
	SolidBrush^ txt_brush = gcnew SolidBrush(Color::Black);
	Point pt_src = source_node->Parent->PointToClient(source_node->get_point_location(source_point));
	Point pt_dst = source_node->Parent->PointToClient(destination_node->get_point_location(destination_point));
	PointF pos = PointF(0, 0);

	System::String^ time_string = ((float)System::Math::Round(m_time->time_f, 3)).ToString();
	time_string = time_string + " frm";
	SizeF sz = g->MeasureString(time_string, source_node->get_style()->font);
	pos.X = pt_src.X + (pt_dst.X - pt_src.X)/3 - sz.Width/2;
	pos.Y = pt_src.Y + (pt_dst.Y - pt_src.Y)/3 - sz.Height;
	RectangleF rect = RectangleF(pos, sz);
	rect.Inflate(2.0f, 2.0f);
	g->FillRectangle(bk_brush, rect);
	g->DrawString(time_string, source_node->get_style()->font, txt_brush, pos);

	time_string = ((float)System::Math::Round(m_time->time_s, 3)).ToString();
	time_string = time_string + " sec";
	sz = g->MeasureString(time_string, source_node->get_style()->font);
	pos.X = pt_src.X + (pt_dst.X - pt_src.X)/3 - sz.Width/2;
	pos.Y = (float)pt_src.Y + (pt_dst.Y - pt_src.Y)/3;
	rect = RectangleF(pos, sz);
	rect.Inflate(2.0f, 2.0f);
	g->FillRectangle(bk_brush, rect);
	g->DrawString(time_string, source_node->get_style()->font, txt_brush, pos);
}

xray::animation_editor::animation_viewer_graph_link_timed::wpf_property_container^ animation_viewer_graph_link_timed::playing_time::get()
{
	return m_time->container();
}
