////////////////////////////////////////////////////////////////////////////
//	Created		: 26.08.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "channels_layout_panel.h"

using xray::animation_editor::channels_layout_panel;

channels_layout_panel::channels_layout_panel()
{
	components = gcnew System::ComponentModel::Container();
	CloseButtonVisible = false;
	Size = System::Drawing::Size(300,300);
	Name = L"channels_layout_panel";
	Text = L"Layout";
	Padding = System::Windows::Forms::Padding(0);
	AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
	HideOnClose = true;
	DockAreas = static_cast<WeifenLuo::WinFormsUI::Docking::DockAreas>(
		WeifenLuo::WinFormsUI::Docking::DockAreas::DockBottom |
		WeifenLuo::WinFormsUI::Docking::DockAreas::DockTop);
	
	m_host = gcnew System::Windows::Forms::Integration::ElementHost();
	m_host->Size = System::Drawing::Size(300,180);
	m_curve_panel = gcnew curve_editor::curve_editor_panel();
	m_curve_panel->is_read_only = true;
	m_host->Child = m_curve_panel;
	m_host->Dock = System::Windows::Forms::DockStyle::Fill;
	Controls->Add(m_host);
}

channels_layout_panel::~channels_layout_panel()
{
	if (components)
		delete components;
}

void channels_layout_panel::add_curves(System::Collections::Generic::List<float_curve^>^ curves_list)
{
	m_curve_panel->edited_curves = curves_list;
}