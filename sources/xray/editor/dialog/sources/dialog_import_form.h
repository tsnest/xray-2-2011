////////////////////////////////////////////////////////////////////////////
//	Created		: 17.05.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef DIALOG_IMPORT_FORM_H_INCLUDED
#define DIALOG_IMPORT_FORM_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray {
namespace dialog_editor {

	ref class dialog_editor_impl;

	public ref class dialog_import_form : public System::Windows::Forms::Form
	{
	public:
		dialog_import_form(dialog_editor_impl^ ed):m_editor(ed)
		{
			InitializeComponent();
		}

		void	show		(System::String^ init_directory);

	protected:
		~dialog_import_form()
		{
			if (components)
				delete components;
		}

	private:
		void	on_import_clicked		(System::Object^, System::EventArgs^ );
		void	on_cancel_clicked		(System::Object^, System::EventArgs^ );

		void InitializeComponent(void)
		{
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();

			// button1
			this->button1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button1->Location = System::Drawing::Point(15, 27);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(107, 23);
			this->button1->TabIndex = 0;
			this->button1->Text = L"Import";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &dialog_import_form::on_import_clicked);

			// button2
			this->button2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->button2->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->button2->Location = System::Drawing::Point(131, 27);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(107, 23);
			this->button2->TabIndex = 1;
			this->button2->Text = L"Cancel";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &dialog_import_form::on_cancel_clicked);

			// openFileDialog1
			this->openFileDialog1->DefaultExt = L"xls";
			this->openFileDialog1->Filter = L"Excel files|*.xls;*.xlsx";
			this->openFileDialog1->RestoreDirectory = true;
			this->openFileDialog1->Title = L"Import file";

			// label1
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(110, 13);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Import localization file:";

			// dialog_import_form
			this->AcceptButton = this->button1;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoSize = true;
			this->CancelButton = this->button2;
			this->ClientSize = System::Drawing::Size(250, 62);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->button1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"dialog_import_form";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->Text = L"Import file";
			this->ResumeLayout(false);
			this->PerformLayout();
		}

	private: 
		dialog_editor_impl^						m_editor;
		System::Windows::Forms::Button^			button1;
		System::Windows::Forms::Button^			button2;
		System::Windows::Forms::OpenFileDialog^ openFileDialog1;
		System::Windows::Forms::Label^			label1;
		System::ComponentModel::Container^		components;
	}; // ref class dialog_import_form
} // namespace dialog_editor
} // namespace xray

#endif // #ifndef DIALOG_IMPORT_FORM_H_INCLUDED