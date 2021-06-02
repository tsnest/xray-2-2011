////////////////////////////////////////////////////////////////////////////
//	Created		: 18.08.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "channels_editor.h"
#include "animation_editor.h"
#include "channels_files_view_panel.h"
#include "channels_document.h"
#include "channels_layout_panel.h"

using xray::animation_editor::channels_editor;
using xray::animation_editor::animation_editor;

channels_editor::channels_editor(animation_editor^ ed)
{
	m_animation_editor = ed;
	components = gcnew System::ComponentModel::Container();
	AllowEndUserDocking = false;
	Size = System::Drawing::Size(300,300);
	Name = L"channels_editor";
	Text = L"Channels";
	Padding = System::Windows::Forms::Padding(0);
	AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
	HideOnClose = true;
	DockAreas = static_cast<WeifenLuo::WinFormsUI::Docking::DockAreas>(
		WeifenLuo::WinFormsUI::Docking::DockAreas::Document);

	m_multidocument_base = gcnew xray::editor::controls::document_editor_base("animation_channels_editor", false);
	m_file_view_panel = gcnew channels_files_view_panel(m_multidocument_base);
	m_layout_panel = gcnew channels_layout_panel();
	m_multidocument_base->main_dock_panel->DockBackColor = System::Drawing::SystemColors::AppWorkspace; 
	m_multidocument_base->creating_new_document = gcnew xray::editor::controls::document_create_callback(this, &channels_editor::on_document_creating);
	m_multidocument_base->content_reloading = gcnew xray::editor::controls::content_reload_callback(this, &channels_editor::find_dock_content);
	bool loaded = m_multidocument_base->load_panels(this);
	if(!loaded)
	{
		m_file_view_panel->Show(m_multidocument_base->main_dock_panel);
		m_layout_panel->Show(m_multidocument_base->main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::DockBottom);
	}
	m_multidocument_base->Dock = DockStyle::Fill;
	Controls->Add(m_multidocument_base);
}

channels_editor::~channels_editor()
{
	if (components)
		delete components;
}

bool channels_editor::close_query()
{
	m_multidocument_base->save_panels(this);
	return true;
}

IDockContent^ channels_editor::find_dock_content(System::String^ persist_string)
{
	if( persist_string == "xray.animation_editor.channels_files_view_panel" )
		return m_file_view_panel;
	else if( persist_string == "xray.animation_editor.channels_layout_panel" )
		return m_layout_panel;

	return ( nullptr );
}

xray::editor::controls::document_base^ channels_editor::on_document_creating( )
{
	return gcnew channels_document(this);
}
