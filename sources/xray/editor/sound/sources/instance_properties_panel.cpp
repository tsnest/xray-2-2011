////////////////////////////////////////////////////////////////////////////
//	Created		: 17.06.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "instance_properties_panel.h"
#include "sound_document_editor_base.h"
#include "sound_editor.h"
#include "sound_object_instance.h"

using xray::sound_editor::instance_properties_panel;
using xray::sound_editor::sound_editor;
using xray::editor::wpf_controls::property_grid::value_changed_event_handler;
using namespace System::ComponentModel;

instance_properties_panel::instance_properties_panel(sound_editor^ ed)
:item_properties_panel_base(ed->multidocument_base),
m_owner(ed)
{
	m_property_grid_host->property_value_changed += gcnew value_changed_event_handler(this, &instance_properties_panel::on_node_property_changed);
	m_property_grid_host->auto_update = true;
	HideOnClose = true;
	Text = "Instance properties";
}

void instance_properties_panel::on_node_property_changed(System::Object^ sender, value_changed_event_args^ e)
{
	m_owner->on_instance_property_changed(sender, e);
}

void instance_properties_panel::show_properties(Object^ object)
{
	super::show_properties(object);
	if(object!=nullptr)
	{
		m_property_grid_host->data_view->SortDescriptions->Clear();
		m_property_grid_host->data_view->SortDescriptions->Add(SortDescription("DataContext.name", ListSortDirection::Ascending));
		m_property_grid_host->data_view->Refresh();
	}
}

