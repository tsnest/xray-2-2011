////////////////////////////////////////////////////////////////////////////
//	Created 	: 31.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef WINDOW_IDE_H_INCLUDED
#define WINDOW_IDE_H_INCLUDED

using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using xray::editor_base::action;

namespace xray {

namespace editor_base { ref class gui_binder;}

namespace editor {

	ref class options_dialog;

	#pragma unmanaged
		class editor_world;
	#pragma managed

	/// <summary>
	/// Summary for window_ide
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class window_ide : public System::Windows::Forms::Form
	{
		typedef Form	super;
	public:
		window_ide(	editor_world& world ) :
			m_editor_world			( world )
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			in_constructor	( );
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~window_ide()
		{
			if (components)
			{
				delete components;
			}

			custom_finalize	( );
		}

	protected: 

	protected: 

	private: System::Windows::Forms::MenuStrip^  menuStrip;
	private: System::Windows::Forms::ToolStripMenuItem^  ViewMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  HelpMenuItem;

	private: System::Windows::Forms::ToolStripMenuItem^  aboutToolStripMenuItem;





	private: System::Windows::Forms::ToolStripMenuItem^  outputToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  ToolsMenuItem;


	private: System::Windows::Forms::ToolStripMenuItem^  optionsToolStripMenuItem;


	private: System::Windows::Forms::ToolStripMenuItem^  FileMenuItem;

	private: System::Windows::Forms::ToolStripSeparator^  toolStripMenuItem1;
	private: System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  EditMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  recentFilesToolStripMenuItem;
	private: System::Windows::Forms::StatusStrip^  statusStrip1;
	private: System::Windows::Forms::ToolStripStatusLabel^  toolStripStatusLabel0;


	private: WeifenLuo::WinFormsUI::Docking::DockPanel^  Editor;
	private: System::Windows::Forms::ToolStripStatusLabel^  toolStripStatusLabel3;
	private: System::Windows::Forms::ToolStripStatusLabel^  toolStripStatusLabel4;
	private: System::Windows::Forms::ToolStripSplitButton^  error_message_button;

	private: System::Windows::Forms::ToolStripStatusLabel^  message_button;
	private: System::Windows::Forms::ToolStripMenuItem^  clearToolStripMenuItem;









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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(window_ide::typeid));
			WeifenLuo::WinFormsUI::Docking::DockPanelSkin^  dockPanelSkin1 = (gcnew WeifenLuo::WinFormsUI::Docking::DockPanelSkin());
			WeifenLuo::WinFormsUI::Docking::AutoHideStripSkin^  autoHideStripSkin1 = (gcnew WeifenLuo::WinFormsUI::Docking::AutoHideStripSkin());
			WeifenLuo::WinFormsUI::Docking::DockPanelGradient^  dockPanelGradient1 = (gcnew WeifenLuo::WinFormsUI::Docking::DockPanelGradient());
			WeifenLuo::WinFormsUI::Docking::TabGradient^  tabGradient1 = (gcnew WeifenLuo::WinFormsUI::Docking::TabGradient());
			WeifenLuo::WinFormsUI::Docking::DockPaneStripSkin^  dockPaneStripSkin1 = (gcnew WeifenLuo::WinFormsUI::Docking::DockPaneStripSkin());
			WeifenLuo::WinFormsUI::Docking::DockPaneStripGradient^  dockPaneStripGradient1 = (gcnew WeifenLuo::WinFormsUI::Docking::DockPaneStripGradient());
			WeifenLuo::WinFormsUI::Docking::TabGradient^  tabGradient2 = (gcnew WeifenLuo::WinFormsUI::Docking::TabGradient());
			WeifenLuo::WinFormsUI::Docking::DockPanelGradient^  dockPanelGradient2 = (gcnew WeifenLuo::WinFormsUI::Docking::DockPanelGradient());
			WeifenLuo::WinFormsUI::Docking::TabGradient^  tabGradient3 = (gcnew WeifenLuo::WinFormsUI::Docking::TabGradient());
			WeifenLuo::WinFormsUI::Docking::DockPaneStripToolWindowGradient^  dockPaneStripToolWindowGradient1 = (gcnew WeifenLuo::WinFormsUI::Docking::DockPaneStripToolWindowGradient());
			WeifenLuo::WinFormsUI::Docking::TabGradient^  tabGradient4 = (gcnew WeifenLuo::WinFormsUI::Docking::TabGradient());
			WeifenLuo::WinFormsUI::Docking::TabGradient^  tabGradient5 = (gcnew WeifenLuo::WinFormsUI::Docking::TabGradient());
			WeifenLuo::WinFormsUI::Docking::DockPanelGradient^  dockPanelGradient3 = (gcnew WeifenLuo::WinFormsUI::Docking::DockPanelGradient());
			WeifenLuo::WinFormsUI::Docking::TabGradient^  tabGradient6 = (gcnew WeifenLuo::WinFormsUI::Docking::TabGradient());
			WeifenLuo::WinFormsUI::Docking::TabGradient^  tabGradient7 = (gcnew WeifenLuo::WinFormsUI::Docking::TabGradient());
			this->menuStrip = (gcnew System::Windows::Forms::MenuStrip());
			this->FileMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->recentFilesToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->EditMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ViewMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->outputToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ToolsMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->optionsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->HelpMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->aboutToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->statusStrip1 = (gcnew System::Windows::Forms::StatusStrip());
			this->toolStripStatusLabel0 = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->toolStripStatusLabel3 = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->toolStripStatusLabel4 = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->error_message_button = (gcnew System::Windows::Forms::ToolStripSplitButton());
			this->clearToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->message_button = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->Editor = (gcnew WeifenLuo::WinFormsUI::Docking::DockPanel());
			this->menuStrip->SuspendLayout();
			this->statusStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// menuStrip
			// 
			this->menuStrip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(5) {this->FileMenuItem, this->EditMenuItem, 
				this->ViewMenuItem, this->ToolsMenuItem, this->HelpMenuItem});
			resources->ApplyResources(this->menuStrip, L"menuStrip");
			this->menuStrip->Name = L"menuStrip";
			// 
			// FileMenuItem
			// 
			this->FileMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->recentFilesToolStripMenuItem, 
				this->toolStripMenuItem1, this->exitToolStripMenuItem});
			this->FileMenuItem->Name = L"FileMenuItem";
			resources->ApplyResources(this->FileMenuItem, L"FileMenuItem");
			this->FileMenuItem->Tag = L"0";
			this->FileMenuItem->DropDownOpening += gcnew System::EventHandler(this, &window_ide::FileMenuItem_DropDownOpening);
			// 
			// recentFilesToolStripMenuItem
			// 
			this->recentFilesToolStripMenuItem->Name = L"recentFilesToolStripMenuItem";
			resources->ApplyResources(this->recentFilesToolStripMenuItem, L"recentFilesToolStripMenuItem");
			// 
			// toolStripMenuItem1
			// 
			this->toolStripMenuItem1->Name = L"toolStripMenuItem1";
			resources->ApplyResources(this->toolStripMenuItem1, L"toolStripMenuItem1");
			// 
			// exitToolStripMenuItem
			// 
			this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
			resources->ApplyResources(this->exitToolStripMenuItem, L"exitToolStripMenuItem");
			this->exitToolStripMenuItem->Tag = L"10";
			this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &window_ide::exitToolStripMenuItem_Click);
			// 
			// EditMenuItem
			// 
			this->EditMenuItem->Name = L"EditMenuItem";
			resources->ApplyResources(this->EditMenuItem, L"EditMenuItem");
			this->EditMenuItem->Tag = L"10";
			// 
			// ViewMenuItem
			// 
			this->ViewMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->outputToolStripMenuItem});
			this->ViewMenuItem->Name = L"ViewMenuItem";
			resources->ApplyResources(this->ViewMenuItem, L"ViewMenuItem");
			this->ViewMenuItem->Tag = L"20";
			// 
			// outputToolStripMenuItem
			// 
			resources->ApplyResources(this->outputToolStripMenuItem, L"outputToolStripMenuItem");
			this->outputToolStripMenuItem->Name = L"outputToolStripMenuItem";
			this->outputToolStripMenuItem->Tag = L"10";
			// 
			// ToolsMenuItem
			// 
			this->ToolsMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->optionsToolStripMenuItem});
			this->ToolsMenuItem->Name = L"ToolsMenuItem";
			resources->ApplyResources(this->ToolsMenuItem, L"ToolsMenuItem");
			this->ToolsMenuItem->Tag = L"30";
			// 
			// optionsToolStripMenuItem
			// 
			this->optionsToolStripMenuItem->Name = L"optionsToolStripMenuItem";
			resources->ApplyResources(this->optionsToolStripMenuItem, L"optionsToolStripMenuItem");
			this->optionsToolStripMenuItem->Tag = L"99";
			this->optionsToolStripMenuItem->Click += gcnew System::EventHandler(this, &window_ide::optionsToolStripMenuItem_Click);
			// 
			// HelpMenuItem
			// 
			this->HelpMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->aboutToolStripMenuItem});
			this->HelpMenuItem->Name = L"HelpMenuItem";
			resources->ApplyResources(this->HelpMenuItem, L"HelpMenuItem");
			this->HelpMenuItem->Tag = L"40";
			// 
			// aboutToolStripMenuItem
			// 
			this->aboutToolStripMenuItem->Name = L"aboutToolStripMenuItem";
			resources->ApplyResources(this->aboutToolStripMenuItem, L"aboutToolStripMenuItem");
			this->aboutToolStripMenuItem->Tag = L"10";
			this->aboutToolStripMenuItem->Click += gcnew System::EventHandler(this, &window_ide::aboutToolStripMenuItem_Click);
			// 
			// statusStrip1
			// 
			this->statusStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(5) {this->toolStripStatusLabel0, 
				this->toolStripStatusLabel3, this->toolStripStatusLabel4, this->error_message_button, this->message_button});
			resources->ApplyResources(this->statusStrip1, L"statusStrip1");
			this->statusStrip1->Name = L"statusStrip1";
			// 
			// toolStripStatusLabel0
			// 
			resources->ApplyResources(this->toolStripStatusLabel0, L"toolStripStatusLabel0");
			this->toolStripStatusLabel0->BorderSides = static_cast<System::Windows::Forms::ToolStripStatusLabelBorderSides>((((System::Windows::Forms::ToolStripStatusLabelBorderSides::Left | System::Windows::Forms::ToolStripStatusLabelBorderSides::Top) 
				| System::Windows::Forms::ToolStripStatusLabelBorderSides::Right) 
				| System::Windows::Forms::ToolStripStatusLabelBorderSides::Bottom));
			this->toolStripStatusLabel0->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->toolStripStatusLabel0->Name = L"toolStripStatusLabel0";
			// 
			// toolStripStatusLabel3
			// 
			resources->ApplyResources(this->toolStripStatusLabel3, L"toolStripStatusLabel3");
			this->toolStripStatusLabel3->BorderSides = static_cast<System::Windows::Forms::ToolStripStatusLabelBorderSides>((((System::Windows::Forms::ToolStripStatusLabelBorderSides::Left | System::Windows::Forms::ToolStripStatusLabelBorderSides::Top) 
				| System::Windows::Forms::ToolStripStatusLabelBorderSides::Right) 
				| System::Windows::Forms::ToolStripStatusLabelBorderSides::Bottom));
			this->toolStripStatusLabel3->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->toolStripStatusLabel3->Name = L"toolStripStatusLabel3";
			// 
			// toolStripStatusLabel4
			// 
			resources->ApplyResources(this->toolStripStatusLabel4, L"toolStripStatusLabel4");
			this->toolStripStatusLabel4->BorderSides = static_cast<System::Windows::Forms::ToolStripStatusLabelBorderSides>((((System::Windows::Forms::ToolStripStatusLabelBorderSides::Left | System::Windows::Forms::ToolStripStatusLabelBorderSides::Top) 
				| System::Windows::Forms::ToolStripStatusLabelBorderSides::Right) 
				| System::Windows::Forms::ToolStripStatusLabelBorderSides::Bottom));
			this->toolStripStatusLabel4->Name = L"toolStripStatusLabel4";
			// 
			// error_message_button
			// 
			resources->ApplyResources(this->error_message_button, L"error_message_button");
			this->error_message_button->BackColor = System::Drawing::SystemColors::Control;
			this->error_message_button->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->error_message_button->DropDownButtonWidth = 15;
			this->error_message_button->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->clearToolStripMenuItem});
			this->error_message_button->ForeColor = System::Drawing::SystemColors::ActiveCaptionText;
			this->error_message_button->Name = L"error_message_button";
			this->error_message_button->ButtonClick += gcnew System::EventHandler(this, &window_ide::error_message_button_ButtonClick);
			// 
			// clearToolStripMenuItem
			// 
			resources->ApplyResources(this->clearToolStripMenuItem, L"clearToolStripMenuItem");
			this->clearToolStripMenuItem->Name = L"clearToolStripMenuItem";
			this->clearToolStripMenuItem->Click += gcnew System::EventHandler(this, &window_ide::clearToolStripMenuItem_Click);
			// 
			// message_button
			// 
			resources->ApplyResources(this->message_button, L"message_button");
			this->message_button->Name = L"message_button";
			// 
			// Editor
			// 
			this->Editor->ActiveAutoHideContent = nullptr;
			resources->ApplyResources(this->Editor, L"Editor");
			this->Editor->DockBackColor = System::Drawing::SystemColors::Control;
			this->Editor->DocumentStyle = WeifenLuo::WinFormsUI::Docking::DocumentStyle::DockingWindow;
			this->Editor->Name = L"Editor";
			dockPanelGradient1->EndColor = System::Drawing::SystemColors::ControlLight;
			dockPanelGradient1->StartColor = System::Drawing::SystemColors::ControlLight;
			autoHideStripSkin1->DockStripGradient = dockPanelGradient1;
			tabGradient1->EndColor = System::Drawing::SystemColors::Control;
			tabGradient1->StartColor = System::Drawing::SystemColors::Control;
			tabGradient1->TextColor = System::Drawing::SystemColors::ControlDarkDark;
			autoHideStripSkin1->TabGradient = tabGradient1;
			dockPanelSkin1->AutoHideStripSkin = autoHideStripSkin1;
			tabGradient2->EndColor = System::Drawing::SystemColors::ControlLightLight;
			tabGradient2->StartColor = System::Drawing::SystemColors::ControlLightLight;
			tabGradient2->TextColor = System::Drawing::SystemColors::ControlText;
			dockPaneStripGradient1->ActiveTabGradient = tabGradient2;
			dockPanelGradient2->EndColor = System::Drawing::SystemColors::Control;
			dockPanelGradient2->StartColor = System::Drawing::SystemColors::Control;
			dockPaneStripGradient1->DockStripGradient = dockPanelGradient2;
			tabGradient3->EndColor = System::Drawing::SystemColors::ControlLight;
			tabGradient3->StartColor = System::Drawing::SystemColors::ControlLight;
			tabGradient3->TextColor = System::Drawing::SystemColors::ControlText;
			dockPaneStripGradient1->InactiveTabGradient = tabGradient3;
			dockPaneStripSkin1->DocumentGradient = dockPaneStripGradient1;
			tabGradient4->EndColor = System::Drawing::SystemColors::ActiveCaption;
			tabGradient4->LinearGradientMode = System::Drawing::Drawing2D::LinearGradientMode::Vertical;
			tabGradient4->StartColor = System::Drawing::SystemColors::GradientActiveCaption;
			tabGradient4->TextColor = System::Drawing::SystemColors::ActiveCaptionText;
			dockPaneStripToolWindowGradient1->ActiveCaptionGradient = tabGradient4;
			tabGradient5->EndColor = System::Drawing::SystemColors::Control;
			tabGradient5->StartColor = System::Drawing::SystemColors::Control;
			tabGradient5->TextColor = System::Drawing::SystemColors::ControlText;
			dockPaneStripToolWindowGradient1->ActiveTabGradient = tabGradient5;
			dockPanelGradient3->EndColor = System::Drawing::SystemColors::ControlLight;
			dockPanelGradient3->StartColor = System::Drawing::SystemColors::ControlLight;
			dockPaneStripToolWindowGradient1->DockStripGradient = dockPanelGradient3;
			tabGradient6->EndColor = System::Drawing::SystemColors::GradientInactiveCaption;
			tabGradient6->LinearGradientMode = System::Drawing::Drawing2D::LinearGradientMode::Vertical;
			tabGradient6->StartColor = System::Drawing::SystemColors::GradientInactiveCaption;
			tabGradient6->TextColor = System::Drawing::SystemColors::ControlText;
			dockPaneStripToolWindowGradient1->InactiveCaptionGradient = tabGradient6;
			tabGradient7->EndColor = System::Drawing::Color::Transparent;
			tabGradient7->StartColor = System::Drawing::Color::Transparent;
			tabGradient7->TextColor = System::Drawing::SystemColors::ControlDarkDark;
			dockPaneStripToolWindowGradient1->InactiveTabGradient = tabGradient7;
			dockPaneStripSkin1->ToolWindowGradient = dockPaneStripToolWindowGradient1;
			dockPanelSkin1->DockPaneStripSkin = dockPaneStripSkin1;
			this->Editor->Skin = dockPanelSkin1;
			// 
			// window_ide
			// 
			resources->ApplyResources(this, L"$this");
			this->Activated += gcnew System::EventHandler(this, &window_ide::on_form_activated);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->Editor);
			this->Controls->Add(this->statusStrip1);
			this->Controls->Add(this->menuStrip);
			this->KeyPreview = true;
			this->MainMenuStrip = this->menuStrip;
			this->Name = L"window_ide";
			this->SizeChanged += gcnew System::EventHandler(this, &window_ide::window_ide_SizeChanged);
			this->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &window_ide::window_ide_KeyUp);
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &window_ide::window_ide_FormClosing);
			this->LocationChanged += gcnew System::EventHandler(this, &window_ide::window_ide_LocationChanged);
			this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &window_ide::window_ide_KeyDown);
			this->menuStrip->ResumeLayout(false);
			this->menuStrip->PerformLayout();
			this->statusStrip1->ResumeLayout(false);
			this->statusStrip1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion


private:
			void	in_constructor		( );
			void	custom_finalize		( );
			void	on_idle				( System::Object ^sender, System::EventArgs ^event_args );
	WeifenLuo::WinFormsUI::Docking::IDockContent ^reload_content	(System::String ^persist_string);
			void	on_form_activated	( System::Object ^sender, System::EventArgs ^event_args );

public:
	System::Windows::Forms::MenuStrip^  get_menu_strip ( ) { return menuStrip; }
	
	inline	bool	in_idle				( )			{ return !!m_in_idle; }
	inline	editor_world&	get_editor_world( )		{ return m_editor_world; }
	
	inline	editor_base::scene_view_panel^	view_window( )			{ return m_view_window; }
	inline  editor_base::camera_view_panel^	camera_view_panel( )	{ return m_camera_view_panel; }

			void	add_window			( WeifenLuo::WinFormsUI::Docking::DockContent^ form, WeifenLuo::WinFormsUI::Docking::DockState dock );


			void	window_ide_KeyDown	(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
			void	window_ide_KeyUp	(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);


	inline	WeifenLuo::WinFormsUI::Docking::DockPanel^ get_editor( )			{ return Editor; }
			bool	is_closing			( ) { return m_is_closing; }


private:
	editor_world&					m_editor_world;
	editor_base::scene_view_panel^	m_view_window;
	editor_base::camera_view_panel^	m_camera_view_panel;
	u32								m_in_idle;
	
	editor_base::options_dialog^	m_options_dialog;
	bool							m_is_closing;

private:
	System::Void	window_ide_FormClosing		( System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^ e );
	System::Void	window_ide_LocationChanged	( System::Object^  sender, System::EventArgs^ e );
	System::Void	window_ide_SizeChanged		( System::Object^  sender, System::EventArgs^ e );
	System::Void	exitToolStripMenuItem_Click	( System::Object^  sender, System::EventArgs^ e );
	
	System::Void	aboutToolStripMenuItem_Click		( System::Object^  sender, System::EventArgs^  e );
	System::Void	optionsToolStripMenuItem_Click		( System::Object^  sender, System::EventArgs^  e );

public:
	void			add_menu_item				( editor_base::gui_binder^ binder, System::String^ key, System::String^ text, int prio ); // add sub-folder
	void			add_action_menu_item		( editor_base::gui_binder^ binder, System::String^ action_name, System::String^ strip_key, int prio );
	void			create_tool_strip			( System::String^ strip_key, int prio );
	void			add_action_button_item		( editor_base::gui_binder^ binder, System::String^ action_name, System::String^ strip_key, int prio );
	void			add_action_button_item		( editor_base::gui_binder^ binder, action^ action, System::String^ strip_key, int prio );
	void			add_button					( ToolStripButton^ button, System::String^ strip_key, int prio );

	void			show_tab					( WeifenLuo::WinFormsUI::Docking::DockContent^ tab );
	void			show_tab					( WeifenLuo::WinFormsUI::Docking::DockContent^ tab, WeifenLuo::WinFormsUI::Docking::DockState state );

	System::Collections::Generic::List<Keys>^		get_held_keys				( );
	int				get_current_keys_string		( System::String^& combination );

	void			set_status_label			( int idx, System::String^ text );
	void			set_mouse_sensivity			( float sensivity );
	IWin32Window^	wnd							( ) {return this;}
	xray::editor_base::options_manager^ get_options_manager		();
	
			void			load_contents_layout		( );
	private: System::Void	FileMenuItem_DropDownOpening( System::Object^  sender, System::EventArgs^ e );
	private: void			RecentFileMenuItem_Click	( System::Object^ sender, System::EventArgs^ e );

public:
	inline	ImageList^		images16x16					( ) { return m_images16x16; }
	virtual void			WndProc						( Message% m ) override;

private:
	void					key_combination_changed		( System::String^ combination );
	void					OnWM_Activate				( WPARAM wParam, LPARAM lParam );
	void					OnWM_ActivateApp			( WPARAM wParam, LPARAM lParam );
	ImageList^				m_images16x16;

public:
	void					close						( );
	void					error_message_out			( System::String^ msg );
	void					clear_error_messages		( );

	private: System::Void error_message_button_ButtonClick(System::Object^  sender, System::EventArgs^  e);
	private: System::Void clearToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
};

} // namespace editor
} // namespace xray

#endif // #ifndef WINDOW_IDE_H_INCLUDED