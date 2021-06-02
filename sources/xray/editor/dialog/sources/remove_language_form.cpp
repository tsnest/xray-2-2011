////////////////////////////////////////////////////////////////////////////
//	Created		: 14.05.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "remove_language_form.h"
#include "dialog_editor.h"
#include "string_table_ids_storage.h"

using xray::dialog_editor::remove_language_form;
using xray::dialog_editor::string_tables::string_table_ids_storage;

void remove_language_form::show()
{
	checkedListBox1->Items->Clear();
	string_tables::string_table_ids_storage::localization_languages_type::const_iterator it = string_tables::get_string_tables()->languages_list()->begin();
	for(; it!=string_tables::get_string_tables()->languages_list()->end(); ++it)
	{
		System::String^ lang_name = gcnew System::String(*it);
		if(lang_name=="origin")
			continue;

		checkedListBox1->Items->Add(lang_name, false);
	}
	this->ShowDialog();
}

void remove_language_form::on_delete_clicked(System::Object^ , System::EventArgs^ )
{
	::DialogResult res = MessageBox::Show(this, "Are you sure to delete these languages?", "Dialog editor", MessageBoxButtons::OKCancel, MessageBoxIcon::Exclamation);
	if(res==::DialogResult::OK)
	{
		for each(System::String^ checked_lang in checkedListBox1->CheckedItems)
			m_editor->remove_language(checked_lang);
	}

	this->Hide();
}

void remove_language_form::on_cancel_clicked(System::Object^ , System::EventArgs^ )
{
	this->Hide();
}
