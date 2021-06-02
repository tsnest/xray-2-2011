////////////////////////////////////////////////////////////////////////////
//	Created		: 17.05.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_setup_properties_panel.h"
#include "animation_setup_manager.h"

using namespace System;
using namespace System::ComponentModel;
using xray::animation_editor::animation_setup_properties_panel;
using xray::animation_editor::animation_setup_manager;
using xray::editor::wpf_controls::property_grid::value_changed_event_handler;

animation_setup_properties_panel::animation_setup_properties_panel(animation_setup_manager^ m)
:xray::editor::controls::item_properties_panel_base(m->multidocument_base),
m_manager(m)
{
	m_property_grid_host->property_value_changed += gcnew value_changed_event_handler(this, &animation_setup_properties_panel::on_property_changed);
	m_property_grid_host->auto_update = true;
	m_property_grid_host->browsable_attribute_state = xray::editor::wpf_controls::property_editors::browsable_state::or;
	m_property_grid_host->browsable_attributes->Add(gcnew CategoryAttribute("Node properties"));
	HideOnClose = true;
}

void animation_setup_properties_panel::on_property_changed(System::Object^ sender, value_changed_event_args^ e)
{
	m_manager->on_property_changed(sender, e);
}

void animation_setup_properties_panel::show_properties(Object^ object)
{
	super::show_properties(object);
	if(object!=nullptr)
	{
		m_property_grid_host->data_view->SortDescriptions->Clear();
		m_property_grid_host->data_view->SortDescriptions->Add(SortDescription("name", ListSortDirection::Ascending));
		m_property_grid_host->data_view->Refresh();
	}
}
