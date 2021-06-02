////////////////////////////////////////////////////////////////////////////
//	Created		: 08.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_EDITOR_H_INCLUDED
#define SOUND_EDITOR_H_INCLUDED

#include "float_curve_editor.h"
#include "float_curve.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace WeifenLuo::WinFormsUI::Docking;

namespace xray
{
	namespace editor
	{
		ref class	sound_files_view_panel;
		ref class	sound_items_view_panel;
		ref class	sound_file_selection_event_args;
		ref class	sound_item_selection_event_args;
		ref class	sound_item_editor_panel;
		ref struct	sound_file_struct;
		ref struct	sound_item_struct;

		namespace controls
		{
			ref class scalable_scroll_bar;
		}

		#pragma unmanaged
			class editor_world;
		#pragma managed

		/// <summary>
		/// Summary for sound_editor
		///
		/// WARNING: If you change the name of this class, you will need to change the
		///          'Resource File Name' property for the managed resource compiler tool
		///          associated with all .resx files this class depends on.  Otherwise,
		///          the designers will not be able to interact properly with localized
		///          resources associated with this form.
		/// </summary>
		public ref class sound_editor : public System::Windows::Forms::Form
		{

		#pragma region | Initialize |

		public:
			sound_editor(xray::editor::editor_world& world);

		protected:
			/// <summary>
			/// Clean up any resources being used.
			/// </summary>
			~sound_editor();

		private:
			/// <summary>
			/// Required designer variable.
			/// </summary>
		#pragma region Windows Form Designer generated code

			/// <summary>
			/// Required method for Designer support - do not modify
			/// the contents of this method with the code editor.
			/// </summary>
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
				this->m_main_dock_panel = (gcnew WeifenLuo::WinFormsUI::Docking::DockPanel());
				this->main_menu_strip = (gcnew System::Windows::Forms::MenuStrip());
				this->save_file_menu_item = (gcnew System::Windows::Forms::ToolStripMenuItem());
				this->main_menu_strip->SuspendLayout();
				this->SuspendLayout();
				// 
				// m_main_dock_panel
				// 
				this->m_main_dock_panel->ActiveAutoHideContent = nullptr;
				this->m_main_dock_panel->Dock = System::Windows::Forms::DockStyle::Fill;
				this->m_main_dock_panel->DockBackColor = System::Drawing::SystemColors::Control;
				this->m_main_dock_panel->DockLeftPortion = 0.5;
				this->m_main_dock_panel->DocumentStyle = WeifenLuo::WinFormsUI::Docking::DocumentStyle::DockingWindow;
				this->m_main_dock_panel->Location = System::Drawing::Point(0, 24);
				this->m_main_dock_panel->Name = L"m_main_dock_panel";
				this->m_main_dock_panel->Size = System::Drawing::Size(499, 479);
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
				this->m_main_dock_panel->TabIndex = 0;
				// 
				// main_menu_strip
				// 
				this->main_menu_strip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->save_file_menu_item});
				this->main_menu_strip->Location = System::Drawing::Point(0, 0);
				this->main_menu_strip->Name = L"main_menu_strip";
				this->main_menu_strip->Size = System::Drawing::Size(499, 24);
				this->main_menu_strip->TabIndex = 1;
				this->main_menu_strip->Text = L"main menu";
				// 
				// save_file_menu_item
				// 
				this->save_file_menu_item->Name = L"save_file_menu_item";
				this->save_file_menu_item->Size = System::Drawing::Size(61, 20);
				this->save_file_menu_item->Text = L"save file";
				this->save_file_menu_item->Click += gcnew System::EventHandler(this, &sound_editor::save_file_menu_item_click);
				// 
				// sound_editor
				// 
				this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				this->ClientSize = System::Drawing::Size(499, 503);
				this->Controls->Add(this->m_main_dock_panel);
				this->Controls->Add(this->main_menu_strip);
				this->MainMenuStrip = this->main_menu_strip;
				this->Name = L"sound_editor";
				this->Text = L"sound_editor";
				this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &sound_editor::sound_editor_FormClosing);
				this->main_menu_strip->ResumeLayout(false);
				this->main_menu_strip->PerformLayout();
				this->ResumeLayout(false);
				this->PerformLayout();

			}
		#pragma endregion

		#pragma endregion 

		#pragma region |   Fields   |

		private: 
			WeifenLuo::WinFormsUI::Docking::DockPanel^		m_main_dock_panel;
			System::Windows::Forms::MenuStrip^				main_menu_strip;
			System::Windows::Forms::ToolStripMenuItem^		save_file_menu_item;
			System::ComponentModel::IContainer^				components;
			sound_files_view_panel^							m_sound_files_panel;
			sound_items_view_panel^							m_sound_items_panel;
			sound_item_editor_panel^						m_sound_item_editor_panel;

		public:
			xray::editor::editor_world&						m_world;
			Dictionary<String^, sound_file_struct^>^		m_sound_files;
			Dictionary<String^, sound_item_struct^>^		m_sound_items;

			static String^									absolute_sound_resources_path;
			static String^									sound_resources_path;

		#pragma endregion 
			
		#pragma region |  Methods   |

			private:
				void				dock_panels							();
				IDockContent^		reload_content						(String^	persist_string);
				void				sound_editor_FormClosing			(Object^	sender, System::Windows::Forms::FormClosingEventArgs^  e);
				void				save_file_menu_item_click			(Object^	sender, System::EventArgs^  e);
				void				sound_item_selected					(Object^	sender, sound_item_selection_event_args^ e);

			public:
				void				show_properties						(Object^ object);
				void				manual_close						();

		#pragma endregion 
		
		}; // class sound_editor
	}// namespace editor
}// namespace xray

#endif // #ifndef SOUND_EDITOR_H_INCLUDED
