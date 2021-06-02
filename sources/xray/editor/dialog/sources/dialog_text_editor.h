////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef DIALOG_TEXT_EDITOR_H_INCLUDED
#define DIALOG_TEXT_EDITOR_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray {
namespace dialog_editor {
	ref class dialog_editor_impl;
	ref class dialog_graph_node_base;
	ref class dialog_graph_node_phrase;

	namespace string_tables {
		class string_table_ids_storage;
	}

	public ref class dialog_text_editor : public System::Windows::Forms::Form/*WeifenLuo::WinFormsUI::Docking::DockContent*/
	{
	public:
		dialog_text_editor(dialog_editor_impl^ ed):m_editor(ed)
		{
			InitializeComponent();
			changed = false;
		}

		void		update_selected_node		(dialog_graph_node_base^ n);
		void		assign_string_table_ids		();

	protected:
		~dialog_text_editor()
		{
			if (components)
				delete components;
		}

	private:
		void		update_editing_node_text	();
		void		on_editor_closing			(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e);
		void		combo_box_selection_changed	(System::Object^ sender, System::EventArgs^ e);
		void		on_key_down					(System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e);
		void		on_key_press				(System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e);
		void		on_ok_clicked				(System::Object^ sender, System::EventArgs^ e);
		void		on_cancel_clicked			(System::Object^ sender, System::EventArgs^ e);
		u32			find_new_str_id_cb_pos		(System::String^ new_str_id);

		property bool changed
		{
			void	set							(bool v)	{m_changed=v; button1->Enabled=m_changed;};
		}

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->text_box = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->combo_box = (gcnew System::Windows::Forms::ComboBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// text_box
			// 
			this->text_box->AcceptsTab = true;
			this->text_box->Location = System::Drawing::Point(1, 62);
			this->text_box->Multiline = true;
			this->text_box->Name = L"text_box";
			this->text_box->Size = System::Drawing::Size(289, 210);
			this->text_box->TabIndex = 0;
			this->text_box->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &dialog_text_editor::on_key_down);
			this->text_box->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &dialog_text_editor::on_key_press);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(-2, 5);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(108, 13);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Phrase string table id:";
			// 
			// combo_box
			// 
			this->combo_box->AutoCompleteMode = System::Windows::Forms::AutoCompleteMode::Suggest;
			this->combo_box->AutoCompleteSource = System::Windows::Forms::AutoCompleteSource::ListItems;
			this->combo_box->FormattingEnabled = true;
			this->combo_box->Location = System::Drawing::Point(1, 21);
			this->combo_box->MaxDropDownItems = 10;
			this->combo_box->Name = L"combo_box";
			this->combo_box->Size = System::Drawing::Size(289, 21);
			this->combo_box->TabIndex = 3;
			this->combo_box->SelectionChangeCommitted += gcnew System::EventHandler(this, &dialog_text_editor::combo_box_selection_changed);
			this->combo_box->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &dialog_text_editor::on_key_down);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(-2, 45);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(63, 13);
			this->label2->TabIndex = 4;
			this->label2->Text = L"Phrase text:";
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(12, 278);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(130, 23);
			this->button1->TabIndex = 5;
			this->button1->Text = L"Apply";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &dialog_text_editor::on_ok_clicked);
			// 
			// button2
			// 
			this->button2->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->button2->Location = System::Drawing::Point(150, 278);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(130, 23);
			this->button2->TabIndex = 6;
			this->button2->Text = L"Exit";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &dialog_text_editor::on_cancel_clicked);
			// 
			// dialog_text_editor
			// 
			//this->AcceptButton = this->button1;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			//this->CancelButton = this->button2;
			this->ClientSize = System::Drawing::Size(292, 305);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->combo_box);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->text_box);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(204)));
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->Name = L"dialog_text_editor";
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->ShowInTaskbar = false;
			this->Text = L"dialog_text_editor";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &dialog_text_editor::on_editor_closing);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private:
		System::ComponentModel::Container^	components;
		System::Windows::Forms::TextBox^	text_box;
		System::Windows::Forms::Label^		label1;
		System::Windows::Forms::ComboBox^	combo_box;
		System::Windows::Forms::Label^		label2;
		System::Windows::Forms::Button^		button1;
		System::Windows::Forms::Button^		button2;
		dialog_editor_impl^					m_editor;
		dialog_graph_node_phrase^			m_editing_node;
		bool								m_changed;
	}; // ref class dialog_text_editor
} // namespace dialog_editor
} // namespace xray

#endif // #ifndef DIALOG_TEXT_EDITOR_H_INCLUDED