#include "pch.h"
#include "float_curve_editor.h"
#include "float_curve.h"

using xray::editor::controls::float_curve_editor;
using xray::editor::float_curve;

Void float_curve_editor::float_curve_editor_Paint(System::Object^, System::Windows::Forms::PaintEventArgs^  e)
{
	if(!m_curve)	return;
	cli::array<System::Drawing::RectangleF>^ rects = gcnew cli::array<System::Drawing::RectangleF>(m_curve->count );
	int count = m_curve->count;

	Drawing2D::Matrix^ M =  e->Graphics->Transform;

	Int32	DrawHeight			= grid_track_bar_visible?Height-max_grid_width_track_bar->Height:Height;

	float	sx					= (Width-m_point_rect_sz.Width) / (m_curve->max_time-m_curve->min_time);
	float	sy					= (DrawHeight-m_point_rect_sz.Height) / (m_curve->max_value-m_curve->min_value);
	M->Scale					(sx, sy);

	//Draw grid
	Int32	grid_size_h			= math::floor(m_curve->max_time-m_curve->min_time);
	Int32	grid_size_v			= math::floor(m_curve->max_value-m_curve->min_value);

	Int32	h_space				= math::ceil(grid_size_h/((Width-m_point_rect_sz.Width)/m_grid_min_h_line_spacing)/m_grid_h_step)*m_grid_h_step;
	Int32	v_space				= math::ceil(grid_size_v/((DrawHeight-m_point_rect_sz.Height)/m_grid_min_v_line_spacing)/m_grid_v_step)*m_grid_v_step;
	cli::array<System::Drawing::PointF>^	grid_lines = gcnew cli::array<System::Drawing::PointF>((grid_size_h/h_space+grid_size_v/v_space+2)*2);

	int i=0;
	for(;i<=grid_size_h/h_space; ++i)
	{
		grid_lines[i*2]			= System::Drawing::PointF(i*(float)h_space, m_curve->min_value/2);
		grid_lines[i*2+1]		= System::Drawing::PointF(i*(float)h_space, m_curve->max_value*2);
	}

	for(int j=0;j<=grid_size_v/v_space; ++j)
	{
		grid_lines[i*2+j*2]		= System::Drawing::PointF(m_curve->min_time/2, (float)(j*v_space));
		grid_lines[i*2+j*2+1]	= System::Drawing::PointF(m_curve->max_time*2, (float)(j*v_space));
	}

	M->TransformPoints(grid_lines);
	
	for(int n=0; n<i*2; n+=2)
	{
		e->Graphics->DrawLine	(m_mesh_pen, grid_lines[n], grid_lines[n+1]);
		e->Graphics->DrawString	((n/2*h_space).ToString(), m_grid_font, m_grid_font_brush, grid_lines[n]);
	}
	if(grid_track_bar_visible)
		e->Graphics->DrawLine	(m_mesh_pen, 0, Height-max_grid_width_track_bar->Height-3, Width, Height-max_grid_width_track_bar->Height-3);
	for(int n=i*2; n<grid_lines->Length; n+=2)
	{
		e->Graphics->DrawLine	(m_mesh_pen, grid_lines[n], grid_lines[n+1]);
		e->Graphics->DrawString	(((n-i*2)/2*v_space).ToString(), m_grid_font, m_grid_font_brush, grid_lines[n]);
	}

	//Draw points
	for(int idx=0; idx<count; ++idx)
	{
		float_curve_pair^ pair	= m_curve->key[idx];
		PointF	pt				= PointF(pair->time, pair->value);
		RectangleF	r			(pt.X*sx, pt.Y*sy, m_point_rect_sz.Width, m_point_rect_sz.Height);
		r.Offset				(-m_point_rect_sz.Width/2.0f, -m_point_rect_sz.Height/2.0f);
		rects[idx]	= r;
	}
	
	m_curve->draw(e->Graphics, m_pen, M);
	
	e->Graphics->DrawRectangles	(m_pen, rects);
}