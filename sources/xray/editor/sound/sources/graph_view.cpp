////////////////////////////////////////////////////////////////////////////
//	Created		: 15.01.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "graph_view.h"

using xray::sound_editor::graph_view;

graph_view::graph_view(int freq)
{
	m_points_frequency = freq;
	InitializeComponent();
	in_constructor();
}

graph_view::~graph_view()
{
	delete m_graph_points;
	if(components)
		delete components;
}

void graph_view::in_constructor()
{
	m_colors = gcnew colors_list();
	m_colors->Add(Drawing::Color::Black);
	m_colors->Add(Drawing::Color::Red);
	m_colors->Add(Drawing::Color::Blue);
	m_colors->Add(Drawing::Color::Green);
	m_colors->Add(Drawing::Color::Yellow);
	m_colors->Add(Drawing::Color::White);
	m_colors->Add(Drawing::Color::Orange);
	this->ResizeRedraw = true;
	m_graph_points = gcnew points_list();
	m_pen = gcnew Pen(Drawing::Color::Green);
	m_mesh_pen = gcnew Pen(Drawing::Color::Gray);
	m_grid_font = gcnew DFont("Arial", 6);
	m_grid_font_brush = gcnew Drawing::SolidBrush(Drawing::Color::Gray);
	clear();
}

void graph_view::clear()
{
	m_graph_points->Clear();
	m_max_graph_x = 0;
	m_max_graph_y = 0;
	m_min_graph_x = 1;
	m_min_graph_y = 1;
	m_max_view_x = m_max_graph_x;
	m_min_view_x = m_min_graph_x;
	m_max_view_y = m_max_graph_y;
	m_min_view_y = m_min_graph_y;
	Invalidate();
}

void graph_view::add_graph_points(array<Single>^ points)
{
	int index = 0;
	array<PointF>^ pnts = gcnew array<PointF>(points->Length);
	float x = (points->Length-1.0f)/m_points_frequency;
	m_max_graph_x = (m_max_graph_x>x) ? m_max_graph_x : x;
	m_min_graph_x = 0.0f;
	m_max_graph_y = (m_graph_points->Count>0) ? m_max_graph_y : Single::MinValue;
	m_min_graph_y = (m_graph_points->Count>0) ? m_min_graph_y : Single::MaxValue;
	for each(Single point in points)
	{
		pnts[index++] = PointF((float)index/m_points_frequency, point);
		if(m_max_graph_y<point)
			m_max_graph_y = point;

		if(m_min_graph_y>point)
			m_min_graph_y = point;
	}

	m_graph_points->Add(pnts);
	m_max_view_x = m_max_graph_x;
	m_min_view_x = m_min_graph_x;
	m_max_view_y = m_max_graph_y;
	m_min_view_y = m_min_graph_y;
	Invalidate();
}

void graph_view::graph_view_Paint(Object^, PaintEventArgs^e)
{
	if(!DesignMode)
	{
		e->Graphics->Transform = gcnew Drawing2D::Matrix(1, 0, 0, 1, 5, 5);
		draw_with_graphics(
			e->Graphics, 
			Drawing::Size(ClientSize.Width-10, ClientSize.Height-10), 
			true, 
			true,
			m_min_view_x, 
			m_max_view_x, 
			m_min_view_y, 
			m_max_view_y
		);
	}
}

void graph_view::draw_with_graphics(Graphics^ dest_surface, Boolean draw_grid, Boolean draw_0_line)
{
	draw_with_graphics(
		dest_surface, 
		dest_surface->VisibleClipBounds.Size.ToSize(),
		draw_grid, 
		draw_0_line,
		m_min_graph_x, 
		m_max_graph_x, 
		m_min_graph_y, 
		m_max_graph_y
	);
}	

void graph_view::draw_with_graphics(
	Graphics^ dest_surface,
	Drawing::Size dest_surface_size,
	Boolean draw_grid,
	Boolean draw_0_line,
	Single min_x, 
	Single max_x, 
	Single min_y, 
	Single max_y
)
{
	//Define and Initialize points array
	if(!m_graph_points || m_graph_points->Count==0)
		return;

	for(int i=0; i<m_graph_points->Count; ++i)
	{
		array<PointF>^ points = safe_cast<array<PointF>^>(m_graph_points[i]->Clone());
		m_pen->Color = m_colors[i%7];
		//Set grapth dimentions and offsets
		Int32 DrawWidth = dest_surface_size.Width;
		Int32 DrawHeight = dest_surface_size.Height;
		//Compute transform matrix
		Single x_scale = DrawWidth/(max_x-min_x);
		Single y_scale = DrawHeight/(max_y-min_y);
		Drawing2D::Matrix^ M = gcnew Drawing2D::Matrix(x_scale, 0, 0, -y_scale, -min_x*x_scale, y_scale*max_y);
		M->TransformPoints(points);
		//Make and draw zero line
		if(draw_0_line)
		{
			array<PointF>^ line0 = gcnew array<PointF>(2);
			line0[0] = PointF(min_x, 0);
			line0[1] = PointF(max_x, 0);
			M->TransformPoints(line0);
			dest_surface->DrawLine(m_mesh_pen, line0[0], line0[1]);
		}
		
		float m_grid_min_h_line_spacing = 15;
		float m_grid_min_v_line_spacing = 15;
		Int32 m_grid_h_step = 2;
		Int32 m_grid_v_step	= 10;
		//Compute grid
		if(draw_grid)
		{
			Single grid_size_h = max_x-min_x;
			Single grid_size_v = max_y-min_y;
			Int32 h_space = math::ceil(grid_size_h/(DrawWidth/m_grid_min_h_line_spacing)/m_grid_h_step)*m_grid_h_step;
			Int32 v_space = math::ceil(grid_size_v/(DrawHeight/m_grid_min_v_line_spacing)/m_grid_v_step)*m_grid_v_step;
			cli::array<System::Drawing::PointF>^ grid_lines = gcnew cli::array<System::Drawing::PointF>((math::floor(grid_size_h)/h_space+math::floor(grid_size_v)/v_space+2)*2);
			int x = 0;
			int x_start;
			for(int i=x_start=math::ceil(min_x/h_space); i<=max_x/h_space; ++i)
			{
				grid_lines[x*2] = System::Drawing::PointF(i*(float)h_space, min_y);
				grid_lines[x*2+1] = System::Drawing::PointF(i*(float)h_space, max_y);
				++x;
			}

			int y = 0;
			int y_start;
			for(int j=y_start=math::ceil(min_y/v_space) ;j<=max_y/v_space; ++j)
			{
				grid_lines[x*2+y*2] = System::Drawing::PointF(min_x, (float)(j*v_space));
				grid_lines[x*2+y*2+1] = System::Drawing::PointF(max_x, (float)(j*v_space));
				++y;
			}
			
			//Transform grid lines to absolute coordinates
			M->TransformPoints(grid_lines);
			//Draw Grid
			for(int n=0; n<x*2; n+=2)
			{
				dest_surface->DrawLine(m_mesh_pen, grid_lines[n], grid_lines[n+1]);
				dest_surface->DrawString(((x_start+(n/2))*h_space).ToString(), m_grid_font, m_grid_font_brush, grid_lines[n+1]);
			}

			for(int n=x*2; n<grid_lines->Length; n+=2)
			{
				dest_surface->DrawLine(m_mesh_pen, grid_lines[n], grid_lines[n+1]);
				dest_surface->DrawString(((y_start+(n-x*2)/2)*v_space).ToString(), m_grid_font, m_grid_font_brush, grid_lines[n]);
			}
		}

		//Draw points
		dest_surface->DrawLines(m_pen, points);
	}
}