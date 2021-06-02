////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_ITEMS_VIEW_PANEL_H_INCLUDED
#define SOUND_ITEMS_VIEW_PANEL_H_INCLUDED

#include "sound_item_selection_event_args.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

using namespace xray::editor::controls;

namespace xray
{
	namespace editor
	{
		ref class sound_item_selection_event_args;
		ref struct sound_item_struct;

		/// <summary>
		/// Summary for sound_items_view_panel
		///
		/// WARNING: If you change the name of this class, you will need to change the
		///          'Resource File Name' property for the managed resource compiler tool
		///          associated with all .resx files this class depends on.  Otherwise,
		///          the designers will not be able to interact properly with localized
		///          resources associated with this form.
		/// </summary>
		public ref class sound_items_view_panel : public WeifenLuo::WinFormsUI::Docking::DockContent
		{

		#pragma region | Initialize |

		public:
			sound_items_view_panel(void)
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
			~sound_items_view_panel()
			{
				if (components)
				{
					delete components;
				}
			}

		private:

			void in_constructor();

			#pragma region Windows Form Designer generated code
			/// <summary>
			/// Required method for Designer support - do not modify
			/// the contents of this method with the code editor.
			/// </summary>
			void InitializeComponent(void)
			{
				this->components = (gcnew System::ComponentModel::Container());
				System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(sound_items_view_panel::typeid));
				this->m_tree_view_image_list = (gcnew System::Windows::Forms::ImageList(this->components));
				this->m_main_tool_strip = (gcnew System::Windows::Forms::ToolStrip());
				this->m_new_sound_item_tool_strip_button = (gcnew System::Windows::Forms::ToolStripButton());
				this->toolStripButton1 = (gcnew System::Windows::Forms::ToolStripButton());
				this->m_item_context_menu_strip = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
				this->newItemToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
				this->newFolderToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
				this->m_sound_items_tree_view = (gcnew xray::editor::controls::tree_view());
				this->m_main_tool_strip->SuspendLayout();
				this->m_item_context_menu_strip->SuspendLayout();
				this->SuspendLayout();
				// 
				// m_tree_view_image_list
				// 
				this->m_tree_view_image_list->ImageStream = (cli::safe_cast<System::Windows::Forms::ImageListStreamer^  >(resources->GetObject(L"m_tree_view_image_list.ImageStream")));
				this->m_tree_view_image_list->TransparentColor = System::Drawing::Color::Transparent;
				this->m_tree_view_image_list->Images->SetKeyName(0, L"folder_closed.bmp");
				this->m_tree_view_image_list->Images->SetKeyName(1, L"folder_opened.bmp");
				this->m_tree_view_image_list->Images->SetKeyName(2, L"sound_file.bmp");
				// 
				// m_main_tool_strip
				// 
				this->m_main_tool_strip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->m_new_sound_item_tool_strip_button, 
					this->toolStripButton1});
				this->m_main_tool_strip->Location = System::Drawing::Point(0, 0);
				this->m_main_tool_strip->Name = L"m_main_tool_strip";
				this->m_main_tool_strip->Size = System::Drawing::Size(284, 25);
				this->m_main_tool_strip->TabIndex = 1;
				this->m_main_tool_strip->Text = L"toolStrip1";
				// 
				// m_new_sound_item_tool_strip_button
				// 
				this->m_new_sound_item_tool_strip_button->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
				this->m_new_sound_item_tool_strip_button->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"m_new_sound_item_tool_strip_button.Image")));
				this->m_new_sound_item_tool_strip_button->ImageTransparentColor = System::Drawing::Color::Magenta;
				this->m_new_sound_item_tool_strip_button->Name = L"m_new_sound_item_tool_strip_button";
				this->m_new_sound_item_tool_strip_button->Size = System::Drawing::Size(33, 22);
				this->m_new_sound_item_tool_strip_button->Text = L"new";
				this->m_new_sound_item_tool_strip_button->Click += gcnew System::EventHandler(this, &sound_items_view_panel::m_new_sound_item_tool_strip_button_Click);
				// 
				// toolStripButton1
				// 
				this->toolStripButton1->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
				this->toolStripButton1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripButton1.Image")));
				this->toolStripButton1->ImageTransparentColor = System::Drawing::Color::Magenta;
				this->toolStripButton1->Name = L"toolStripButton1";
				this->toolStripButton1->Size = System::Drawing::Size(34, 22);
				this->toolStripButton1->Text = L"save";
				this->toolStripButton1->Click += gcnew System::EventHandler(this, &sound_items_view_panel::toolStripButton1_Click);
				// 
				// m_item_context_menu_strip
				// 
				this->m_item_context_menu_strip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->newItemToolStripMenuItem, 
					this->newFolderToolStripMenuItem});
				this->m_item_context_menu_strip->Name = L"m_item_context_menu_strip";
				this->m_item_context_menu_strip->Size = System::Drawing::Size(133, 48);
				// 
				// newItemToolStripMenuItem
				// 
				this->newItemToolStripMenuItem->Name = L"newItemToolStripMenuItem";
				this->newItemToolStripMenuItem->Size = System::Drawing::Size(132, 22);
				this->newItemToolStripMenuItem->Text = L"Add Item";
				this->newItemToolStripMenuItem->Click += gcnew System::EventHandler(this, &sound_items_view_panel::newItemToolStripMenuItem_Click);
				// 
				// newFolderToolStripMenuItem
				// 
				this->newFolderToolStripMenuItem->Name = L"newFolderToolStripMenuItem";
				this->newFolderToolStripMenuItem->Size = System::Drawing::Size(132, 22);
				this->newFolderToolStripMenuItem->Text = L"Add Folder";
				this->newFolderToolStripMenuItem->Click += gcnew System::EventHandler(this, &sound_items_view_panel::newFolderToolStripMenuItem_Click);
				// 
				// m_sound_items_tree_view
				// 
				this->m_sound_items_tree_view->auto_expand_on_filter = false;
				this->m_sound_items_tree_view->ContextMenuStrip = this->m_item_context_menu_strip;
				this->m_sound_items_tree_view->Dock = System::Windows::Forms::DockStyle::Fill;
				this->m_sound_items_tree_view->filter_visible = false;
				this->m_sound_items_tree_view->HideSelection = false;
				this->m_sound_items_tree_view->ImageIndex = 0;
				this->m_sound_items_tree_view->ImageList = this->m_tree_view_image_list;
				this->m_sound_items_tree_view->Indent = 22;
				this->m_sound_items_tree_view->is_multiselect = false;
				this->m_sound_items_tree_view->is_selectable_groups = true;
				this->m_sound_items_tree_view->ItemHeight = 20;
				this->m_sound_items_tree_view->LabelEdit = true;
				this->m_sound_items_tree_view->Location = System::Drawing::Point(0, 25);
				this->m_sound_items_tree_view->Name = L"m_sound_items_tree_view";
				this->m_sound_items_tree_view->PathSeparator = L"/";
				this->m_sound_items_tree_view->SelectedImageIndex = 0;
				this->m_sound_items_tree_view->Size = System::Drawing::Size(311, 365);
				this->m_sound_items_tree_view->source = nullptr;
				this->m_sound_items_tree_view->TabIndex = 0;
				this->m_sound_items_tree_view->AfterSelect		+= gcnew TreeViewEventHandler(this, &sound_items_view_panel::sound_items_tree_view_after_select);
				this->m_sound_items_tree_view->group_create		+= gcnew EventHandler<tree_view_event_args^>(this, &sound_items_view_panel::sound_items_tree_view_folder_create);
				this->m_sound_items_tree_view->item_create		+= gcnew EventHandler<tree_view_event_args^>(this, &sound_items_view_panel::sound_items_tree_view_file_create);
				this->m_sound_items_tree_view->item_remove		+= gcnew EventHandler<tree_view_event_args^>(this, &sound_items_view_panel::sound_items_tree_view_item_remove);
				this->m_sound_items_tree_view->AfterLabelEdit	+= gcnew NodeLabelEditEventHandler(this, &sound_items_view_panel::sound_items_tree_view_item_label_edit);
				// 
				// sound_items_view_panel
				// 
				this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				this->ClientSize = System::Drawing::Size(284, 264);
				this->Controls->Add(this->m_sound_items_tree_view);
				this->Controls->Add(this->m_main_tool_strip);
				this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(204)));
				this->Name = L"sound_items_view_panel";
				this->Text = L"Sound Items";
				this->m_main_tool_strip->ResumeLayout(false);
				this->m_main_tool_strip->PerformLayout();
				this->m_item_context_menu_strip->ResumeLayout(false);
				this->ResumeLayout(false);
				this->PerformLayout();

			}
			#pragma endregion

		#pragma endregion

		#pragma region |   Events   |

		public:
			event EventHandler<sound_item_selection_event_args^>^ sound_item_selected;
			event EventHandler<sound_item_selection_event_args^>^ sound_item_deselected;
			
		#pragma endregion

		#pragma region |   Fields   |

		private:
			System::Windows::Forms::ImageList^			m_tree_view_image_list;
			System::Windows::Forms::ToolStrip^			m_main_tool_strip;
			System::Windows::Forms::ToolStripButton^	m_new_sound_item_tool_strip_button;
			System::Windows::Forms::ToolStripButton^	toolStripButton1;
			System::Windows::Forms::ContextMenuStrip^	m_item_context_menu_strip;

			System::Windows::Forms::ToolStripMenuItem^  newItemToolStripMenuItem;
			System::Windows::Forms::ToolStripMenuItem^  newFolderToolStripMenuItem;
			xray::editor::controls::tree_view^			m_sound_items_tree_view;

			System::ComponentModel::IContainer^			components;

		#pragma endregion

		#pragma region |   Methods  |

		private:
			void	sound_items_tree_view_after_select			(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e);
			void	load_complete								(sound_item_struct^ loader);
			void	m_new_sound_item_tool_strip_button_Click	(System::Object^  sender, System::EventArgs^  e);
			void	toolStripButton1_Click						(System::Object^  sender, System::EventArgs^  e);

			void	sound_items_tree_view_item_label_edit		(System::Object^  sender, NodeLabelEditEventArgs^  e);
			void	sound_items_tree_view_file_create			(System::Object^  sender, tree_view_event_args^  e);
			void	sound_items_tree_view_folder_create			(System::Object^  sender, tree_view_event_args^  e);
			void	sound_items_tree_view_item_remove			(System::Object^  sender, tree_view_event_args^  e);
			void	newItemToolStripMenuItem_Click				(System::Object^  sender, System::EventArgs^  e);
			void	newFolderToolStripMenuItem_Click			(System::Object^  sender, System::EventArgs^  e);

		#pragma endregion

		};//class sound_items_view_panel
	}//namespace editor
}//namespace xray

#endif // #ifndef SOUND_ITEMS_VIEW_PANEL_H_INCLUDED
