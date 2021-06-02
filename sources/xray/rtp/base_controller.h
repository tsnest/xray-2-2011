////////////////////////////////////////////////////////////////////////////
//	Created		: 23.04.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BASE_CONTROLLER_H_INCLUDED
#define BASE_CONTROLLER_H_INCLUDED

#include <xray/render/facade/model.h>
#include <xray/animation/i_animation_action.h>
#include <xray/animation/camera_follower.h>

namespace xray {

namespace render {
namespace editor {
	class renderer;
} // namespace editor

namespace game {
	class renderer;
} // namespace game

namespace debug {
	class renderer;
} // namespace debug

class scene_renderer;

} // namespace render

namespace animation {
	struct world;
} // namespace animation

namespace input {
	struct world;
} // namespace input

namespace configs {
	class lua_config_value;
} // namespace configs

namespace rtp {

struct	controller_resource_user_data
{
	controller_resource_user_data( ): empty_set( false ), name( 0 ) {}

	bool							empty_set;
	pstr							name;
};


class base_controller: 
	public resources::unmanaged_resource 
{

public:
	virtual						~base_controller( ){ }


	virtual	float				learn_step			( )												= 0;
	virtual	bool				learn_step			( float stop_residual, u32 stop_iterations )	= 0;
	virtual	void				learn				( )												= 0;
	virtual	void				learn_init			( )												= 0;
	virtual	bool				learn_initialized	( ) const										= 0;
	virtual	void				learn_stop			( )												= 0;
	virtual	void				step_logic			( float stop_residual )							= 0;
	virtual	size_t				memory_size			( )												= 0;
	virtual	void				on_attach			( )												= 0;

public:
	virtual	animation::camera_follower*	get_camera_follower( )										= 0;

public:
virtual	render::skeleton_model_ptr	model				( )											= 0;	

#ifndef MASTER_GOLD
	virtual	void				save_training_sets	( xray::configs::lua_config_value cfg )const=0;
#endif // MASTER_GOLD

	virtual	void				load_training_sets	( const xray::configs::binary_config_value &cfg )=0;

#ifndef MASTER_GOLD
	virtual	void				save				( xray::configs::lua_config_value cfg, bool training_trees )const = 0;
#endif // MASTER_GOLD

	virtual	void				load				( const xray::configs::binary_config_value &cfg )=0;
	virtual bool				is_loaded			( ) = 0;

	virtual	bool				compare_value		( const base_controller& ctrl ) const = 0;

	virtual	pcstr				type				( )const = 0;
	virtual	pcstr				name				( )const = 0;

	virtual	void				render				( render::scene_ptr const& scene, render::scene_renderer& scene_renderer, render::debug::renderer& debug_renderer, xray::render::skeleton_model_ptr const& model, bool render_debug_info ) const = 0;

	virtual	bool				dbg_update_walk		( float dt ) = 0;
	virtual	void				move_control		( float2 const& move_direction, float2 const& torso_direction, animation::e_gait gait, bool jump ) =0;

	virtual	void				dbg_move_control	( float2 const& move_direction, float2 const& torso_direction, bool jump ) =0;
	virtual	void				dbg_move_control	( float4x4 &, input::world&  ) {};
	virtual	animation::i_animation_controller*	get_controller( ) = 0;
	
}; // class base_controller


typedef	resources::resource_ptr<
	base_controller,
	resources::unmanaged_intrusive_base
> animation_controller_ptr;


} // namespace rtp
} // namespace xray

#endif // #ifndef BASE_CONTROLLER_H_INCLUDED