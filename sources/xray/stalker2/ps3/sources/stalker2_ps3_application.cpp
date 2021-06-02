////////////////////////////////////////////////////////////////////////////
//	Created 	: 26.08.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "stalker2_ps3_application.h"
#include <xray/engine/api.h>

using stalker2::application;

void application::initialize( pcstr const command_line )
{
	m_exit_code					= 0;
	xray::engine::preinitialize	( m_game_proxy, command_line, "stalker2", __DATE__ );
	xray::engine::initialize	( );
}

void application::finalize	( )
{
	xray::engine::finalize		( );
}

void application::execute	( )
{
	xray::engine::execute		( );
}