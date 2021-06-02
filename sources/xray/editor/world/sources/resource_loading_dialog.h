////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCE_LOADING_DIALOG_H_INCLUDED
#define RESOURCE_LOADING_DIALOG_H_INCLUDED

using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "project_defines.h"
namespace xray{
namespace editor {
	/// <summary>
	/// Summary for resource_loading_dialog
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>

	public ref class resource_loading_dialog : public System::Windows::Forms::Form
	{
	public:
		resource_loading_dialog():m_tool(nullptr)
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
		~resource_loading_dialog()
		{
			if (components)
			{
				delete components;
			}
		}

	protected: 

	private: System::Windows::Forms::Label^  message1_label;
	private: System::Windows::Forms::Button^  select_button;
	protected: 


	private: System::Windows::Forms::Button^  ignore_button;
	private: System::Windows::Forms::Label^  message2_label;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::RadioButton^  btn_all;

	private: System::Windows::Forms::RadioButton^  btn_this_name;

	private: System::Windows::Forms::RadioButton^  btn_this_only;




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
			this->message1_label = (gcnew System::Windows::Forms::Label());
			this->select_button = (gcnew System::Windows::Forms::Button());
			this->ignore_button = (gcnew System::Windows::Forms::Button());
			this->message2_label = (gcnew System::Windows::Forms::Label());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->btn_this_only = (gcnew System::Windows::Forms::RadioButton());
			this->btn_this_name = (gcnew System::Windows::Forms::RadioButton());
			this->btn_all = (gcnew System::Windows::Forms::RadioButton());
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// message1_label
			// 
			this->message1_label->AutoSize = true;
			this->message1_label->Location = System::Drawing::Point(1, 5);
			this->message1_label->Name = L"message1_label";
			this->message1_label->Size = System::Drawing::Size(12, 13);
			this->message1_label->TabIndex = 1;
			this->message1_label->Text = L"s";
			// 
			// select_button
			// 
			this->select_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->select_button->Location = System::Drawing::Point(146, 124);
			this->select_button->Name = L"select_button";
			this->select_button->Size = System::Drawing::Size(99, 23);
			this->select_button->TabIndex = 2;
			this->select_button->Text = L"Select from library";
			this->select_button->UseVisualStyleBackColor = true;
			this->select_button->Click += gcnew System::EventHandler(this, &resource_loading_dialog::select_button_Click);
			// 
			// ignore_button
			// 
			this->ignore_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->ignore_button->Location = System::Drawing::Point(251, 124);
			this->ignore_button->Name = L"ignore_button";
			this->ignore_button->Size = System::Drawing::Size(56, 23);
			this->ignore_button->TabIndex = 3;
			this->ignore_button->Text = L"Ignore";
			this->ignore_button->UseVisualStyleBackColor = true;
			this->ignore_button->Click += gcnew System::EventHandler(this, &resource_loading_dialog::ignore_button_Click);
			// 
			// message2_label
			// 
			this->message2_label->AutoSize = true;
			this->message2_label->Location = System::Drawing::Point(1, 29);
			this->message2_label->Name = L"message2_label";
			this->message2_label->Size = System::Drawing::Size(12, 13);
			this->message2_label->TabIndex = 4;
			this->message2_label->Text = L"s";
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->btn_all);
			this->groupBox1->Controls->Add(this->btn_this_name);
			this->groupBox1->Controls->Add(this->btn_this_only);
			this->groupBox1->Location = System::Drawing::Point(4, 52);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(137, 96);
			this->groupBox1->TabIndex = 6;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Apply to:";
			// 
			// btn_this_only
			// 
			this->btn_this_only->AutoSize = true;
			this->btn_this_only->Location = System::Drawing::Point(7, 20);
			this->btn_this_only->Name = L"btn_this_only";
			this->btn_this_only->Size = System::Drawing::Size(95, 17);
			this->btn_this_only->TabIndex = 0;
			this->btn_this_only->Text = L"this object only";
			this->btn_this_only->UseVisualStyleBackColor = true;
			// 
			// btn_this_name
			// 
			this->btn_this_name->AutoSize = true;
			this->btn_this_name->Checked = true;
			this->btn_this_name->Location = System::Drawing::Point(7, 42);
			this->btn_this_name->Name = L"btn_this_name";
			this->btn_this_name->Size = System::Drawing::Size(120, 17);
			this->btn_this_name->TabIndex = 1;
			this->btn_this_name->TabStop = true;
			this->btn_this_name->Text = L"all objects this name";
			this->btn_this_name->UseVisualStyleBackColor = true;
			// 
			// btn_all
			// 
			this->btn_all->AutoSize = true;
			this->btn_all->Location = System::Drawing::Point(7, 66);
			this->btn_all->Name = L"btn_all";
			this->btn_all->Size = System::Drawing::Size(109, 17);
			this->btn_all->TabIndex = 2;
			this->btn_all->Text = L"all missing objects";
			this->btn_all->UseVisualStyleBackColor = true;
			// 
			// resource_loading_dialog
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(312, 150);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->message2_label);
			this->Controls->Add(this->ignore_button);
			this->Controls->Add(this->select_button);
			this->Controls->Add(this->message1_label);
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"resource_loading_dialog";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->TopMost = true;
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void select_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void ignore_button_Click(System::Object^  sender, System::EventArgs^  e);
			tool_base^			m_tool;
			System::String^		m_missing_resource_name;
	public:
		System::Windows::Forms::DialogResult		m_dialog_result;

		void						init_labels		( System::String^ label1, System::String^ label2 );
		subst_resource_result		do_dialog		( tool_base^ tool, System::String^ missing_resource_name );
		property System::String^	selected_resource_name;

};

} // namespace editor
} // namespace xray

#endif // #ifndef RESOURCE_LOADING_DIALOG_H_INCLUDED