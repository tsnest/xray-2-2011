////////////////////////////////////////////////////////////////////////////
//	Created		: 24.04.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "joint_hinge.h"
#include "ode_include.h"
#include "ode_convert.h"
#include "joint_ode.h"

joint_hinge::joint_hinge():
	joint_single_base_impl( new_ode_hinge( ) )
{
	
}

joint_hinge::~joint_hinge()
{
	detach( m_joint );
	delete_joint( m_joint );
}


void	joint_hinge::set_axis( const float3 &axis )
{
	dJointSetHingeAxis ( m_joint, axis.x, axis.y, axis.z );
}
void	joint_hinge::set_angle( float angle )
{
	XRAY_UNREFERENCED_PARAMETER	(angle);
}

void	joint_hinge::set_low_limit( float limit )
{
	dJointSetHingeParam( m_joint, dParamLoStop ,limit );
}
void	joint_hinge::set_high_limit	( float limit )
{
	dJointSetHingeParam( m_joint, dParamHiStop ,limit );
}
float	joint_hinge::get_low_limit	( )const
{
	return dJointGetHingeParam( m_joint, dParamLoStop );
}
float	joint_hinge::get_high_limit( )const
{
	return dJointGetHingeParam( m_joint, dParamHiStop );
}

static float get_stop_erp( dxJoint *hinge )
{
	return dJointGetHingeParam( hinge, dParamStopERP );
	
}
static float get_stop_cfm( dxJoint *hinge )
{
	return dJointGetHingeParam( hinge, dParamStopCFM );
}


static float set_stop_erp( dxJoint *hinge, float erp  )
{
	dJointSetHingeParam(hinge, dParamStopERP, erp );
	return 0;
}
static float set_stop_cfm( dxJoint *hinge, float cfm )
{
	dJointSetHingeParam( hinge, dParamStopCFM,  cfm );
	return 0;
}

void	joint_hinge::set_stop_spring( float spring )	
{
	const float dampg = get_stop_damping( );
	
	const float l_erp = erp( spring, dampg, base_param_step() );
	const float l_cfm = cfm( spring, dampg, base_param_step() );

	set_stop_erp( m_joint, l_erp );
	set_stop_cfm( m_joint, l_cfm );
}

void	joint_hinge::set_stop_damping( float damping )
{
	const float sprng = get_stop_spring( );
	
	const float l_erp = erp( sprng, damping, base_param_step() );
	const float l_cfm = cfm( sprng, damping, base_param_step() );

	set_stop_erp( m_joint, l_erp );
	set_stop_cfm( m_joint, l_cfm );
}

float	joint_hinge::get_stop_spring( )	
{
	float cfm = get_stop_cfm( m_joint );
	float erp = get_stop_erp( m_joint );

	return ::spring( cfm, erp, base_param_step() );
}
float	joint_hinge::get_stop_damping( )
{
	float cfm = get_stop_cfm( m_joint );
	float erp = get_stop_erp( m_joint );

	return damping( cfm, erp );
}


float set_normal_cfm( dxJoint *hinge, float cfm )
{
	dJointSetHingeParam( hinge, dParamCFM,  cfm );
	return 0;
}
float get_normal_cfm( dxJoint *hinge )
{
	return dJointGetHingeParam( hinge, dParamStopCFM );
}


void	joint_hinge::set_normal_spring( float spring )
{
	const float l_damping = get_normal_damping( );
	
	const float l_cfm = cfm( spring, l_damping, base_param_step() );
	//const float l_erp = erp( spring, l_damping, base_param_step() );
	set_normal_cfm( m_joint, l_cfm );
}
void	joint_hinge::set_normal_damping( float dumping )
{
	const float l_spring = get_normal_spring( );
	const float l_cfm = cfm( l_spring, dumping, base_param_step() );
	set_normal_cfm( m_joint, l_cfm );
}

float	joint_hinge::get_normal_spring( )
{
	const float l_erp = default_erp;
	const float l_cfm = get_normal_cfm( m_joint );
	
	return spring( l_cfm, l_erp, base_param_step() );
}

float	joint_hinge::get_normal_damping( )
{
	const float l_erp = default_erp;
	const float l_cfm = get_normal_cfm( m_joint );
	return damping( l_cfm, l_erp );
}

void	joint_hinge::set_anchor( const float3	&anchor )
{
		dJointSetHingeAnchor(  m_joint, anchor.x, anchor.y, anchor.z ) ;
}
void	joint_hinge::get_anchor  ( float3	&anchor )const
{
	dVector3 res;
	dJointGetHingeAnchor( dJointID( m_joint ), res );
	anchor = cast_xr( res );
}