#pragma once

using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray{
namespace editor {

	ref class level_editor;

	/// <summary>
	/// Summary for logic_objects_explorer
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class logic_objects_explorer : public WeifenLuo::WinFormsUI::Docking::DockContent
	{
	public:
		logic_objects_explorer(level_editor^ editor)
		:m_level_editor(editor)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~logic_objects_explorer()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;
		level_editor^						m_level_editor;
public: xray::editor::controls::tree_view^	treeView;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = gcnew System::ComponentModel::Container();
			this->Size = System::Drawing::Size(300,300);
			this->Text = L"Logic objects explorer";
			this->Padding = System::Windows::Forms::Padding(0);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;

			this->HideOnClose = true;

			System::Windows::Forms::TreeNode^  treeNode1 = (gcnew System::Windows::Forms::TreeNode(L"Logic Objects"));
			this->treeView = (gcnew xray::editor::controls::tree_view());

			this->treeView->auto_expand_on_filter = false;
			this->treeView->Dock = System::Windows::Forms::DockStyle::Fill;
			this->treeView->filter_visible = false;
			this->treeView->HideSelection = false;
			this->treeView->ImageIndex = 0;
			//this->treeView->ImageList = this->imageList1;
			this->treeView->Indent = 27;
			this->treeView->is_multiselect = false;
			this->treeView->ItemHeight = 20;
			this->treeView->Location = System::Drawing::Point(0, 0);
			this->treeView->Name = L"treeView";
			this->treeView->PathSeparator = L"/";
			treeNode1->Name = L"";
			treeNode1->Text = L"";
			this->treeView->root = treeNode1;
			this->treeView->SelectedImageIndex = 0;
			this->treeView->Size = System::Drawing::Size(212, 368);
			this->treeView->source = nullptr;
			this->treeView->TabIndex = 6;
		//	this->treeView->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &scripting_tool_tab::on_after_select);

			this->treeView->selected_items_changed += gcnew System::EventHandler(this, &logic_objects_explorer::treeView_selected_items_changed);

			this->Controls->Add(this->treeView);
        }
#pragma endregion


		private: System::Void treeView_selected_items_changed(System::Object^  sender, System::EventArgs^  e);
	};
}
}
