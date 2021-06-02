#include "pch.h"
#include "sectors_view.h"
#include "sectors_controls.h"

namespace xray{
namespace model_editor{

void sectors_view::in_constructor()
{
	m_sectrors_controls			= gcnew sectors_controls();
	m_sectrors_controls->Dock	= System::Windows::Forms::DockStyle::Fill;
	bottom_panel->SuspendLayout	();
	bottom_panel->Controls->Add	( m_sectrors_controls );
	bottom_panel->ResumeLayout	();

	m_property_grid_host = gcnew xray::editor::wpf_controls::property_grid::host;
	m_property_grid_host->Dock	= System::Windows::Forms::DockStyle::Fill;
	sectors_view_panel->SuspendLayout	();
	sectors_view_panel->Controls->Add( m_property_grid_host );
	sectors_view_panel->ResumeLayout();
}

void sectors_view::set_property_container( xray::editor::wpf_controls::property_container^ cont )
{
	m_property_grid_host->selected_object	= cont;
}

}
}
