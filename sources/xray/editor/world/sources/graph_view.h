////////////////////////////////////////////////////////////////////////////
//	Created		: 15.01.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GRAPH_VIEW_H_INCLUDED
#define GRAPH_VIEW_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

typedef System::Drawing::Font DFont;

namespace xray
{
	namespace editor
	{
		/// <summary>
		/// Summary for graph_view
		/// </summary>
		public ref class graph_view : public System::Windows::Forms::UserControl
		{

		#pragma region | Initialize |

		public:
			graph_view(void)
			{
				InitializeComponent();
				//
				//TODO: Add the constructor code here
				//
				in_constructor();
			}

		protected:
			/// <summary>
			/// Clean up any resources being used.
			/// </summary>
			~graph_view()
			{
				if (components)
				{
					delete components;
				}
			}

		private:
			void in_constructor();

			/// <summary>
			/// Required designer variable.
			/// </summary>
			System::ComponentModel::Container ^components;

		#pragma region Windows Form Designer generated code
			/// <summary>
			/// Required method for Designer support - do not modify
			/// the contents of this method with the code editor.
			/// </summary>
			void InitializeComponent(void)
			{
				this->SuspendLayout();
				// 
				// graph_view
				// 
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
		#pragma endregion

		#pragma endregion

		#pragma region |   Fields   |

		private:
			array<PointF>^		m_graph_points;	
			array<Single>^		m_graph_samples;
			int					m_points_frequency;

			Pen^	m_pen;
			Pen^	m_mesh_pen;
			DFont^	m_grid_font;
			Brush^	m_grid_font_brush;
			float	m_max_graph_x;
			float	m_min_graph_x;
			float	m_max_graph_y;
			float	m_min_graph_y;
			float	m_max_view_x;
			float	m_min_view_x;
			float	m_max_view_y;
			float	m_min_view_y;

		#pragma endregion

		#pragma region | Properties |

		public:
			property float	max_graph_x {
				float	get(){return m_max_graph_x;};
				void	set(float value){m_max_graph_x=value; Invalidate();};
			}
			property float	min_graph_x	{
				float	get(){return m_min_graph_x;};
				void	set(float value){m_min_graph_x=value; Invalidate();};
			}
			property float	max_graph_y	{
				float	get(){return m_max_graph_y;};
				void	set(float value){m_max_graph_y=value; Invalidate();};
			}
			property float	min_graph_y	{
				float	get(){return m_min_graph_y;};
				void	set(float value){m_min_graph_y=value; Invalidate();};
			}
			property float	max_view_x {
				float	get(){return m_max_view_x;};
				void	set(float value){m_max_view_x=value; Invalidate();};
			}
			property float	min_view_x	{
				float	get(){return m_min_view_x;};
				void	set(float value){m_min_view_x=value; Invalidate();};
			}
			property float	max_view_y	{
				float	get(){return m_max_view_y;};
				void	set(float value){m_max_view_y=value; Invalidate();};
			}
			property float	min_view_y	{
				float	get(){return m_min_view_y;};
				void	set(float value){m_min_view_y=value; Invalidate();};
			}

		#pragma endregion

		#pragma region |  Methods   |

		private:
			void	graph_view_Paint	(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e);

		public:
			void	set_graph_points	(array<PointF>^ points);
			void	set_graph_points	(array<Single>^ samples, Int32 points_frequency);

			void	draw_with_graphics	(Graphics^ dest_surface);
			void	draw_with_graphics	(Graphics^ dest_surface, Drawing::Size dest_surface_size);
			void	draw_with_graphics	(Graphics^ dest_surface, Boolean draw_grid, Boolean draw_0_line);
			void	draw_with_graphics	(Graphics^ dest_surface, Drawing::Size dest_surface_size,  Boolean draw_grid, Boolean draw_0_line,
				float mix_x, float max_x, float min_y, float max_y);

		#pragma endregion

		};//class graph_view
	}//namespace editor
}//namespace xray
#endif // #ifndef GRAPH_VIEW_H_INCLUDED