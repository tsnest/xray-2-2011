////////////////////////////////////////////////////////////////////////////
//	Created		: 17.05.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_import_form.h"
#include "dialog_editor.h"
#include "excel_file.h"
#include "string_table_ids_storage.h"
#include "string_table_id.h"

using xray::dialog_editor::dialog_import_form;
using xray::dialog_editor::excel_file;
using xray::dialog_editor::dialog_editor_impl;
using namespace xray::dialog_editor::string_tables;

void dialog_import_form::show(System::String^ init_directory)
{
	openFileDialog1->InitialDirectory = init_directory+"texts/";
	::DialogResult res = openFileDialog1->ShowDialog();
	if(res==::DialogResult::OK)
	{
		label1->Text = "Import localization file "+openFileDialog1->FileName+"?";
		this->ShowDialog();
	}
}

void dialog_import_form::on_import_clicked(System::Object^, System::EventArgs^ )
{
	System::String^ excel_file_path = openFileDialog1->FileName;
	excel_file^	xl_file = gcnew excel_file();
	xl_file->load_file(excel_file_path);

	System::String^ cur_batch_file_name = openFileDialog1->SafeFileName;
	cur_batch_file_name = cur_batch_file_name->Remove(cur_batch_file_name->Length-5);

	System::String^ lang_name = excel_file_path;
	u32 index_of__ = lang_name->LastIndexOf("_");
	++index_of__;
	u32 index_of_dot = lang_name->LastIndexOf(".");
	lang_name = lang_name->Substring(index_of__, index_of_dot-index_of__);
	
	string_table_ids_storage::localization_languages_type::const_iterator lang_it = get_string_tables()->languages_list()->find(unmanaged_string(lang_name).c_str());
	R_ASSERT(lang_it!=get_string_tables()->languages_list()->end());
	Int32 lines_counter = 2;
	System::String^ str_id = xl_file->read_string("B"+lines_counter.ToString());
	while(str_id!="")
	{
		string_table_ids_storage::string_table_ids_type::const_iterator str_ids_it = get_string_tables()->string_table_ids()->find(unmanaged_string(str_id).c_str());
		if(str_ids_it!=get_string_tables()->string_table_ids()->end())
		{
			System::String^ bfn = m_editor->batch_file_name(str_id, lang_name);
			if(bfn!=cur_batch_file_name)
				continue;

			str_ids_it->second->remove_text(*lang_it);
			unmanaged_string loc_text = xl_file->read_string("E"+lines_counter.ToString());
			str_ids_it->second->new_text(*lang_it, loc_text.c_str());
		}
		++lines_counter;
		str_id = xl_file->read_string("B"+lines_counter.ToString());
	}
	
	m_editor->save_string_table();
	delete xl_file;
	this->Hide();
}

void dialog_import_form::on_cancel_clicked(System::Object^, System::EventArgs^ )
{
	this->Hide();
}
