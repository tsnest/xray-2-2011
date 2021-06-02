	
////////////////////////////////////////////////////////////////////////////
//	Created 	: 09.02.2008
//	Author		: Konstantin Slipchenko
//	Description : body - physics bone (no collision)
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "body_ode.h"

#include "island.h"

#include "ode_include.h"
#include "ode_convert.h"
#include <xray/render/facade/debug_renderer.h>
#include <xray/console_command.h>
#include <xray/collision/primitives.h>
#include <xray/collision/geometry_instance.h>

bool s_autosleep = true;
static console_commands::cc_bool s_cmd_autosleep( "physics_autosleep", s_autosleep, true, console_commands::command_type_user_specific );


body_ode::body_ode			():
m_body( dBodyCreate(0) ),
m_previous_rotation( float4( 0,0,0,1) ),
m_previous_position( 0, 0, 0 )
{

}

body_ode::~body_ode			()
{
	dBodyDestroy(m_body);
	m_body = 0;
}
void	body_ode::fill_island				( island	&i )
{
	i.ode().add_body( m_body );
}
bool	body_ode::integrate				( const step_counter_ref& sr, float time_delta )
{
	//update_pose( );
	float4x4 m = get_pose( m );
	float3 a_vel = cast_xr( dBodyGetAngularVel( m_body ) );
	autosleep_update_params dp( 
		
		cast_xr( dBodyGetLinearVel( m_body ) ), 
		a_vel,
		a_vel*time_delta,
		m, 
		sr );

	if( !s_autosleep )
		return true;

	bool ret = m_auto_sleep.update( dp );

	//return true;
	return ret;
}

void body_ode::on_step_start( island	&i, float time_delta )
{
	XRAY_UNREFERENCED_PARAMETERS( &i, time_delta );
	d_quaternion_and_position( m_previous_rotation, m_previous_position, m_body );
}

void body_ode::interpolate_pose( float factor, float4x4 &transform )const
{
	math::quaternion current_rotation;
	float3	current_position;
	d_quaternion_and_position( current_rotation, current_position, m_body );
	
	math::quaternion quat = math::slerp( m_previous_rotation, current_rotation, factor );

	transform = math::create_rotation( quat );
	transform.c.xyz() = m_previous_position * factor + d_position( m_body ) * ( 1.f - factor );
}

//void body_ode::update_pose( )
//{
//	ASSERT( m_body );
//	float4x4 m;
//	d_body_matrix( m, m_body );
//	m_pose_anchor.set( m );
//}
float4x4 body_ode::get_pose( )const
{
	float4x4 m;
	return get_pose( m );
}
float4x4	&body_ode::get_pose	( float4x4 &pose ) const
{
	d_body_matrix( pose, m_body );
	return pose;
}

void	body_ode::render( render::scene_ptr const& scene, render::debug::renderer& renderer ) const
{
	float4x4 m;
	get_pose( m );
	renderer.draw_sphere( scene, m.c.xyz(), 0.03f, math::color( 255u, 0u, 0u, 255u ) );
}

void body_ode::move_position( const float3 &delta )
{
	float4x4 m;
	get_pose( m );
	float3 new_position = m.c.xyz() + delta;
	set_position( new_position );
}

void body_ode::set_position			( const float3 &p )
{
	dBodySetPosition( m_body, p.x, p.y, p.z );
	m_previous_position = p;
	m_auto_sleep.reset( get_pose(), cast_xr( dBodyGetLinearVel( m_body ) ), cast_xr( dBodyGetAngularVel( m_body ) ) );
}

void body_ode::set_pose( const float4x4 &pose )
{
	dMatrix3 R; dVector3 p;
	d_matrix( R, p, pose );

	dBodySetRotation( m_body, R );
	dBodySetPosition( m_body, p[0], p[1], p[2] );
	d_quaternion_and_position( m_previous_rotation, m_previous_position, m_body );

	m_auto_sleep.reset( get_pose(), cast_xr( dBodyGetLinearVel( m_body ) ), cast_xr( dBodyGetAngularVel( m_body ) ) );
}

struct calculate_mass_tensor: public xray::collision::enumerate_primitives_callback
{
	calculate_mass_tensor( float density_ ): density( density_ )
	{
		dMassSetZero( &result );
	}

	virtual void enumerate( xray::math::float4x4 const& transform_, xray::collision::primitive const & p )
	{
		dMass add;
		xray::math::float4x4 transform = transform_;
		xray::math::remove_scale( transform );
		switch( p.type )
		{
			
			case xray::collision::primitive_sphere: {
					float r = p.sphere().radius;
					dMassSetSphere( &add, density, r );
				} break;

			case xray::collision::primitive_box: {
					float3 sides( p.box().half_side * 2.f );
					dMassSetBox( &add, density, sides.x, sides.y, sides.z );
				} break;

			case xray::collision::primitive_cylinder: {
					float r = p.cylinder().radius;
					float l = p.cylinder().half_length * 2.f;
					dMassSetCylinder( &add, density, 1, r, l );
				} break;

			default:
				NODEFAULT();
		}
		
		dMatrix3 R; dVector3 P;
		d_matrix( R, P, transform );
		dMassRotate( &add, R );
		dMassTranslate( &add, P[0], P[1], P[2] );
		dMassAdd( &result, &add );
	}
	 
	dMass result;
	float density;
};


void	body_ode::set_mass	( float density, const xray::collision::geometry_instance &og,  float4x4 const &pose_in_body )
{
	
	calculate_mass_tensor v( density );

	//float4x4 pose_in_body = pose_in_body_;
	//xray::math::remove_scale( pose_in_body );
	
	float4x4		body_transform	= get_pose( body_transform );
	float4x4 const	body_inverted	= xray::math::invert4x3( body_transform );
	float4x4 const transf = pose_in_body * body_transform; 
	float4x4 const itransf =  xray::math::invert4x3( transf ); 
	og.enumerate_primitives( itransf, v );
	dBodySetMass( m_body, &v.result );
}