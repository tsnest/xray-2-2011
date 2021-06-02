////////////////////////////////////////////////////////////////////////////
//	Created		: 28.07.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef HUD_H_INCLUDED
#define HUD_H_INCLUDED

#include "game_camera.h"
#include <xray/input/handler.h>
#include <xray/render/facade/model.h>
#include <xray/animation/animation_player.h>
#include <xray/animation/instant_interpolator.h>
#include "weapon.h"

namespace xray{
namespace physics{
	class bt_character_controller;
	class bt_rigid_body;
}
namespace collision{
	class geometry_instance;

}
}

namespace stalker2{

class game_world;



class hud :		public game_camera,
				public xray::input::handler

{
	typedef game_camera super;
public:
					hud					( game_world& w );
	virtual			~hud				( );
	void			load				( );
	void			unload				( );

	virtual	bool		on_keyboard_action	( input::world* input_world, input::enum_keyboard key, input::enum_keyboard_action action );
	virtual	bool		on_gamepad_action	( input::world* input_world, input::gamepad_button button, input::enum_gamepad_action action );
	virtual	bool		on_mouse_key_action	( input::world* input_world, input::mouse_button button, input::enum_mouse_key_action action );
	virtual	bool		on_mouse_move		( input::world* input_world, int x, int y, int z );
	
	virtual int			input_priority		( ) {return 10;}
	virtual void		tick				( );
	virtual void		on_activate			( camera_director* cd );
	virtual void		on_deactivate		( );
	virtual void		on_focus			( bool b_focus_enter);

	collision::geometry_instance&	get_caracter_capsule( );

protected:
			void		input_tick			( );
			void		camera_move_on_frame( math::float2 const& raw_angles, float forward, float right );
			void		shot				( physics::bt_rigid_body* body, float3 const& ray_dir, float3 const& point );

	camera_director_ptr		m_camera_director;

	struct frame_events
	{
			bool		keyb_event_present	( int e );
			bool		mouse_event_present	( int e );
			bool		empty				( ) const;
			void		reset				( );

		u32						m_last_frame_time_ms;
		u32						m_last_frame_time_delta;
		vector<int>				m_keyb_events;
		vector<int>				m_mouse_events;
		float3					m_mouse_move;

		float					m_onframe_move_fwd;
		float					m_onframe_run_fwd;
		float					m_onframe_move_right;
		float					m_onframe_run_right;
		bool					m_onframe_shift;
		bool					m_onframe_back;
		float					m_onframe_turn_y;
		float					m_onframe_turn_x;
		bool					m_onframe_jump;
		bool					m_onframe_reload;
		bool					m_onframe_shoot;
	} m_frame_events;

private:
	void				on_resources_loaded			( resources::queries_result& data );
	void				add_models_to_scene			( );
	void				remove_models_from_scene	( );

	float4x4			head_matrix					( float4x4* const matrices ) const;
	float4x4			weapon_matrix				( float4x4* const matrices ) const;

	float4x4							m_character_transform;
	float4x4							m_camera_inv_view;
//	render::static_model_ptr			m_weapon_model;
	render::skeleton_model_ptr			m_character_model;
	animation::animation_player*		m_animation_player;

	animation::skeleton_animation_ptr	m_idle_stand_animation;
//	animation::skeleton_animation_ptr	m_crouch_animation;

	animation::skeleton_animation_ptr	m_look_animation_add; //additive


	bool								m_crouch;
	animation::instant_interpolator		m_interpolator;

	timing::timer						m_anim_timer;
	animation::bone_index_type			m_head_bone_idx;
	animation::bone_index_type			m_weapon_bone_idx;
	bool								m_is_active;
	bool								m_model_added;

	xray::physics::bt_character_controller*	m_actor_controller;

	weapon_ptr		m_weapon;
}; // class hud

} // namespace stalker2

#endif // #ifndef HUD_H_INCLUDED
