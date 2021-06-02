////////////////////////////////////////////////////////////////////////////
//	Created		: 07.09.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_node_connection_editor.h"
#include "viewer_document.h"

using xray::animation_editor::animation_node_connection_editor;
using xray::animation_editor::viewer_document;
using xray::editor::controls::hypergraph::node;

void animation_node_connection_editor::execute(node^ source, node^ destination)
{
	m_document->add_connection(source, destination);
}
