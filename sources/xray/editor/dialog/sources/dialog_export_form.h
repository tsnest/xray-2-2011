////////////////////////////////////////////////////////////////////////////
//	Created		: 17.05.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef DIALOG_EXPORT_FORM_H_INCLUDED
#define DIALOG_EXPORT_FORM_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray {
namespace dialog_editor {
	ref class excel_file;
	ref class dialog_editor_impl;

	public ref class dialog_export_form : public System::Windows::Forms::Form
	{
		typedef System::Collections::Generic::Dictionary<System::String^, System::String^> columns_type;
		typedef System::Collections::Generic::KeyValuePair<System::String^, System::String^> columns_iter_type;
	public:
		dialog_export_form(dialog_editor_impl^ ed):m_editor(ed)
		{
			InitializeComponent();
			in_constructor();
		}

		void	show		(System::String^ init_directory);

	protected:
		~dialog_export_form()
		{
			if (components)
				delete components;
		}

	private: 
		void	in_constructor			();
		void	on_export_clicked		(System::Object^, System::EventArgs^ );
		void	on_cancel_clicked		(System::Object^, System::EventArgs^ );

		void InitializeComponent(void)
		{
			this->checkedListBox1 = (gcnew System::Windows::Forms::CheckedListBox());
			this->checkedListBox2 = (gcnew System::Windows::Forms::CheckedListBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
			this->saveFileDialog1 = (gcnew System::Windows::Forms::SaveFileDialog());
			this->SuspendLayout();

			// checkedListBox1
			this->checkedListBox1->CheckOnClick = true;
			this->checkedListBox1->FormattingEnabled = true;
			this->checkedListBox1->Location = System::Drawing::Point(12, 25);
			this->checkedListBox1->Name = L"checkedListBox1";
			this->checkedListBox1->Size = System::Drawing::Size(112, 109);
			this->checkedListBox1->TabIndex = 0;

			// checkedListBox2
			this->checkedListBox2->CheckOnClick = true;
			this->checkedListBox2->FormattingEnabled = true;
			this->checkedListBox2->Location = System::Drawing::Point(143, 25);
			this->checkedListBox2->Name = L"checkedListBox2";
			this->checkedListBox2->Size = System::Drawing::Size(112, 109);
			this->checkedListBox2->TabIndex = 1;

			// label1
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(92, 13);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Select languages:";

			// label2
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(140, 9);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(92, 13);
			this->label2->TabIndex = 3;
			this->label2->Text = L"Select catogories:";

			// button1
			this->button1->Location = System::Drawing::Point(12, 163);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(112, 23);
			this->button1->TabIndex = 4;
			this->button1->Text = L"Export";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &dialog_export_form::on_export_clicked);

			// button2
			this->button2->Location = System::Drawing::Point(143, 163);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(112, 23);
			this->button2->TabIndex = 5;
			this->button2->Text = L"Cancel";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &dialog_export_form::on_cancel_clicked);

			// checkBox1
			this->checkBox1->AutoSize = true;
			this->checkBox1->Checked = true;
			this->checkBox1->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox1->Location = System::Drawing::Point(12, 140);
			this->checkBox1->Name = L"checkBox1";
			this->checkBox1->Size = System::Drawing::Size(111, 17);
			this->checkBox1->TabIndex = 6;
			this->checkBox1->Text = L"Only new story ids";
			this->checkBox1->UseVisualStyleBackColor = true;

			// saveFileDialog1
			this->saveFileDialog1->DefaultExt = L"xlsx";
			this->saveFileDialog1->Filter = L"Excel files|*.xlsx";
			this->saveFileDialog1->OverwritePrompt = false;
			this->saveFileDialog1->RestoreDirectory = true;
			this->saveFileDialog1->Title = L"Export file";

			// dialog_export_form
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(269, 197);
			this->Controls->Add(this->checkBox1);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->checkedListBox2);
			this->Controls->Add(this->checkedListBox1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"dialog_export_form";
			this->ShowInTaskbar = false;
			this->Text = L"Export file";
			this->AcceptButton = this->button1;
			this->CancelButton = this->button2;
			this->ResumeLayout(false);
			this->PerformLayout();
		}

	private: 
		dialog_editor_impl^									m_editor;
		columns_type^									m_columns;
		System::Windows::Forms::CheckedListBox^			checkedListBox1;
		System::Windows::Forms::CheckedListBox^			checkedListBox2;
		System::Windows::Forms::Label^					label1;
		System::Windows::Forms::Label^					label2;
		System::Windows::Forms::Button^					button1;
		System::Windows::Forms::Button^					button2;
		System::Windows::Forms::CheckBox^				checkBox1;
		System::ComponentModel::Container^				components;
		System::Windows::Forms::SaveFileDialog^			saveFileDialog1;
	}; // ref class dialog_export_form
} // namespace dialog_editor
} // namespace xray

#endif // #ifndef DIALOG_EXPORT_FORM_H_INCLUDED