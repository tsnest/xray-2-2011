////////////////////////////////////////////////////////////////////////////
//	Created 	: 20.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "input_world.h"
#include "receiver_gamepad.h"
#include "receiver_keyboard.h"
#include "receiver_mouse.h"
#include <xray/input/handler.h>

using xray::input::input_world;
using xray::input::handler;
using xray::input::keyboard;
using xray::input::mouse;
using xray::input::gamepad;

input_world::input_world	( xray::input::engine& engine, HWND const window_handle ) :
	m_engine		( engine ),
	m_direct_input	( 0 ),
	m_gamepad		( 0 ),
	m_keyboard		( 0 ),
	m_mouse			( 0 ),
	m_target_state	( input_state_processed ),
	m_acquired		( false )
{
	create_devices	( window_handle );
}

input_world::~input_world	( )
{
	destroy_devices	( );
}

void input_world::tick		( )
{
	if ( !m_acquired )		return;

	if ( m_target_state != input_state_processed ) {

		if( m_target_state == input_state_acquired )
			process_on_activate		( );
		else
			process_on_deactivate	( );

		threading::interlocked_exchange( (threading::atomic32_type&)m_target_state, input_state_processed );
	}

	if ( m_handlers.empty( ) )
		return;

	if ( m_gamepad )
		m_gamepad->execute		( );

	if ( m_keyboard )
		m_keyboard->execute		( );

	if ( m_mouse )
		m_mouse->execute		( );

	if ( m_gamepad )
		m_gamepad->process		( m_handlers );

	if ( m_keyboard )
		m_keyboard->process		( m_handlers );

	if ( m_mouse )
		m_mouse->process		( m_handlers );
}

void input_world::clear_resources		( )
{
}

void input_world::process_on_deactivate	( )
{
	if ( m_gamepad )
		m_gamepad->on_deactivate	( );

	if ( m_keyboard )
		m_keyboard->on_deactivate	( );

	if ( m_mouse )
		m_mouse->on_deactivate		( );
}

void input_world::process_on_activate	( )
{
	if ( m_gamepad )
		m_gamepad->on_activate	( );

	if ( m_keyboard )
		m_keyboard->on_activate	( );

	if ( m_mouse )
		m_mouse->on_activate	( );

}

void input_world::on_activate	( )
{
	threading::interlocked_exchange( m_target_state, input_state_acquired );
}

void input_world::on_deactivate	( )
{
	threading::interlocked_exchange( m_target_state, input_state_unacquired );
}

struct handler_prio_less
{
	bool operator () (xray::input::handler* handler, int prio)
	{
		return handler->input_priority() < prio;
	}
};

void input_world::add_handler( xray::input::handler& handler )
{
	R_ASSERT			(
		std::find( m_handlers.begin( ), m_handlers.end( ), &handler ) == 
		m_handlers.end( )
	);

	int prio				= handler.input_priority();
	handler_prio_less		p;
	handlers_type::iterator it	= std::lower_bound(m_handlers.begin(), m_handlers.end(), prio, p);
	m_handlers.insert		(it, &handler);
}

void input_world::remove_handler	( xray::input::handler& handler )
{
	handlers_type::iterator it	= std::find( m_handlers.begin(), m_handlers.end(), &handler );
	ASSERT					( it!=m_handlers.end() );
	m_handlers.erase		( it );
}

void input_world::acquire				( )
{
	if( m_acquired )	return;

	m_acquired			= true;
	on_activate			( );
}

void input_world::unacquire			( )
{
	if( !m_acquired )	return;

	m_acquired			= false;
	process_on_deactivate( );
}

input_world::gamepad_ptr_type input_world::get_gamepad		( )
{
	return m_gamepad; 
}

input_world::keyboard_ptr_type input_world::get_keyboard	( )
{
	return m_keyboard; 
}

input_world::mouse_ptr_type input_world::get_mouse			( )
{
	return m_mouse; 
}