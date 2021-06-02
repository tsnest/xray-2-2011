////////////////////////////////////////////////////////////////////////////
//	Created		: 20.01.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_node_connection_editor.h"
#include "particle_graph_document.h"

using xray::editor::particle_node_connection_editor;
using xray::editor::particle_graph_document;
using xray::editor::controls::hypergraph::node;

void particle_node_connection_editor::execute(node^ source, node^ destination)
{
	m_document->add_connection(source, destination);
}
