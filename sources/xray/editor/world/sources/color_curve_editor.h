#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray {
namespace editor {
ref class color_curve;
ref struct color_curve_pair;

namespace controls {

	/// <summary>
	/// Summary for color_curve_editor
	/// </summary>
	public ref class color_curve_editor : public System::Windows::Forms::UserControl
	{
	public:
		color_curve_editor(void)
		{
			InitializeComponent();
			in_constructor();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~color_curve_editor()
		{
			if (components)
			{
				delete components;
			}
		}
		void in_constructor();

	private: System::Windows::Forms::ColorDialog^  colorDialog1;
	protected: 
		color_curve_pair^									m_captured;
		color_curve^										m_curve;
		System::Drawing::Pen^								m_pen;
		System::Drawing::Drawing2D::LinearGradientBrush^	m_fill_brush;

		float				client_point_to_curve	(Point client_point);
	public:
		property color_curve^	SelectedObject{
			color_curve^	get ()					{return m_curve;}
			void			set (color_curve^ c)	{m_curve=c; Enabled = (c!=nullptr); Invalidate(false);}
		}
	private:
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
			this->colorDialog1 = (gcnew System::Windows::Forms::ColorDialog());
			this->SuspendLayout();
			// 
			// colorDialog1
			// 
			this->colorDialog1->FullOpen = true;
			// 
			// color_curve_editor
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::GradientInactiveCaption;
			this->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->Cursor = System::Windows::Forms::Cursors::Cross;
			this->DoubleBuffered = true;
			this->Name = L"color_curve_editor";
			this->Size = System::Drawing::Size(256, 30);
			this->MouseLeave += gcnew System::EventHandler(this, &color_curve_editor::color_curve_editor_MouseLeave);
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &color_curve_editor::color_curve_editor_Paint);
			this->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &color_curve_editor::color_curve_editor_MouseMove);
			this->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &color_curve_editor::color_curve_editor_MouseDoubleClick);
			this->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &color_curve_editor::color_curve_editor_MouseDown);
			this->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &color_curve_editor::color_curve_editor_MouseUp);
			this->MouseEnter += gcnew System::EventHandler(this, &color_curve_editor::color_curve_editor_MouseEnter);
			this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &color_curve_editor::color_curve_editor_KeyDown);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void color_curve_editor_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void color_curve_editor_MouseEnter(System::Object^  sender, System::EventArgs^  e);
	private: System::Void color_curve_editor_MouseLeave(System::Object^  sender, System::EventArgs^  e);
	private: System::Void color_curve_editor_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void color_curve_editor_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void color_curve_editor_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e);
	private: System::Void color_curve_editor_MouseDoubleClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void color_curve_editor_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
	};
}; //namespace controls
}; //namespace editor
}; //namespace xray
