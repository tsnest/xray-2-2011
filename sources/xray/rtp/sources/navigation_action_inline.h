////////////////////////////////////////////////////////////////////////////
//	Created		: 01.11.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef NAVIGATION_ACTION_INLINE_H_INCLUDED
#define NAVIGATION_ACTION_INLINE_H_INCLUDED

namespace xray {
namespace rtp {
	extern float g_strafe_discount;
	extern float g_ro_factor;
	extern float g_delta_theta_factor;
	extern float g_theta_factor;
	extern float g_gait_state_factor;
	extern float g_ro_theta_factor;
	extern float g_torso_factor;
	
	extern float g_gait_state_factor;
	extern float g_gait_self_transition_reward_factor;
	extern float g_gait_transition_reward_factor;

	extern float g_ro_transition_discount_factor;
	extern float g_theta_transition_discount_factor;
	extern float g_tau_transition_discount_factor;
	extern bool	 g_disable_theta;
	extern bool	 g_disable_ro;
	extern bool	 g_disable_tau;

	static const float	angle_out_of_range_forbid_reward = -10000000;

	inline	navigation_theta_action::motion_angles	navigation_theta_action::angles( u32 blend, navigation_theta_action const* from_action,  u32 from_blend ) const
	{
		u32 from_action_id = u32( -1 ); // from_action->id()
		if( from_action )
			from_action_id = from_action->id();

		u32 id_chached = cached_transform_id(  blend, from_action_id, from_blend );
		return m_cache_angles[ id_chached ];
	}

	inline		bool	navigation_theta_action::is_in_range( const navigation_theta_params& params )const
	{
		
		float view_motion  =  math::angle_normalize_signed( params.tau() - params.theta_ro() );

		return	view_motion >= m_view_motion_angle_min	&&	view_motion	<= m_view_motion_angle_max;
	}

	inline		void	navigation_theta_action::run	( navigation_theta_params& from, const navigation_theta_action& af, u32 from_sample, u32 blend_sample  ) const
	{
		XRAY_UNREFERENCED_PARAMETERS(  &af, &from_sample, &blend_sample );


		motion_angles const l_motion_angles = angles(  blend_sample, &af, from_sample );

		motion_angles const l_previous_motion_angles = af.angles( from_sample, &af, from_sample );
		//motion_angles const l_motion_angles = angles(  blend_sample, af, from_sample );


		ASSERT( math::valid( l_motion_angles.delta_theta ) );
		ASSERT( math::valid( l_motion_angles.ro ) );
		ASSERT( math::valid( l_motion_angles.tau ) );
#if 0
		if( m_cache_angles[ id_chached ] == math::infinity )
		{
			float4x4 transform =  calculate_transform( af, from_sample, blend_sample );
			float angle = math::angle_normalize_signed( transform.get_angles_xyz().y );

			m_cache_angles[ id_chached ] = angle;
		}
#endif

		float const delta_theta = !g_disable_theta	? ( l_motion_angles.delta_theta	)						: 0;
		float const delta_ro	= !g_disable_ro		? ( l_motion_angles.ro - l_previous_motion_angles.ro )	: 0;
		float const delta_tau	= !g_disable_tau	? ( l_motion_angles.tau - l_previous_motion_angles.tau ): 0;

		//float inital_tau = from.tau();

		//float result_tau = inital_tau +  delta_theta + delta_tau;

		//float ratio = math::abs ( inital_tau )/( math::abs ( result_tau ) + 0.1f ) / ( math::abs ( delta_tau ) + 0.1 );
		//
		//math::random32 rnd32( math::floor( from.tau() + from.theta_ro() + delta_theta + delta_ro  ) );
 		//float rnd = (float) random( rnd32 ) * 0.01f * ratio;
		
		from.theta_ro()	+= delta_theta + delta_ro;
		from.tau()		+= delta_theta + delta_tau;
		//from.tau()		+= result_tau + rnd;

#ifdef	DEBUG
		float theta = math::angle_normalize_signed( from.theta_ro() );
		float tau = math::angle_normalize_signed( from.tau() );
		float theta_reward = - math::abs( theta );
		float tau_reward = - math::abs( tau );
		XRAY_UNREFERENCED_PARAMETERS(  theta_reward, tau_reward );
#endif	
	}
	
	inline	navigation_theta_action::separate_reward_type		navigation_theta_action::reward		( const navigation_theta_params& p, u32 blend ) const
	{
		XRAY_UNREFERENCED_PARAMETERS(  blend );
		
		//if ( !is_in_range( p ) )
			//return separate_reward_type().set( angle_out_of_range_forbid_reward );

		if ( gait() == animation::gait_walk_jump )
			 return separate_reward_type( ).set( jump_reward( p ) );
		
		motion_angles const l_motion_angles = angles(  blend,(p.training_data().previos_action), p.training_data().previous_blend_id );
		float ro			=	l_motion_angles.ro;
		float delta_theta	=	l_motion_angles.delta_theta;
		//float tau			=	l_motion_angles.tau;

		//float theta_factor			= g_theta_factor;
		float ro_factor				= g_ro_factor;
		float delta_theta_factor	= g_delta_theta_factor;
		float ro_theta_factor		= g_ro_theta_factor;
		float gait_state_factor		= g_gait_state_factor;
		float torso_factor			= g_torso_factor;
		float strafe_discount		= g_strafe_discount;

		//float	ro_theta = math::angle_normalize_signed(  p.theta()  ); // //+ ro
		float	ro_theta =   math::angle_normalize_signed( p.theta_ro() ); // //+ ro

		float ret_move = 
					- math::abs( ro_theta )										* ro_theta_factor
					//- math::abs( p.theta() )									* theta_factor 
					- math::abs( ro )											* ro_factor
					- math::abs( delta_theta )									* delta_theta_factor
					- math::abs( ( (s32)p.discrete_value() - (s32)gait() ) )	* gait_state_factor ; // -  a.x() * 
		
		float	tau =   math::angle_normalize_signed( p.tau() ); // //+ ro

		float ret_torso = - math::abs( tau ) * torso_factor; // - l_motion_angles.tau ) * torso_factor; // -tau //- l_motion_angles.tau
		//float ret_torso = - tau * tau * torso_factor; 
		
		separate_reward_type ret;
		ret[0] = ret_move;
		ret[1] = ret_torso;
		
		if( flags() & animation::group_flag_strafe )
		{
			ret[0] -= strafe_discount;
			ret[1] -= strafe_discount;
		}

		return ret;
	}

inline	float	navigation_theta_action::jump_reward( const navigation_theta_action& a )const
{
	
	if( a.gait() == animation::gait_walk )
		return 100;
	
	return -1000.f;
}

inline float	navigation_theta_action::jump_reward( const navigation_theta_params& p )const
{
	
	if( p.gait() == animation::gait_walk_jump )
		return 1000.f;

	return -1000.f;

}

inline	navigation_theta_action::separate_reward_type	navigation_theta_action::transition_reward( u32 my_blend, const navigation_theta_action& a, u32 to_blend ) const
{
	XRAY_UNREFERENCED_PARAMETERS(  &a );
	
	if( ( flags() & animation::group_flag_step_left ) &&  ( a.flags() & animation::group_flag_step_left ) ||
		( flags() & animation::group_flag_step_right ) &&  ( a.flags() & animation::group_flag_step_right )
		) return separate_reward_type().set( -1000 );

	if( gait() == animation::gait_walk_jump )
		return separate_reward_type().set( jump_reward( a ) );

 //-300 +
	float ret = g_gait_self_transition_reward_factor + g_gait_transition_reward_factor - math::abs( ( (s32)a.gait() - (s32)gait() ) ) * g_gait_transition_reward_factor ;

	if( !!( flags( ) & animation::group_flag_idle ) && a.gait() == gait() )
		ret += 100.f;

	//if( a.flags() & animation::group_flag_strafe )
	//	ret -= 30.f;

	motion_angles my_angles = angles( my_blend, this, my_blend );
	motion_angles to_angles = a.angles( to_blend, this, my_blend );
	
	float ro_transition_discount_factor		= g_ro_transition_discount_factor;
	float theta_transition_discount_factor	= g_theta_transition_discount_factor;
	float tau_transition_discount_factor	= g_tau_transition_discount_factor;

	//separate_reward_type ret_reward;
	//ret_reward.set( ret );

	ret -= ro_transition_discount_factor		* math::abs( my_angles.ro - to_angles.ro );
	ret -= theta_transition_discount_factor		* math::abs( my_angles.delta_theta - to_angles.delta_theta );

	//float tau_product = math::max( 0.00001f, math::abs( my_angles.tau ) * math::abs( to_angles.tau ) );

	//ret += tau_transition_discount_factor * 0.01f / math::max( 0.01f, math::abs( my_angles.tau - to_angles.tau ) );

	ret -= tau_transition_discount_factor * math::abs( my_angles.tau - to_angles.tau );

	return separate_reward_type().set( ret );
}



} // namespace rtp
} // namespace xray

#endif // #ifndef NAVIGATION_ACTION_INLINE_H_INCLUDED
