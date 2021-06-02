////////////////////////////////////////////////////////////////////////////
//	Created		: 02.06.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef NAVIGATION_CONTROLLER_H_INCLUDED
#define NAVIGATION_CONTROLLER_H_INCLUDED
#include <xray/animation/i_animation_action.h>
#include <xray/render/facade/model.h>
namespace xray {

namespace animation {
	class i_animation_controller_set;
} // namespace animation

namespace input { 
	struct world; 
} // namespace input

namespace rtp {


template< class base_controller_type >
class navigation_controller :
	public base_controller_type
{
	typedef base_controller_type super;
	typedef typename base_controller_type::space_param_type			space_param_type;
	typedef typename base_controller_type::world_space_param_type	world_space_param_type;

	using super::action_start_world_pos;		
	using super::current_action;			
	using super::m_controller_position;	
	using super::m_debug_wolk_initialized;
	using super::update;					

public:
						navigation_controller	( pcstr name, xray::animation::i_animation_controller_set *set );
						navigation_controller	( pcstr name, animation::world &w, pcstr animation_set_path );
						navigation_controller	( pcstr name, animation::world &w, pcstr animation_set_path, const xray::configs::binary_config_ptr &cfg );
						navigation_controller	( pcstr name, animation::world &w, const xray::configs::binary_config_ptr &cfg );
	virtual				~navigation_controller	( );

	virtual		void	debug_render( 
									render::scene_ptr const& scene,
									render::scene_renderer& scene_renderer,
									render::debug::renderer& debug_renderer
						) const;

public:
	virtual		size_t	memory_size				( )	{ return sizeof(*this); }

public:
	virtual		bool	dbg_update_walk			( float td );

	virtual		void	dbg_move_control		( float4x4 &view_inverted,  xray::input::world& input_world  );
	virtual		void	dbg_move_control		( float2 const& move_direction, float2 const& torso_direction, bool jump );
	virtual		void	move_control			( float2 const& move_direction, float2 const& torso_direction, animation::e_gait gait, bool jump );


#ifndef MASTER_GOLD
	virtual	void		save					( xray::configs::lua_config_value cfg, bool training_trees )const;
#endif // MASTER_GOLD

	virtual	void		load					( const xray::configs::binary_config_value &cfg );

private:
	virtual		pcstr	type					( )const { return "navigation"; }

private:
	virtual		void	from_world				( const world_space_param_type& w, space_param_type &p )const;
	virtual		void	on_new_walk_action		( world_space_param_type& w, space_param_type	&params );
	virtual		void	on_attach				( );

private:
		
#ifndef MASTER_GOLD
	void				save_settings			( xray::configs::lua_config_value cfg )const;
#endif // MASTER_GOLD

	void				load_settings			( const xray::configs::binary_config_value &cfg );

private:
	float2				m_control_path;
	float2				m_control_view_dir;
	animation::e_gait	m_control_gait;
	bool				m_control_jump;
}; // class navigation_controller

} // namespace rtp
} // namespace xray

#endif // #ifndef NAVIGATION_CONTROLLER_H_INCLUDED