#pragma once

#include "editor_world_defines.h"
#include "input_keys_holder.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;



namespace xray {
namespace editor {

	public delegate void	enter_edit_mode_del			( );

	ref class window_ide;
	/// <summary>
	/// Summary for window_view
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class window_view :	public WeifenLuo::WinFormsUI::Docking::DockContent//, public editor_view 
	{
	public:
		window_view(window_ide %ide) :
			m_ide	( %ide ), m_mouse_in_view(false)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			in_constructor	( );
		}

			System::Windows::Forms::ToolStripPanel^	m_top_toolstrip_panel;
	private: System::Windows::Forms::Panel^  top_panel;
	public: 
	private: System::Windows::Forms::Panel^  ViewPanel;
	public: bool m_mouse_in_view;

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~window_view()
		{
			if (components)
			{
				delete components;
			}
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
			this->top_panel = (gcnew System::Windows::Forms::Panel());
			this->ViewPanel = (gcnew System::Windows::Forms::Panel());
			this->SuspendLayout();
			// 
			// top_panel
			// 
			this->top_panel->AutoSize = true;
			this->top_panel->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->top_panel->Dock = System::Windows::Forms::DockStyle::Top;
			this->top_panel->Location = System::Drawing::Point(0, 0);
			this->top_panel->MinimumSize = System::Drawing::Size(0, 35);
			this->top_panel->Name = L"top_panel";
			this->top_panel->Size = System::Drawing::Size(531, 35);
			this->top_panel->TabIndex = 19;
			// 
			// ViewPanel
			// 
			this->ViewPanel->BackColor = System::Drawing::SystemColors::ActiveCaption;
			this->ViewPanel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->ViewPanel->Location = System::Drawing::Point(0, 35);
			this->ViewPanel->Name = L"ViewPanel";
			this->ViewPanel->Size = System::Drawing::Size(531, 283);
			this->ViewPanel->TabIndex = 20;
			this->ViewPanel->MouseLeave += gcnew System::EventHandler(this, &window_view::ViewPanel_MouseLeave);
			this->ViewPanel->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &window_view::ViewPanel_MouseMove);
			this->ViewPanel->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &window_view::ViewPanel_MouseDown);
			this->ViewPanel->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &window_view::ViewPanel_MouseUp);
			this->ViewPanel->MouseEnter += gcnew System::EventHandler(this, &window_view::ViewPanel_MouseEnter);
			// 
			// window_view
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(531, 318);
			this->CloseButton = false;
			this->Controls->Add(this->ViewPanel);
			this->Controls->Add(this->top_panel);
			this->DockAreas = WeifenLuo::WinFormsUI::Docking::DockAreas::Document;
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(204)));
			this->HideOnClose = true;
			this->KeyPreview = true;
			this->Name = L"window_view";
			this->TabText = L"view";
			this->Text = L"view";
			this->Load += gcnew System::EventHandler(this, &window_view::window_view_Load);
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &window_view::window_view_Paint);
			this->SizeChanged += gcnew System::EventHandler(this, &window_view::window_view_SizeChanged);
			this->LocationChanged += gcnew System::EventHandler(this, &window_view::window_view_LocationChanged);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
private:
	window_ide^						m_ide;

	System::Drawing::Point			m_mouse_start;
	System::Drawing::Rectangle		m_default_clip;

public:
	input_keys_holder				m_input_key_holder;
//	ArrayList						m_held_keys;

	System::Drawing::Point			m_mouse_real;

	::Cursor^						m_tranform_cursor;
	bool							m_infinite_mode;

	enum_editor_cursors				m_current_cursor;
	System::Collections::Generic::SortedDictionary<enum_editor_cursors,::Cursor^> cursors; 


private:
	void			in_constructor				( );

public:
	void			clip_cursor					( );
	void			unclip_cursor				( );
	void			on_idle						( );
	System::Void	window_view_Activated		( );
	System::Void	window_view_Deactivate		( );
	System::Void	window_view_LocationChanged	(System::Object^  sender, System::EventArgs^  e);
	System::IntPtr	draw_handle					( );

	Point			get_view_center				( );
	::Size			get_view_size				( );
	Point			get_mouse_position			( );
	void			set_mouse_position			( Point p );
	enum_editor_cursors get_cursor				( );
	enum_editor_cursors set_cursor				( enum_editor_cursors cursor );
	void			set_infinite_mode			( bool inifinte_mode );

private:
	System::Void	window_view_Paint			(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e);
	System::Void	window_view_SizeChanged		(System::Object^  sender, System::EventArgs^  e);
	System::Void	window_view_Load			(System::Object^  sender, System::EventArgs^  e);
	System::Void	ViewPanel_MouseMove			(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	System::Void ViewPanel_MouseEnter			(System::Object^  , System::EventArgs^ );
	System::Void ViewPanel_MouseLeave			(System::Object^  , System::EventArgs^ );
	System::Void ViewPanel_MouseDown			(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	System::Void ViewPanel_MouseUp				(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
public:
	System::Windows::Forms::ToolStrip^  GetToolStrip(System::String^ name);
	System::Windows::Forms::ToolStrip^  CreateToolStrip(System::String^ name, int prio);
};

} // namespace editor
} // namespace xray