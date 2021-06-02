#include "pch.h"
#include "window_tab.h"
#include "property_holder.h"

using xray::editor::window_tab;
using xray::editor::controls::property_grid;

void window_tab::in_constructor	( System::String^ tab_id, xray::editor::property_holder& property_holder )
{
	Text					= tab_id;
	TabText					= tab_id;

	m_property_grid			= gcnew property_grid( );
	m_property_grid->Dock	= System::Windows::Forms::DockStyle::Fill;

	::property_holder&		properties = dynamic_cast< ::property_holder& >( property_holder );
	m_property_grid->SelectedObject	= properties. container( );

	SuspendLayout			( );
	Controls->Add			( m_property_grid );
	ResumeLayout			( false );
}