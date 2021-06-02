////////////////////////////////////////////////////////////////////////////
//	Created		: 03.03.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef MODELS_PANEL_H_INCLUDED
#define MODELS_PANEL_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray {
namespace animation_editor {
	public enum class models_panel_mode
	{
		add = 0,
		remove = 1,
		select = 2,
	};

	public ref class models_panel : public System::Windows::Forms::Form
	{
	public:
		models_panel(String^ name, Generic::List<String^>^ lst, models_panel_mode mode)
		{
			InitializeComponent();
			in_constructor(name, lst, mode);
		}
		~models_panel()
		{
			if (components)
				delete components;
		}

		String^		get_name			();
		void		get_selected_list	(Generic::List<String^>% lst);

	private: 
		System::ComponentModel::Container^	components;
		System::Windows::Forms::Label^		label1;
		System::Windows::Forms::TextBox^	textBox1;
		System::Windows::Forms::ListBox^	listBox1;
		System::Windows::Forms::Button^		button1;
		models_panel_mode					m_mode;

	private:
		void		in_constructor		(String^ name, Generic::List<String^>^ lst, models_panel_mode mode);
		void InitializeComponent(void)
		{
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->listBox1 = (gcnew System::Windows::Forms::ListBox());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// button1
			// 
			this->button1->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->button1->Location = System::Drawing::Point(59, 178);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 0;
			this->button1->Text = L"OK";
			this->button1->UseVisualStyleBackColor = true;
			// 
			// listBox1
			// 
			this->listBox1->FormattingEnabled = true;
			this->listBox1->Location = System::Drawing::Point(12, 25);
			this->listBox1->Name = L"listBox1";
			this->listBox1->SelectionMode = System::Windows::Forms::SelectionMode::MultiExtended;
			this->listBox1->Size = System::Drawing::Size(170, 147);
			this->listBox1->TabIndex = 1;
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(12, 25);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(170, 20);
			this->textBox1->TabIndex = 2;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(95, 13);
			this->label1->TabIndex = 3;
			this->label1->Text = L"Enter model name:";
			// 
			// models_panel
			// 
			this->AcceptButton = this->button1;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(193, 209);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->textBox1);
			this->Controls->Add(this->listBox1);
			this->Controls->Add(this->button1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"models_panel";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->Text = L"Models panel";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
	};
} // namespace animation_editor
} // namespace xray
#endif // #ifndef MODELS_PANEL_H_INCLUDED