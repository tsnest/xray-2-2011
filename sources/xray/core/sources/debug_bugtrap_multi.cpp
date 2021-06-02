////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "debug_bugtrap.h"
#include "debug.h"
#include <xray/core/core.h>

bool xray::core::debug::bugtrap::initialized	( )
{
	return		( true );
}

void xray::core::debug::bugtrap::initialize		( )
{
}

void xray::core::debug::bugtrap::finalize		( )
{
}

bool xray::core::debug::bugtrap::on_thread_spawn( )
{
	return		( true );
}

void xray::core::debug::bugtrap::add_file		( pcstr full_path_file_name )
{
	XRAY_UNREFERENCED_PARAMETER	( full_path_file_name );
}

void xray::core::debug::bugtrap::change_usage	( error_mode error_mode, bugtrap_usage bugtrap_usage )
{
	XRAY_UNREFERENCED_PARAMETERS( error_mode, bugtrap_usage );
}

void xray::core::debug::on_error				( pcstr message )
{
	XRAY_UNREFERENCED_PARAMETER	( message );
}