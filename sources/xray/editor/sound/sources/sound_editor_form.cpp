////////////////////////////////////////////////////////////////////////////
//	Created		: 16.06.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "sound_editor_form.h"
#include "sound_document_editor_base.h"
#include "sound_editor.h"
#include "tree_view_panel.h"
#include "instance_properties_panel.h"
#include "object_properties_panel.h"
#include "toolbar_panel.h"

using xray::sound_editor::sound_editor_form;
using xray::sound_editor::sound_editor;

sound_editor_form::sound_editor_form(sound_editor^ ed)
:m_editor(ed)
{
	m_multidocument_base = gcnew sound_document_editor_base(ed);
	m_multidocument_base->main_dock_panel->DockBackColor = System::Drawing::SystemColors::AppWorkspace;
	m_multidocument_base->Dock = DockStyle::Fill;
	InitializeComponent();
	m_multidocument_base->add_menu_items(m_menu_view);
	m_multidocument_base->add_menu_items(m_menu_options);
	Controls->Add(m_multidocument_base);
}

sound_editor_form::~sound_editor_form()
{
	if (components)
		delete components;
}

void sound_editor_form::view_drop_down(Object^, EventArgs^)
{
	m_menu_view_tree_view->Checked = !(m_editor->files_view->IsHidden);
	m_menu_view_instance_properties->Checked = !(m_editor->instance_properties->IsHidden);
	m_menu_view_object_properties->Checked = !(m_editor->object_properties->IsHidden);
	m_menu_view_toolbar->Checked = !(m_editor->toolbar->IsHidden);
}

void sound_editor_form::tree_view_click(Object^, EventArgs^)
{
	if(m_menu_view_tree_view->Checked)
		m_editor->files_view->Hide();
	else
		m_editor->files_view->Show(m_multidocument_base->main_dock_panel);
}

void sound_editor_form::instance_properties_click(Object^, EventArgs^)
{
	if(m_menu_view_instance_properties->Checked)
		m_editor->instance_properties->Hide();
	else
		m_editor->instance_properties->Show(m_multidocument_base->main_dock_panel);
}

void sound_editor_form::object_properties_click(Object^, EventArgs^)
{
	if(m_menu_view_object_properties->Checked)
		m_editor->object_properties->Hide();
	else
		m_editor->object_properties->Show(m_multidocument_base->main_dock_panel);
}

void sound_editor_form::toolbar_click(Object^, EventArgs^)
{
	if(m_menu_view_toolbar->Checked)
		m_editor->toolbar->Hide();
	else
		m_editor->toolbar->Show(m_multidocument_base->main_dock_panel);
}
void sound_editor_form::options_drop_down(Object^, EventArgs^)
{
	m_menu_options_xaudio->Checked = (m_editor->calculation==xray::sound::xaudio);
}

void sound_editor_form::xaudio_click(Object^, EventArgs^)
{
	m_editor->calculation = (m_menu_options_xaudio->Checked) ?
		xray::sound::openal :
		xray::sound::xaudio;
}