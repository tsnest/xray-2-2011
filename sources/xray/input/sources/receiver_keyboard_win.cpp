////////////////////////////////////////////////////////////////////////////
//	Created 	: 21.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "receiver_keyboard.h"
#include "direct_input_include.h"
#include <xray/input/handler.h>

using xray::input::receiver::keyboard;
using xray::input::handler;
using xray::input::enum_keyboard;
using xray::input::world;

keyboard::keyboard			( IDirectInput8A& direct_input, HWND const window_handle, world& input_world ) :
	m_window_handle		( window_handle ),
	m_device			( 0 ),
	m_world				( input_world )
{
	HRESULT	result		= direct_input.CreateDevice(GUID_SysKeyboard, &m_device, 0); 
	R_ASSERT			( !FAILED( result ) );

	result				= m_device->SetDataFormat( &c_dfDIKeyboard );
	R_ASSERT			( !FAILED( result ) );

    result				= 
		m_device->SetCooperativeLevel(
			window_handle, 
			DISCL_FOREGROUND | DISCL_NONEXCLUSIVE
		);
	R_ASSERT			( !FAILED( result ) );

	memory::zero		( &m_current_key_buffer,  sizeof(m_current_key_buffer)  );
	memory::zero		( &m_previous_key_buffer, sizeof(m_previous_key_buffer) );
}

keyboard::~keyboard			( )
{
	m_device->Unacquire	( );
	m_device->Release	( );
}

void keyboard::on_activate	( )
{
	HRESULT	result		= m_device->Acquire	( );
	if(FAILED(result))
		LOG_ERROR("KeyboardDevice Acquire FAILED");
}

void keyboard::on_deactivate( )
{
	m_device->Unacquire	( );
}

bool keyboard::is_key_down	( char const value ) const
{
	return				( !!( value & 0x80 ) );
}

bool keyboard::is_key_down	( enum_keyboard const key ) const
{
	return				( is_key_down( m_current_key_buffer[ key ] ) );
}

void keyboard::execute		( )
{
	memory::copy		( &m_previous_key_buffer, sizeof(m_previous_key_buffer), &m_current_key_buffer, sizeof( m_current_key_buffer ) );
	HRESULT	result		= m_device->GetDeviceState( sizeof( m_current_key_buffer ), &m_current_key_buffer );
	if ( !FAILED( result ) )
		return;

	if(result == DIERR_INPUTLOST)
	{
		LOG_ERROR("DIERR_INPUTLOST");
	}else
	if(result == DIERR_INVALIDPARAM)
	{
		LOG_ERROR("DIERR_INVALIDPARAM");
	}else
	if(result == DIERR_NOTACQUIRED)
	{
		LOG_ERROR("DIERR_NOTACQUIRED");
	}else
	if(result == DIERR_NOTINITIALIZED)
	{
		LOG_ERROR("DIERR_NOTINITIALIZED");
	}else
	if(result == E_PENDING)
	{
		LOG_ERROR("E_PENDING");
	}

	if ( result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED )
	{
		LOG_WARNING			( "keyboard device is lost" );
		result				= m_device->Acquire();
		if ( FAILED( result ) ) {
			LOG_ERROR		( "can't acquire keyboard device" );
			return;
		}

		result				= m_device->GetDeviceState( sizeof( m_current_key_buffer ), &m_current_key_buffer );
		if ( FAILED( result ) )
			LOG_ERROR		( "can't get mouse keyboard" );
	}
}

