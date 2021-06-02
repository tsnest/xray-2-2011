////////////////////////////////////////////////////////////////////////////
//	Created		: 27.01.2011
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_editor_form.h"
#include "dialog_language_panel.h"
#include "dialog_files_view_panel.h"
#include "dialog_control_panel.h"
#include "dialog_item_properties_panel.h"

using xray::dialog_editor::dialog_editor_form;

void dialog_editor_form::in_constructor()
{
	add_language_menu_item->Click += gcnew System::EventHandler(m_editor, &dialog_editor_impl::on_add_language_click);
	remove_language_menu_item->Click += gcnew System::EventHandler(m_editor, &dialog_editor_impl::on_remove_language_click);
	export_to_xls_menu_item->Click += gcnew System::EventHandler(m_editor, &dialog_editor_impl::on_export_click);
	import_from_xls_menu_item->Click += gcnew System::EventHandler(m_editor, &dialog_editor_impl::on_import_click);

	m_multidocument_base = gcnew xray::editor::controls::document_editor_base(m_editor->name());
	m_multidocument_base->create_base_panel_objects = false;
	m_multidocument_base->main_dock_panel->DockBackColor = System::Drawing::SystemColors::AppWorkspace;
	m_multidocument_base->add_menu_items(view_menu_item);
	m_multidocument_base->add_menu_items(localizations_menu_item);
	m_multidocument_base->Dock = DockStyle::Fill;
	Controls->Add(m_multidocument_base);

	Name = m_editor->name();
	m_save_file_dialog->InitialDirectory = System::IO::Path::GetFullPath(m_editor->resources_path)+"configs\\";
}

void dialog_editor_form::on_view_menu_item_select(Object^ , EventArgs^ )
{
	language_panel_menu_item->Checked = !m_editor->language_panel->IsHidden;
	file_view_panel_menu_item->Checked = !m_editor->view_panel->IsHidden;
	properties_panel_menu_item->Checked = !m_editor->properties_panel->IsHidden;
	control_panel_menu_item->Checked = !m_editor->dialog_tool_bar->IsHidden;
}

void dialog_editor_form::on_language_panel_menu_select(Object^ , EventArgs^ )
{
	if(!(language_panel_menu_item->Checked))
		m_editor->language_panel->Hide();
	else
		m_editor->language_panel->Show(m_multidocument_base->main_dock_panel, DockState::DockRight);
}

void dialog_editor_form::on_file_view_panel_menu_select(Object^ , EventArgs^ )
{
	if(!(file_view_panel_menu_item->Checked))
		m_editor->view_panel->Hide();
	else
		m_editor->view_panel->Show(m_multidocument_base->main_dock_panel, DockState::DockLeft);
}

void dialog_editor_form::on_properties_panel_menu_select(Object^ , EventArgs^ )
{
	if(!(properties_panel_menu_item->Checked))
		m_editor->properties_panel->Hide();
	else
		m_editor->properties_panel->Show(m_multidocument_base->main_dock_panel, DockState::DockRight);
}

void dialog_editor_form::on_control_panel_menu_select(Object^ , EventArgs^ )
{
	if(!(control_panel_menu_item->Checked))
		m_editor->dialog_tool_bar->Hide();
	else
		m_editor->dialog_tool_bar->Show(m_multidocument_base->main_dock_panel, DockState::DockRight);
}
