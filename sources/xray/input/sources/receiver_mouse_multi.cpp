////////////////////////////////////////////////////////////////////////////
//	Created 	: 21.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "receiver_mouse.h"
#include <xray/input/handler.h>

using xray::input::receiver::mouse;
using xray::input::handler;
using xray::input::world;

mouse::mouse			( IDirectInput8A& direct_input, HWND const window_handle, world& input_world ) 
:	m_window_handle		( window_handle ),
	m_device			( 0 ),
	m_world				( input_world )
{
	XRAY_UNREFERENCED_PARAMETER	( direct_input );
}

mouse::~mouse			( )
{
}

xray::input::mouse::state const& mouse::get_state	( ) const
{
	return				( m_current_state );
}

void mouse::on_activate	( )
{
}

void mouse::on_deactivate ( )
{
}

void mouse::execute		( )
{
}