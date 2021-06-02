////////////////////////////////////////////////////////////////////////////
//	Created		: 24.05.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef CAMERA_BEHAVIOUR_H_INCLUDED
#define CAMERA_BEHAVIOUR_H_INCLUDED

#include "object_behaviour.h"
#include <xray/input/handler.h>

namespace stalker2 {
class object_track;
class game_camera;

class camera_follow_by_track_behaviour : public object_behaviour
{
	typedef object_behaviour super;

public:
						camera_follow_by_track_behaviour	( object_scene_job* owner, configs::binary_config_value const& data, pcstr name );
	virtual				~camera_follow_by_track_behaviour	( ) { }
	virtual void		attach_to_object					( object_controlled* o );
	virtual void		detach_from_object					( object_controlled* o );
	virtual void		tick								( );
private:
	game_camera*		m_camera;
	object_track*		m_track;
	float				m_track_length;
	timing::timer		m_timer;
	bool				m_bcyclic;
	bool				m_animation_end_raised;
};

class camera_free_fly_behaviour :	public object_behaviour,
									public xray::input::handler
{
	typedef object_behaviour super;

public:
						camera_free_fly_behaviour	( object_scene_job* owner, configs::binary_config_value const& data, pcstr name );
	virtual				~camera_free_fly_behaviour	( );
	//behaviour
	virtual void		attach_to_object			( object_controlled* o );
	virtual void		detach_from_object			( object_controlled* o );
	virtual void		tick						( );

	// input
	virtual	bool		on_keyboard_action	( input::world* input_world, input::enum_keyboard key, input::enum_keyboard_action action );
	virtual	bool		on_gamepad_action	( input::world* input_world, input::gamepad_button button, input::enum_gamepad_action action );
	virtual	bool		on_mouse_key_action	( input::world* input_world, input::mouse_button button, input::enum_mouse_key_action action );
	virtual	bool		on_mouse_move		( input::world* input_world, int x, int y, int z );
	virtual int			input_priority		( ) {return 10;}

protected:
			void		build_view_matrix	( math::float2 const& raw_angles, float forward, float right, float up );

			bool		keyb_event_present	( int e );
			bool		mouse_event_present	( int e );

private:
	game_camera*		m_camera;
	u32					m_start_time_ms;
	vector<int>			m_keyb_events;
	vector<int>			m_mouse_events;
	float3				m_mouse_move;
};

class camera_director_behaviour : public object_behaviour
{
	typedef object_behaviour super;

public:
						camera_director_behaviour	( object_scene_job* owner, configs::binary_config_value const& data, pcstr name );
	virtual				~camera_director_behaviour	( ) { }
	virtual void		attach_to_object			( object_controlled* o );
	virtual void		detach_from_object			( object_controlled* o );
};


} // namespace xray

#endif // #ifndef CAMERA_BEHAVIOUR_H_INCLUDED