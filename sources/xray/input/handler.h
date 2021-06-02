////////////////////////////////////////////////////////////////////////////
//	Created 	: 21.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_INPUT_HANDLER_H_INCLUDED
#define XRAY_INPUT_HANDLER_H_INCLUDED

#ifdef __SNC__
#	include <xray/input/gamepad.h>
#	include <xray/input/mouse.h>
#	include <xray/input/keyboard.h>
#endif // #ifdef __SNC__

namespace xray {
namespace input {

struct world;

struct gamepad;
struct keyboard;
struct mouse;

enum gamepad_button;
enum enum_gamepad_action;

enum enum_keyboard;
enum enum_keyboard_action;

enum mouse_button;
enum enum_mouse_key_action;

struct XRAY_NOVTABLE handler {
public:
	virtual	bool	on_keyboard_action	( world* world, enum_keyboard key, enum_keyboard_action action )	= 0;
	virtual	bool	on_gamepad_action	( world* world, gamepad_button button, enum_gamepad_action action )	= 0;
	virtual	bool	on_mouse_key_action	( world* world, mouse_button button, enum_mouse_key_action action )	= 0;
	virtual	bool	on_mouse_move		( world* world, int x, int y, int z )								= 0;
	virtual int		input_priority		( ) = 0;
	virtual void	update				( input::world*  )													{ NODEFAULT(); }

public:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( handler )
}; // class handler

} // namespace input
} // namespace xray

#endif // #ifndef XRAY_INPUT_HANDLER_H_INCLUDED