////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2009
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "command_change_node_properties.h"
#include "dialog_hypergraph.h"
#include "dialog_graph_node_base.h"

using xray::dialog_editor::command_change_node_properties;
using System::Reflection::BindingFlags;
using xray::dialog_editor::dialog_hypergraph;

command_change_node_properties::command_change_node_properties(
	dialog_hypergraph^ h, 
	xray::dialog_editor::dialog_graph_node_base^ n, 
	System::String^ property_name, 
	System::Object^ new_v, 
	System::Object^ old_v
)
:m_hypergraph(h),
m_node_id(n->id),
m_changed_property_name(property_name),
m_new_value(new_v),
m_old_value(old_v)
{}

bool command_change_node_properties::commit()
{
	dialog_graph_node_base^ n = dialog_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), m_node_id);
	n->GetType()->GetProperty(m_changed_property_name)->SetValue(n, m_new_value, nullptr);
	n->Invalidate(false);
	return true;
}

void command_change_node_properties::rollback()
{
	dialog_graph_node_base^ n = dialog_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), m_node_id);
	n->GetType()->GetProperty(m_changed_property_name)->SetValue(n, m_old_value, nullptr);
	n->Invalidate(false);
}