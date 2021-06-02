////////////////////////////////////////////////////////////////////////////
//	Created		: 25.03.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_graph_link.h"
#include "dialog_link.h"
#include "dialog_graph_node_base.h"
#include "dialog_node_base.h"
#include "dialog_hypergraph.h"

using xray::dialog_editor::dialog_graph_link;
using xray::dialog_editor::dialog_link;
using xray::dialog_editor::dialog_node_base;
using xray::dialog_editor::dialog_link_action;
using xray::dialog_editor::dialog_link_precondition;
using xray::dialog_editor::dialog_hypergraph;
using namespace System::Collections;

dialog_graph_link::dialog_graph_link(connection_point^ src, connection_point^ dst, bool create_dialog_link)
:super(src, dst)
{
	if(create_dialog_link)
		m_link = safe_cast<dialog_graph_node_base^>(src->owner)->object->new_link(safe_cast<dialog_graph_node_base^>(dst->owner)->object);
}

dialog_graph_link::~dialog_graph_link()
{
	safe_cast<dialog_graph_node_base^>(source_node)->object->remove_link(m_link);
	DELETE(m_link);
}

void dialog_graph_link::set_preds_and_acts(dialog_link_precondition^ prec_root, Generic::List<dialog_link_action^>^ act_list)
{
	safe_cast<dialog_hypergraph^>(source_node->get_parent())->set_preconditions_and_actions(this, prec_root, act_list);
}