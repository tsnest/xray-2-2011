////////////////////////////////////////////////////////////////////////////
//	Created		: 14.05.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef REMOVE_LANGUAGE_FORM_H_INCLUDED
#define REMOVE_LANGUAGE_FORM_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray {
namespace dialog_editor {
	ref class dialog_editor_impl;

	public ref class remove_language_form : public System::Windows::Forms::Form
	{
	public:
		remove_language_form(dialog_editor_impl^ ed):m_editor(ed)
		{
			InitializeComponent();
		}

		void		show						();

	protected:
		~remove_language_form()
		{
			if (components)
				delete components;
		}

	private: 
		dialog_editor_impl^						m_editor;
		System::Windows::Forms::CheckedListBox^ checkedListBox1;
		System::Windows::Forms::Button^			button1;
		System::Windows::Forms::Button^			button2;
		System::Windows::Forms::Label^			label1;
		System::ComponentModel::Container^		components;

		void		on_delete_clicked			(System::Object^ sender, System::EventArgs^ e);
		void		on_cancel_clicked			(System::Object^ sender, System::EventArgs^ e);

		void InitializeComponent(void)
		{
			this->checkedListBox1 = (gcnew System::Windows::Forms::CheckedListBox());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();

			// checkedListBox1
			this->checkedListBox1->FormattingEnabled = true;
			this->checkedListBox1->Location = System::Drawing::Point(3, 25);
			this->checkedListBox1->Name = L"checkedListBox1";
			this->checkedListBox1->Size = System::Drawing::Size(179, 124);
			this->checkedListBox1->TabIndex = 0;
			this->checkedListBox1->CheckOnClick = true;
			this->checkedListBox1->Sorted = false;

			// button1
			this->button1->Location = System::Drawing::Point(3, 155);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(85, 23);
			this->button1->TabIndex = 1;
			this->button1->Text = L"Delete";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &remove_language_form::on_delete_clicked);

			// button2
			this->button2->Location = System::Drawing::Point(97, 155);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(85, 23);
			this->button2->TabIndex = 2;
			this->button2->Text = L"Cancel";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &remove_language_form::on_cancel_clicked);

			// label1
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(0, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(182, 13);
			this->label1->TabIndex = 3;
			this->label1->Text = L"Select languages you want to delete:";

			// remove_language_form
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(184, 184);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->checkedListBox1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->Name = L"remove_language_form";
			this->Text = L"Remove language";
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->AcceptButton = this->button1;
			this->CancelButton = this->button2;
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->ResumeLayout(false);
			this->PerformLayout();
		}
	}; // ref class remove_language_form
} // namespace dialog_editor
} // namespace xray

#endif // #ifndef REMOVE_LANGUAGE_FORM_H_INCLUDED