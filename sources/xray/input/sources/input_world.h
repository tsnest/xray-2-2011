////////////////////////////////////////////////////////////////////////////
//	Created 	: 20.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef INPUT_WORLD_H_INCLUDED
#define INPUT_WORLD_H_INCLUDED

#include <xray/input/world.h>

struct IDirectInput8A;

namespace xray {
namespace input {

struct handler;

namespace receiver {
	class gamepad;
	class keyboard;
	class mouse;
} // namespace receiver

struct engine;

class input_world :
	public input::world,
	private boost::noncopyable
{
public:
							input_world		( input::engine& engine, HWND window_handle );
	virtual					~input_world	( );
	virtual	void			tick			( );
	virtual	void			clear_resources	( );
	virtual	void			on_activate		( );
	virtual	void			on_deactivate	( );
	virtual	void			add_handler		( handler& handler );
	virtual	void			remove_handler	( handler& handler );
	virtual	void			acquire			( );
	virtual	void			unacquire		( );

	virtual gamepad_ptr_type	get_gamepad	( );
	virtual keyboard_ptr_type	get_keyboard( );
	virtual mouse_ptr_type		get_mouse	( );

private:
			void			create_devices	( HWND window_handle );
			void			destroy_devices	( );
			void		process_on_activate	( );
			void	process_on_deactivate	( );

private:
	typedef vector< handler* >	handlers_type;

private:
	enum enum_input_state {
		input_state_acquired	= threading::atomic32_value_type(0),
		input_state_unacquired	= threading::atomic32_value_type(1),
		input_state_processed	= threading::atomic32_value_type(2),
	}; // enum enum_input_state

private:
	handlers_type				m_handlers;
	engine&						m_engine;
	IDirectInput8A*				m_direct_input;
	receiver::gamepad*			m_gamepad;
	receiver::keyboard*			m_keyboard;
	receiver::mouse*			m_mouse;
	threading::atomic32_type	m_target_state;
	bool						m_acquired;
}; // class world

} // namespace input
} // namespace xray

#endif // #ifndef INPUT_WORLD_H_INCLUDED