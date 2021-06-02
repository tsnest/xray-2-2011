////////////////////////////////////////////////////////////////////////////
//	Created		: 17.06.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef APPLY_REVERT_FORM_H_INCLUDED
#define APPLY_REVERT_FORM_H_INCLUDED

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;

namespace xray {
namespace sound_editor {

	ref class sound_editor;

	public ref class apply_revert_form: public System::Windows::Forms::Form
	{
	public:
			apply_revert_form						(sound_editor^ ed);
			~apply_revert_form						();
		void fill_list_box_items					(List<String^>^ item_names);

	private:
		void m_apply_button_Click					(Object^, EventArgs^);
		void m_revert_button_Click					(Object^, EventArgs^);
		void m_cancel_button_Click					(Object^, EventArgs^);
		void applyChangesToolStripMenuItem_Click	(Object^, EventArgs^);
		void revertChangesToolStripMenuItem_Click	(Object^, EventArgs^);
		void m_list_box_MouseUp						(Object^, MouseEventArgs^ e);
		void InitializeComponent					(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			this->m_cancel_button = (gcnew System::Windows::Forms::Button());
			this->m_revert_button = (gcnew System::Windows::Forms::Button());
			this->m_apply_button = (gcnew System::Windows::Forms::Button());
			this->m_list_box = (gcnew System::Windows::Forms::ListBox());
			this->contextMenuStrip1 = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->applyChangesToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->revertChangesToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->panel1->SuspendLayout();
			this->panel2->SuspendLayout();
			this->contextMenuStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// panel1
			// 
			this->panel1->Controls->Add(this->m_apply_button);
			this->panel1->Controls->Add(this->m_revert_button);
			this->panel1->Controls->Add(this->m_cancel_button);
			this->panel1->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->panel1->Location = System::Drawing::Point(0, 236);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(224, 26);
			this->panel1->TabIndex = 4;
			// 
			// panel2
			// 
			this->panel2->Controls->Add(this->m_list_box);
			this->panel2->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panel2->Location = System::Drawing::Point(0, 0);
			this->panel2->Name = L"panel2";
			this->panel2->Size = System::Drawing::Size(224, 236);
			this->panel2->TabIndex = 5;
			// 
			// m_cancel_button
			// 
			this->m_cancel_button->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->m_cancel_button->Dock = System::Windows::Forms::DockStyle::Right;
			this->m_cancel_button->Location = System::Drawing::Point(149, 0);
			this->m_cancel_button->Name = L"m_cancel_button";
			this->m_cancel_button->Size = System::Drawing::Size(75, 26);
			this->m_cancel_button->TabIndex = 0;
			this->m_cancel_button->Text = L"Cancel";
			this->m_cancel_button->UseVisualStyleBackColor = true;
			this->m_cancel_button->Click += gcnew System::EventHandler(this, &apply_revert_form::m_cancel_button_Click);
			// 
			// m_revert_button
			// 
			this->m_revert_button->Dock = System::Windows::Forms::DockStyle::Right;
			this->m_revert_button->Location = System::Drawing::Point(74, 0);
			this->m_revert_button->Name = L"m_revert_button";
			this->m_revert_button->Size = System::Drawing::Size(75, 26);
			this->m_revert_button->TabIndex = 1;
			this->m_revert_button->Text = L"Revert";
			this->m_revert_button->UseVisualStyleBackColor = true;
			this->m_revert_button->Click += gcnew System::EventHandler(this, &apply_revert_form::m_revert_button_Click);
			// 
			// m_apply_button
			// 
			this->m_apply_button->Dock = System::Windows::Forms::DockStyle::Right;
			this->m_apply_button->Location = System::Drawing::Point(-1, 0);
			this->m_apply_button->Name = L"m_apply_button";
			this->m_apply_button->Size = System::Drawing::Size(75, 26);
			this->m_apply_button->TabIndex = 2;
			this->m_apply_button->Text = L"Apply";
			this->m_apply_button->UseVisualStyleBackColor = true;
			this->m_apply_button->Click += gcnew System::EventHandler(this, &apply_revert_form::m_apply_button_Click);
			// 
			// m_list_box
			// 
			this->m_list_box->Dock = System::Windows::Forms::DockStyle::Fill;
			this->m_list_box->FormattingEnabled = true;
			this->m_list_box->Location = System::Drawing::Point(0, 0);
			this->m_list_box->Name = L"m_list_box";
			this->m_list_box->SelectionMode = System::Windows::Forms::SelectionMode::MultiExtended;
			this->m_list_box->Size = System::Drawing::Size(224, 225);
			this->m_list_box->TabIndex = 0;
			this->m_list_box->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &apply_revert_form::m_list_box_MouseUp);
			// 
			// contextMenuStrip1
			// 
			this->contextMenuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->applyChangesToolStripMenuItem, 
				this->revertChangesToolStripMenuItem});
			this->contextMenuStrip1->Name = L"contextMenuStrip1";
			this->contextMenuStrip1->Size = System::Drawing::Size(155, 48);
			// 
			// applyChangesToolStripMenuItem
			// 
			this->applyChangesToolStripMenuItem->Name = L"applyChangesToolStripMenuItem";
			this->applyChangesToolStripMenuItem->Size = System::Drawing::Size(154, 22);
			this->applyChangesToolStripMenuItem->Text = L"Apply changes";
			this->applyChangesToolStripMenuItem->Click += gcnew System::EventHandler(this, &apply_revert_form::applyChangesToolStripMenuItem_Click);
			// 
			// revertChangesToolStripMenuItem
			// 
			this->revertChangesToolStripMenuItem->Name = L"revertChangesToolStripMenuItem";
			this->revertChangesToolStripMenuItem->Size = System::Drawing::Size(154, 22);
			this->revertChangesToolStripMenuItem->Text = L"Revert changes";
			this->revertChangesToolStripMenuItem->Click += gcnew System::EventHandler(this, &apply_revert_form::revertChangesToolStripMenuItem_Click);
			// 
			// apply_revert_form
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->m_cancel_button;
			this->ClientSize = System::Drawing::Size(224, 262);
			this->Controls->Add(this->panel2);
			this->Controls->Add(this->panel1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"apply_revert_form";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->Text = L"Changes";
			this->panel1->ResumeLayout(false);
			this->panel2->ResumeLayout(false);
			this->contextMenuStrip1->ResumeLayout(false);
			this->ResumeLayout(false);

		}

	private:
		sound_editor^		m_editor;
		Panel^				panel1;
		Panel^				panel2;
		Button^ 			m_apply_button;
		Button^ 			m_revert_button;
		Button^ 			m_cancel_button;
		ListBox^			m_list_box;
		ToolStripMenuItem^	applyChangesToolStripMenuItem;
		ToolStripMenuItem^	revertChangesToolStripMenuItem;
		System::ComponentModel::IContainer^			components;
		System::Windows::Forms::ContextMenuStrip^	contextMenuStrip1;
	}; // ref class apply_revert_form
} // namespace sound_editor
} // namespace xray
#endif // #ifndef APPLY_REVERT_FORM_H_INCLUDED