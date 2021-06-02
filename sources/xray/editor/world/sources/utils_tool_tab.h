#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray{
namespace editor {
class tool_utils;

	/// <summary>
	/// Summary for utils_tool_tab
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class utils_tool_tab : public System::Windows::Forms::UserControl
	{
	public:
		utils_tool_tab(tool_utils& tool):m_tool(tool)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	public: 
	private: System::Windows::Forms::StatusStrip^  statusStrip1;
	public: System::Windows::Forms::ToolStripStatusLabel^  toolStripStatusLabel1;
	private: 

	private: System::Windows::Forms::Panel^  preview_panel;

	public: System::Windows::Forms::TreeView^  treeView;
	private: 




	public: System::Windows::Forms::Label^  selected_label;
	private: System::Windows::Forms::ImageList^  imageList1;
	public: 

	public: 
	private: 

	public: 
		tool_utils&		m_tool;
	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~utils_tool_tab()
		{
			if (components)
			{
				delete components;
			}
		}
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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(utils_tool_tab::typeid));
			this->statusStrip1 = (gcnew System::Windows::Forms::StatusStrip());
			this->toolStripStatusLabel1 = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->preview_panel = (gcnew System::Windows::Forms::Panel());
			this->selected_label = (gcnew System::Windows::Forms::Label());
			this->treeView = (gcnew System::Windows::Forms::TreeView());
			this->imageList1 = (gcnew System::Windows::Forms::ImageList(this->components));
			this->statusStrip1->SuspendLayout();
			this->preview_panel->SuspendLayout();
			this->SuspendLayout();
			// 
			// statusStrip1
			// 
			this->statusStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->toolStripStatusLabel1});
			this->statusStrip1->Location = System::Drawing::Point(0, 363);
			this->statusStrip1->Name = L"statusStrip1";
			this->statusStrip1->Size = System::Drawing::Size(242, 22);
			this->statusStrip1->TabIndex = 1;
			this->statusStrip1->Text = L"statusStrip1";
			// 
			// toolStripStatusLabel1
			// 
			this->toolStripStatusLabel1->Name = L"toolStripStatusLabel1";
			this->toolStripStatusLabel1->Size = System::Drawing::Size(34, 17);
			this->toolStripStatusLabel1->Text = L"total:";
			// 
			// preview_panel
			// 
			this->preview_panel->BackColor = System::Drawing::SystemColors::ButtonShadow;
			this->preview_panel->Controls->Add(this->selected_label);
			this->preview_panel->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->preview_panel->Location = System::Drawing::Point(0, 287);
			this->preview_panel->Name = L"preview_panel";
			this->preview_panel->Size = System::Drawing::Size(242, 76);
			this->preview_panel->TabIndex = 2;
			// 
			// selected_label
			// 
			this->selected_label->AutoSize = true;
			this->selected_label->Location = System::Drawing::Point(3, 3);
			this->selected_label->Name = L"selected_label";
			this->selected_label->Size = System::Drawing::Size(50, 13);
			this->selected_label->TabIndex = 1;
			this->selected_label->Text = L"selected:";
			// 
			// treeView
			// 
			this->treeView->Dock = System::Windows::Forms::DockStyle::Fill;
			this->treeView->FullRowSelect = true;
			this->treeView->ImageIndex = 0;
			this->treeView->ImageList = this->imageList1;
			this->treeView->Location = System::Drawing::Point(0, 0);
			this->treeView->Name = L"treeView";
			this->treeView->PathSeparator = L"/";
			this->treeView->SelectedImageIndex = 0;
			this->treeView->Size = System::Drawing::Size(242, 287);
			this->treeView->TabIndex = 4;
			this->treeView->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &utils_tool_tab::on_after_select);
			// 
			// imageList1
			// 
			this->imageList1->ImageStream = (cli::safe_cast<System::Windows::Forms::ImageListStreamer^  >(resources->GetObject(L"imageList1.ImageStream")));
			this->imageList1->TransparentColor = System::Drawing::Color::Transparent;
			this->imageList1->Images->SetKeyName(0, L"group");
			this->imageList1->Images->SetKeyName(1, L"mesh");
			this->imageList1->Images->SetKeyName(2, L"camera");
			this->imageList1->Images->SetKeyName(3, L"curve");
			// 
			// utils_tool_tab
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->treeView);
			this->Controls->Add(this->preview_panel);
			this->Controls->Add(this->statusStrip1);
			this->Name = L"utils_tool_tab";
			this->Size = System::Drawing::Size(242, 385);
			this->statusStrip1->ResumeLayout(false);
			this->statusStrip1->PerformLayout();
			this->preview_panel->ResumeLayout(false);
			this->preview_panel->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void on_after_select(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e);
};

} // namespace editor
} // namespace xray