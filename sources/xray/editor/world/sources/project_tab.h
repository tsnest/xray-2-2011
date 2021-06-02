////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PROJECT_TAB_H_INCLUDED
#define PROJECT_TAB_H_INCLUDED

using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using xray::editor::controls::tree_view_selection_event_args;

namespace xray {
namespace editor {
	
	ref class level_editor;
	/// <summary>
	/// Summary for project_tab
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class project_tab : public WeifenLuo::WinFormsUI::Docking::DockContent
	{
	private:
			void	in_constructor		( );


	private: System::Windows::Forms::ContextMenuStrip^  global_view_menu;

	private: System::Windows::Forms::ToolStripMenuItem^  addToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  newFilterToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  removeToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  addToIgnoreListToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  removeFromIgnoreListToolStripMenuItem;
	private: System::Windows::Forms::Timer^  scroll_timer;
	private: System::Windows::Forms::StatusStrip^  statusStrip1;
	private: System::Windows::Forms::ToolStripStatusLabel^  totalStatusLabel1;
	private: System::Windows::Forms::ToolStripStatusLabel^  selectedStatusLabel1;









			 level_editor^		m_level_editor;
	public:
		project_tab(level_editor^ le):m_level_editor(le)
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
		~project_tab()
		{
			if (components)
			{
				delete components;
			}
		}











	public: xray::editor::controls::tree_view^  treeView;
	private: 




	private: System::ComponentModel::IContainer^  components;







	protected: 

	protected: 


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
			this->treeView = (gcnew xray::editor::controls::tree_view());
			this->global_view_menu = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->addToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->newFilterToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->removeToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->addToIgnoreListToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->removeFromIgnoreListToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->scroll_timer = (gcnew System::Windows::Forms::Timer(this->components));
			this->statusStrip1 = (gcnew System::Windows::Forms::StatusStrip());
			this->totalStatusLabel1 = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->selectedStatusLabel1 = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->global_view_menu->SuspendLayout();
			this->statusStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// treeView
			// 
			this->treeView->AllowDrop = true;
			this->treeView->auto_expand_on_filter = false;
			this->treeView->BackColor = System::Drawing::SystemColors::Window;
			this->treeView->ContextMenuStrip = this->global_view_menu;
			this->treeView->Dock = System::Windows::Forms::DockStyle::Fill;
			this->treeView->filter_visible = false;
			this->treeView->FullRowSelect = true;
			this->treeView->Indent = 27;
			this->treeView->is_multiselect = true;
			this->treeView->is_selectable_groups = true;
			this->treeView->is_selection_or_groups_or_items = false;
			this->treeView->ItemHeight = 20;
			this->treeView->keyboard_search_enabled = false;
			this->treeView->LabelEdit = true;
			this->treeView->Location = System::Drawing::Point(0, 0);
			this->treeView->Name = L"treeView";
			this->treeView->PathSeparator = L"/";
			this->treeView->Size = System::Drawing::Size(295, 380);
			this->treeView->Sorted = true;
			this->treeView->source = nullptr;
			this->treeView->TabIndex = 3;
			this->treeView->NodeMouseDoubleClick += gcnew System::Windows::Forms::TreeNodeMouseClickEventHandler(this, &project_tab::treeView_node_double_click);
			this->treeView->DragLeave += gcnew System::EventHandler(this, &project_tab::treeView_DragLeave);
			this->treeView->AfterLabelEdit += gcnew System::Windows::Forms::NodeLabelEditEventHandler(this, &project_tab::treeView_AfterLabelEdit);
			this->treeView->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &project_tab::treeView_DragDrop);
			this->treeView->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &project_tab::treeView_DragEnter);
			this->treeView->BeforeLabelEdit += gcnew System::Windows::Forms::NodeLabelEditEventHandler(this, &project_tab::treeView_BeforeLabelEdit);
			this->treeView->selected_items_changed += gcnew System::EventHandler<xray::editor::controls::tree_view_selection_event_args^ >(this, &project_tab::treeView_selected_items_changed);
			this->treeView->ItemDrag += gcnew System::Windows::Forms::ItemDragEventHandler(this, &project_tab::treeView_ItemDrag);
			this->treeView->DragOver += gcnew System::Windows::Forms::DragEventHandler(this, &project_tab::treeView_DragOver);
			// 
			// global_view_menu
			// 
			this->global_view_menu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->addToolStripMenuItem, 
				this->removeToolStripMenuItem, this->addToIgnoreListToolStripMenuItem, this->removeFromIgnoreListToolStripMenuItem});
			this->global_view_menu->Name = L"treeViewContextMenu";
			this->global_view_menu->Size = System::Drawing::Size(202, 92);
			this->global_view_menu->Opening += gcnew System::ComponentModel::CancelEventHandler(this, &project_tab::global_view_menu_Opening);
			// 
			// addToolStripMenuItem
			// 
			this->addToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->newFilterToolStripMenuItem});
			this->addToolStripMenuItem->Name = L"addToolStripMenuItem";
			this->addToolStripMenuItem->Size = System::Drawing::Size(201, 22);
			this->addToolStripMenuItem->Text = L"Add";
			// 
			// newFilterToolStripMenuItem
			// 
			this->newFilterToolStripMenuItem->Name = L"newFilterToolStripMenuItem";
			this->newFilterToolStripMenuItem->Size = System::Drawing::Size(127, 22);
			this->newFilterToolStripMenuItem->Text = L"New Filter";
			this->newFilterToolStripMenuItem->Click += gcnew System::EventHandler(this, &project_tab::newFilterToolStripMenuItem_Click);
			// 
			// removeToolStripMenuItem
			// 
			this->removeToolStripMenuItem->Name = L"removeToolStripMenuItem";
			this->removeToolStripMenuItem->Size = System::Drawing::Size(201, 22);
			this->removeToolStripMenuItem->Text = L"Remove";
			this->removeToolStripMenuItem->Click += gcnew System::EventHandler(this, &project_tab::removeToolStripMenuItem_Click);
			// 
			// addToIgnoreListToolStripMenuItem
			// 
			this->addToIgnoreListToolStripMenuItem->Name = L"addToIgnoreListToolStripMenuItem";
			this->addToIgnoreListToolStripMenuItem->Size = System::Drawing::Size(201, 22);
			this->addToIgnoreListToolStripMenuItem->Text = L"Add to ignore list";
			this->addToIgnoreListToolStripMenuItem->Click += gcnew System::EventHandler(this, &project_tab::add_remove_IgnoreListToolStripMenuItem_Click);
			// 
			// removeFromIgnoreListToolStripMenuItem
			// 
			this->removeFromIgnoreListToolStripMenuItem->Name = L"removeFromIgnoreListToolStripMenuItem";
			this->removeFromIgnoreListToolStripMenuItem->Size = System::Drawing::Size(201, 22);
			this->removeFromIgnoreListToolStripMenuItem->Text = L"Remove from ignore list";
			this->removeFromIgnoreListToolStripMenuItem->Click += gcnew System::EventHandler(this, &project_tab::add_remove_IgnoreListToolStripMenuItem_Click);
			// 
			// scroll_timer
			// 
			this->scroll_timer->Tick += gcnew System::EventHandler(this, &project_tab::scroll_timer_Tick);
			// 
			// statusStrip1
			// 
			this->statusStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->totalStatusLabel1, 
				this->selectedStatusLabel1});
			this->statusStrip1->Location = System::Drawing::Point(0, 358);
			this->statusStrip1->Name = L"statusStrip1";
			this->statusStrip1->Size = System::Drawing::Size(295, 22);
			this->statusStrip1->TabIndex = 4;
			this->statusStrip1->Text = L"statusStrip1";
			// 
			// totalStatusLabel1
			// 
			this->totalStatusLabel1->Name = L"totalStatusLabel1";
			this->totalStatusLabel1->Size = System::Drawing::Size(37, 17);
			this->totalStatusLabel1->Text = L"Total:";
			// 
			// selectedStatusLabel1
			// 
			this->selectedStatusLabel1->Name = L"selectedStatusLabel1";
			this->selectedStatusLabel1->Size = System::Drawing::Size(54, 17);
			this->selectedStatusLabel1->Text = L"Selected:";
			// 
			// project_tab
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(295, 380);
			this->Controls->Add(this->statusStrip1);
			this->Controls->Add(this->treeView);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(204)));
			this->HideOnClose = true;
			this->Name = L"project_tab";
			this->TabText = L"project";
			this->Text = L"project";
			this->global_view_menu->ResumeLayout(false);
			this->statusStrip1->ResumeLayout(false);
			this->statusStrip1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	System::Drawing::Font^ font_normal;
	System::Drawing::Font^ font_ignored;	
	int scroll_direction;
	private: System::Void newFilterToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void removeToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void global_view_menu_Opening(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e);
	private: System::Void treeView_BeforeLabelEdit(System::Object^  sender, System::Windows::Forms::NodeLabelEditEventArgs^  e);
	private: System::Void treeView_AfterLabelEdit(System::Object^  sender, System::Windows::Forms::NodeLabelEditEventArgs^  e);
	private: System::Void treeView_selected_items_changed(System::Object^  sender, controls::tree_view_selection_event_args^ e);
	private: System::Void treeView_ItemDrag(System::Object^  sender, System::Windows::Forms::ItemDragEventArgs^  e);
	private: System::Void treeView_DragDrop(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e);
	private: System::Void treeView_DragEnter(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e);
	private: System::Void treeView_DragLeave(System::Object^  sender, System::EventArgs^  e);
	private: System::Void treeView_DragOver(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e);
	private: System::Void add_remove_IgnoreListToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void scroll_timer_Tick(System::Object^  sender, System::EventArgs^  e);			 
	private: System::Void treeView_node_double_click(System::Object^  sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^  e);
	public: void set_total_objects_count( u32 value );
	public: void set_selected_objects_count( u32 value );

};

} // namespace editor
} // namespace xray

#endif // #ifndef PROJECT_TAB_H_INCLUDED