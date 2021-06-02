////////////////////////////////////////////////////////////////////////////
//	Created		: 22.04.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_grasping_controller.h"

#include <xray/animation/i_animation_controller_set.h>
#include <xray/animation/i_animation_action.h>
#include <xray/render/facade/debug_renderer.h>

namespace xray {
namespace rtp {



animation_grasping_controller::animation_grasping_controller ( pcstr name, xray::animation::i_animation_controller_set *set ):
super( name, set )
{
		dbg_set_random_target	();
}

animation_grasping_controller::animation_grasping_controller( pcstr name, animation::world &w, pcstr animation_set_path ) :
	super					( name, w, animation_set_path )
{
	dbg_set_random_target	( );
}

animation_grasping_controller::animation_grasping_controller( pcstr name, animation::world &w, xray::configs::binary_config_ptr const& cfg ) :
	super					( name, w, cfg )
{
	dbg_set_random_target	( );
}

void animation_grasping_controller::dbg_set_random_target	( )
{
	space_param_type rnd;
	rnd.set_random();
	m_controller_position.target_pos.identity();
	
	float scale_param_area = 0.75f;

	m_controller_position.target_pos.c.xyz() = m_controller_position.controller_pos.c.xyz();
	m_controller_position.target_pos.c.xyz() += float3( rnd.target_position().x * scale_param_area, 0, rnd.target_position().y * scale_param_area );
}



void		animation_grasping_controller::from_world( const world_space_param_type& w, space_param_type &p )const
{
	XRAY_UNREFERENCED_PARAMETERS( &w, &p );	

	float4x4 iw; R_ASSERT( iw.try_invert( w.controller_pos ) );

	float4x4 t = m_controller_position.target_pos * iw;

	p.target_position().x = t.c.x;
	p.target_position().y = t.c.z;

	//transform_to_param( t, p );

}

void		animation_grasping_controller::debug_render(
		render::scene_ptr const& scene,
		render::scene_renderer& scene_renderer,
		render::debug::renderer& debug_renderer
	) const
{

	super::debug_render( scene, scene_renderer, debug_renderer );
	debug_renderer.draw_sphere( scene, m_controller_position.target_pos.c.xyz(), 0.3f, math::color( 255,0,0 ) );
}



bool	animation_grasping_controller::dbg_update_walk			( float dt )
{ 
	
	super::dbg_update_walk( dt );
	
	if( super::value().empty() )
		return false;

	if( update( m_controller_position, dt ) || step() > 20   )
	{
		dbg_set_random_target( );

		walk_init( start_action_id, 0, m_controller_position );
		
		//m_debug_global.m_set->reset();
		LOG_DEBUG( "animation_grasping_controller target!" );
		return true;
	
	}
	
	
	return false; 
}

} // namespace rtp
} // namespace xray