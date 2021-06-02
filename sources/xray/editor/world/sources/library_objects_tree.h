////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef LIBRARY_OBJECTS_TREE_H_INCLUDED
#define LIBRARY_OBJECTS_TREE_H_INCLUDED

using namespace System::ComponentModel;
using namespace System::Windows::Forms;
using namespace System::Data;

public delegate void current_path_changed ( System::String^ new_path, System::String^ prev_path );

namespace xray{
namespace editor {

	ref class tool_base;
	ref struct lib_item_base;

	public delegate void double_clicked ( lib_item_base^ item );
	/// <summary>
	/// Summary for library_objects_tree
	/// </summary>
	public ref class library_objects_tree : public System::Windows::Forms::UserControl
	{
	public:
		library_objects_tree( tool_base^ tool )
		:m_tool(tool)
		{
			InitializeComponent ( );
			post_constructor	( );
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~library_objects_tree()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::ContextMenuStrip^  menu_strip;
	public: 
	private: System::Windows::Forms::ToolStripMenuItem^  delete_item_menu_item;

	private: System::Windows::Forms::ToolStripMenuItem^  new_folder_menu_item;


	private: System::Windows::Forms::StatusStrip^  status_strip;
	public: xray::editor::controls::tree_view^  tree_view;
	private: System::Windows::Forms::ToolStripMenuItem^  rename_menu_item;
	public: 

	public: 
	private: 
	protected: 
	private: System::ComponentModel::IContainer^  components;
	public: 

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
			this->components = (gcnew System::ComponentModel::Container());
			this->menu_strip = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->delete_item_menu_item = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->new_folder_menu_item = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->rename_menu_item = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->status_strip = (gcnew System::Windows::Forms::StatusStrip());
			this->tree_view = (gcnew xray::editor::controls::tree_view());
			this->menu_strip->SuspendLayout();
			this->SuspendLayout();
			// 
			// menu_strip
			// 
			this->menu_strip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->delete_item_menu_item, 
				this->new_folder_menu_item, this->rename_menu_item});
			this->menu_strip->Name = L"contextMenuStrip1";
			this->menu_strip->Size = System::Drawing::Size(133, 70);
			this->menu_strip->Opening += gcnew System::ComponentModel::CancelEventHandler(this, &library_objects_tree::menu_strip_Opening);
			// 
			// delete_item_menu_item
			// 
			this->delete_item_menu_item->Name = L"delete_item_menu_item";
			this->delete_item_menu_item->Size = System::Drawing::Size(132, 22);
			this->delete_item_menu_item->Text = L"Delete";
			this->delete_item_menu_item->Click += gcnew System::EventHandler(this, &library_objects_tree::delete_item_menu_item_Click);
			// 
			// new_folder_menu_item
			// 
			this->new_folder_menu_item->Name = L"new_folder_menu_item";
			this->new_folder_menu_item->Size = System::Drawing::Size(132, 22);
			this->new_folder_menu_item->Text = L"New folder";
			this->new_folder_menu_item->Click += gcnew System::EventHandler(this, &library_objects_tree::new_folder_menu_item_Click);
			// 
			// rename_menu_item
			// 
			this->rename_menu_item->Name = L"rename_menu_item";
			this->rename_menu_item->Size = System::Drawing::Size(132, 22);
			this->rename_menu_item->Text = L"Rename";
			this->rename_menu_item->Click += gcnew System::EventHandler(this, &library_objects_tree::rename_menu_item_Click);
			// 
			// status_strip
			// 
			this->status_strip->Location = System::Drawing::Point(0, 203);
			this->status_strip->Name = L"status_strip";
			this->status_strip->Size = System::Drawing::Size(189, 22);
			this->status_strip->TabIndex = 15;
			this->status_strip->Text = L"statusStrip1";
			// 
			// tree_view
			// 
			this->tree_view->AllowDrop = true;
			this->tree_view->auto_expand_on_filter = false;
			this->tree_view->ContextMenuStrip = this->menu_strip;
			this->tree_view->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tree_view->filter_visible = false;
			this->tree_view->HideSelection = false;
			this->tree_view->Indent = 27;
			this->tree_view->is_multiselect = false;
			this->tree_view->is_selectable_groups = true;
			this->tree_view->is_selection_or_groups_or_items = false;
			this->tree_view->ItemHeight = 20;
			this->tree_view->keyboard_search_enabled = false;
			this->tree_view->LabelEdit = true;
			this->tree_view->Location = System::Drawing::Point(0, 0);
			this->tree_view->Name = L"tree_view";
			this->tree_view->PathSeparator = L"/";
			this->tree_view->Size = System::Drawing::Size(189, 203);
			this->tree_view->Sorted = true;
			this->tree_view->source = nullptr;
			this->tree_view->TabIndex = 16;
			this->tree_view->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &library_objects_tree::tree_view_MouseDoubleClick);
			this->tree_view->AfterLabelEdit += gcnew System::Windows::Forms::NodeLabelEditEventHandler(this, &library_objects_tree::tree_view_AfterLabelEdit);
			this->tree_view->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &library_objects_tree::tree_view_DragDrop);
			this->tree_view->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &library_objects_tree::tree_view_AfterSelect);
			this->tree_view->ItemDrag += gcnew System::Windows::Forms::ItemDragEventHandler(this, &library_objects_tree::tree_view_ItemDrag);
			this->tree_view->DragOver += gcnew System::Windows::Forms::DragEventHandler(this, &library_objects_tree::tree_view_DragOver);
			// 
			// library_objects_tree
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->tree_view);
			this->Controls->Add(this->status_strip);
			this->Name = L"library_objects_tree";
			this->Size = System::Drawing::Size(189, 225);
			this->menu_strip->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		public:
			bool			fill_objects_list				( System::String^ initial );

			property System::String^ current_path
			{
				System::String^	get		( )		{ return m_current_path; }
				void			set		( System::String^ );
			}

			event current_path_changed^	m_current_path_changed;
			event double_clicked^		m_double_clicked;

			property bool				show_empty_folders;
			property bool sync_tool_selected_name
			{
				bool			get		( )		{ return m_sync_tool_selected_name; }
				void			set		( bool value );
			}
			property System::String^	filter;
			property bool				allow_folder_creation;
			property bool				allow_items_reparent;
			property bool				allow_items_rename;
			property bool				allow_items_delete;
			property bool				allow_folder_selection;

		private:
			tool_base^					m_tool;
			System::String^				m_current_path;
			bool						m_sync_tool_selected_name;

			controls::tree_node^		get_selected_node( );

			void			post_constructor				( );
			void			tree_view_AfterSelect			( System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e);
			void			tree_view_AfterLabelEdit		( System::Object^  sender, System::Windows::Forms::NodeLabelEditEventArgs^  e);
			void			new_folder_menu_item_Click		( System::Object^  sender, System::EventArgs^  e);
			void			delete_item_menu_item_Click		( System::Object^  sender, System::EventArgs^  e);
			void			menu_strip_Opening				( System::Object^  sender, System::ComponentModel::CancelEventArgs^  e);
			void			update_tool_selected_name		( System::String^ new_path, System::String^ prev_path );
			void			rename_menu_item_Click			( System::Object^  sender, System::EventArgs^  e );
			void			tree_view_ItemDrag				( System::Object^  sender, System::Windows::Forms::ItemDragEventArgs^  e);
			void			tree_view_DragDrop				( System::Object^  sender, System::Windows::Forms::DragEventArgs^  e);
			void			tree_view_DragOver				( System::Object^  sender, System::Windows::Forms::DragEventArgs^  e);
			void			tree_view_MouseDoubleClick		( System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
};

}
}

#endif // #ifndef LIBRARY_OBJECTS_TREE_H_INCLUDED