////////////////////////////////////////////////////////////////////////////
//	Created 	: 24.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef INPUT_HANDLER_FREE_FLY_H_INCLUDED
#define INPUT_HANDLER_FREE_FLY_H_INCLUDED

#include <xray/input/handler.h>

namespace stalker2{

class game;

class free_fly_handler :	public xray::input::handler,
							private boost::noncopyable
{
public:
					free_fly_handler	( game& game );
	virtual			~free_fly_handler	( );

	virtual	bool	on_keyboard_action	( input::world& input_world, input::enum_keyboard key, input::enum_keyboard_action action );
	virtual	bool	on_gamepad_action	( input::world& input_world, input::gamepad_button button, input::enum_gamepad_action action );
	virtual	bool	on_mouse_key_action	( input::world& input_world, input::mouse_button button, input::enum_mouse_key_action action );
	virtual	bool	on_mouse_move		( input::world& input_world, int x, int y, int z );
	
	virtual int		input_priority		( ) {return 10;}

			void	tick			( input::world& input_world);

protected:
			void	set_view_matrix	( math::float2 const& raw_angles, float forward, float right, float up );
			bool	keyb_event_present	( int e );
			bool	mouse_event_present	( int e );
protected:
	game&			m_game;
	u32				m_start_time_ms;
	vector<int>		m_keyb_events;
	vector<int>		m_mouse_events;
	int				m_mouse_x;
	int				m_mouse_y;
	int				m_mouse_z;
}; // class world

} // namespace stalker2

#endif // #ifndef INPUT_HANDLER_FREE_FLY_H_INCLUDED
