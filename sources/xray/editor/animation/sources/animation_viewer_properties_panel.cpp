////////////////////////////////////////////////////////////////////////////
//	Created		: 14.09.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_viewer_properties_panel.h"
#include "viewer_editor.h"
#include "animation_viewer_graph_link_weighted.h"
#include "animation_viewer_graph_link_timed.h"

using namespace System;
using xray::animation_editor::animation_viewer_properties_panel;
using xray::animation_editor::viewer_editor;
using xray::animation_editor::animation_viewer_graph_link_weighted;

using xray::editor::wpf_controls::property_grid::value_changed_event_handler;
typedef xray::editor::wpf_controls::property_container wpf_property_container;

animation_viewer_properties_panel::animation_viewer_properties_panel(viewer_editor^ ed)
:xray::editor::controls::item_properties_panel_base(ed->multidocument_base),
m_owner(ed)
{
	m_property_grid_host->property_value_changed += gcnew value_changed_event_handler(this, &animation_viewer_properties_panel::on_node_property_changed);
	m_property_grid_host->auto_update = true;
	m_property_grid_host->browsable_attribute_state = xray::editor::wpf_controls::property_editors::browsable_state::or;
	m_property_grid_host->browsable_attributes->Add(gcnew CategoryAttribute("Node properties"));
	m_property_grid_host->browsable_attributes->Add(gcnew CategoryAttribute("Link properties"));
	HideOnClose = true;
}

void animation_viewer_properties_panel::on_node_property_changed(System::Object^ sender, value_changed_event_args^ e)
{
	m_owner->on_node_property_changed(sender, e);
}

void animation_viewer_properties_panel::show_properties(Object^ object)
{
	super::show_properties(object);
	if(object!=nullptr)
	{
		m_property_grid_host->data_view->SortDescriptions->Clear();
		m_property_grid_host->data_view->SortDescriptions->Add(SortDescription("DataContext.name", ListSortDirection::Ascending));
		m_property_grid_host->data_view->Refresh();
	}
}
