#include "pch.h"
#include "tools_tab.h"
#include "tool_tab.h"

namespace xray{
namespace editor{

void tools_tab::add_tab(pcstr name, System::Windows::Forms::UserControl^ tab)
{
	tab_control->SuspendLayout();

	System::Windows::Forms::TabPage^ page = (gcnew System::Windows::Forms::TabPage());
	page->Text	= gcnew System::String(name);
//	page->Controls->SuspendLayout();
	page->Controls->Add(tab);
	tab->Dock = System::Windows::Forms::DockStyle::Fill;
	tab_control->Controls->Add(page);
//	page->Controls->ResumeLayout();
	tab_control->ResumeLayout();
}

void tools_tab::activate_tab( int idx )
{
	if(tab_control->SelectedIndex == idx)
		tab_control_SelectedIndexChanged( nullptr, nullptr);
	else
		tab_control->SelectedIndex = idx;
}

void tools_tab::tab_control_SelectedIndexChanged(System::Object^ , System::EventArgs^ )
{
	//m_prev_active_tab
	int current_index = tab_control->SelectedIndex;

	tool_tab^ prev_page		= safe_cast<tool_tab^>(tab_control->TabPages[m_prev_active_tab]->Controls[0]);
	tool_tab^ current_page	= safe_cast<tool_tab^>(tab_control->TabPages[current_index]->Controls[0]);

	prev_page->on_deactivate		( );
	current_page->on_activate		( );
	
}

} //namespace editor
} //namespace xray
