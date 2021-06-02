////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_text_editor.h"
#include "dialog_editor.h"
#include "string_table_ids_storage.h"
#include "string_table_id.h"
#include "dialog_graph_node_phrase.h"

using xray::dialog_editor::dialog_text_editor;
using xray::dialog_editor::string_tables::string_table_ids_storage;
using xray::dialog_editor::dialog_editor_impl;
using xray::editor::controls::AlphanumComparator;
using namespace System;
using namespace System::Collections;
using namespace xray::dialog_editor::string_tables;
using xray::dialog_editor::dialog_graph_node_base;

void dialog_text_editor::assign_string_table_ids()
{
	combo_box->Items->Clear();
	Generic::List<String^ >^ lst = gcnew Generic::List<String^ >();

	const string_table_ids_storage::string_table_ids_type* str_tbl = string_tables::get_string_tables()->string_table_ids();
	string_table_ids_storage::string_table_ids_type::const_iterator b = str_tbl->begin();
	for(; b!=str_tbl->end(); ++b)
		lst->Add(gcnew String(b->first));

	lst->Sort(gcnew AlphanumComparator());

	for each(String^ v in lst)
		combo_box->Items->Add(v);

	lst->Clear();
	if(m_editing_node!=nullptr)
		text_box->Text = m_editor->get_text_by_id(m_editing_node->string_table);
}

void dialog_text_editor::update_selected_node(xray::dialog_editor::dialog_graph_node_base^ n)
{
	if(n->is_dialog())
	{
		m_editing_node = nullptr;
		Hide();
		return;
	}

	if(!Visible)
		return;

	m_editing_node = safe_cast<dialog_graph_node_phrase^>(n);
	combo_box->SelectedItem = m_editing_node->string_table;
	text_box->Text = m_editor->get_text_by_id(m_editing_node->string_table);
	ActiveControl = text_box;
	changed = false;
}

void dialog_text_editor::on_editor_closing(System::Object^ , System::Windows::Forms::FormClosingEventArgs^ e)
{
	changed = false;
	m_editing_node = nullptr;
	Hide();
	e->Cancel = true;
}

void dialog_text_editor::combo_box_selection_changed(System::Object^ , System::EventArgs^ )
{
	text_box->Text = m_editor->get_text_by_id((String^)combo_box->SelectedItem);
	changed = true;
}

void dialog_text_editor::on_key_down(System::Object^ , System::Windows::Forms::KeyEventArgs^ e)
{
	changed = true;
	if(e->KeyCode==Keys::Return)
	{
		update_editing_node_text();
		m_editing_node = nullptr;
		Hide();
		changed = false;
	}
	else if(e->KeyCode==Keys::Escape)
	{
		m_editing_node = nullptr;
		Hide();
		changed = false;
	}
}

void dialog_text_editor::on_key_press(System::Object^ , System::Windows::Forms::KeyPressEventArgs^ e)
{
	if(e->KeyChar==(char)Keys::Return || e->KeyChar==(char)Keys::LineFeed)
		e->Handled = true;
}

void dialog_text_editor::update_editing_node_text()
{
	if(m_editing_node!=nullptr && combo_box->Text!="")
	{
		if(combo_box->SelectedItem==nullptr)
		{
			combo_box->Items->Insert(find_new_str_id_cb_pos(combo_box->Text), combo_box->Text);
			m_editor->add_new_id(combo_box->Text, text_box->Text);
		}

		m_editor->update_node_text(m_editing_node, combo_box->Text, text_box->Text);
	}
}

void dialog_text_editor::on_ok_clicked(System::Object^ , System::EventArgs^ )
{
	update_editing_node_text();
	changed = false;
}

void dialog_text_editor::on_cancel_clicked(System::Object^ , System::EventArgs^ )
{
	m_editing_node = nullptr;
	Hide();
	changed = false;
}

u32 dialog_text_editor::find_new_str_id_cb_pos(System::String^ new_str_id)
{
	s32 index = 0;
	AlphanumComparator^ comparer = gcnew AlphanumComparator();
	while(combo_box->Items->Count<index && comparer->Compare(new_str_id, (String^)combo_box->Items[index])>=0)
		++index;

	return index;
}
