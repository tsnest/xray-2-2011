////////////////////////////////////////////////////////////////////////////
//	Created		: 28.04.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_data_sources_panel.h"

using namespace System::Drawing;
using namespace System::Windows;

namespace xray {
namespace particle_editor {

void particle_data_sources_panel::in_constructor(){
	this->SuspendLayout();
	// 
	// particle_data_sources_panel
	// 
	this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
	this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
	this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
		static_cast<System::Byte>(204)));
	this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
	this->MaximizeBox = false;
	this->MinimizeBox = false;
	//this->CloseButton = false;
	this->Name = L"particle_data_sources_panel";
	this->Text = L"Data Sources";
	this->BackColor = System::Drawing::SystemColors::Control;
	this->ResumeLayout(false);
	this->AutoScroll = true;
	
	m_dock_panel = gcnew WeifenLuo::WinFormsUI::Docking::DockPanel();	
	m_dock_panel->ActiveAutoHideContent = nullptr;
	m_dock_panel->Dock = System::Windows::Forms::DockStyle::Fill;
	m_dock_panel->DockBackColor = System::Drawing::SystemColors::Control;
	m_dock_panel->DockLeftPortion = 0.5;
	m_dock_panel->DocumentStyle = WeifenLuo::WinFormsUI::Docking::DocumentStyle::DockingWindow;
	m_dock_panel->Location = System::Drawing::Point(0, 24);
	m_dock_panel->Name = L"m_main_dock_panel";
	m_dock_panel->Size = System::Drawing::Size(292, 249);
	
	this->Controls->Add(this->m_dock_panel);

	HideOnClose = true;

	m_panels_dictionary = gcnew System::Collections::Generic::Dictionary< System::String^, DockContent^>();
}

void particle_data_sources_panel::initialize_panels( Collections::Generic::Dictionary< String^, String^ >^ data_source_types )
{
	for each(Collections::Generic::KeyValuePair< System::String^, System::String^ >^ pair in data_source_types )
	{
		editor::wpf_controls::data_sources::data_sources_panel_host^ data_sources_panel = gcnew editor::wpf_controls::data_sources::data_sources_panel_host();
		WeifenLuo::WinFormsUI::Docking::DockContent^ panel = gcnew WeifenLuo::WinFormsUI::Docking::DockContent();
		panel->AutoSize = true;
		panel->AutoScroll = true;
		panel->DockAreas = WeifenLuo::WinFormsUI::Docking::DockAreas::Document;
		panel->Dock = System::Windows::Forms::DockStyle::Fill;
		panel->TabText = pair->Value;
		data_sources_panel->Dock = System::Windows::Forms::DockStyle::Fill;
		panel->Controls->Add(data_sources_panel);
		panel->Show( m_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::Document );

		m_panels_dictionary->Add( pair->Key, panel );
	}
}

void particle_data_sources_panel::show_data_sources( data_sources_lists_by_types_dictionary^ data_sources )
{
	for each ( System::Collections::Generic::KeyValuePair< System::String^, data_sources_list^>^ pair in data_sources )
	{
		data_sources_list^ outgoing_list = pair->Value;
		
		show_panel_sources( outgoing_list, pair->Key );
	}
}

void particle_data_sources_panel::show_panel_sources	( data_sources_list^ data_sources, System::String^ panel_name )
{
	editor::wpf_controls::data_sources::data_sources_panel_host^ data_sources_panel = (editor::wpf_controls::data_sources::data_sources_panel_host^)m_panels_dictionary[panel_name]->Controls[0];
	
	data_sources_panel->clear_data_sources( );
	
	for each ( particle_data_source_base^ data_source in data_sources )
	{
		data_sources_panel->add_data_source( data_source );
	}
	
}

void particle_data_sources_panel::add_data_source_to_panel( particle_data_source_base^ data_source, System::String^ panel_name )
{
	editor::wpf_controls::data_sources::data_sources_panel_host^ data_sources_panel = (editor::wpf_controls::data_sources::data_sources_panel_host^)m_panels_dictionary[panel_name]->Controls[0];	
	data_sources_panel->add_data_source( data_source );
}


}//namespace particle_editor
}//namespace xray;