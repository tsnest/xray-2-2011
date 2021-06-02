////////////////////////////////////////////////////////////////////////////
//	Created		: 07.09.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_viewer_graph_link_weighted.h"
#include "animation_viewer_graph_node_operator.h"
#include "animation_node_interpolator.h"

using xray::animation_editor::animation_viewer_graph_link_weighted;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace xray::animation;
using xray::editor::wpf_controls::property_descriptor;

animation_viewer_graph_link_weighted::animation_viewer_graph_link_weighted(connection_point^ src, connection_point^ dst)
:super(src, dst)
{
	weight = 0.0f;
	m_interpolator = gcnew animation_node_interpolator(this);
	m_container->owner = this;
	m_container->properties->add_from_obj(this);

	property_descriptor^ desc = interpolator->properties["Node properties/type"];
	interpolator->properties->remove(desc);
	interpolator->inner_value = desc;
}

animation_viewer_graph_link_weighted::~animation_viewer_graph_link_weighted()
{
	delete m_container;
	delete m_interpolator;
}

void animation_viewer_graph_link_weighted::load(xray::configs::lua_config_value const& cfg)
{
	super::load(cfg);
	weight = (float)cfg["weight"];
	m_interpolator->load(cfg);
}

void animation_viewer_graph_link_weighted::save(xray::configs::lua_config_value cfg)
{
	super::save(cfg);
	cfg["weight"] = weight;
	m_interpolator->save(cfg);
}

void animation_viewer_graph_link_weighted::draw(System::Drawing::Graphics^ g, System::Drawing::Pen^ pen, xray::editor::controls::hypergraph::nodes_link_style link_style)
{
	super::draw(g, pen, link_style);
	System::String^ m_koef_str = weight.ToString();
	System::Drawing::SizeF m_koef_str_size = g->MeasureString(m_koef_str, source_node->get_style()->font);

	System::Drawing::PointF m_koef_str_pos = System::Drawing::PointF(0, 0);
	Point pt_src = source_node->Parent->PointToClient(source_node->get_point_location(source_point));
	Point pt_dst = source_node->Parent->PointToClient(destination_node->get_point_location(destination_point));
	m_koef_str_pos.X = pt_src.X + (pt_dst.X - pt_src.X)/3 - m_koef_str_size.Width/2;
	m_koef_str_pos.Y = pt_src.Y + (pt_dst.Y - pt_src.Y)/3 - m_koef_str_size.Height/2;

	System::Drawing::RectangleF rect = System::Drawing::RectangleF(m_koef_str_pos, m_koef_str_size);
	rect.Inflate(2.0f, 2.0f);
	System::Drawing::SolidBrush^ brush = gcnew System::Drawing::SolidBrush(System::Drawing::Color::White);
	g->FillRectangle(brush, rect);
	brush->Color = System::Drawing::Color::Black;
	g->DrawString(m_koef_str, source_node->get_style()->font, brush, m_koef_str_pos);
}

animation_viewer_graph_link_weighted::wpf_property_container^ animation_viewer_graph_link_weighted::interpolator::get()
{
	return m_interpolator->container();
}