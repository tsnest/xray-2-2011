#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray{
namespace editor {

	/// <summary>
	/// Summary for behaviour_editor_form
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class behaviour_editor_form : public System::Windows::Forms::Form
	{
	public:
		behaviour_editor_form(void)
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
		~behaviour_editor_form()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TreeView^  behaviours_tree_view;
	protected: 

	private: System::Windows::Forms::Panel^  behaviour_edit_panel;

	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->behaviours_tree_view = (gcnew System::Windows::Forms::TreeView());
			this->behaviour_edit_panel = (gcnew System::Windows::Forms::Panel());
			this->SuspendLayout();
			// 
			// behaviours_tree_view
			// 
			this->behaviours_tree_view->Dock = System::Windows::Forms::DockStyle::Left;
			this->behaviours_tree_view->Location = System::Drawing::Point(0, 0);
			this->behaviours_tree_view->Name = L"behaviours_tree_view";
			this->behaviours_tree_view->Size = System::Drawing::Size(187, 555);
			this->behaviours_tree_view->TabIndex = 0;
			// 
			// behaviour_edit_panel
			// 
			this->behaviour_edit_panel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->behaviour_edit_panel->Location = System::Drawing::Point(187, 0);
			this->behaviour_edit_panel->Name = L"behaviour_edit_panel";
			this->behaviour_edit_panel->Size = System::Drawing::Size(756, 555);
			this->behaviour_edit_panel->TabIndex = 1;
			// 
			// behaviour_editor_form
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(943, 555);
			this->Controls->Add(this->behaviour_edit_panel);
			this->Controls->Add(this->behaviours_tree_view);
			this->Name = L"behaviour_editor_form";
			this->Text = L"behaviour_editor_form";
			this->ResumeLayout(false);

		}
#pragma endregion

	};

}
}
