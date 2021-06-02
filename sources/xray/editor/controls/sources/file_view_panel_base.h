////////////////////////////////////////////////////////////////////////////
//	Created		: 20.01.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef FILE_VIEW_PANEL_BASE_H_INCLUDED
#define FILE_VIEW_PANEL_BASE_H_INCLUDED

#include "tree_view.h"
#include "tree_view_event_args.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray
{
	namespace editor
	{
		namespace controls
		{
			ref class document_editor_base;
			ref class tree_node;

			public ref class file_view_panel_base :  public WeifenLuo::WinFormsUI::Docking::DockContent
			{
			public:
				file_view_panel_base(document_editor_base^ ed, String^ panel_name):m_editor(ed)
				{
					InitializeComponent();
					this->Text = panel_name;
					in_constructor();
				}
				file_view_panel_base(document_editor_base^ ed):m_editor(ed)
				{
					InitializeComponent();
					in_constructor();
				}
				virtual	void	track_active_item			(System::String^ path);

				property System::String^ source_path;
				property System::String^ extension;

			private:
				void in_constructor();
				file_view_panel_base(){};

			protected:
				~file_view_panel_base()
				{
					if(components)
						delete components;
				}

			public:
				event TreeNodeMouseClickEventHandler^ file_double_click
				{
					void add	(TreeNodeMouseClickEventHandler^ value){m_raw_files_tree_view->NodeMouseDoubleClick+=value;}
					void remove	(TreeNodeMouseClickEventHandler^ value){m_raw_files_tree_view->NodeMouseDoubleClick-=value;}
				};

				virtual	void	remove_node				(System::Object^ , tree_view_event_args^ ) {};
				virtual	void	copy					(bool del) {};
				virtual	void	paste					() {};

			protected:
				virtual	void	on_node_double_click	(System::Object^ , System::Windows::Forms::TreeNodeMouseClickEventArgs^ ) {};
				virtual	void	on_node_key_down		(System::Object^ , System::Windows::Forms::KeyEventArgs^ ) {};
				virtual	void	add_new_folder			(System::Object^ , tree_view_event_args^ ) {};
				virtual	void	add_new_file			(System::Object^ , tree_view_event_args^ ) {};
				virtual	void	on_node_name_edit		(System::Object^ , System::Windows::Forms::NodeLabelEditEventArgs^ ) {};
				virtual	void	on_node_drag_over		(System::Object^ , System::Windows::Forms::DragEventArgs^ ) {};
				virtual	void	on_node_drag_drop		(System::Object^ , System::Windows::Forms::DragEventArgs^ ) {};
				virtual	void	on_tree_mouse_down		(System::Object^ , System::Windows::Forms::MouseEventArgs^ ) {};
				virtual	void	on_tree_mouse_move		(System::Object^ , System::Windows::Forms::MouseEventArgs^ ) {};
						void	add_folders_and_files	(System::Windows::Forms::TreeNodeCollection^ col, System::String^ path, Int32 folder_icon_collapsed, Int32 folder_icon_expanded, Int32 file_icon);
						void	iterate_folders			(System::IO::DirectoryInfo^ di, System::String^ path);
						bool	check_parent			(System::String^ dest_path, System::String^ node_name);

		#pragma region Windows Form Designer generated code

		private: System::ComponentModel::IContainer^  components;

				void InitializeComponent(void)
				{
					this->components = (gcnew System::ComponentModel::Container());
					this->m_raw_files_tree_view = (gcnew xray::editor::controls::tree_view());
					this->SuspendLayout();
					// 
					// m_raw_files_tree_view
					// 
					this->m_raw_files_tree_view->AllowDrop = true;
					this->m_raw_files_tree_view->auto_expand_on_filter = false;
					this->m_raw_files_tree_view->Dock = System::Windows::Forms::DockStyle::Fill;
					this->m_raw_files_tree_view->filter_visible = false;
					this->m_raw_files_tree_view->HideSelection = false;
					this->m_raw_files_tree_view->Indent = 22;
					this->m_raw_files_tree_view->is_multiselect = false;
					this->m_raw_files_tree_view->is_selectable_groups = true;
					this->m_raw_files_tree_view->ItemHeight = 20;
					this->m_raw_files_tree_view->LabelEdit = true;
					this->m_raw_files_tree_view->Location = System::Drawing::Point(0, 0);
					this->m_raw_files_tree_view->Name = L"m_raw_files_tree_view";
					this->m_raw_files_tree_view->PathSeparator = L"/";
					this->m_raw_files_tree_view->Size = System::Drawing::Size(371, 460);
					this->m_raw_files_tree_view->source = nullptr;
					this->m_raw_files_tree_view->TabIndex = 0;
					this->m_raw_files_tree_view->NodeMouseDoubleClick += gcnew System::Windows::Forms::TreeNodeMouseClickEventHandler(this, &file_view_panel_base::on_node_double_click);
//					this->m_raw_files_tree_view->item_create += gcnew System::EventHandler<xray::editor::tree_view_event_args^ >(this, &file_view_panel_base::add_new_file);
					this->m_raw_files_tree_view->AfterLabelEdit += gcnew System::Windows::Forms::NodeLabelEditEventHandler(this, &file_view_panel_base::on_node_name_edit);
					this->m_raw_files_tree_view->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &file_view_panel_base::on_node_drag_drop);
					this->m_raw_files_tree_view->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &file_view_panel_base::on_tree_mouse_move);
					this->m_raw_files_tree_view->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &file_view_panel_base::on_tree_mouse_down);
//					this->m_raw_files_tree_view->group_create += gcnew System::EventHandler<xray::editor::tree_view_event_args^ >(this, &file_view_panel_base::add_new_folder);
					this->m_raw_files_tree_view->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &file_view_panel_base::on_node_key_down);
					this->m_raw_files_tree_view->DragOver += gcnew System::Windows::Forms::DragEventHandler(this, &file_view_panel_base::on_node_drag_over);
//					this->m_raw_files_tree_view->item_remove += gcnew System::EventHandler<xray::editor::tree_view_event_args^ >(this, &file_view_panel_base::remove_node);
					// 
					// file_view_panel_base
					// 
					this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
					this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
					this->ClientSize = System::Drawing::Size(371, 460);
					this->Controls->Add(this->m_raw_files_tree_view);
					this->DockAreas = static_cast<WeifenLuo::WinFormsUI::Docking::DockAreas>(((((WeifenLuo::WinFormsUI::Docking::DockAreas::Float | WeifenLuo::WinFormsUI::Docking::DockAreas::DockLeft) 
						| WeifenLuo::WinFormsUI::Docking::DockAreas::DockRight) 
						| WeifenLuo::WinFormsUI::Docking::DockAreas::DockTop) 
						| WeifenLuo::WinFormsUI::Docking::DockAreas::DockBottom));
					this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
						static_cast<System::Byte>(0)));
					this->Name = L"file_view_panel_base";
					this->Text = L"Files viewer";
					this->ResumeLayout(false);

				} // void InitializeComponent(void)
		#pragma endregion

			protected:


			protected: 

				document_editor_base^					m_editor;
				xray::editor::controls::tree_view^		m_raw_files_tree_view;
				tree_node^								m_drag_node;

			public:
				property tree_view_source^		files_source
				{
					tree_view_source^		get()								{return m_raw_files_tree_view->source;};
					void					set(tree_view_source^ value)		{m_raw_files_tree_view->source=value;};
				}

				property controls::tree_view^	tree_view
				{
					controls::tree_view^ get(){return m_raw_files_tree_view;}
				}

				property IContainer^  Components
				{
					IContainer^ get(){return components;}
				}

			public:
				void	refresh();
			}; // class file_view_panel_base
		} // namespace controls
	} // namespace editor
} // namespace xray

#endif // #ifndef FILE_VIEW_PANEL_BASE_H_INCLUDED