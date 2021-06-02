////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2009
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "command_change_node_string_table.h"
#include "dialog_editor.h"
#include "dialog_hypergraph.h"
#include "dialog_document.h"
#include "dialog_graph_node_phrase.h"

using xray::dialog_editor::command_change_node_string_table;
using System::Reflection::BindingFlags;
using xray::dialog_editor::dialog_editor_impl;
using xray::dialog_editor::dialog_hypergraph;
using xray::dialog_editor::dialog_document;
using xray::dialog_editor::dialog_graph_node_phrase;

command_change_node_string_table::command_change_node_string_table(
	dialog_hypergraph^ h, 
	xray::dialog_editor::dialog_graph_node_phrase^ n, 
	System::String^ new_str, 
	System::String^ new_text, 
	System::String^ old_str, 
	System::String^ old_text
)
:m_hypergraph(h),
m_node_id(n->id),
m_old_str(old_str),
m_new_str(new_str),
m_old_text(old_text),
m_new_text(new_text)
{
	m_editor = m_hypergraph->get_owner()->get_editor();
	m_backup_str_text = m_editor->get_text_by_id(m_new_str);
	m_first_run = true;
}

bool command_change_node_string_table::commit()
{
	dialog_graph_node_base^ nd = dialog_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), m_node_id);
	dialog_graph_node_phrase^ n = safe_cast<dialog_graph_node_phrase^>(nd);
	if((System::String^)m_old_str==(System::String^)m_new_str)
	{
		if(m_editor->change_ids_text((System::String^)m_new_str, (System::String^)m_new_text, m_first_run))
		{
			n->Invalidate(false);
			m_editor->show_properties(n);
			return true;
		}
		return false;
	}
	if((System::String^)m_backup_str_text==(System::String^)m_new_text)
	{
		n->string_table = (System::String^)m_new_str;
		n->Invalidate(false);
		m_editor->change_references_count((System::String^)m_new_str, (System::String^)m_old_str);
		m_editor->show_properties(n);
		return true;
	}

	m_editor->change_references_count((System::String^)m_new_str, (System::String^)m_old_str);
	if(m_editor->change_ids_text((System::String^)m_new_str, (System::String^)m_new_text, m_first_run))
	{
		n->string_table = (System::String^)m_new_str;
		n->Invalidate(false);
		m_editor->update_documents_text();
		return true;
	}
	m_editor->change_references_count((System::String^)m_old_str, (System::String^)m_new_str);
	m_editor->update_documents_text();
	return false;
}

void command_change_node_string_table::rollback()
{
	m_first_run = false;
	dialog_graph_node_base^ nd = dialog_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), m_node_id);
	dialog_graph_node_phrase^ n = safe_cast<dialog_graph_node_phrase^>(nd);
	if((System::String^)m_old_str==(System::String^)m_new_str)
	{
		m_editor->change_ids_text((System::String^)m_old_str, (System::String^)m_old_text, m_first_run);
		n->Invalidate(false);
		m_editor->show_properties(n);
		return;
	}
	if((System::String^)m_backup_str_text==(System::String^)m_new_text)
	{
		n->string_table = (System::String^)m_old_str;
		n->Invalidate(false);
		m_editor->change_references_count((System::String^)m_old_str, (System::String^)m_new_str);
		m_editor->show_properties(n);
		return;
	}

	n->string_table = (System::String^)m_old_str;
	m_editor->show_properties(n);
	m_editor->change_references_count((System::String^)m_old_str, (System::String^)m_new_str);
	m_editor->change_ids_text((System::String^)m_new_str, (System::String^)m_backup_str_text, m_first_run);
	n->Invalidate(false);
}