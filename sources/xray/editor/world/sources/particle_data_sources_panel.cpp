////////////////////////////////////////////////////////////////////////////
//	Created		: 28.04.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_data_sources_panel.h"
#include "particle_editor.h"
#include "particle_data_source_nodes.h"

using namespace System::Drawing;
using namespace System::Windows;

namespace xray {
namespace editor {

	void d(Object^ , EventArgs^ ){

	}
	void s(Object^ , EventArgs^ ){

	}


void particle_data_sources_panel::in_constructor(){
	this->SuspendLayout();
	// 
	// particle_data_sources_panel
	// 
	this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
	this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
	//this->ClientSize = System::Drawing::Size(292, 273);
	this->DockAreas = WeifenLuo::WinFormsUI::Docking::DockAreas::DockBottom;
	this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
		static_cast<System::Byte>(204)));
	this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
	this->MaximizeBox = false;
	this->MinimizeBox = false;
	this->CloseButton = false;
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

	fc_page = gcnew WeifenLuo::WinFormsUI::Docking::DockContent();
	fc_page->Text = "Float Curves";
	fc_page->AutoSize = true;
	fc_page->AutoScroll = true;
	fc_page->DockAreas = WeifenLuo::WinFormsUI::Docking::DockAreas::Document;
	
	Forms::ContextMenuStrip^	context_menu		= gcnew Forms::ContextMenuStrip();
	context_menu->Items->AddRange					(gcnew array<ToolStripMenuItem^>{
		gcnew ToolStripMenuItem("New Data Source", nullptr, gcnew EventHandler(&d)),		
	});
	fc_page->ContextMenuStrip = context_menu;


	cc_page = gcnew WeifenLuo::WinFormsUI::Docking::DockContent();
	cc_page->Text = "Color Curves";
	cc_page->AutoSize = true;
	cc_page->AutoScroll = true;
	cc_page->DockAreas = WeifenLuo::WinFormsUI::Docking::DockAreas::Document;
	
	context_menu		= gcnew Forms::ContextMenuStrip();
	context_menu->Items->AddRange					(gcnew array<ToolStripMenuItem^>{
		gcnew ToolStripMenuItem("New Data Source", nullptr, gcnew EventHandler(&d)),		
	});
	cc_page->ContextMenuStrip = context_menu;

 	fc_page->Show(m_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::Document );	
 	cc_page->Show(m_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::Document );	
}

void particle_data_sources_panel::add_new_control(Control^ c, DockContent^ to_page){
	if (to_page->Controls->Count == 0){
		to_page->Controls->Add(c);
		c->Location = System::Drawing::Point(10, Location.Y + 5);
		return;
	}
	Control^ last_control = to_page->Controls[to_page->Controls->Count - 1];
	to_page->Controls->Add(c);
	c->Location = System::Drawing::Point(last_control->Location.X + last_control->Width + 10, Location.Y + 5);
}

System::String^ particle_data_sources_panel::get_float_curves(s32 idx)
{
	if (idx == 0)
		return "Const";

	return fc_page->Controls[idx - 1]->Text;
}
s32 particle_data_sources_panel::get_float_curves_count()
{
	return fc_page->Controls->Count + 1;
}


String^ particle_data_sources_panel::get_color_curves(s32 idx)
{
	if (idx == 0)
		return "Const";

	return cc_page->Controls[idx - 1]->Text;
}
s32 particle_data_sources_panel::get_color_curves_count()
{
	return cc_page->Controls->Count + 1;
}


void particle_data_sources_panel::save(configs::lua_config_value config){
	configs::lua_config_value fc_configs = config["float_curves"].create_table();
	for each(particle_data_source_fc_node^ fc_node in fc_page->Controls){
		fc_configs[(pcstr)unmanaged_string(fc_node->Text).c_str()].assign_lua_value(*fc_node->fc->curve_config);
	}

	configs::lua_config_value cc_configs = config["color_curves"].create_table();
	for each(particle_data_source_cc_node^ cc_node in cc_page->Controls){
		cc_configs[(pcstr)unmanaged_string(cc_node->Text).c_str()].assign_lua_value(*cc_node->cc->curve_config);
	}

}

void particle_data_sources_panel::load(configs::lua_config_value config){

}

}//namespace editor 
}//namespace xray