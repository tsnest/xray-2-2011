////////////////////////////////////////////////////////////////////////////
//	Created 	: 21.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_INPUT_CONTROLLER_H_INCLUDED
#define XRAY_INPUT_CONTROLLER_H_INCLUDED

namespace xray {
namespace input {

enum enum_gamepad_action{
	gp_key_down,
	gp_key_up, 
	gp_key_hold
};

enum gamepad_button {
	gamepad_dpad_up			= u16( 0x0001 ),
	gamepad_dpad_down		= u16( 0x0002 ),
	gamepad_dpad_left		= u16( 0x0004 ),
	gamepad_dpad_right		= u16( 0x0008 ),
	gamepad_start			= u16( 0x0010 ),
	gamepad_back			= u16( 0x0020 ),
	gamepad_left_thumb		= u16( 0x0040 ),
	gamepad_right_thumb		= u16( 0x0080 ),
	gamepad_left_shoulder	= u16( 0x0100 ),
	gamepad_right_shoulder	= u16( 0x0200 ),
	gamepad_bigbutton		= u16( 0x0800 ),
	gamepad_a				= u16( 0x1000 ),
	gamepad_b				= u16( 0x2000 ),
	gamepad_x				= u16( 0x4000 ),
	gamepad_y				= u16( 0x8000 ),
}; // enum gamepad_buttons

enum gamepad_vibrators {
	gamepad_vibrator_left,
	gamepad_vibrator_right,
}; // enum gamepad_vibrators

struct XRAY_NOVTABLE gamepad {
public:
	struct state {
		math::float2		left_thumb_stick;
		math::float2		right_thumb_stick;
		float				left_trigger;
		float				right_trigger;
		gamepad_button		buttons;
	}; // struct state

public:
	virtual	state const&	get_state		( ) const = 0;
	virtual	float			get_vibration	( gamepad_vibrators vibrator ) const = 0;
	virtual	void			set_vibration	( gamepad_vibrators vibrator, float value ) = 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( gamepad )
}; // class handler

} // namespace input
} // namespace xray

#endif // #ifndef XRAY_INPUT_CONTROLLER_H_INCLUDED