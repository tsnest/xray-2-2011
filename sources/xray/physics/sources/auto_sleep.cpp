////////////////////////////////////////////////////////////////////////////
//	Created		: 29.05.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "auto_sleep.h"

auto_sleep_vector::auto_sleep_vector()
{
	reset( float3( 0, 0, 0 ) );
}

void	auto_sleep_vector::reset( const float3& new_vector )
{
	from =( new_vector );
	to = ( new_vector );
}



float	auto_sleep_vector::update( const float3& new_vector )
{
	float ret = get_difference_to_last( new_vector );
	to = new_vector;
	return ret;
}

float	auto_sleep_vector::update_add					( const float3& new_difference )
{
	return update( to + new_difference );
}


float	auto_sleep_vector::get_difference_to_last( const float3& new_vector )const
{
	return ( new_vector - to ).length();
}

float	auto_sleep_vector::sum_length( const float3& cur_vector )
{
	XRAY_UNREFERENCED_PARAMETER(cur_vector);
	return (to-from).length();
}







auto_sleep_update_state::auto_sleep_update_state()
{
	reset();
}



void	auto_sleep_update_state::reset()
{
	m_disable	=	false				;
	m_enable	=	false				;
}

auto_sleep_update_state& auto_sleep_update_state::operator &=	(auto_sleep_update_state& lstate)
{
	m_disable	=	m_disable	&&	lstate.m_disable;
	m_enable	=	m_enable	||	lstate.m_enable;
	return *this;
}

base_auto_sleep_data::base_auto_sleep_data(): m_disabled(false)
{

	m_frames	=world_autosleep_params.objects_params.l2_frames		;
	reset();
	
}

void	base_auto_sleep_data::reset( )	
{
	m_count		=m_frames											;
	m_state_l1	.reset()											;
	m_state_l2	.reset()											;
	
}
bool	base_auto_sleep_data::update( const autosleep_update_params &v )
{
	
	if( m_last_frame_updated.is_equal( v.step_counter ) )
		return !m_disabled;
	m_last_frame_updated = v.step_counter;

	m_count--;

	update_l1( v );

	check_state( m_state_l1 );

	if( !v.step_counter.multiple_of( m_frames ) )
		return !m_disabled;

	if(m_count==0)
	{
		update_l2( v );
		check_state(m_state_l2);
	}
	m_count=m_frames;
	
	return !m_disabled;

	//const	dReal* force=dBodyGetForce(body);
	//const	dReal* torqu=dBodyGetTorque(body);
	//if(dDOT(force,force)>0.f || dDOT(torqu,torqu)>0.f) m_disabled=false;
	//if(dBodyIsEnabled(body))
	//{
	//	ReEnable();
	//	if(!m_disabled&& (ph_world->disable_count!=m_count%world_autosleep_params.objects_params.L2frames))
	//	{
	//		m_count=m_frames+ph_world->disable_count;
	//	}
	//}
	//if(m_disabled)	
	//			disable();//dBodyDisable(body);

}

void	autosleep_base::reset( )
{

	base_auto_sleep_data::reset()	;

	//bool disable=!dBodyIsEnabled(get_body());

	m_state_l1.m_disable=m_disabled;
	m_state_l1.m_enable=!m_disabled;
	m_state_l2.m_disable=m_disabled;
	m_state_l2.m_enable=!m_disabled;
}
bool	autosleep_base::do_updating()
{
	return u32(m_frames-m_count)==m_last_frame_updated.remainder_of( m_frames ) || 
			m_last_frame_updated.multiple_of( m_frames ) ;
}
void	autosleep_base::update_values_l1( const autosleep_update_params &v )
{

	if( do_updating() )
	{
		float			velocity_param		=	velocity_update( v );								
		float			acceleration_param	=	acceleration_update( v );							
		check_state		(m_state_l1,velocity_param*m_frames,acceleration_param*m_frames)	;
	}
	else
	{
		float			velocity_param		=	velocity_last_difference( v );					
		float			acceleration_param	=	acceleration_last_difference( v );				
		check_state		(m_state_l1,velocity_param*m_frames,acceleration_param*m_frames)			;
	}
}
void	autosleep_base::update_values_l2( const autosleep_update_params &v )		
{
					m_state_l2			.	reset						()							;

	float			velocity_param		=	velocity_sum( v )/m_frames	;
	float			acceleration_param	=	acceleration_sum( v )/m_frames	;

	check_state		( m_state_l2, velocity_param, acceleration_param )								;

}

void	autosleep_base::set_disable_params( const autosleep_params& param )
{
	m_params=param;
}


autosleep_rotational::autosleep_rotational()
{
	m_params=world_autosleep_params.objects_params.rotational											;
}


inline float3 angular_diff( float3 &d, const float4x4 &m )
{
	return 
		d.set( 
			( m.e32-m.e23 )/2.f,
			( m.e13-m.e31 )/2.f,
			( m.e21-m.e12 )/2.f
		);
}
inline float3 angular( float3 &d, const float4x4 &m )
{
	return 
		d.set( 
			( m.e12 ),
			( m.e21 ),
			( m.e02 )
		);
}
float	autosleep_rotational::velocity_update				( const autosleep_update_params &v )
{
	return m_mean_velocity	.update_add( v.angular_rotation );
}
float	autosleep_rotational::acceleration_update			( const autosleep_update_params &v )
{
	return m_mean_acceleration	.update( v.angular_velocity )		;
}
float	autosleep_rotational::velocity_last_difference	( const autosleep_update_params &v ) const	
{
	return v.angular_rotation.length();//m_mean_velocity		.get_difference_to_last( v.angular_rotation );
}
float	autosleep_rotational::acceleration_last_difference( const autosleep_update_params &v )	const
{
	return m_mean_acceleration	.get_difference_to_last( v.angular_velocity );
}

float			autosleep_rotational::velocity_sum				( const autosleep_update_params &v )
{
	return m_mean_velocity.sum_length( v.angular_rotation );//		.sum_length()
}
float			autosleep_rotational::acceleration_sum			( const autosleep_update_params &v )
{
	return	m_mean_acceleration.sum_length( v.angular_velocity );
}

void	autosleep_rotational::reset( const float4x4 &transform, const float3 &a_vel )	
{
	XRAY_UNREFERENCED_PARAMETER( transform );
	autosleep_base::reset( );

	m_mean_velocity.reset(	float3(0,0,0) );
	m_mean_acceleration	.reset( a_vel );
}

void	autosleep_rotational::update_l1( const autosleep_update_params &v )
{
	m_state_l1			.	reset						()							;

	float3	vrotation	= angular	( vrotation, v.transform );
	
	autosleep_base::update_values_l1( v );

}
void	autosleep_rotational::update_l2( const autosleep_update_params &v )
{
	//float3	vrotation	= angular	( vrotation, v.transform );
	autosleep_base::update_values_l2( v );
	m_mean_velocity		.	reset	( float3( 0,0,0) );
	m_mean_acceleration	.	reset	( v.angular_velocity );

}
void autosleep_rotational::set_disable_params(const autosleep_params_full& params)
{
	autosleep_base::set_disable_params(params.rotational);
	m_frames=params.l2_frames;
}





autosleep_translational::autosleep_translational()
{
	m_params=world_autosleep_params.objects_params.translational;
}

void	autosleep_translational::reset( const float4x4 &transform, const float3 &l_vel )
{
	autosleep_base::reset						( )								;
	m_mean_velocity		.reset		( transform.c.xyz() )						;
	m_mean_acceleration	.reset		( l_vel )									;
}
void	autosleep_translational::update_l1( const autosleep_update_params &v )
{
	m_state_l1.	reset					()												;
	autosleep_base::update_values_l1( v );
}
void	autosleep_translational::update_l2( const autosleep_update_params &v )
{
	autosleep_base::update_values_l2( v );
	m_mean_velocity		.	reset	( v.transform.c.xyz() );
	m_mean_acceleration	.	reset	( v.linear_velocity );

}
void autosleep_translational::set_disable_params(const autosleep_params_full& params)
{
	autosleep_base::set_disable_params(params.translational);
	m_frames=params.l2_frames;
}



float	autosleep_translational::velocity_update				( const autosleep_update_params &v )
{
	return m_mean_velocity		.update( v.transform.c.xyz() )		;
}
float	autosleep_translational::acceleration_update			( const autosleep_update_params &v )
{
	return m_mean_acceleration	.update( v.linear_velocity )		;
}
float	autosleep_translational::velocity_last_difference	( const autosleep_update_params &v ) const	
{
	return m_mean_velocity		.get_difference_to_last( v.transform.c.xyz() );
}
float	autosleep_translational::acceleration_last_difference( const autosleep_update_params &v )	const
{
	return m_mean_acceleration	.get_difference_to_last( v.linear_velocity );
}


float			autosleep_translational::velocity_sum				( const autosleep_update_params &v )
{
	return m_mean_velocity.sum_length( v.transform.c.xyz() );
}
float			autosleep_translational::acceleration_sum			( const autosleep_update_params &v )
{
	return m_mean_velocity.sum_length( v.linear_velocity );
}


void	autosleep_full::reset( const float4x4 &transform, const float3 &l_vel, const float3 &a_vel )
{
	autosleep_rotational::reset( transform,  a_vel );
	autosleep_translational::reset( transform, l_vel );
}
void	autosleep_full::update_l1( const autosleep_update_params &v )
{
	auto_sleep_update_state state;
	autosleep_rotational::update_l1( v );
	state=m_state_l1;
	autosleep_translational::update_l1( v );
	m_state_l1&=state; 
}

void	autosleep_full::update_l2( const autosleep_update_params &v )
{
	auto_sleep_update_state state;
	autosleep_rotational::update_l2(v);
	state=m_state_l2;
	autosleep_translational::update_l2(v);
	m_state_l2&=state;
}

void autosleep_full::set_disable_params( const autosleep_params_full& params )
{
	autosleep_rotational::set_disable_params(params);
	autosleep_translational::set_disable_params(params);
}