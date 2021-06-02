////////////////////////////////////////////////////////////////////////////
//	Created		: 20.04.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef LEARNING_PROPERTIES_FORM_H_INCLUDED
#define LEARNING_PROPERTIES_FORM_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray {
namespace animation_editor {

	ref class animation_editor;

	public ref class learning_properties_form : public System::Windows::Forms::Form
	{
	public:
		learning_properties_form(animation_editor^ ed, String^ controller_name);
		~learning_properties_form();

	private:
		void ok_clicked(System::Object^, System::EventArgs^);
		void cancel_clicked(System::Object^, System::EventArgs^);
		void InitializeComponent(void)
		{
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->numericUpDown1 = (gcnew System::Windows::Forms::NumericUpDown());
			this->numericUpDown2 = (gcnew System::Windows::Forms::NumericUpDown());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown2))->BeginInit();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(34, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(97, 13);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Residual maximum:";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(32, 48);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(99, 13);
			this->label2->TabIndex = 1;
			this->label2->Text = L"Iterations maximum:";
			// 
			// numericUpDown1
			// 
			this->numericUpDown1->DecimalPlaces = 2;
			this->numericUpDown1->Location = System::Drawing::Point(24, 25);
			this->numericUpDown1->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, 0});
			this->numericUpDown1->Name = L"numericUpDown1";
			this->numericUpDown1->Size = System::Drawing::Size(120, 20);
			this->numericUpDown1->TabIndex = 2;
			// 
			// numericUpDown2
			// 
			this->numericUpDown2->Location = System::Drawing::Point(24, 64);
			this->numericUpDown2->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, 0});
			this->numericUpDown2->Name = L"numericUpDown2";
			this->numericUpDown2->Size = System::Drawing::Size(120, 20);
			this->numericUpDown2->TabIndex = 3;
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(3, 90);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 4;
			this->button1->Text = L"OK";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &learning_properties_form::ok_clicked);
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(93, 90);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(75, 23);
			this->button2->TabIndex = 5;
			this->button2->Text = L"Cancel";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &learning_properties_form::cancel_clicked);
			// 
			// learning_properties_form
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(171, 118);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->numericUpDown2);
			this->Controls->Add(this->numericUpDown1);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"learning_properties_form";
			this->ShowInTaskbar = false;
			this->Text = L"Learning properties";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown2))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();
		}

	private: 
		System::ComponentModel::Container^	components;
		animation_editor^					m_editor;
		String^								m_controller_name;
		Label^								label1;
		Label^								label2;
		NumericUpDown^						numericUpDown1;
		NumericUpDown^						numericUpDown2;
		Button^								button1;
		Button^								button2;
	};
} // namespace animation_editor
} // namespace xray
#endif // #ifndef LEARNING_PROPERTIES_FORM_H_INCLUDED