////////////////////////////////////////////////////////////////////////////
//	Created		: 13.05.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "receiver_gamepad.h"
#include <xray/input/handler.h>

using xray::input::receiver::gamepad;

gamepad::gamepad				( u32 const user_index, world& input_world ) :
	m_user_index	( user_index ),
	m_connected		( false ),
	m_inserted		( false ),
	m_removed		( false ),
	m_world			( input_world )
{
}

gamepad::~gamepad				( )
{
}

xray::input::gamepad::state const& gamepad::get_state	( ) const
{
	return			( m_current_state );
}

float gamepad::get_vibration	( gamepad_vibrators vibrator ) const
{
	return			0.f;
}

void gamepad::set_vibration		( gamepad_vibrators vibrator, float value )
{
}

void gamepad::on_activate		( )
{
}

void gamepad::on_deactivate		( )
{
}

void gamepad::execute			( )
{
}

void gamepad::process			( handlers_type& handlers )
{
}