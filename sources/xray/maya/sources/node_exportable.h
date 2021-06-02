////////////////////////////////////////////////////////////////////////////
//	Created		: 23.06.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef NODE_EXPORTABLE_H_INCLUDED
#define NODE_EXPORTABLE_H_INCLUDED

#include <xray/configs.h>
class node_exportable 
{
public:
		MStatus		export_node		( xray::configs::lua_config_value cfg );
private:
virtual	MStatus		do_export_node		( xray::configs::lua_config_value cfg ) =0;
protected:

private:
	bool	m_processing;
}; // class node_exportable

#endif // #ifndef NODE_EXPORTABLE_H_INCLUDED