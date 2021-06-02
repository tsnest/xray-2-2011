////////////////////////////////////////////////////////////////////////////
//	Created		: 20.01.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "item_properties_panel_base.h"

using namespace System;
using namespace System::Collections::Generic;

namespace xray {
namespace editor {
namespace controls {

void item_properties_panel_base::in_constructor()
{
	m_property_grid_host = gcnew host();
	m_property_grid_host->Dock = System::Windows::Forms::DockStyle::Fill;
	
	SuspendLayout();
	Controls->Add(m_property_grid_host);
	m_property_grid_host->selected_object = nullptr;
	ResumeLayout(false);
}

void item_properties_panel_base::show_properties(Object^ object)
{
	show_properties( object, false );
}

void item_properties_panel_base::show_properties(Object^ object, Boolean is_read_only )
{
	m_property_grid_host->property_grid->read_only_mode = is_read_only;

	array<Object^>^ objects = dynamic_cast<array<Object^>^>(object);
	if(objects != nullptr)
		m_property_grid_host->selected_objects = objects;
	else
		m_property_grid_host->selected_object = object;
}

void item_properties_panel_base::update_properties( )
{
	m_property_grid_host->property_grid->update( );
}

} // namespace controls
} // namespace editor
} // namespace xray