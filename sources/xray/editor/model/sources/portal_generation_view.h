#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray{
namespace model_editor {
class bsp_tree;
ref class bsp_tree_control;
	/// <summary>
	/// Summary for portal_generation_view
	/// </summary>
	public ref class portal_generation_view : public WeifenLuo::WinFormsUI::Docking::DockContent
	{
	public:
		explicit portal_generation_view( bsp_tree* tree ):
		m_bsp_tree( tree )
		{
			InitializeComponent();
			in_constructor();
			//
			//TODO: Add the constructor code here
			//
		}
		property System::Windows::Forms::TreeView^ tree_view
		{
			System::Windows::Forms::TreeView^ get()
			{
				return bsp_tree_view;
			}
		}
	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~portal_generation_view()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Panel^  top_panel;
	private: System::Windows::Forms::Panel^  bottom_panel;
	private: System::Windows::Forms::TreeView^  bsp_tree_view;
	protected: 





















	private: System::Windows::Forms::ContextMenuStrip^  bsp_tree_context_menu;
	private: System::Windows::Forms::ToolStripMenuItem^  serializeNodeToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  serialiseWholeTreeToolStripMenuItem;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripMenuItem1;
	private: System::Windows::Forms::ToolStripMenuItem^  loadWholeTreeToolStripMenuItem;
	private: System::Windows::Forms::SaveFileDialog^  bsp_node_save_dialog;
	private: System::Windows::Forms::OpenFileDialog^  bsp_node_open_dialog;
	private: System::ComponentModel::IContainer^  components;
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
			this->top_panel = (gcnew System::Windows::Forms::Panel());
			this->bottom_panel = (gcnew System::Windows::Forms::Panel());
			this->bsp_tree_view = (gcnew System::Windows::Forms::TreeView());
			this->bsp_tree_context_menu = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->serializeNodeToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->serialiseWholeTreeToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->loadWholeTreeToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->bsp_node_save_dialog = (gcnew System::Windows::Forms::SaveFileDialog());
			this->bsp_node_open_dialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->bsp_tree_context_menu->SuspendLayout();
			this->SuspendLayout();
			// 
			// top_panel
			// 
			this->top_panel->Dock = System::Windows::Forms::DockStyle::Top;
			this->top_panel->Location = System::Drawing::Point(0, 0);
			this->top_panel->Name = L"top_panel";
			this->top_panel->Size = System::Drawing::Size(292, 39);
			this->top_panel->TabIndex = 0;
			// 
			// bottom_panel
			// 
			this->bottom_panel->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->bottom_panel->Location = System::Drawing::Point(0, 365);
			this->bottom_panel->Name = L"bottom_panel";
			this->bottom_panel->Size = System::Drawing::Size(292, 273);
			this->bottom_panel->TabIndex = 1;
			// 
			// bsp_tree_view
			// 
			this->bsp_tree_view->ContextMenuStrip = this->bsp_tree_context_menu;
			this->bsp_tree_view->Dock = System::Windows::Forms::DockStyle::Fill;
			this->bsp_tree_view->Location = System::Drawing::Point(0, 39);
			this->bsp_tree_view->Name = L"bsp_tree_view";
			this->bsp_tree_view->Size = System::Drawing::Size(292, 326);
			this->bsp_tree_view->TabIndex = 2;
			this->bsp_tree_view->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &portal_generation_view::bsp_tree_view_AfterSelect);
			// 
			// bsp_tree_context_menu
			// 
			this->bsp_tree_context_menu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->serializeNodeToolStripMenuItem, 
				this->serialiseWholeTreeToolStripMenuItem, this->toolStripMenuItem1, this->loadWholeTreeToolStripMenuItem});
			this->bsp_tree_context_menu->Name = L"bsp_tree_context_menu";
			this->bsp_tree_context_menu->Size = System::Drawing::Size(168, 76);
			// 
			// serializeNodeToolStripMenuItem
			// 
			this->serializeNodeToolStripMenuItem->Name = L"serializeNodeToolStripMenuItem";
			this->serializeNodeToolStripMenuItem->Size = System::Drawing::Size(167, 22);
			this->serializeNodeToolStripMenuItem->Text = L"Serialise node";
			this->serializeNodeToolStripMenuItem->Click += gcnew System::EventHandler(this, &portal_generation_view::serializeNodeToolStripMenuItem_Click);
			// 
			// serialiseWholeTreeToolStripMenuItem
			// 
			this->serialiseWholeTreeToolStripMenuItem->Name = L"serialiseWholeTreeToolStripMenuItem";
			this->serialiseWholeTreeToolStripMenuItem->Size = System::Drawing::Size(167, 22);
			this->serialiseWholeTreeToolStripMenuItem->Text = L"Serialise whole tree";
			this->serialiseWholeTreeToolStripMenuItem->Click += gcnew System::EventHandler(this, &portal_generation_view::serialiseWholeTreeToolStripMenuItem_Click);
			// 
			// toolStripMenuItem1
			// 
			this->toolStripMenuItem1->Name = L"toolStripMenuItem1";
			this->toolStripMenuItem1->Size = System::Drawing::Size(164, 6);
			// 
			// loadWholeTreeToolStripMenuItem
			// 
			this->loadWholeTreeToolStripMenuItem->Name = L"loadWholeTreeToolStripMenuItem";
			this->loadWholeTreeToolStripMenuItem->Size = System::Drawing::Size(167, 22);
			this->loadWholeTreeToolStripMenuItem->Text = L"Load whole tree";
			this->loadWholeTreeToolStripMenuItem->Click += gcnew System::EventHandler(this, &portal_generation_view::loadWholeTreeToolStripMenuItem_Click);
			// 
			// bsp_node_save_dialog
			// 
			this->bsp_node_save_dialog->DefaultExt = L"lua";
			this->bsp_node_save_dialog->Filter = L"lua | *.lua";
			this->bsp_node_save_dialog->RestoreDirectory = true;
			// 
			// bsp_node_open_dialog
			// 
			this->bsp_node_open_dialog->Filter = L"lua | *.lua";
			this->bsp_node_open_dialog->RestoreDirectory = true;
			// 
			// portal_generation_view
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(292, 638);
			this->Controls->Add(this->bsp_tree_view);
			this->Controls->Add(this->bottom_panel);
			this->Controls->Add(this->top_panel);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(204)));
			this->HideOnClose = true;
			this->Name = L"portal_generation_view";
			this->Text = L"Portal generation";
			this->bsp_tree_context_menu->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
private: 
		System::Void bsp_tree_view_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e);
		System::Void serializeNodeToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
		System::Void serialiseWholeTreeToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
		System::Void loadWholeTreeToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
public:
	void set_model_name( System::String^ model_name );
	void set_property_container			( xray::editor::wpf_controls::property_container^ cont );
private:
	void on_tree_restore_file_ready	( resources::queries_result& data );
	void on_mesh_restore_file_ready	( resources::queries_result& data );
	void in_constructor				();
	bsp_tree* m_bsp_tree;
	System::String^ m_model_name;
	xray::editor::wpf_controls::property_grid::host^	m_property_grid_host;
	bsp_tree_control^	m_bsp_tree_control;
};
}
}
