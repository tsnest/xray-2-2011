#include "pch.h"
#include "object_portals_panel.h"
#include "model_editor.h"
#include "edit_object_base.h"
#include "edit_portal.h"

using namespace xray::editor::wpf_controls;
using namespace System::Windows::Forms;
namespace xray{
namespace model_editor{

void object_portals_panel::in_constructor( )
{
	m_property_grid_host		= gcnew property_grid::host( );
	m_property_grid_host->Dock	= System::Windows::Forms::DockStyle::Fill;
	m_property_grid_host->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &object_portals_panel::property_grid_host_key_press);
	SuspendLayout			( );
	Controls->Add			( m_property_grid_host );
	ResumeLayout			( false );
}

void object_portals_panel::property_grid_host_key_press(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) 
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
}

void object_portals_panel::set_property_container( xray::editor::wpf_controls::property_container^ cont )
{
	m_property_grid_host->selected_object	= cont;
}

}//namespace model_editor
}//namespace xray

