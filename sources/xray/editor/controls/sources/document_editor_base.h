////////////////////////////////////////////////////////////////////////////
//	Created		: 20.01.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef DOCUMENT_EDITOR_BASE_H_INCLUDED
#define DOCUMENT_EDITOR_BASE_H_INCLUDED

#include "delegates.h"
#include "file_view_panel_base.h"
#include "item_properties_panel_base.h"
#include "document_event_args.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace WeifenLuo::WinFormsUI::Docking;


namespace xray
{
	namespace editor
	{
		namespace controls
		{
			ref class document_base;
			ref class file_view_panel_base;
			ref class item_properties_panel_base;
			ref class toolbar_panel_base;

			public ref class document_editor_base : public System::Windows::Forms::UserControl
			{

			#pragma region |   Events   |

			public:
				document_create_callback^		creating_new_document;
				content_reload_callback^		content_reloading;

				event EventHandler^				panels_loaded;
				event EventHandler^				panels_saved;

				event EventHandler^				editor_exited;
				event EventHandler^				active_saved;
				event EventHandler^				all_saved;
				event EventHandler^				history_undo;
				event EventHandler^				history_redo;
				event EventHandler^				cuted;
				event EventHandler^				deleted;
				event EventHandler^				copied;
				event EventHandler^				pasted;
				event EventHandler^				all_selected;
				event EventHandler^				form_closed;

				event EventHandler^				document_closed;
				event EventHandler^				document_activated;
				event EventHandler^				document_loaded;
				event EventHandler<document_event_args^>^	document_created;

			#pragma endregion

			#pragma region | Initialize |

			public:
				document_editor_base()
				{
					m_create_menu				= true;
					InitializeComponent();
					this->Name					= "_";
					//view_panel					= nullptr;
					//properties_panel			= nullptr;
					//toolbar_panel				= nullptr;
					m_active_document			= nullptr;

					create_view_panel			= true;
					create_properties_panel		= true;
					create_toolbar_panel		= true;
					m_loaded					= false;

					m_opened_documents			= gcnew Collections::Generic::List<document_base^>();
					in_constructor();
				}
				document_editor_base(System::String^ name)
				{
					m_create_menu				= true;
					InitializeComponent();
					this->Name					= name;
					/*view_panel					= nullptr;
					properties_panel			= nullptr;
					toolbar_panel				= nullptr;*/
					m_active_document			= nullptr;

					create_view_panel			= true;
					create_properties_panel		= true;
					create_toolbar_panel		= true;
					m_loaded					= false;

					m_opened_documents			= gcnew Collections::Generic::List<document_base^>();
					in_constructor();
				}
				document_editor_base(System::String^ name, Boolean create_menu)
				{
					m_create_menu				= create_menu;
					InitializeComponent();
					this->Name					= name;
					/*view_panel					= nullptr;
					properties_panel			= nullptr;
					toolbar_panel				= nullptr;*/
					m_active_document			= nullptr;

					create_view_panel			= true;
					create_properties_panel		= true;
					create_toolbar_panel		= true;

					m_opened_documents			= gcnew Collections::Generic::List<document_base^>();
					in_constructor();
				}

			protected:
				~document_editor_base()
				{
					if (components)
						delete components;
				}
			private:
				void	in_constructor();

			protected:
				System::ComponentModel::Container^			components;
				System::Windows::Forms::ToolStripMenuItem^  editToolStripMenuItem;							

			public:
				System::Windows::Forms::MenuStrip^			menuStrip1;
				System::Windows::Forms::ToolStripMenuItem^  fileToolStripMenuItem;
				System::Windows::Forms::ToolStripMenuItem^  newToolStripMenuItem;
				System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;
				System::Windows::Forms::ToolStripMenuItem^  saveToolStripMenuItem;
				System::Windows::Forms::ToolStripMenuItem^  saveAllToolStripMenuItem;
				System::Windows::Forms::ToolStripMenuItem^  undoToolStripMenuItem;
				System::Windows::Forms::ToolStripMenuItem^  redoToolStripMenuItem;
				System::Windows::Forms::ToolStripMenuItem^  cutToolStripMenuItem;
				System::Windows::Forms::ToolStripMenuItem^  copyToolStripMenuItem;
				System::Windows::Forms::ToolStripMenuItem^  pasteToolStripMenuItem;
				System::Windows::Forms::ToolStripMenuItem^  selectAllToolStripMenuItem;
				System::Windows::Forms::ToolStripMenuItem^  delToolStripMenuItem;
			
			#pragma region Windows Form Designer generated code

				virtual	void InitializeComponent(void)
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

					
					this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
					this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
					this->newToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
					this->saveToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
					this->saveAllToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
					this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
					this->editToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
					this->undoToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
					this->redoToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
					this->cutToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
					this->copyToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
					this->pasteToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
					this->selectAllToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
					this->delToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
					this->menuStrip1->SuspendLayout();
					
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
					this->m_main_dock_panel->Size = System::Drawing::Size(292, 249);
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
					// menuStrip1
					// 
					this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->fileToolStripMenuItem, 
						this->editToolStripMenuItem});
					this->menuStrip1->Location = System::Drawing::Point(0, 0);
					this->menuStrip1->Name = L"menuStrip1";
					this->menuStrip1->Size = System::Drawing::Size(292, 24);
					this->menuStrip1->TabIndex = 1;
					this->menuStrip1->Text = L"menuStrip1";
					// 
					// fileToolStripMenuItem
					// 
					this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->newToolStripMenuItem, 
						this->saveToolStripMenuItem, this->saveAllToolStripMenuItem, this->exitToolStripMenuItem});
					this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
					this->fileToolStripMenuItem->Size = System::Drawing::Size(37, 20);
					this->fileToolStripMenuItem->Text = L"&File";
					// 
					// newToolStripMenuItem
					// 
					this->newToolStripMenuItem->Name = L"newToolStripMenuItem";
					this->newToolStripMenuItem->Size = System::Drawing::Size(197, 22);
					this->newToolStripMenuItem->Text = L"&New Document";
					//this->newToolStripMenuItem->Click += gcnew System::EventHandler(this, &document_editor_base::new_document);
					// 
					// saveToolStripMenuItem
					// 
					this->saveToolStripMenuItem->Name = L"saveToolStripMenuItem";
					//this->saveToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::S));
					this->saveToolStripMenuItem->Size = System::Drawing::Size(197, 22);
					this->saveToolStripMenuItem->Text = L"&Save Document";
					//this->saveToolStripMenuItem->Click += gcnew System::EventHandler(this, &document_editor_base::save_active);
					// 
					// saveAllToolStripMenuItem
					// 
					this->saveAllToolStripMenuItem->Name = L"saveAllToolStripMenuItem";
					//this->saveAllToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>(((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::Shift) 
					//	| System::Windows::Forms::Keys::S));
					this->saveAllToolStripMenuItem->Size = System::Drawing::Size(197, 22);
					this->saveAllToolStripMenuItem->Text = L"Save &All";
					//this->saveAllToolStripMenuItem->Click += gcnew System::EventHandler(this, &document_editor_base::save_all);
					// 
					// exitToolStripMenuItem
					// 
					this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
					this->exitToolStripMenuItem->Size = System::Drawing::Size(197, 22);
					this->exitToolStripMenuItem->Text = L"&Exit";
					//this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &document_editor_base::exit_editor);
					// 
					// editToolStripMenuItem
					// 
					this->editToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(7) {this->undoToolStripMenuItem, 
						this->redoToolStripMenuItem, this->cutToolStripMenuItem, this->copyToolStripMenuItem, this->pasteToolStripMenuItem, this->selectAllToolStripMenuItem, 
						this->delToolStripMenuItem});
					this->editToolStripMenuItem->Name = L"editToolStripMenuItem";
					this->editToolStripMenuItem->Size = System::Drawing::Size(39, 20);
					this->editToolStripMenuItem->Text = L"&Edit";
					// 
					// undoToolStripMenuItem
					// 
					this->undoToolStripMenuItem->Name = L"undoToolStripMenuItem";
					//this->undoToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::Z));
					this->undoToolStripMenuItem->Size = System::Drawing::Size(164, 22);
					this->undoToolStripMenuItem->Text = L"&Undo";
					//this->undoToolStripMenuItem->Click += gcnew System::EventHandler(this, &document_editor_base::undo);
					// 
					// redoToolStripMenuItem
					// 
					this->redoToolStripMenuItem->Name = L"redoToolStripMenuItem";
					//this->redoToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::Y));
					this->redoToolStripMenuItem->Size = System::Drawing::Size(164, 22);
					this->redoToolStripMenuItem->Text = L"&Redo";
					//this->redoToolStripMenuItem->Click += gcnew System::EventHandler(this, &document_editor_base::redo);
					// 
					// cutToolStripMenuItem
					// 
					this->cutToolStripMenuItem->Name = L"cutToolStripMenuItem";
					//this->cutToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::X));
					this->cutToolStripMenuItem->Size = System::Drawing::Size(164, 22);
					this->cutToolStripMenuItem->Text = L"Cu&t";
					//this->cutToolStripMenuItem->Click += gcnew System::EventHandler(this, &document_editor_base::cut);
					// 
					// copyToolStripMenuItem
					// 
					this->copyToolStripMenuItem->Name = L"copyToolStripMenuItem";
					//this->copyToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::C));
					this->copyToolStripMenuItem->Size = System::Drawing::Size(164, 22);
					this->copyToolStripMenuItem->Text = L"&Copy";
					//this->copyToolStripMenuItem->Click += gcnew System::EventHandler(this, &document_editor_base::copy);
					// 
					// pasteToolStripMenuItem
					// 
					this->pasteToolStripMenuItem->Name = L"pasteToolStripMenuItem";
					//this->pasteToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::V));
					this->pasteToolStripMenuItem->Size = System::Drawing::Size(164, 22);
					this->pasteToolStripMenuItem->Text = L"&Paste";
					//this->pasteToolStripMenuItem->Click += gcnew System::EventHandler(this, &document_editor_base::paste);
					// 
					// selectAllToolStripMenuItem
					// 
					this->selectAllToolStripMenuItem->Name = L"selectAllToolStripMenuItem";
					//this->selectAllToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::A));
					this->selectAllToolStripMenuItem->Size = System::Drawing::Size(164, 22);
					this->selectAllToolStripMenuItem->Text = L"Select &All";
					//this->selectAllToolStripMenuItem->Click += gcnew System::EventHandler(this, &document_editor_base::select_all);
					// 
					// delToolStripMenuItem
					// 
					this->delToolStripMenuItem->Name = L"delToolStripMenuItem";
					//this->delToolStripMenuItem->ShortcutKeys = System::Windows::Forms::Keys::Delete;
					this->delToolStripMenuItem->Size = System::Drawing::Size(164, 22);
					this->delToolStripMenuItem->Text = L"&Delete";
					//this->delToolStripMenuItem->Click += gcnew System::EventHandler(this, &document_editor_base::del);
					// 
					// document_editor_base
					// 
					this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
					this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
					this->ClientSize = System::Drawing::Size(292, 273);
					this->Controls->Add(this->m_main_dock_panel);
					this->Name = L"document_editor_base";
					this->Controls->Add(this->menuStrip1);
					this->menuStrip1->ResumeLayout(false);
					this->menuStrip1->PerformLayout();			
					this->ResumeLayout(false);
					this->PerformLayout();

				}

			#pragma endregion
			#pragma endregion

			#pragma region |   Fields   |
			
			private:
				Boolean										m_create_menu;
				
			protected:
				Boolean										m_loaded;
				WeifenLuo::WinFormsUI::Docking::DockPanel^	m_main_dock_panel;
				document_base^								m_active_document;
				Collections::Generic::List<document_base^>^	m_opened_documents;

			#pragma endregion

			#pragma region | Properties |

			public:
				property Boolean							create_view_panel;
				property Boolean							create_properties_panel;
				property Boolean							create_toolbar_panel;
				property Boolean							is_reuse_single_document;

				/*property file_view_panel_base^				view_panel;
				property item_properties_panel_base^		properties_panel;
				property toolbar_panel_base^				toolbar_panel;*/
				property Boolean							create_base_panel_objects;

				/*property Boolean							is_properties_read_only
				{
					Boolean						get(){return !properties_panel->Enabled;}
					void						set(Boolean value){properties_panel->Enabled = !value;}
				}*/
				property Boolean							is_fixed_panels
				{
					Boolean						get(){return !m_main_dock_panel->AllowEndUserDocking;}
					void						set(Boolean value);
				}
				property Boolean							is_single_document
				{
					Boolean						get(){return m_main_dock_panel->DocumentStyle == DocumentStyle::DockingSdi;}
					void						set(Boolean value){m_main_dock_panel->DocumentStyle = (value)?DocumentStyle::DockingSdi:DocumentStyle::DockingMdi;};
				}
				property document_base^						active_document
				{
					document_base^				get(){return m_active_document;}
					void						set(document_base^ value){m_active_document = value;}
				}
				property Collections::Generic::List<document_base^>^	opened_documents
				{
					Collections::Generic::List<document_base^>^		get(){return m_opened_documents;}
				}
				property DockPanel^							main_dock_panel
				{
					DockPanel^					get(){return m_main_dock_panel;}
				}

			#pragma endregion 

			#pragma region |   Methods  |

			public:
				virtual document_base^	create_new_document			();
				virtual void			new_document				();
				virtual void			add_document				( document_base^ doc );
				virtual document_base^	get_document				( String^ doc_name );
				virtual	void			exit_editor					();
				virtual	void			save_active					();
				virtual	void			save_all					();
				virtual	void			undo						();
				virtual	void			redo						();
				virtual	void			cut							();
				virtual	void			del							();
				virtual	void			copy						();
				virtual	void			paste						();
				virtual	void			select_all					();
				virtual IDockContent^	reload_content				(String^ panel_full_name);
						void			close_all_documents			();
				inline	bool			has_active_document			(){return m_active_document != nullptr;}

			public:
				virtual	void			on_document_closing			(document_base^ closing_doc);
				virtual	void			on_document_activated		(document_base^ doc);
				virtual	void			load_document				(String^ full_path);
				virtual	document_base^	load_document				(String^ full_path, Boolean need_show);
						document_base^	load_document_part			(String^ full_path, String^ part_path, Boolean need_show);
				virtual	bool			is_opened					(String^ doc_name);
						void			remove_context_menu			();
						void			main_panel_content_added	(Object^ sender, DockContentEventArgs^ e);

				virtual Boolean			load_panels					(Form^ parent);
				void					save_panels					(Form^ parent);

						void			add_menu_items				(MenuStrip^ parent);
						void			add_menu_items				(ToolStripMenuItem^ menu_item);

			#pragma endregion

			};//class document_editor_base
		}//namespace controls
	}//namespace editor
}//namespace xray

#endif // #ifndef DOCUMENT_EDITOR_BASE_H_INCLUDED