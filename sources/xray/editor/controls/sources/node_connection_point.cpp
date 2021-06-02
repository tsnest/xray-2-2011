#include "pch.h"
#include "node_connection_point.h"
#include "drawing2d_utils.h"


using xray::editor::controls::hypergraph::connection_point;
using xray::editor::controls::hypergraph::node;
using xray::editor::controls::hypergraph::link;
using xray::editor::controls::hypergraph::point_align;
using xray::editor::controls::hypergraph::connection_type;
using xray::editor::controls::hypergraph::connection_point_style;
using xray::editor::controls::hypergraph::nodes_link_style;

connection_point::connection_point(node^ o_owner, System::Type^ t, connection_point_style^ st)
:m_owner(o_owner),
m_data_type(t),
m_style(st)
{
	draw_text = true;
}

Point connection_point::get_point()
{
	// relative to parent node
	Point result = m_style->rect.Location;
	result.X += m_style->rect.Width/2;
	result.Y += m_style->rect.Height/2;
	return result;
}

bool link::visible()
{
	return destination_node->Visible && source_node->Visible;
}

bool link::pick(Point p)
{
	double pick_eps	= 5.0f;
	Point pt_src	= source_node->Parent->PointToClient(source_node->get_point_location(source_point));
	Point pt_dst	= destination_node->Parent->PointToClient(destination_node->get_point_location(destination_point));

	if(Math::Abs(pt_src.X-pt_dst.X)<pick_eps)
	{
		if(pt_src.X<pt_dst.X)
		{
			pt_src.X -= int(Math::Ceiling(pick_eps/2));
			pt_dst.X += int(Math::Ceiling(pick_eps/2));
		}
		else
		{
			pt_src.X += int(Math::Ceiling(pick_eps/2));
			pt_dst.X -= int(Math::Ceiling(pick_eps/2));
		}
	}
	else if(Math::Abs(pt_src.Y-pt_dst.Y)<pick_eps)
	{
		if(pt_src.Y<pt_dst.Y)
		{
			pt_src.Y -= int(Math::Ceiling(pick_eps/2));
			pt_dst.Y += int(Math::Ceiling(pick_eps/2));
		}
		else
		{
			pt_src.Y += int(Math::Ceiling(pick_eps/2));
			pt_dst.Y -= int(Math::Ceiling(pick_eps/2));
		}
	}

	if(!drawing2d::is_between(pt_src, pt_dst, p))
		return false;

	double t;
	t= Math::Pow(( (p.X-pt_src.X)*(pt_dst.Y-pt_src.Y)-(p.Y-pt_src.Y)*(pt_dst.X-pt_src.X)),2)/
			 (Math::Pow((pt_dst.X-pt_src.X),2) + Math::Pow((pt_dst.Y-pt_src.Y),2));

	double dist = Math::Sqrt(t);
	
	return dist<pick_eps;
}


link::link(connection_point^ src, connection_point^ dst)
{
	source_point		= src;
	destination_point	= dst;
}

bool link::intersect(Drawing::Rectangle r)
{
	Point pt_src	= source_node->Parent->PointToClient(source_node->get_point_location(source_point));
	Point pt_dst	= destination_node->Parent->PointToClient(destination_node->get_point_location(destination_point));

	//fast check on bound rect
	if(r.Contains(pt_src) || r.Contains(pt_dst))
		return true;

	if(!r.IntersectsWith(drawing2d::bound_rectangle(pt_src, pt_dst)))
		return false;

	return
	drawing2d::segments_intersect(pt_src, pt_dst, r.Location, Drawing::Point(r.Right, r.Top))		||
	drawing2d::segments_intersect(pt_src, pt_dst, Drawing::Point(r.Right, r.Top), Drawing::Point(r.Right, r.Bottom)) ||
	drawing2d::segments_intersect(pt_src, pt_dst, Drawing::Point(r.Right, r.Bottom), Drawing::Point(r.Left, r.Bottom))	||
	drawing2d::segments_intersect(pt_src, pt_dst, Drawing::Point(r.Left, r.Bottom), r.Location);
}

void link::draw(Drawing::Graphics^ g, Drawing::Pen^ pen, nodes_link_style link_style)
{
	Point pt_src		= source_node->get_point_location(source_point);
	Point pt_dst		= destination_node->get_point_location(destination_point);
	pt_src				= source_node->Parent->PointToClient(pt_src);
	pt_dst				= source_node->Parent->PointToClient(pt_dst);
	if (link_style == nodes_link_style::arrow)
		drawing2d::draw_arrow(pt_src, pt_dst, pen, g);
	else if (link_style == nodes_link_style::line)
		drawing2d::draw_line(pt_src, pt_dst, pen, g);
}

bool link::endpoints_equal(link^ lnk)
{
	return (source_node==lnk->source_node) && (destination_node==lnk->destination_node);
}

bool link::is_relative(node^ node)
{
	return (source_node==node) || (destination_node==node);
}