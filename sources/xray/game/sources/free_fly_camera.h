////////////////////////////////////////////////////////////////////////////
//	Created 	: 24.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef FREE_FLY_CAMERA_H_INCLUDED
#define FREE_FLY_CAMERA_H_INCLUDED

#include "game_camera.h"
#include <xray/input/handler.h>

namespace stalker2{

class logic_action;
class game_world;
class camera_director;

class free_fly_camera :		public game_camera,
							public xray::input::handler
{
	typedef	game_camera super;
public:
						free_fly_camera	( game_world& w );

	virtual	bool		on_keyboard_action	( input::world* input_world, input::enum_keyboard key, input::enum_keyboard_action action );
	virtual	bool		on_gamepad_action	( input::world* input_world, input::gamepad_button button, input::enum_gamepad_action action );
	virtual	bool		on_mouse_key_action	( input::world* input_world, input::mouse_button button, input::enum_mouse_key_action action );
	virtual	bool		on_mouse_move		( input::world* input_world, int x, int y, int z );
	
	virtual int			input_priority		( ) {return 10;}
	virtual void		tick				( );
	virtual void		on_activate			( camera_director* cd );
	virtual void		on_focus			( bool b_focus_enter);

protected:
			void		build_view_matrix	( math::float2 const& raw_angles, float forward, float right, float up );

			bool		keyb_event_present	( int e );
			bool		mouse_event_present	( int e );

protected:
	camera_director_ptr		m_camera_director;
	u32						m_start_time_ms;
	vector<int>				m_keyb_events;
	vector<int>				m_mouse_events;
	float3					m_mouse_move;
};

class game_world;

} // namespace stalker2

#endif // #ifndef FREE_FLY_CAMERA_H_INCLUDED
