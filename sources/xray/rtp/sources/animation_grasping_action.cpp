////////////////////////////////////////////////////////////////////////////
//	Created		: 22.04.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_grasping_action.h"
#include "space_params.h"

#include <xray/animation/i_animation_action.h>
#include <xray/animation/i_animation_controller_set.h>


namespace xray {
namespace rtp {


static const float target_reward						=  1000000000;
static const float out_of_range_reward				=  0;
static const float target_action_not_in_target_reward = -1000000000;
static const float target_action_exit_reward			= -1000000000;

static const float action_transition_reward			= 10;

u32 animation_grasping_action::super::max_samples = 30;

animation_grasping_action::animation_grasping_action( 
	action_base<animation_grasping_action> &ab,  
	animation::i_animation_action  const *anim ):
	super( ab, anim )
{
	


}

animation_grasping_action::animation_grasping_action( 
	xray::rtp::action_base<animation_grasping_action> &ab
	):
	super( ab )
{
	
}

void	animation_grasping_action::construct( )
{
	dbg_set_target_params( );
	super::construct( );
}


void	animation_grasping_action::dbg_set_target_params	( )
{
	is_target = id() <= 3;
}

#ifndef	MASTER_GOLD
void	animation_grasping_action::save( xray::configs::lua_config_value cfg )const
{
	super::save( cfg );
	cfg["is_target"]			= is_target;
}
#endif	// MASTER_GOLD

void	animation_grasping_action::load( const xray::configs::binary_config_value &cfg )
{
	super::load	( cfg );
	is_target	= cfg["is_target"];
}

animation_grasping_space_params::separate_reward_type	animation_grasping_action::transition_reward( u32 my_blend, const animation_grasping_action& a, u32 to_blend ) const
{
	XRAY_UNREFERENCED_PARAMETERS( my_blend, &a, to_blend );
	if( is_target_action() )
		return separate_reward_type().set( target_action_exit_reward );

	return separate_reward_type().set( action_transition_reward );
}

bool	animation_grasping_action::is_target_action		( )const
{
	return is_target;
}

bool	animation_grasping_action::target_blends( u32 blend[4], float factors[2], animation_grasping_space_params const &p  )const
{
	
	if( is_target_action	( ) &&
		m_target_space_param_devision.target_blends( blend, factors, p )
		) return true;

	return false;
}

bool	animation_grasping_action::target( const animation_grasping_space_params& p )const
{

	u32 blends[4] = { u32( -1 ), u32( -1 ), u32( -1 ), u32( -1 ) };
	float factors[2] = { -1, -1 };
	return target_blends( blends, factors, p );

}

animation_grasping_space_params::separate_reward_type	animation_grasping_action::reward( const animation_grasping_space_params& a, u32 blend  ) const
{
	XRAY_UNREFERENCED_PARAMETERS(  &a, blend );
	if( target( a ) )
		return separate_reward_type().set(target_reward);

	if( is_target_action() )
		return separate_reward_type().set( target_action_not_in_target_reward );

	if( !in_range( a )  )
		return separate_reward_type().set( out_of_range_reward );

	return separate_reward_type().set( 0 );
}

void	animation_grasping_action::run( animation_grasping_space_params& from, const animation_grasping_action& af, u32 from_sample, u32 blend_sample  )const
{
	
	XRAY_UNREFERENCED_PARAMETERS( &from, &af, &from_sample, &blend_sample );
	


	float4x4 transform = calculate_transform( af, from_sample, blend_sample );

	//transform_to_param( m_cache_transforms[blend_sample], from );
	transform_to_param( transform, from );

}

void animation_grasping_action::on_walk_run( animation_grasping_world_params& wfrom, const animation_grasping_space_params& lfrom, const float4x4 &disp, buffer_vector< float > &weights  )const
{
	super::on_walk_run( wfrom, lfrom, disp, weights );

/////////////////////////////////////////////////////////////////////////////	
	u32 blends[4] = { u32( -1 ), u32( -1 ), u32( -1 ), u32( -1 ) };
	float factors[2] = { -1, -1 };
	if( target_blends( blends, factors,  lfrom ) )
	{
		
		weight_mean( weights, blends, factors );
#ifdef	DEBUG

		 animation_grasping_space_params dbgp = get_param_target( weights );
		 lfrom.similar( dbgp );
#endif
	}
/////////////////////////////////////////////////////////////////////////////

}

float	animation_grasping_action::run( animation_grasping_world_params& wfrom, const animation_grasping_space_params& lfrom, const animation_grasping_action& afrom, u32 from_sample, u32 blend_sample, u32 step, float time )const
{
	XRAY_UNREFERENCED_PARAMETERS( &wfrom, &afrom, &from_sample, &blend_sample, &time, &step );
	
	return super::run( wfrom, lfrom, afrom, from_sample,  blend_sample, step, time );

}


//void	animation_grasping_action::transform_params( animation_grasping_space_params& param, buffer_vector< float > &weights )const
//{
//	
//	float4x4 m;
//	m_animation_action->get_displacement_transform( m, weights );
//	
//	transform_to_param( m, param );
//
//}

animation_grasping_space_params animation_grasping_action::get_param_target( const buffer_vector< float > &weights )const
{
		

		u32 lh = m_animation_action->bone_index( "LeftHand" );
		u32 rh = m_animation_action->bone_index( "RightHand" );

		float4x4 ml, mr;
		m_animation_action->local_bone_matrix( ml, lh, weights );
		m_animation_action->local_bone_matrix( mr, rh, weights );

		float3 p= 0.5f * ( ml.c.xyz() +  mr.c.xyz() );
		
		return animation_grasping_space_params( float2( p.x, p.z ) );

}

void	animation_grasping_action::cache_blend_transform( u32 id_transform, u32 blend, buffer_vector< float > const &weights, u32 from_action_id, buffer_vector< float >  const &from_weights )
{
	super::cache_blend_transform( id_transform, blend, weights, from_action_id, from_weights );

	if( is_target )
		m_target_space_param_devision.add( get_param_target( weights ), blend );

}

void	animation_grasping_action::cache_transforms	( )
{
	super::cache_transforms( );
	m_target_space_param_devision.sort();
}


//animation_grasping_world_params		&animation_grasping_global_params::current_position()
//{
//	ASSERT( m_set );
//	return m_current_position;
//}


struct cmp
{
	bool operator () ( const std::pair< float, u32 >& l, const std::pair< float, u32 >& r )
	{
		return l.first < r.first;
	}
};

void	target_space_param_devision::sort()
{
	std::sort( m_x_sorted_target.begin(), m_x_sorted_target.end(), cmp() );
	std::sort( m_y_sorted_target.begin(), m_y_sorted_target.end(), cmp() );
}

void	target_space_param_devision::add( animation_grasping_space_params const &p, u32 blend )
{
	m_x_sorted_target.push_back( std::pair< float, u32 >( p.target_position().x, blend ) );
	m_y_sorted_target.push_back( std::pair< float, u32 >( p.target_position().y, blend ) );
}




bool	target_space_param_devision::target_blends( u32 blend[4], float factors[2], animation_grasping_space_params const &p )const
{
	std::pair< float, u32 > vx( p.target_position().x, u32(-1) ), vy(  p.target_position().y, u32(-1) ) ;  
	
	vector< std::pair< float, u32 > >::const_iterator i_x = std::lower_bound(  m_x_sorted_target.begin(), m_x_sorted_target.end(), vx , cmp() );
	
	if( i_x == m_x_sorted_target.end() || i_x == m_x_sorted_target.begin() )
		return false;

	vector< std::pair< float, u32 > >::const_iterator i_y = std::lower_bound(  m_y_sorted_target.begin(), m_y_sorted_target.end(), vy,  cmp() );
	
	if( i_y == m_y_sorted_target.end() || i_y == m_y_sorted_target.begin() )
		return false;
	
	blend[ 0 ] = ( i_x - 1 )->second;
	blend[ 1 ] = ( i_x )->second;
	
	factors[0] =  ( vx.first - ( i_x - 1 )->first ) / ( ( i_x )->first - ( i_x - 1 )->first );
	ASSERT( factors[0] >= 0 && factors[0] <=1 );
		
	blend[ 2 ] = ( i_y - 1 )->second;
	blend[ 3 ] = ( i_y )->second;

	factors[1] =  ( vy.first - ( i_y - 1 )->first ) / ( ( i_y )->first - ( i_y - 1 )->first );
	ASSERT( factors[1] >= 0 && factors[1] <=1 );

	return true;
}

} // namespace rtp
} // namespace xray
