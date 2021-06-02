////////////////////////////////////////////////////////////////////////////
//	Created 	: 20.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "input_world.h"
#include "direct_input_include.h"
#include "receiver_gamepad.h"
#include "receiver_mouse.h"
#include "receiver_keyboard.h"
#include <xray/input/handler.h>

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")

using xray::input::input_world;

void input_world::create_devices	( HWND const window_handle )
{
	ASSERT					( !m_direct_input );
	HINSTANCE				hInstance = GetModuleHandle( 0 );
	HRESULT	const			result =
		DirectInput8Create(
			hInstance,
			DIRECTINPUT_VERSION,
			IID_IDirectInput8,
			(pvoid*)&m_direct_input,
			0
		);
	XRAY_UNREFERENCED_PARAMETER	( result );
	R_ASSERT				( !FAILED(result) );
	m_gamepad				= NEW ( receiver::gamepad ) ( 0, *this );
	m_keyboard				= NEW ( receiver::keyboard ) ( *m_direct_input, window_handle, *this );
	m_mouse					= NEW ( receiver::mouse ) ( *m_direct_input, window_handle, *this );
}

void input_world::destroy_devices	( )
{
	XRAY_DELETE_IMPL		( *g_allocator, m_mouse );
	XRAY_DELETE_IMPL		( *g_allocator, m_keyboard );
	XRAY_DELETE_IMPL		( *g_allocator, m_gamepad );
	m_direct_input->Release	( );
}