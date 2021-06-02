////////////////////////////////////////////////////////////////////////////
//	Created		: 14.05.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ADD_LANGUAGE_FORM_H_INCLUDED
#define ADD_LANGUAGE_FORM_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray {
namespace dialog_editor {
	ref class dialog_editor_impl;

	public ref class add_language_form : public System::Windows::Forms::Form
	{
	public:
		add_language_form(dialog_editor_impl^ ed):m_editor(ed)
		{
			InitializeComponent();
		}

	protected:
		~add_language_form()
		{
			if (components)
				delete components;
		}

	private: 
		dialog_editor_impl^					m_editor;
		System::ComponentModel::Container^	components;
		System::Windows::Forms::Label^		label1;
		System::Windows::Forms::TextBox^	textBox1;
		System::Windows::Forms::Button^		button1;
		System::Windows::Forms::Button^		button2;

		void		on_ok_clicked			(System::Object^ sender, System::EventArgs^ e);
		void		on_cancel_clicked		(System::Object^ sender, System::EventArgs^ e);

		void InitializeComponent(void)
		{
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();

			// label1
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(23, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(134, 13);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Enter new language name:";

			// textBox1
			this->textBox1->Location = System::Drawing::Point(12, 25);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(160, 20);
			this->textBox1->TabIndex = 1;

			// button1
			this->button1->Location = System::Drawing::Point(12, 50);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 2;
			this->button1->Text = L"OK";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &add_language_form::on_ok_clicked);

			// button2
			this->button2->Location = System::Drawing::Point(97, 50);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(75, 23);
			this->button2->TabIndex = 3;
			this->button2->Text = L"Cancel";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &add_language_form::on_cancel_clicked);

			// add_language_form
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(184, 85);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->textBox1);
			this->Controls->Add(this->label1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->AcceptButton = this->button1;
			this->CancelButton = this->button2;
			this->Name = L"add_language_form";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->Text = L"Add new language";
			this->ResumeLayout(false);
			this->PerformLayout();
		}
	}; // ref class add_language_form
} // namespace dialog_editor
} // namespace xray

#endif // #ifndef ADD_LANGUAGE_FORM_H_INCLUDED