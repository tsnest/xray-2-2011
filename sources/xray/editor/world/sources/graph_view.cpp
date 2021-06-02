#include "pch.h"
#include "graph_view.h"

using namespace System;
using namespace System::Drawing;

namespace xray
{
	namespace editor
	{
		void				graph_view::in_constructor()
		{
			this->ResizeRedraw  = true;

			m_graph_points		= gcnew array<PointF>(5);
			m_graph_points[0]	= PointF(-24, 35);
			m_graph_points[1]	= PointF(-6, -20);
			m_graph_points[2]	= PointF(12, 0);
			m_graph_points[3]	= PointF(24, -30);
			m_graph_points[4]	= PointF(32, 40);
			set_graph_points	(m_graph_points);

			m_pen				= gcnew Pen(Color::Green);
			m_mesh_pen			= gcnew Pen(Color::Gray);
			m_grid_font			= gcnew DFont("Arial", 6);
			m_grid_font_brush	= gcnew Drawing::SolidBrush(Drawing::Color::Gray);
		}

		void				graph_view::set_graph_points	(array<PointF>^ points)
		{
			m_graph_points		= points;
			m_graph_samples		= nullptr;

			m_max_graph_x 		= (m_max_graph_y = Single::MinValue);
			m_min_graph_x 		= (m_min_graph_y = Single::MaxValue);

			if(points != nullptr)
				for each(PointF point in points)
				{
					if(m_max_graph_x < point.X)m_max_graph_x = point.X;
					if(m_min_graph_x > point.X)m_min_graph_x = point.X;
					if(m_max_graph_y < point.Y)m_max_graph_y = point.Y;
					if(m_min_graph_y > point.Y)m_min_graph_y = point.Y;
				}

			m_max_view_x = m_max_graph_x;
			m_min_view_x = m_min_graph_x;
			m_max_view_y = m_max_graph_y;
			m_min_view_y = m_min_graph_y;

			Invalidate();
		}
		void				graph_view::set_graph_points	(array<Single>^ samples, Int32 points_frequency)
		{
			m_graph_points		= nullptr;
			m_graph_samples		= samples;
			m_points_frequency	= points_frequency;

			m_max_graph_x = (samples->Length-1.0f)/points_frequency;
			m_min_graph_x = 0;
			m_max_graph_y = Single::MinValue;
			m_min_graph_y = Single::MaxValue;

			for each(Single sample in samples)
			{
				if(m_max_graph_y < sample)m_max_graph_y = sample;
				if(m_min_graph_y > sample)m_min_graph_y = sample;
			}

			m_max_view_x = m_max_graph_x;
			m_min_view_x = m_min_graph_x;
			m_max_view_y = m_max_graph_y;
			m_min_view_y = m_min_graph_y;

			Invalidate();
		}

		void				graph_view::graph_view_Paint(System::Object^ , System::Windows::Forms::PaintEventArgs^  e)
		{
			if(!DesignMode)
			{
				e->Graphics->Transform		= gcnew Drawing2D::Matrix(1, 0, 0, 1, 5, 5);
				draw_with_graphics(e->Graphics, Drawing::Size(ClientSize.Width-10, ClientSize.Height-10), true, true,
					m_min_view_x, m_max_view_x, m_min_view_y, m_max_view_y);
			}
		}

		void				graph_view::draw_with_graphics	(Graphics^ dest_surface)
		{
			draw_with_graphics(dest_surface, dest_surface->VisibleClipBounds.Size.ToSize());
		}

		void				graph_view::draw_with_graphics	(Graphics^ dest_surface, Drawing::Size dest_surface_size)
		{
			draw_with_graphics(dest_surface, dest_surface_size, false, true, m_min_graph_x, m_max_graph_x, m_min_graph_y, m_max_graph_y);
		}

		void				graph_view::draw_with_graphics	(Graphics^ dest_surface, Boolean draw_grid, Boolean draw_0_line)
		{
			draw_with_graphics(dest_surface, Drawing::Size(ClientSize.Width-10, ClientSize.Height-10), draw_grid, draw_0_line,
				m_min_graph_x, m_max_graph_x, m_min_graph_y, m_max_graph_y);
		}	

		void				graph_view::draw_with_graphics	(Graphics^ dest_surface, Drawing::Size dest_surface_size,  Boolean draw_grid,
			Boolean draw_0_line, Single min_x, Single max_x, Single min_y, Single max_y)
		{
			//Define and Initialize points array
			array<PointF>^ points;
			if(m_graph_points)
			{
				points		= safe_cast<array<PointF>^>(m_graph_points->Clone());
			}
			else if(m_graph_samples)
			{
				points		= gcnew array<PointF>(m_graph_samples->Length);
				int index	= 0;
				for each(Single sample in m_graph_samples)
				{
					points[index]	= PointF(index*1.0f/m_points_frequency, sample);
					index++;
				}
			}
			else
				return;
			if(points->Length == 0)		return;

			//Set grapth dimentions and offsets
			Int32 DrawWidth				= dest_surface_size.Width;
			Int32 DrawHeight			= dest_surface_size.Height;

			//Compute transform matrix
			Single x_scale				= DrawWidth/(max_x-min_x);
			Single y_scale				= DrawHeight/(max_y-min_y);
			Drawing2D::Matrix^ M		= gcnew Drawing2D::Matrix(x_scale, 0, 0, -y_scale, -min_x*x_scale, y_scale*max_y);
			M->TransformPoints			(points);

			//Make and draw zero line
			if(draw_0_line)
			{
				array<PointF>^ line0		= gcnew array<PointF>(2);
				line0[0]					= PointF(min_x, 0);
				line0[1]					= PointF(max_x, 0);
				M->TransformPoints			(line0);
				dest_surface->DrawLine		(m_mesh_pen, line0[0], line0[1]);
			}
			
			Int32 m_grid_min_h_line_spacing = 15;
			Int32 m_grid_min_v_line_spacing = 15;
			Int32 m_grid_h_step				= 2;
			Int32 m_grid_v_step				= 10;

			//Compute grid
			if(draw_grid)
			{
				Single	grid_size_h			= max_x-min_x;
				Single	grid_size_v			= max_y-min_y;

				Int32	h_space				= math::ceil(1.0f*grid_size_h/(DrawWidth/m_grid_min_h_line_spacing)/m_grid_h_step)*m_grid_h_step;
				Int32	v_space				= math::ceil(1.0f*grid_size_v/(DrawHeight/m_grid_min_v_line_spacing)/m_grid_v_step)*m_grid_v_step;
				cli::array<System::Drawing::PointF>^	grid_lines = gcnew cli::array<System::Drawing::PointF>((math::floor(grid_size_h)/h_space+math::floor(grid_size_v)/v_space+2)*2);

				int x=0;
				int x_start;
				for(int i=x_start=math::ceil(min_x/h_space);i<=max_x/h_space; ++i)
				{
					grid_lines[x*2]			= System::Drawing::PointF(i*(float)h_space, min_y);
					grid_lines[x*2+1]		= System::Drawing::PointF(i*(float)h_space, max_y);
					++x;
				}

				int y=0;
				int y_start;
				for(int j=y_start=math::ceil(min_y/v_space);j<=max_y/v_space; ++j)
				{
					grid_lines[x*2+y*2]		= System::Drawing::PointF(min_x, (float)(j*v_space));
					grid_lines[x*2+y*2+1]	= System::Drawing::PointF(max_x, (float)(j*v_space));
					++y;
				}
				
				//Transform grid lines to absolute coordinates
				M->TransformPoints(grid_lines);

				//Draw Grid
				for(int n=0; n<x*2; n+=2)
				{
					dest_surface->DrawLine	(m_mesh_pen, grid_lines[n], grid_lines[n+1]);
					dest_surface->DrawString	(((x_start+(n/2))*h_space).ToString(), m_grid_font, m_grid_font_brush, grid_lines[n+1]);
				}
	//			e->Graphics->DrawLine	(m_mesh_pen, 0, Height-max_grid_width_track_bar->Height-3, Width, Height-max_grid_width_track_bar->Height-3);
				for(int n=x*2; n<grid_lines->Length; n+=2)
				{
					dest_surface->DrawLine	(m_mesh_pen, grid_lines[n], grid_lines[n+1]);
					dest_surface->DrawString	(((y_start+(n-x*2)/2)*v_space).ToString(), m_grid_font, m_grid_font_brush, grid_lines[n]);
				}
			}

			//Draw points
			dest_surface->DrawLines		(m_pen, points);
		}
	}
}