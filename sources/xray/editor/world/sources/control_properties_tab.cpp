#include "pch.h"
#include "control_properties_tab.h"
#include "property_holder.h"
#include "editor_control_base.h"

using xray::editor::control_properties_tab;
using xray::editor::controls::property_grid;

void control_properties_tab::in_constructor()
{
	m_property_grid			= gcnew property_grid( );
	m_property_grid->Dock	= System::Windows::Forms::DockStyle::Fill;

	SuspendLayout			( );
	Controls->Add			( m_property_grid );
	ResumeLayout			( false );
}


void control_properties_tab::show_properties(xray::editor::editor_control_base^ o)
{
	m_property_grid->SelectedObject	= o;
}
