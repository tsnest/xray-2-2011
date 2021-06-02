////////////////////////////////////////////////////////////////////////////
//	Created		: 17.05.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_export_form.h"
#include "string_table_ids_storage.h"
#include "string_table_id.h"
#include "dialog_editor.h"
#include "excel_file.h"
#include <xray/fs_utils.h>
#include "dialog_graph_node_phrase.h"

using xray::dialog_editor::dialog_export_form;
using xray::dialog_editor::excel_file;
using xray::dialog_editor::dialog_editor_impl;
using namespace xray::dialog_editor::string_tables;

void dialog_export_form::in_constructor()
{
	m_columns = gcnew columns_type();
	m_columns->Add("Numeric_ID", "A");
	m_columns->Add("String_ID", "B");
	m_columns->Add("Text_Type", "C");
	m_columns->Add("Original_Text", "D");
	m_columns->Add("Translated_Text", "E");
}

void dialog_export_form::show(System::String^ init_directory)
{
	saveFileDialog1->InitialDirectory = init_directory+"texts/batches/";
	checkedListBox1->Items->Clear();
	string_table_ids_storage::localization_languages_type::const_iterator it = get_string_tables()->languages_list()->begin();
	for(; it!=get_string_tables()->languages_list()->end(); ++it)
	{
		System::String^ lang_name = gcnew System::String(*it);
		if(lang_name=="origin")
			continue;

		checkedListBox1->Items->Add(lang_name, false);
	}
	checkedListBox1->SetItemChecked(0, true);

	checkedListBox2->Items->Clear();
	for each(String^ str in Enum::GetNames(dialog_graph_node_phrase::e_id_category::typeid))
	{
		if(str=="dialogs")
			checkedListBox2->Items->Add(str, true);
		else
			checkedListBox2->Items->Add(str, false);
	}

	this->ShowDialog();
}

void dialog_export_form::on_export_clicked(System::Object^, System::EventArgs^ )
{
	::DialogResult res = saveFileDialog1->ShowDialog();
	if(res==::DialogResult::OK)
	{
		for each(System::String^ sel_lang_name in checkedListBox1->CheckedItems)
		{
			System::String^ excel_file_path = saveFileDialog1->FileName;
			excel_file_path = excel_file_path->Insert(excel_file_path->Length-5, "_"+sel_lang_name);
			excel_file^	xl_file = gcnew excel_file();
			xl_file->create_file(excel_file_path);
			for each(columns_iter_type^ kvp in m_columns)
			{
				System::String^ cell_name = kvp->Value+"1";
				xl_file->write_value(cell_name, kvp->Key, true, false);
			}
			
			Int32 lines_counter = 2;
			excel_file_path = excel_file_path->Replace("\\", "/");
			unmanaged_string file_name = unmanaged_string(excel_file_path);
			fs_new::virtual_path_string config_directory_path;
			fs_new::virtual_path_string config_file_name;
			fs_new::get_path_without_last_item(&config_directory_path, file_name.c_str());
			fs_new::file_name_with_no_extension_from_path(&config_file_name, file_name.c_str());
			config_directory_path.append("/");
			config_directory_path.append(config_file_name.c_str());
			config_directory_path.append(".batch");
			configs::lua_config_ptr const& cfg = configs::create_lua_config(config_directory_path.c_str());
			xray::configs::lua_config_value root = cfg->get_root()["translators_id_to_string_table_id"];
			u32 last_translators_id = m_editor->last_translators_id(sel_lang_name);
			if(last_translators_id==u32(-1))
				last_translators_id = 0;
			//R_ASSERT(last_translators_id!=u32(-1));

			string_table_ids_storage::string_table_ids_type::const_iterator it = get_string_tables()->string_table_ids()->begin();
			for(; it!=get_string_tables()->string_table_ids()->end(); ++it)
			{
				System::String^ cur_str_id = gcnew System::String(it->first);
				dialog_graph_node_phrase::e_id_category cat = (dialog_graph_node_phrase::e_id_category)m_editor->id_category(cur_str_id);
				System::String^ cat_name = cat.ToString();
				if(!(checkedListBox2->CheckedItems->Contains(cat_name)))
					continue;

				if(m_editor->references_count(cur_str_id)==0)
					continue;

				if(checkBox1->Checked && !(m_editor->is_batch_file_name_empty(cur_str_id, sel_lang_name)))
					continue;

				for each(columns_iter_type^ kvp in m_columns)
				{
					System::String^ cell_name = kvp->Value+lines_counter.ToString();
					if(kvp->Key=="Numeric_ID")
						xl_file->write_value(cell_name, (s32)++last_translators_id);
					else if(kvp->Key=="String_ID")
						xl_file->write_value(cell_name, cur_str_id);
					else if(kvp->Key=="Text_Type")
						xl_file->write_value(cell_name, cat_name);
					else if(kvp->Key=="Original_Text")
						xl_file->write_value(cell_name, gcnew System::String(it->second->text()));
				}
				root[pcstr(it->first)] = last_translators_id;
				++lines_counter;
				m_editor->set_batch_file_name(cur_str_id, sel_lang_name, gcnew System::String(config_file_name.c_str()));
			}
			m_editor->set_last_translators_id(sel_lang_name, last_translators_id);

			cfg->save(configs::target_sources);
			xl_file->save();
			delete xl_file;
		}
		m_editor->save_options();
		this->Hide();
	}
}

void dialog_export_form::on_cancel_clicked(System::Object^, System::EventArgs^ )
{
	this->Hide();
}