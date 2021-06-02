////////////////////////////////////////////////////////////////////////////
//	Created		: 15.02.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_LEXEME_EDITOR_H_INCLUDED
#define ANIMATION_LEXEME_EDITOR_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace WeifenLuo::WinFormsUI::Docking;
using namespace xray::editor::wpf_controls::animation_lexeme_panel;

using WeifenLuo::WinFormsUI::Docking::DockContent;

namespace xray {
namespace animation_editor {
	
	ref class viewer_editor;
	ref class animation_viewer_graph_node_base;
	ref class animation_viewer_graph_node_animation;
	ref class animations_view_panel;
	ref class animation_node_clip_instance;
	value class intervals_request;

	public ref class animation_lexeme_editor: public System::Windows::Forms::Form
	{
		typedef Dictionary<String^, List<intervals_request>^ > intervals_request_type;
	public:
		animation_lexeme_editor(viewer_editor^ ed);
		~animation_lexeme_editor();
		void show(WeifenLuo::WinFormsUI::Docking::DockPanel^ form, animation_viewer_graph_node_base^ n);
		void add_animation(System::String^ anim_name, bool presaved);

		property xray::editor::wpf_controls::animation_lexeme_panel::animation_lexeme_panel^ lexeme_panel
		{
			xray::editor::wpf_controls::animation_lexeme_panel::animation_lexeme_panel^ get () {return m_lexeme_panel_host->panel;};
		};

	private:
		void				on_ok_clicked				( System::Object^, System::EventArgs^ );
		void				on_cancel_clicked			( System::Object^, System::EventArgs^ );
		void				on_form_closing				( System::Object^, System::Windows::Forms::FormClosingEventArgs^ e );
		void				on_animation_loaded			( animation_node_clip_instance^ new_clip );
		void				on_lexeme_modified			( System::Object^, System::EventArgs^ );
		void				on_presaved_clip_loaded		( xray::resources::queries_result& data );
		IDockContent^		find_dock_content			( String^ persist_string );

	private:
		System::ComponentModel::Container^		components;
		System::Windows::Forms::Button^			button1;
		System::Windows::Forms::Button^			button2;
		viewer_editor^							m_editor;
		animation_viewer_graph_node_animation^	m_node;
		animations_view_panel^					m_layout_panel;
		DockPanel^								m_main_dock_panel;
		animation_lexeme_panel_host^			m_lexeme_panel_host;
		DockContent^							m_animation_lexeme_panel;
		bool									m_lexeme_modified;
		System::Windows::Forms::Panel^			panel1;
		bool									m_dont_handle_events;
		bool									m_presaved_mode;
		intervals_request_type^					m_request;

		void InitializeComponent(void)
		{
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
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->m_main_dock_panel = (gcnew WeifenLuo::WinFormsUI::Docking::DockPanel());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->panel1->SuspendLayout();
			this->SuspendLayout();
			// 
			// button1
			// 
			this->button1->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->button1->Dock = System::Windows::Forms::DockStyle::Right;
			this->button1->Location = System::Drawing::Point(586, 0);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 25);
			this->button1->TabIndex = 0;
			this->button1->Text = L"OK";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &animation_lexeme_editor::on_ok_clicked);
			// 
			// button2
			// 
			this->button2->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->button2->Dock = System::Windows::Forms::DockStyle::Right;
			this->button2->Location = System::Drawing::Point(661, 0);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(75, 25);
			this->button2->TabIndex = 1;
			this->button2->Text = L"Cancel";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &animation_lexeme_editor::on_cancel_clicked);
			// 
			// m_main_dock_panel
			// 
			this->m_main_dock_panel->ActiveAutoHideContent = nullptr;
			this->m_main_dock_panel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->m_main_dock_panel->DockBackColor = System::Drawing::SystemColors::Control;
			this->m_main_dock_panel->DocumentStyle = WeifenLuo::WinFormsUI::Docking::DocumentStyle::DockingSdi;
			this->m_main_dock_panel->Location = System::Drawing::Point(0, 0);
			this->m_main_dock_panel->Name = L"m_main_dock_panel";
			this->m_main_dock_panel->Size = System::Drawing::Size(736, 490);
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
			this->m_main_dock_panel->Skin = dockPanelSkin1;
			this->m_main_dock_panel->TabIndex = 2;
			// 
			// panel1
			// 
			this->panel1->Controls->Add(this->button1);
			this->panel1->Controls->Add(this->button2);
			this->panel1->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->panel1->Location = System::Drawing::Point(0, 465);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(736, 25);
			this->panel1->TabIndex = 3;
			// 
			// animation_lexeme_editor
			// 
			this->AcceptButton = this->button1;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->button2;
			this->ClientSize = System::Drawing::Size(736, 490);
			this->Controls->Add(this->m_main_dock_panel);
			this->Controls->Add(this->panel1);
			this->Name = L"animation_lexeme_editor";
			this->ShowInTaskbar = false;
			this->Text = L"Animation lexeme editor";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &animation_lexeme_editor::on_form_closing);
			this->panel1->ResumeLayout(false);
			this->ResumeLayout(false);
		}
	}; // class animation_lexeme_editor
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_LEXEME_EDITOR_H_INCLUDED