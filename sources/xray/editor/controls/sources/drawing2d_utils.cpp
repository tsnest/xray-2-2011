////////////////////////////////////////////////////////////////////////////
//	Created		: 25.11.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "drawing2d_utils.h"

using System::Drawing::Rectangle;
using System::Drawing::Point;
using System::Drawing::PointF;
using System::Drawing::Pen;
using System::Drawing::Graphics;
using System::Math;

namespace xray {
namespace editor {
namespace drawing2d{

void draw_line(	Point pt_start, 
				Point pt_end, 
				Pen^ pen,
				Graphics^ g)
{
	g->DrawLine(pen, pt_start, pt_end);
}

void draw_arrow(	Point pt_start, 
					Point pt_end, 
					Pen^ pen,
					Graphics^ g)
{
	draw_line	(pt_start, pt_end, pen, g);

	//draw arrow
	PointF p( (pt_end.X-pt_start.X)*0.6f + pt_start.X, (pt_end.Y-pt_start.Y)*0.6f + pt_start.Y );
	
	PointF pdir( float(pt_end.X-pt_start.X), float(pt_end.Y-pt_start.Y));
	float len	= (float)Math::Sqrt(pdir.X*pdir.X+pdir.Y*pdir.Y);
	len			= 5.0f/len;
	pdir.X		*= len;
	pdir.Y		*= len;
	
	PointF pe	= p;
	pe.X		+= pdir.X*2;
	pe.Y		+= pdir.Y*2;

	PointF l	= p;
	l.X		+= pdir.Y;
	l.Y		+= -pdir.X;
	g->DrawLine(pen, l, pe);

	PointF r	= p;
	r.X		+= -pdir.Y;
	r.Y		+= pdir.X;
	g->DrawLine(pen, r, pe);

	g->DrawLine(pen, r, l);
	g->FillPolygon(pen->Brush, gcnew cli::array< PointF>(3) {l, r, pe});
}

Rectangle bound_rectangle(Point p1, Point p2)
{
	int x1		= Math::Min(p1.X, p2.X);
	int y1		= Math::Min(p1.Y, p2.Y);
	int x2		= Math::Max(p1.X, p2.X);
	int y2		= Math::Max(p1.Y, p2.Y);
	return Rectangle (x1, y1, x2-x1, y2-y1);
}

bool is_between(Point pa, Point pb, Point pc)
{
	int min_x = Math::Min(pa.X, pb.X);
	int max_x = Math::Max(pa.X, pb.X);
	int min_y = Math::Min(pa.Y, pb.Y);
	int max_y = Math::Max(pa.Y, pb.Y);
	bool result = 
		((min_x <= pc.X) && (max_x >= pc.X)) &&
		((min_y <= pc.Y) && (max_y >= pc.Y));
		
	return result;
}

int calc_area(Point pa, Point pb, Point pc)
{
	return (pa.X-pb.X)*(pc.Y-pa.Y)-(pa.Y-pb.Y)*(pc.X-pa.X);
}

bool segments_intersect(Point pa, Point pb, Point pc, Point pd)
{ //pa, pb -first segment; pc, pd-second
	//Drawing::Rectangle bound1 = bound_rectangle(pa, pb);
	//Drawing::Rectangle bound2 = bound_rectangle(pc, pd);
	//if(!bound1.IntersectsWith(bound2))
	//	return false;

	int s1 = calc_area(pc, pd, pa);
	int s2 = calc_area(pc, pd, pb);
	int s3 = calc_area(pa, pb, pc);
	int s4 = calc_area(pa, pb, pd);
	if(
		((s1>0 && s2<0) || (s1<0 && s2>0)) &&
		((s3>0 && s4<0) || (s3<0 && s4>0))
		)
		return true;
	else
	if(s1==0 && is_between(pc, pd, pa))
		return true;
	else
	if(s2==0 && is_between(pc, pd, pb))
		return true;
	else
	if(s3==0 && is_between(pa, pb, pc))
		return true;
	else
	if(s4==0 && is_between(pa, pb, pd))
		return true;
	else 
		return false;
}

};// namespace drawing2d
};// namespace editor
};// namespace xray
