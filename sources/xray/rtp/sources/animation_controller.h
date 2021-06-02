////////////////////////////////////////////////////////////////////////////
//	Created		: 02.06.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_CONTROLLER_H_INCLUDED
#define ANIMATION_CONTROLLER_H_INCLUDED

#include "controller.h"
#include <xray/animation/camera_follower.h>

namespace xray {

namespace animation {
class	i_animation_controller_set;
class	i_animation_controller;
class	i_animation_action;
struct	world;
} // namespace animation

namespace render {
namespace editor {
	class renderer;
} // namespace editor

namespace game {
	class renderer;
} // namespace game

} // namespace render


namespace rtp {


template < class controller_type >
class animation_controller:
	public controller_type,
	public animation::camera_follower_interface
{

typedef controller_type super;

typedef typename controller_type::action_type action_type;

public:
		animation_controller ( pcstr name, xray::animation::i_animation_controller_set *set );
		animation_controller ( pcstr name, animation::world &w, pcstr animation_set_path );

		animation_controller ( pcstr name, animation::world &w, pcstr animation_set_path, const xray::configs::binary_config_ptr &cfg );
		animation_controller ( pcstr name, animation::world &w, const xray::configs::binary_config_ptr &cfg );

virtual	~animation_controller();

public:
	virtual		bool	dbg_update_walk			( float td );

	virtual		void	debug_render				(
							render::scene_ptr const& scene,
							render::scene_renderer& scene_renderer,
							render::debug::renderer& debug_renderer
						) const;
	virtual		void	render						(
							render::scene_ptr const& scene,
							render::scene_renderer& scene_renderer,
							render::debug::renderer& debug_renderer,
							render::skeleton_model_ptr const& model,
							bool render_debug_info
						) const
	{
		render_impl ( scene, scene_renderer, debug_renderer, model, render_debug_info );
	}

	virtual		void	move_control				( float2 const& move_direction, float2 const& torso_direction, animation::e_gait gait, bool jump ){ XRAY_UNREFERENCED_PARAMETERS( &move_direction, &torso_direction, &gait, jump ); }
	virtual		void	dbg_move_control			( float2 const& move_direction, float2 const& torso_direction, bool jump ){ XRAY_UNREFERENCED_PARAMETERS( &move_direction, &torso_direction, jump ); }
	virtual	animation::i_animation_controller*	get_controller( );

public:
	virtual	animation::camera_follower*	get_camera_follower()	{ return &m_dbg_camera_follower; }

private:
				void	render_impl					(
							render::scene_ptr const& scene,
							render::scene_renderer& scene_renderer,
							render::debug::renderer& debug_renderer,
							render::skeleton_model_ptr const& model,
							bool render_debug_info
						) const;

private:
	virtual		float4x4 get_object_matrix_for_camera( ) const;

public:
	virtual			render::skeleton_model_ptr	model( ) ;

public:
#ifndef	MASTER_GOLD
	virtual	void		save					( xray::configs::lua_config_value cfg, bool training_trees )const;
#endif	// MASTER_GOLD

	virtual	void		load					( const xray::configs::binary_config_value &cfg );
	virtual	void		learn_init				( );
	virtual		bool	is_loaded				( ){ return m_loaded; }			

public:
				void	create_actions			( );

private:
				void	cached_data_load( );

private:
				void	on_load_set_controller_create	( );
				void	on_load_set_controller_load		( );

				void	on_action_cached_data_loaded	( );
				void	actions_config_load				( );

protected:
	enum {
		start_action_id	= 4,
	};

protected:
	animation::camera_follower					m_dbg_camera_follower;

protected:
	typename action_type::world_space_param_type m_controller_position;
	animation::i_animation_controller			*m_animation_controller;

protected:
	animation::world							*m_animation_world;
	const configs::binary_config_ptr			m_config;
	bool										m_debug_wolk_initialized;
	bool										m_loaded;
}; // class animation_controller

} // namespace rtp
} // namespace xray

#include "animation_controller_inline.h"

#endif // #ifndef ANIMATION_CONTROLLER_H_INCLUDED
