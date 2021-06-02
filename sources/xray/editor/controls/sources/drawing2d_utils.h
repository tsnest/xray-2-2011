////////////////////////////////////////////////////////////////////////////
//	Created		: 25.11.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef DRAWING2D_UTILS_H_INCLUDED
#define DRAWING2D_UTILS_H_INCLUDED

namespace xray {
namespace editor {
namespace drawing2d{

void draw_line(		System::Drawing::Point pt_start, 
					System::Drawing::Point pt_end, 
					System::Drawing::Pen^ pen,
					System::Drawing::Graphics^ g);

void draw_arrow(	System::Drawing::Point pt_start, 
					System::Drawing::Point pt_end, 
					System::Drawing::Pen^ pen,
					System::Drawing::Graphics^ g);

System::Drawing::Rectangle bound_rectangle(System::Drawing::Point p1, System::Drawing::Point p2);
bool is_between(System::Drawing::Point pa, System::Drawing::Point pb, System::Drawing::Point pc);

// double area of triangle pa-pb-pc. 
// signed value.
// negative if triangle (pa-pb-pc) has clockwise orientation, positive otherwise
int calc_area(System::Drawing::Point pa, System::Drawing::Point pb, System::Drawing::Point pc);

bool segments_intersect(System::Drawing::Point pa, System::Drawing::Point pb, System::Drawing::Point pc, System::Drawing::Point pd);
}; //namespace drawing2d
}; // namespace editor
}; // namespace xray

#endif // #ifndef DRAWING2D_UTILS_H_INCLUDED