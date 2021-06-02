////////////////////////////////////////////////////////////////////////////
//	Created		: 18.11.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TEST_GRASPING_CONTROLLER_H_INCLUDED
#define TEST_GRASPING_CONTROLLER_H_INCLUDED

#include "test_grasping_action.h"
#include "controller.h"

namespace xray {
namespace rtp {

class test_grasping_controller :
	public learning_controller< test_grasping_action >
{

public:
						test_grasping_controller();
				void	create_actions			();
//				float	learn_step				();

	virtual		bool	dbg_update_walk				( float dt );
				void	dbg_set_random_target		();

public:
	virtual	void				debug_render		( xray::render::debug::renderer& renderer ) const ;
	virtual	void				render				( xray::render::game::renderer &, xray::render::skeleton_model_ptr& )const {};
	virtual	void				render				( xray::render::editor::renderer &, xray::render::skeleton_model_ptr&  )const {};

public:
	virtual	animation::camera_follower*	get_camera_follower( ){ return 0 ;}

public:
	virtual		void	move_control				( float2 const& move_direction, float2 const& torso_direction, animation::e_gait gait, bool jump ){ XRAY_UNREFERENCED_PARAMETERS(move_direction,torso_direction, gait, jump); }
	virtual		void	dbg_move_control			( float2 const& move_direction, float2 const& torso_direction, bool jump ){ XRAY_UNREFERENCED_PARAMETERS(move_direction,torso_direction, jump); }
	virtual	animation::i_animation_controller*	get_controller( ) {return NULL;};
	
public:
	virtual	render::skeleton_model_ptr	model	( ){ return render::skeleton_model_ptr( 0 ); }	

public:
	virtual		size_t	memory_size				( )	{ return sizeof(*this); }


private:
	virtual		pcstr		type				( ) const { return "test_grasping"; }
	virtual		void		from_world			( const world_space_param_type& w, space_param_type &p )const;
	virtual		void		on_new_walk_action	( world_space_param_type& w, space_param_type	&params );

private:
	float2								m_target_position;
	//test_grasping_world_params			m_position;
	global_actions_params< test_grasping_action > m_position;
	u32									m_current_walk_step;

}; // class test_grasping_controller

} // namespace rtp
} // namespace xray

#endif // #ifndef TEST_GRASPING_CONTROLLER_H_INCLUDED