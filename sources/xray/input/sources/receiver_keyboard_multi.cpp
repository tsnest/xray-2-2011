////////////////////////////////////////////////////////////////////////////
//	Created 	: 21.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "receiver_keyboard.h"
#include <xray/input/handler.h>

using xray::input::receiver::keyboard;
using xray::input::handler;
using xray::input::enum_keyboard;
using xray::input::world;

keyboard::keyboard			( IDirectInput8A& direct_input, HWND const window_handle, world& input_world ) :
	m_window_handle				( window_handle ),
	m_device					( 0 ),
	m_world						( input_world )
{
	XRAY_UNREFERENCED_PARAMETER	( direct_input );
}

keyboard::~keyboard			( )
{
}

void keyboard::on_activate	( )
{
}

void keyboard::on_deactivate( )
{
}

bool keyboard::is_key_down	( char const value ) const
{
	XRAY_UNREFERENCED_PARAMETER	( value );
	return						( false );
}

bool keyboard::is_key_down	( enum_keyboard const key ) const
{
	XRAY_UNREFERENCED_PARAMETER	( key );
	return						( false );
}

void keyboard::execute		( )
{
}