#include "pch.h"
#include "dialog_language_panel.h"
#include "dialog_editor.h"
#include "add_language_form.h"
#include "remove_language_form.h"
#include "string_table_ids_storage.h"

using xray::dialog_editor::dialog_language_panel;
using xray::dialog_editor::dialog_editor_impl;
using xray::dialog_editor::add_language_form;
using xray::dialog_editor::remove_language_form;
using namespace xray::dialog_editor::string_tables;
using xray::dialog_editor::string_tables::string_table_ids_storage;

dialog_language_panel::dialog_language_panel(dialog_editor_impl^ ed)
:m_editor(ed)
{
	m_height = 10;
	group_box = (gcnew System::Windows::Forms::GroupBox());
	m_context_menu = (gcnew System::Windows::Forms::ContextMenuStrip());
	add_language_menu_item = (gcnew System::Windows::Forms::ToolStripMenuItem());
	remove_language_menu_item = (gcnew System::Windows::Forms::ToolStripMenuItem());
	m_add_lang_form = gcnew add_language_form(ed);
	m_add_lang_form->Hide();
	m_del_lang_form = gcnew remove_language_form(ed);
	m_del_lang_form->Hide();
	m_context_menu->SuspendLayout();
	SuspendLayout();
	m_context_menu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
		this->add_language_menu_item, 
		this->remove_language_menu_item,
	});
	m_context_menu->Name = L"m_context_menu";
	m_context_menu->Size = System::Drawing::Size(153, 92);

	add_language_menu_item->Name = L"add_language_menu_item";
	add_language_menu_item->Size = System::Drawing::Size(152, 22);
	add_language_menu_item->Text = L"Add language";
	add_language_menu_item->Click += gcnew System::EventHandler(this, &dialog_language_panel::add_language);

	remove_language_menu_item->Name = L"remove_language_menu_item";
	remove_language_menu_item->Size = System::Drawing::Size(152, 22);
	remove_language_menu_item->Text = L"Remove language";
	remove_language_menu_item->Click += gcnew System::EventHandler(this, &dialog_language_panel::remove_language);

	m_context_menu->ResumeLayout(false);
	ContextMenuStrip = m_context_menu;

	// group_box
	group_box->Location = System::Drawing::Point(1, 1);
	group_box->Name = L"group_box";
	group_box->Size = System::Drawing::Size(150, m_height);
	group_box->TabIndex = 0;
	group_box->TabStop = false;

	// dialog_language_panel
	AutoScaleDimensions = System::Drawing::SizeF(6, 13);
	AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
	ClientSize = System::Drawing::Size(62, m_height+2);
	Controls->Add(group_box);
	DockAreas = static_cast<WeifenLuo::WinFormsUI::Docking::DockAreas>(
		WeifenLuo::WinFormsUI::Docking::DockAreas::DockLeft | 
		WeifenLuo::WinFormsUI::Docking::DockAreas::DockRight | 
		WeifenLuo::WinFormsUI::Docking::DockAreas::DockTop | 
		WeifenLuo::WinFormsUI::Docking::DockAreas::DockBottom | 
		WeifenLuo::WinFormsUI::Docking::DockAreas::Float);
	Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(204)));
	Name = L"dialog_language_panel";
	Text = L"Current language";
	HideOnClose = true;
	ResumeLayout(false);
}

void dialog_language_panel::refresh_languages()
{
	m_height = 10;
	group_box->Controls->Clear();
	const string_table_ids_storage::localization_languages_type* lst = get_string_tables()->languages_list();
	string_table_ids_storage::localization_languages_type::const_iterator b = lst->begin();
	for(; b!=lst->end(); ++b)
		add_new_language(gcnew String(*b), false);

	set_active_language(gcnew String(get_string_tables()->cur_language_name()));
}

void dialog_language_panel::add_new_language(String^ loc, bool is_checked)
{
	for each(System::Windows::Forms::RadioButton^ rb in group_box->Controls)
	{
		if(rb->Text==loc)
			return;
	}

	group_box->SuspendLayout();

	System::Windows::Forms::RadioButton^ rb = gcnew System::Windows::Forms::RadioButton();
	rb->AutoSize = true;
	rb->Checked = is_checked;
	rb->Location = System::Drawing::Point(5, m_height);
	rb->Name = "rb_"+loc;
	rb->Size = System::Drawing::Size(50, 15);
	rb->Text = loc;
	rb->UseVisualStyleBackColor = true;
	rb->CheckedChanged += gcnew System::EventHandler(this, &dialog_language_panel::on_language_changed);

	group_box->Controls->Add(rb);
	m_height += 20;
	group_box->Height = m_height+5;
	group_box->ResumeLayout(false);
	group_box->PerformLayout();
}

void dialog_language_panel::set_active_language(String^ loc)
{
	for each(System::Windows::Forms::RadioButton^ rb in group_box->Controls)
	{
		if(rb->Text==loc)
			rb->Checked = true;
		else
			rb->Checked = false;
	}
}

void dialog_language_panel::on_language_changed(System::Object^ sender, System::EventArgs^ )
{
	if(safe_cast<System::Windows::Forms::RadioButton^>(sender)->Checked)
		m_editor->set_cur_language(safe_cast<System::Windows::Forms::RadioButton^>(sender)->Text);
}

void dialog_language_panel::add_language(System::Object^ , System::EventArgs^ )
{
	m_add_lang_form->ShowDialog();
}

void dialog_language_panel::remove_language(System::Object^ , System::EventArgs^ )
{
	m_del_lang_form->show();
}
