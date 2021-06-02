////////////////////////////////////////////////////////////////////////////
//	Created		: 27.01.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_grid_panel.h"

using namespace xray::editor::wpf_controls;

namespace xray{
namespace model_editor{

void property_grid_panel::in_constructor( )
{
	m_property_grid_host		= gcnew property_grid::host( );
	m_property_grid_host->Dock	= System::Windows::Forms::DockStyle::Fill;

	SuspendLayout			( );
	Controls->Add			( m_property_grid_host );
	ResumeLayout			( false );
}

void property_grid_panel::set_property_container( property_container^ cont )
{
	m_property_grid_host->selected_object	= cont;
}

void property_grid_panel::refresh_properties( )
{
	m_property_grid_host->update();
}

} // namespace model_editor
} // namespace xray