////////////////////////////////////////////////////////////////////////////
//	Created 	: 21.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "receiver_mouse.h"
#include "direct_input_include.h"
#include <xray/input/handler.h>

using xray::input::receiver::mouse;
using xray::input::handler;
using xray::input::world;

mouse::mouse			( IDirectInput8A& direct_input, HWND const window_handle, world& input_world ) 
:	m_window_handle		( window_handle ),
	m_device			( 0 ),
	m_world				( input_world )
{
	HRESULT	result		= direct_input.CreateDevice(GUID_SysMouse, &m_device, 0); 
	R_ASSERT			( !FAILED( result ) );

	result				= m_device->SetDataFormat( &c_dfDIMouse2 );
	R_ASSERT			( !FAILED( result ) );

    result				= 
		m_device->SetCooperativeLevel(
			window_handle, 
			DISCL_FOREGROUND  /*| DISCL_EXCLUSIVE */| DISCL_NONEXCLUSIVE
		);
	R_ASSERT			( !FAILED( result ) );

	memory::zero		( &m_current_state,  sizeof(m_current_state)  );
	memory::zero		( &m_previous_state, sizeof(m_previous_state) );
}

mouse::~mouse			( )
{
	m_device->Unacquire	( );
	m_device->Release	( );
}

xray::input::mouse::state const& mouse::get_state	( ) const
{
	return				( m_current_state );
}

void mouse::on_activate	( )
{
	m_device->Acquire	( );
}

void mouse::on_deactivate ( )
{
	m_device->Unacquire	( );
}

static void fill_state	( mouse::state& result, DIMOUSESTATE2& state )
{
	result.buttons		= 0;
	for ( u32 i = 0; i < xray::input::mouse_button_count; ++i ) {
		if ( state.rgbButtons[i] )
			result.buttons	|= 1 << i;
	}

	result.x			= state.lX;
	result.y			= state.lY;
	result.z			= state.lZ;
}

void mouse::execute		( )
{
	memory::copy		( &m_previous_state, sizeof(m_previous_state), &m_current_state, sizeof( m_current_state ) );

	DIMOUSESTATE2		state;
	HRESULT	result		= m_device->GetDeviceState( sizeof( state ), &state );
	if ( !FAILED( result ) ) {
		fill_state		( m_current_state, state );
		return;
	}

	if ( ( result != DIERR_NOTACQUIRED ) && ( result != DIERR_INPUTLOST ) )
		return;

	result				= m_device->Acquire();
	if ( FAILED( result ) )
		return;

	result				= m_device->GetDeviceState( sizeof( state ), &state );
	if ( !FAILED( result ) ) {
		fill_state		( m_current_state, state );
		return;
	}

	if ( ( result != DIERR_NOTACQUIRED ) && ( result != DIERR_INPUTLOST ) )
		return;

	LOG_INFO			( "mouse device is lost" );
}