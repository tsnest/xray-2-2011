////////////////////////////////////////////////////////////////////////////
//	Created		: 13.05.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "receiver_mouse.h"
#include <xray/input/handler.h>

using xray::input::receiver::mouse;

void mouse::process		( handlers_type& handlers )
{
	handlers_type::iterator it	= handlers.begin();
	handlers_type::iterator it_e = handlers.end();

	u16 const changed_buttons	= u16( m_current_state.buttons ^ m_previous_state.buttons );

	u16 const buttons_down		= u16( m_current_state.buttons & changed_buttons );
	for ( u16 i = buttons_down; i; i &= i - 1 )
		for(; it!=it_e; ++it)
			if( (*it)->on_mouse_key_action( &m_world, mouse_button( i ^ ( i & ( i - 1 ) ) ), ms_key_down ) )
				break;

	it							= handlers.begin();
	u16 const buttons_up		= u16( ~m_current_state.buttons & changed_buttons );
	for ( u16 i = buttons_up; i; i &= i - 1 )
		for(; it!=it_e; ++it)
			if( (*it)->on_mouse_key_action( &m_world, mouse_button( i ^ ( i & ( i - 1 ) ) ), ms_key_up ) )
				break;

	it							= handlers.begin();
	u16 const hold_buttons		= u16( m_current_state.buttons & m_previous_state.buttons );
	for ( u16 i = hold_buttons; i; i &= i - 1 )
		for(; it!=it_e; ++it)
			if( (*it)->on_mouse_key_action( &m_world, mouse_button( i ^ ( i & ( i - 1 ) ) ), ms_key_hold ) )
				break;

	it							= handlers.begin();

	if( m_current_state.x || m_current_state.y || m_current_state.z)
		for(; it!=it_e; ++it)
			if( (*it)->on_mouse_move( &m_world, m_current_state.x, m_current_state.y, m_current_state.z ) )
				break;
}