////////////////////////////////////////////////////////////////////////////
//	Created		: 22.04.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RTP_ANIMATION_GRASPING_CONTROLLER_H_INCLUDED
#define XRAY_RTP_ANIMATION_GRASPING_CONTROLLER_H_INCLUDED

#include "animation_controller.h"
#include "animation_grasping_action.h"

namespace xray {
namespace rtp {

class animation_grasping_controller:
	public animation_controller< learning_controller< animation_grasping_action >  > 
{
	typedef animation_controller< learning_controller< animation_grasping_action > > super;

public:
	animation_grasping_controller ( pcstr name, xray::animation::i_animation_controller_set *set );
	animation_grasping_controller ( pcstr name, animation::world &w, pcstr animation_set_path );
	animation_grasping_controller ( pcstr name, animation::world &w, const xray::configs::binary_config_ptr &cfg );

public:
	virtual		size_t	memory_size				( )	{ return sizeof(*this); }

public:
	virtual		bool	dbg_update_walk			( float td );

public:
	virtual	void		debug_render			(
							render::scene_ptr const& scene,
							render::scene_renderer& scene_renderer,
							render::debug::renderer& debug_renderer
						) const;

private:
	virtual		pcstr	type					( ) const { return "animation_grasping"; }
	virtual		void	on_attach				( ) { }

private:
	virtual		void	from_world				( const world_space_param_type& w, space_param_type &p )const;

private:
				void	dbg_set_random_target	();

private:

}; // class animation_grasping_controller

} // namespace rtp
} // namespace xray

#endif // #ifndef XRAY_RTP_ANIMATION_GRASPING_CONTROLLER_H_INCLUDED