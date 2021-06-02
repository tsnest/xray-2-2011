#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

typedef Drawing::Brush MyBrush; 
typedef Drawing::Font MyFont;

namespace xray {
namespace editor {
ref class float_curve;
ref struct float_curve_pair;
ref class float_curve_event_args;

namespace controls {

	/// <summary>
	/// Summary for float_curve_editor
	/// </summary>
	public ref class float_curve_editor : public System::Windows::Forms::UserControl
	{
	public:
		float_curve_editor(void)
		{
			InitializeComponent();
			in_constructor();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~float_curve_editor()
		{
			if (components)
			{
				delete components;
			}
		}
		void							in_constructor			();
		float_curve^					m_curve;
		System::Drawing::Pen^			m_pen;
		System::Drawing::Pen^			m_mesh_pen;
		System::Drawing::Brush^			m_fill_brush;
		float_curve_pair^				m_captured;
		SizeF							m_point_rect_sz;
		int								m_grid_min_h_line_spacing;
		int								m_grid_min_v_line_spacing;
		int								m_grid_h_step;
		int								m_grid_v_step;
		Drawing::Font^					m_grid_font;
		Drawing::Brush^					m_grid_font_brush;
		PointF				client_point_to_curve	(Point client_point);
	public:
		property float_curve^	SelectedObject{
			float_curve^	get	()					{return m_curve;}
			void			set	(float_curve^ c)	{m_curve=c; Enabled=(c!=nullptr); Invalidate();}
		}
		property int			grid_min_h_line_spacing{
			int				get	()					{return m_grid_min_h_line_spacing;}
			void			set	(int value)			{m_grid_min_h_line_spacing=value;}
		}
		property int			grid_min_v_line_spacing{
			int				get	()					{return m_grid_min_v_line_spacing;}
			void			set	(int value)			{m_grid_min_v_line_spacing=value;}
		}
		property int			grid_h_line_step{
			int				get	()					{return m_grid_h_step;}
			void			set	(int value)			{m_grid_h_step=value;}
		}
		property int			grid_v_line_step{
			int				get	()					{return m_grid_v_step;}
			void			set	(int value)			{m_grid_v_step=value;}
		}
		property int			grid_h_max{
			int				get	()					{return max_grid_width_track_bar->Maximum;}
			void			set	(int value)			{max_grid_width_track_bar->Maximum=value;}
		}
		property int			grid_h_min{
			int				get	()					{return max_grid_width_track_bar->Minimum;}
			void			set	(int value)			{max_grid_width_track_bar->Minimum=value;}
		}
		property String^		grid_track_bar_label{
			String^			get	()					{return dimention_label->Text->TrimEnd(':');}
			void			set	(String^ value)		{dimention_label->Text=value+":";}
		}
		property bool		grid_track_bar_visible{
			bool			get	()					{return dimention_label->Visible;}
			void			set	(bool value)		{dimention_label->Visible=value;max_grid_width_track_bar->Visible=value;}
		}
		property MyFont^		grid_font{
			MyFont^			get	()					{return m_grid_font;}
			void			set	(MyFont^ value)		{m_grid_font=value;}
		}
		property MyBrush^		grid_font_brush{
			MyBrush^		get	()					{return m_grid_font_brush;}
			void			set	(MyBrush^ value)	{m_grid_font_brush=value;}
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::Windows::Forms::TrackBar^  max_grid_width_track_bar;
		System::Windows::Forms::Label^  dimention_label;
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->max_grid_width_track_bar = (gcnew System::Windows::Forms::TrackBar());
			this->dimention_label = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->max_grid_width_track_bar))->BeginInit();
			this->SuspendLayout();
			// 
			// max_grid_width_track_bar
			// 
			this->max_grid_width_track_bar->Anchor = safe_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->max_grid_width_track_bar->AutoSize = false;
			this->max_grid_width_track_bar->Cursor = System::Windows::Forms::Cursors::SizeWE;
			this->max_grid_width_track_bar->Location = System::Drawing::Point(150, 32);
			this->max_grid_width_track_bar->Maximum = 100;
			this->max_grid_width_track_bar->Name = L"max_grid_width_track_bar";
			this->max_grid_width_track_bar->Size = System::Drawing::Size(104, 16);
			this->max_grid_width_track_bar->TabIndex = 0;
			this->max_grid_width_track_bar->TickFrequency = 5;
			this->max_grid_width_track_bar->Value = 25;
			this->max_grid_width_track_bar->Scroll += gcnew System::EventHandler(this, &float_curve_editor::max_grid_width_track_bar_Scroll);
			// 
			// dimention_label
			// 
			this->dimention_label->Anchor = safe_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->dimention_label->Location = System::Drawing::Point(0, 32);
			this->dimention_label->Name = L"dimention_label";
			this->dimention_label->Size = System::Drawing::Size(153, 16);
			this->dimention_label->TabIndex = 1;
			this->dimention_label->Text = L"max distance:";
			this->dimention_label->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// float_curve_editor
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::GradientInactiveCaption;
			this->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->Controls->Add(this->dimention_label);
			this->DoubleBuffered = true;
			this->Controls->Add(this->max_grid_width_track_bar);
			this->Cursor = System::Windows::Forms::Cursors::Cross;
			this->Name = L"float_curve_editor";
			this->Size = System::Drawing::Size(256, 50);
			this->MouseLeave += gcnew System::EventHandler(this, &float_curve_editor::float_curve_editor_MouseLeave);
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &float_curve_editor::float_curve_editor_Paint);
			this->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &float_curve_editor::float_curve_editor_MouseMove);
			this->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &float_curve_editor::float_curve_editor_MouseDoubleClick);
			this->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &float_curve_editor::float_curve_editor_MouseDown);
			this->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &float_curve_editor::float_curve_editor_MouseUp);
			this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &float_curve_editor::float_curve_editor_KeyDown);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->max_grid_width_track_bar))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void float_curve_editor_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e);
	private: System::Void float_curve_editor_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void float_curve_editor_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void float_curve_editor_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void float_curve_editor_MouseLeave(System::Object^  sender, System::EventArgs^  e);
	private: System::Void float_curve_editor_MouseDoubleClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void float_curve_editor_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
	private: System::Void max_grid_width_track_bar_Scroll(System::Object^  sender, System::EventArgs^  e);	
};
};// namespace controls
};// namespace editor
};// namespace xray

