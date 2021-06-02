////////////////////////////////////////////////////////////////////////////
//	Created 	: 20.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_INPUT_WORLD_H_INCLUDED
#define XRAY_INPUT_WORLD_H_INCLUDED

namespace xray {
namespace input {

struct handler;
struct keyboard;
struct mouse;
struct gamepad;

struct XRAY_NOVTABLE world {
public:
	virtual	void		tick			( ) = 0;
	virtual	void		clear_resources	( ) = 0;
	virtual	void		on_activate		( ) = 0;
	virtual	void		on_deactivate	( ) = 0;
	virtual	void		add_handler		( handler& handler ) = 0;
	virtual	void		remove_handler	( handler& handler ) = 0;
	virtual	void		acquire			( ) = 0;
	virtual	void		unacquire		( ) = 0;

public:
	typedef raw<gamepad>::ptr			gamepad_ptr_type;
	typedef raw<keyboard const>::ptr	keyboard_ptr_type;
	typedef raw<mouse const>::ptr		mouse_ptr_type;

public:
	virtual gamepad_ptr_type	get_gamepad	( ) = 0;
	virtual keyboard_ptr_type	get_keyboard( ) = 0;
	virtual mouse_ptr_type		get_mouse	( ) = 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( world )
}; // class world

} // namespace input
} // namespace xray

#endif // #ifndef XRAY_INPUT_WORLD_H_INCLUDED