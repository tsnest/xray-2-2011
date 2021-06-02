////////////////////////////////////////////////////////////////////////////
//	Created		: 15.01.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef GRAPH_VIEW_H_INCLUDED
#define GRAPH_VIEW_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray {
namespace sound_editor {
	public ref class graph_view : public System::Windows::Forms::UserControl
	{
		typedef List<array<PointF>^> points_list;
		typedef System::Drawing::Font DFont;
		typedef List<System::Drawing::Color> colors_list;
	public:
				graph_view			(int freq);
				~graph_view			();
		void	clear				();
		void	add_graph_points	(array<Single>^ points);
		void	draw_with_graphics	(Graphics^ dest_surface, Boolean draw_grid, Boolean draw_0_line);
		void	draw_with_graphics	(
			Graphics^ dest_surface, 
			Drawing::Size dest_surface_size,
			Boolean draw_grid, 
			Boolean draw_0_line,
			float mix_x, 
			float max_x, 
			float min_y, 
			float max_y
		);

		property float max_graph_x
		{
			float get() {return m_max_graph_x;};
			void set(float value) {m_max_graph_x=value; Invalidate();};
		}
		property float min_graph_x
		{
			float get() {return m_min_graph_x;};
			void set(float value) {m_min_graph_x=value; Invalidate();};
		}
		property float max_graph_y
		{
			float get() {return m_max_graph_y;};
			void set(float value) {m_max_graph_y=value; Invalidate();};
		}
		property float min_graph_y
		{
			float get() {return m_min_graph_y;};
			void set(float value) {m_min_graph_y=value; Invalidate();};
		}
		property float max_view_x
		{
			float get() {return m_max_view_x;};
			void set(float value) {m_max_view_x=value; Invalidate();};
		}
		property float min_view_x
		{
			float get() {return m_min_view_x;};
			void set(float value) {m_min_view_x=value; Invalidate();};
		}
		property float max_view_y
		{
			float get() {return m_max_view_y;};
			void set(float value) {m_max_view_y=value; Invalidate();};
		}
		property float min_view_y
		{
			float get() {return m_min_view_y;};
			void set(float value) {m_min_view_y=value; Invalidate();};
		}

	private:
		void graph_view_Paint	(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e);
		void in_constructor		();
		void InitializeComponent(void)
		{
			this->SuspendLayout();
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::GradientInactiveCaption;
			this->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->DoubleBuffered =true;
			this->Name = L"graph_view";
			this->Size = System::Drawing::Size(148, 148);
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &graph_view::graph_view_Paint);
			this->ResumeLayout(false);
		}

	private:
		System::ComponentModel::Container^	components;
		points_list^	m_graph_points;	
		int				m_points_frequency;
		colors_list^	m_colors;
		Pen^			m_pen;
		Pen^			m_mesh_pen;
		DFont^			m_grid_font;
		Brush^			m_grid_font_brush;
		float			m_max_graph_x;
		float			m_min_graph_x;
		float			m_max_graph_y;
		float			m_min_graph_y;
		float			m_max_view_x;
		float			m_min_view_x;
		float			m_max_view_y;
		float			m_min_view_y;
	};//class graph_view
}//namespace sound_editor
}//namespace xray
#endif // #ifndef GRAPH_VIEW_H_INCLUDED