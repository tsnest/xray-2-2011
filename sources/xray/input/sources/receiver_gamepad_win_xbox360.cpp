////////////////////////////////////////////////////////////////////////////
//	Created 	: 21.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "receiver_gamepad.h"
#include <xray/input/handler.h>

#if XRAY_PLATFORM_WINDOWS
#	pragma comment( lib, "xinput.lib")
#endif // #if XRAY_PLATFORM_WINDOWS

using xray::input::receiver::gamepad;
using xray::input::handler;
using xray::input::world;
using xray::math::float2;
using xray::input::gamepad_vibrators;

gamepad::gamepad			( u32 const user_index, world& input_world ) :
	m_user_index	( user_index ),
	m_connected		( false ),
	m_inserted		( false ),
	m_removed		( false ),
	m_world			( input_world )
{
#ifdef _MSC_VER
	memory::zero	( &m_device_capabilities, sizeof( m_device_capabilities ) );
	memory::zero	( &m_current_vibration, sizeof( m_current_vibration ) );
#endif // #ifdef _MSC_VER

	memory::zero	( &m_current_state, sizeof( m_current_state ) );
	memory::zero	( &m_previous_state, sizeof( m_previous_state ) );
}

gamepad::~gamepad			( )
{
}

xray::input::gamepad::state const& gamepad::get_state	( ) const
{
	return			( m_current_state );
}

void gamepad::on_activate	( )
{
}

void gamepad::on_deactivate	( )
{
}

static float convert_stick_value	( s16 const value, u16 const dead_zone )
{
    if( value > +dead_zone )
        return		( value - dead_zone ) / ( 32767.f - dead_zone );

    if( value < -dead_zone )
        return		( value + dead_zone + 1.f ) / ( 32767.f - dead_zone );

	return			( 0.f );
}

static float2 convert_stick_values	( s16 const x, s16 const y, u16 const dead_zone )
{
	return			( float2( convert_stick_value( x, dead_zone ), convert_stick_value( y, dead_zone ) ) );
}

void gamepad::execute		( )
{
	if ( !m_connected )
		return;

	XINPUT_STATE	input_state;
	bool const		was_connected = m_connected;
	m_connected		= XInputGetState( m_user_index, &input_state ) == ERROR_SUCCESS;
	m_removed		= was_connected && !m_connected;
	m_inserted		= !was_connected && m_connected;
	if ( !m_connected )
		return;

	if ( m_inserted ) {
		ASSERT					( m_connected );
		memory::zero			( &m_current_state, sizeof( m_current_state ) );
		XInputGetCapabilities	( m_user_index, XINPUT_FLAG_GAMEPAD, &m_device_capabilities );

		memory::copy			( &m_current_vibration, sizeof(m_current_vibration), &m_device_capabilities.Vibration, sizeof( m_device_capabilities.Vibration )  );
		XInputSetState			( m_user_index, &m_current_vibration );
	}

	memory::copy	( &m_previous_state, sizeof(m_previous_state), &m_current_state, sizeof( m_current_state ) );
	
	m_current_state.buttons				= (gamepad_button)input_state.Gamepad.wButtons;
	m_current_state.left_trigger		= float( input_state.Gamepad.bLeftTrigger )/255.f;
	m_current_state.right_trigger		= float( input_state.Gamepad.bRightTrigger )/255.f;
	m_current_state.left_thumb_stick	= convert_stick_values( input_state.Gamepad.sThumbLX, input_state.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE );
	m_current_state.right_thumb_stick	= convert_stick_values( input_state.Gamepad.sThumbRX, input_state.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE );
}

void gamepad::process		( handlers_type& handlers )
{
	if ( !m_inserted )
		return;

	handlers_type::iterator it	= handlers.begin();
	handlers_type::iterator it_e = handlers.end();

	u16 const changed_buttons	= u16( m_current_state.buttons ^ m_previous_state.buttons );

	u16 const buttons_down		= u16( m_current_state.buttons & changed_buttons );
	for ( u16 i = buttons_down; i; i &= i - 1 )
		for(; it!=it_e; ++it)
			if( (*it)->on_gamepad_action( &m_world, gamepad_button( i ^ ( i & ( i - 1 ) ) ), gp_key_down ) )
				break;

	it							= handlers.begin();
	u16 const buttons_up		= u16( ~m_current_state.buttons & changed_buttons );
	for ( u16 i = buttons_up; i; i &= i - 1 )
		if( (*it)->on_gamepad_action( &m_world, gamepad_button( i ^ ( i & ( i - 1 ) ) ), gp_key_up ) )
				break;

	it							= handlers.begin();
	u16 const hold_buttons		= u16( m_current_state.buttons & m_previous_state.buttons );
	for ( u16 i = hold_buttons; i; i &= i - 1 )
		if( (*it)->on_gamepad_action( &m_world, gamepad_button( i ^ ( i & ( i - 1 ) ) ), gp_key_hold ) )
				break;
}

void gamepad::set_vibration		( gamepad_vibrators const vibrator, float value_raw )
{
	if ( !m_inserted )
		return;

	u16 const value				= (u16)math::clamp_r( math::floor( value_raw*65535.f ), 0, 65535 );

	if ( vibrator == gamepad_vibrator_left )
		m_current_vibration.wLeftMotorSpeed		= value;
	else {
		ASSERT									( vibrator == gamepad_vibrator_right );
		m_current_vibration.wRightMotorSpeed	= value;
	}

    u32 const result			= XInputSetState( m_user_index, &m_current_vibration );
	if ( result == ERROR_SUCCESS )
		return;

//	ASSERT						( result == ERROR_DEVICE_NOT_CONNECTED );
}

float gamepad::get_vibration	( gamepad_vibrators const vibrator ) const
{
	if ( vibrator == gamepad_vibrator_left )
		return					( float( m_current_vibration.wLeftMotorSpeed )/65535.f );

	ASSERT						( vibrator == gamepad_vibrator_right );
	return						( float( m_current_vibration.wRightMotorSpeed )/65535.f );
}