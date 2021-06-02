////////////////////////////////////////////////////////////////////////////
//	Created		: 23.06.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "node_exportable.h"

MStatus	node_exportable::export_node( xray::configs::lua_config_value  cfg )
{
	m_processing = true;
	MStatus stat = do_export_node( cfg );
	m_processing = false;
	return stat;
}