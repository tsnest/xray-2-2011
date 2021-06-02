////////////////////////////////////////////////////////////////////////////
//	Created		: 27.05.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_editor.h"
#include "dialog_text_editor.h"
#include "dialog_language_panel.h"
#include "dialog_document.h"
#include "dialog_precondition_action_editor.h"
#include "dialog_export_form.h"
#include "dialog_import_form.h"
#include "dialog_graph_node_style.h"
#include "dialog_files_view_panel.h"
#include "dialog_item_properties_panel.h"
#include "dialog_control_panel.h"
#include "dialog_graph_node_base.h"
#include "dialog_graph_node_phrase.h"
#include "dialog_editor_contexts_manager.h"
#include "dialog_editor_form.h"

using xray::dialog_editor::dialog_editor_impl;
using xray::dialog_editor::dialog_document;
using xray::dialog_editor::dialog_graph_link;
using namespace WeifenLuo::WinFormsUI::Docking;
using namespace System::Windows::Forms;
using xray::editor_base::action_delegate;
using xray::editor_base::execute_delegate_managed;

void dialog_editor_impl::initialize_components()
{
	m_form = gcnew dialog_editor_form(this);
	m_form->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &dialog_editor_impl::on_editor_closing);

	m_node_style_mgr = gcnew dialog_graph_nodes_style_mgr();

	m_export_form = gcnew dialog_export_form(this);
	m_import_form = gcnew dialog_import_form(this);
	m_text_editor = gcnew dialog_text_editor(this);
	m_pred_act_editor = gcnew dialog_precondition_action_editor(this);
	m_language_panel = gcnew dialog_language_panel(this);
	m_properties_panel	= gcnew dialog_item_properties_panel(this);
	m_dialog_tool_bar	= gcnew dialog_control_panel(m_form->multidocument_base);
	m_view_panel		= gcnew dialog_files_view_panel(this);
	m_view_panel->source_path = System::IO::Path::GetFullPath("../../resources/sources/dialogs/configs/")->Replace("\\", "/");

	m_form->multidocument_base->active_saved += gcnew EventHandler(this, &dialog_editor_impl::save_active);
	m_form->multidocument_base->all_saved += gcnew EventHandler(this, &dialog_editor_impl::save_all);
	m_form->multidocument_base->editor_exited += gcnew EventHandler(this, &dialog_editor_impl::on_editor_exiting);
	m_form->multidocument_base->creating_new_document = gcnew document_create_callback(this, &dialog_editor_impl::on_document_creating);
	m_form->multidocument_base->content_reloading = gcnew content_reload_callback(this, &dialog_editor_impl::find_dock_content);
	m_form->multidocument_base->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &dialog_editor_impl::on_editor_key_down);
	bool loaded = m_form->multidocument_base->load_panels(m_form);
	if(!loaded)
	{
		m_view_panel->Show(m_form->multidocument_base->main_dock_panel, DockState::DockLeft);
		m_dialog_tool_bar->Show(m_form->multidocument_base->main_dock_panel, DockState::DockRight);
		m_properties_panel->Show(m_dialog_tool_bar->Pane, WeifenLuo::WinFormsUI::Docking::DockAlignment::Bottom, 0.75f);
		m_language_panel->Show(m_properties_panel->Pane, WeifenLuo::WinFormsUI::Docking::DockAlignment::Top, 0.2f);
	}
	m_form->multidocument_base->ActiveControl = m_view_panel;

	m_input_engine = gcnew xray::editor_base::input_engine( xray::editor_base::input_keys_holder::ref, gcnew dialog_editor_contexts_manager(this));
  	m_gui_binder = gcnew xray::editor_base::gui_binder(m_input_engine); 

	action_delegate^ a = nullptr;
	a = gcnew action_delegate("New Document", gcnew execute_delegate_managed(multidocument_base, &xray::editor::controls::document_editor_base::new_document));
	m_input_engine->register_action(a, "Control+N(dialog_editor)");

	a = gcnew action_delegate("Save Document", gcnew execute_delegate_managed(multidocument_base, &xray::editor::controls::document_editor_base::save_active));
	m_input_engine->register_action(a, "Control+S(dialog_editor)");

	a = gcnew action_delegate("Save All", gcnew execute_delegate_managed(multidocument_base, &xray::editor::controls::document_editor_base::save_all));
	m_input_engine->register_action(a, "Control+Shift+S(dialog_editor)");

	a = gcnew action_delegate("Exit", gcnew execute_delegate_managed(multidocument_base, &xray::editor::controls::document_editor_base::exit_editor));
	m_input_engine->register_action(a, "");

	a = gcnew action_delegate("Undo", gcnew execute_delegate_managed(multidocument_base, &xray::editor::controls::document_editor_base::undo));
	m_input_engine->register_action(a, "Control+Z(dialog_editor_active_document)");

	a = gcnew action_delegate("Redo", gcnew execute_delegate_managed(multidocument_base, &xray::editor::controls::document_editor_base::redo));
	m_input_engine->register_action(a, "Control+Y(dialog_editor_active_document)");

	a = gcnew action_delegate("Cut", gcnew execute_delegate_managed(multidocument_base, &xray::editor::controls::document_editor_base::cut));
	m_input_engine->register_action(a, "Control+X(dialog_editor_active_document)");

	a = gcnew action_delegate("Copy", gcnew execute_delegate_managed(multidocument_base, &xray::editor::controls::document_editor_base::copy));
	m_input_engine->register_action(a, "Control+C(dialog_editor_active_document)");

	a = gcnew action_delegate("Paste", gcnew execute_delegate_managed(multidocument_base, &xray::editor::controls::document_editor_base::paste));
	m_input_engine->register_action(a, "Control+V(dialog_editor_active_document)");

	a = gcnew action_delegate("Select All", gcnew execute_delegate_managed(multidocument_base, &xray::editor::controls::document_editor_base::select_all));
	m_input_engine->register_action(a, "Control+A(dialog_editor_active_document)");

	a = gcnew action_delegate("Delete", gcnew execute_delegate_managed(multidocument_base, &xray::editor::controls::document_editor_base::del));
	m_input_engine->register_action(a, "Delete(dialog_editor_active_document)");

	m_gui_binder->bind(m_form->multidocument_base->newToolStripMenuItem, "New Document");
	m_gui_binder->bind(m_form->multidocument_base->saveToolStripMenuItem, "Save Document");
	m_gui_binder->bind(m_form->multidocument_base->saveAllToolStripMenuItem, "Save All");
	m_gui_binder->bind(m_form->multidocument_base->exitToolStripMenuItem, "Exit");
	m_gui_binder->bind(m_form->multidocument_base->undoToolStripMenuItem, "Undo");
	m_gui_binder->bind(m_form->multidocument_base->redoToolStripMenuItem, "Redo");
	m_gui_binder->bind(m_form->multidocument_base->cutToolStripMenuItem, "Cut");
	m_gui_binder->bind(m_form->multidocument_base->copyToolStripMenuItem, "Copy");
	m_gui_binder->bind(m_form->multidocument_base->pasteToolStripMenuItem, "Paste");
	m_gui_binder->bind(m_form->multidocument_base->selectAllToolStripMenuItem, "Select All");
	m_gui_binder->bind(m_form->multidocument_base->delToolStripMenuItem, "Delete");
}

void dialog_editor_impl::destroy_components()
{
  	delete m_gui_binder;
  	delete m_input_engine;
	delete m_pred_act_editor;
	delete m_text_editor;
	delete m_language_panel;
	delete m_node_style_mgr;
	delete m_form;
}

void dialog_editor_impl::Show( System::String^ , System::String^ )
{
	m_form->Show();
}

IDockContent^ dialog_editor_impl::find_dock_content(System::String^ persist_string)
{
	if(persist_string == "xray.dialog_editor.dialog_files_view_panel")
		return m_view_panel;

	if (persist_string == "xray.dialog_editor.dialog_item_properties_panel")
		return m_properties_panel;

	if (persist_string == "xray.dialog_editor.dialog_control_panel")
		return m_dialog_tool_bar;

	if (persist_string == "xray.dialog_editor.dialog_language_panel")
		return m_language_panel;

	return (nullptr);
}


void dialog_editor_impl::on_selection_changed(xray::dialog_editor::dialog_graph_node_base^ n)
{
	m_text_editor->update_selected_node(n);
}

void dialog_editor_impl::on_editor_closing(Object^ , FormClosingEventArgs^ e)
{
	e->Cancel = true;
	if(save_confirmation())
		close_internal ( );
}

void dialog_editor_impl::on_editor_exiting(Object^ , EventArgs^ )
{
	if(save_confirmation())
		close_internal ( );
}

xray::editor::controls::document_base^ dialog_editor_impl::on_document_creating()
{
	if(creating_new_document)
		return gcnew dialog_document(this);

	if(m_form->save_file_dialog->ShowDialog(m_form)==::DialogResult::OK)
	{
		System::String^ file_name = m_form->save_file_dialog->FileName->Replace("\\", "/");
		System::String^ path = file_name->Remove(0, file_name->LastIndexOf("/")+1);
		path = path->Remove(path->Length-4);
		if(path->Contains("."))
		{
			System::String^ path2 = path->Replace(".", "_");
			file_name = file_name->Replace(path, path2);
			MessageBox::Show(m_form, "Name contains '.'!! Replacing with '_'", 
				m_form->Text, MessageBoxButtons::OK, MessageBoxIcon::Error);
		}

		System::IO::FileStream^ fs = System::IO::File::Create(file_name);
		if(fs)
		{
			delete (System::IDisposable^)fs;
			m_view_panel->refresh();
		}
	}
	return nullptr;
}

void dialog_editor_impl::on_node_property_changed(System::Object^ sender, value_changed_event_args^ e)
{
	xray::editor::wpf_controls::property_grid::property_grid_control^ g = safe_cast<xray::editor::wpf_controls::property_grid::property_grid_control^>(sender);
	dialog_graph_node_phrase^ n = safe_cast<dialog_graph_node_phrase^>(g->selected_object);

	if(e->changed_property->name=="text")
		safe_cast<dialog_document^>(m_form->multidocument_base->active_document)->on_node_text_changed(n, n->string_table, (String^)e->new_value, n->string_table, (String^)e->old_value);
	else if(e->changed_property->name=="string_table")
	{
		String^ new_str = (String^)e->new_value;
		String^ old_str = (String^)e->old_value;
		String^ new_text = get_text_by_id(new_str);
		String^ old_text = get_text_by_id(old_str);
		safe_cast<dialog_document^>(m_form->multidocument_base->active_document)->on_node_text_changed(n, new_str, new_text, old_str, old_text);
	}
	else
		safe_cast<dialog_document^>(m_form->multidocument_base->active_document)->on_node_property_changed(safe_cast<dialog_graph_node_base^>(g->selected_object), e->changed_property->name, e->new_value, e->old_value);
}

void dialog_editor_impl::update_node_text(xray::dialog_editor::dialog_graph_node_phrase^ n, System::String^ id, System::String^ txt)
{
	if(id==n->string_table && txt==n->get_text())
		return;

	safe_cast<dialog_document^>(m_form->multidocument_base->active_document)->on_node_text_changed(n, id, txt, n->string_table, n->get_text());
}

void dialog_editor_impl::update_documents_text()
{
	for each(dialog_document^ d in m_form->multidocument_base->opened_documents)
		d->update_texts();
}

void dialog_editor_impl::show_prec_act_editor(dialog_graph_link^ lnk)
{
	m_pred_act_editor->show(lnk, true, true);
}

void dialog_editor_impl::show_prec_act_editor(dialog_graph_link^ lnk, bool act_v, bool prec_v)
{
	m_pred_act_editor->show(lnk, act_v, prec_v);
}

void dialog_editor_impl::show_text_editor(xray::dialog_editor::dialog_graph_node_phrase^ n)
{
	if(n!=nullptr && !(m_text_editor->Visible))
	{
		m_text_editor->assign_string_table_ids();
		m_text_editor->Show(m_form->multidocument_base);
		m_text_editor->update_selected_node(n);
	}
}

void dialog_editor_impl::on_editor_key_down(Object^ , KeyEventArgs^ e)
{
	if(e->KeyCode==Keys::Tab && e->Control)
	{
		int cur_doc_count = m_form->multidocument_base->opened_documents->Count;
		if(cur_doc_count>1)
		{
			for(int i=0; i<cur_doc_count; i++)
				if(m_form->multidocument_base->opened_documents->default[i]==m_form->multidocument_base->active_document)
				{
					if(i==cur_doc_count-1)
						i = -1;

					m_form->multidocument_base->opened_documents->default[i+1]->Activate();
					m_form->multidocument_base->active_document = m_form->multidocument_base->opened_documents->default[i+1];
					return;
				}
		}
	}
	else if(e->KeyCode==Keys::F4 && e->Control)
	{
		if(m_form->multidocument_base->active_document!=nullptr)
		{
			m_form->multidocument_base->active_document->Close();
			return;
		}
	}
}

void dialog_editor_impl::on_add_language_click(System::Object^ , System::EventArgs^ )
{
	m_language_panel->add_language(nullptr, nullptr);
}

void dialog_editor_impl::on_remove_language_click(System::Object^ , System::EventArgs^ )
{
	m_language_panel->remove_language(nullptr, nullptr);
}

void dialog_editor_impl::on_export_click(System::Object^ , System::EventArgs^ )
{
	m_export_form->show(System::IO::Path::GetFullPath(resources_path));
}

void dialog_editor_impl::on_import_click(System::Object^ , System::EventArgs^ )
{
	m_import_form->show(System::IO::Path::GetFullPath(resources_path));
}

document_editor_base^ dialog_editor_impl::multidocument_base::get()
{	
	return m_form->multidocument_base;
}
