////////////////////////////////////////////////////////////////////////////
//	Created 	: 26.08.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "stalker2_pc_application.h"
#include <xray/engine/api.h>

using stalker2::application;

void application::initialize( )
{
	m_exit_code						= 0;

	preinitialize					( );
	xray::engine::initialize		( );
	postinitialize					( );
}

void application::finalize	( )
{
	xray::engine::finalize			( );
}

void application::execute	( )
{
	xray::engine::execute			( );
	m_exit_code						= xray::engine::get_exit_code();
}