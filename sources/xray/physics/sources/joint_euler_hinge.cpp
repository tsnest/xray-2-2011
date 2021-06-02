////////////////////////////////////////////////////////////////////////////
//	Created		: 16.04.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "joint_euler_hinge.h"
#include "pose_anchor.h"
#include "ode_include.h"
#include "ode_convert.h"
#include "body.h"
#include "joint_ode.h"
euler_hinge::euler_hinge		( ): 
	joint_double_base_impl( 
		new_ode_angular_motor( ),
		new_ode_ball( )
	)
{
	dJointSetAMotorMode(  angular_motor	( ), dAMotorEuler );

	dJointSetAMotorParam( angular_motor	( ), dParamStopERP, default_erp	);
	dJointSetAMotorParam( angular_motor	( ), dParamStopCFM, default_cfm	);

	dJointSetAMotorParam( angular_motor	( ), dParamStopERP2, default_erp );
	dJointSetAMotorParam( angular_motor	( ), dParamStopCFM2, default_cfm );

	dJointSetAMotorParam( angular_motor	( ), dParamStopERP3, default_erp );
	dJointSetAMotorParam( angular_motor	( ), dParamStopCFM3, default_cfm );

	dJointSetAMotorParam( angular_motor	( ), dParamCFM ,default_cfm );
	dJointSetAMotorParam( angular_motor	( ), dParamCFM2 ,default_cfm );
	dJointSetAMotorParam( angular_motor	( ), dParamCFM3 ,default_cfm );
	
}
euler_hinge::~euler_hinge	( )
{
	detach( m_joint0 );
	delete_joint( m_joint0 );

	detach( m_joint1 );
	delete_joint( m_joint1 );
}





void	euler_hinge::set_anchor  ( const float3	&anchor )
{
	dJointSetBallAnchor( ball(), anchor.x, anchor.y, anchor.z ) ;

}
void	euler_hinge::get_anchor  ( float3	&anchor )const
{
	dVector3 res;
	dJointGetBallAnchor( dJointID( ball() ), res );
	anchor = cast_xr( res );
}



void	euler_hinge::set_axis		( axis axis_num, const float3 &axis )	
{
	//dJointSetAMotorAxis ( angular_motor(), axis_num, 0, axis.x, axis.y, axis.z );

	// set 
	//if( axis_num == axis  ) //&& body0()
	//{
	//	body0()->get_pose().transform_direction( axis );
	//	dJointSetAMotorAxis (angular_motor(), 0, 1, axis.x, axis.y, axis.z);
	//}
	//else if( axis_num == 2  && body1() )
	//{
	//	body1()->get_pose().transform_direction( axis );
	//	dJointSetAMotorAxis (angular_motor(), 2, 2, axis.x, axis.y, axis.z);
	//}
	////else
	////	UNREACHABLE_CODE();

	switch(axis_num)
	{
		case x_axis:	dJointSetAMotorAxis (angular_motor(), x_axis, 1, axis.x, axis.y, axis.z) ;			break;
		case y_axis:	break;//dJointSetAMotorAxis (angular_motor(), y_axis, 2, axis.x, axis.y, axis.z);			break;
		case z_axis:	dJointSetAMotorAxis (angular_motor(), z_axis, 2, axis.x, axis.y, axis.z);			break;
		default: NODEFAULT();
	}

}
void	euler_hinge::set_angle		( axis axis_num, float angle )
{
	XRAY_UNREFERENCED_PARAMETERS	(axis_num, angle);
}

void	euler_hinge::set_low_limit	( axis axis_num, float limit )
{
	ASSERT( limit<=0.f );
	switch(axis_num)
	{
		case x_axis:	dJointSetAMotorParam(angular_motor(),dParamLoStop ,limit); break;
		case y_axis:	dJointSetAMotorParam(angular_motor(),dParamLoStop2 ,limit); break;
		case z_axis:	dJointSetAMotorParam(angular_motor(),dParamLoStop3 ,limit); break;
		default: NODEFAULT();
	}
	
}

void	euler_hinge::set_high_limit	( axis axis_num, float limit )	
{
	ASSERT( limit>=0.f );
	switch(axis_num)
	{
		case x_axis: dJointSetAMotorParam(angular_motor(),dParamHiStop ,limit); break;
		case y_axis: dJointSetAMotorParam(angular_motor(),dParamHiStop2 ,limit); break;
		case z_axis: dJointSetAMotorParam(angular_motor(),dParamHiStop3 ,limit); break;
		default: NODEFAULT();
	}
}

float	euler_hinge::get_low_limit	( axis axis_num )const				
{
	 switch(axis_num)
	{
		case x_axis:	return	dJointGetAMotorParam(angular_motor(),dParamLoStop );
		case y_axis:	return	dJointGetAMotorParam(angular_motor(),dParamLoStop2);
		case z_axis:	return	dJointGetAMotorParam(angular_motor(),dParamLoStop3);
		default: NODEFAULT(return 0);
	}
}

float	euler_hinge::get_high_limit	( axis axis_num )const				
{
	switch(axis_num)
	{
		case x_axis:	return	dJointGetAMotorParam(angular_motor(),dParamHiStop );
		case y_axis:	return	dJointGetAMotorParam(angular_motor(),dParamHiStop2);
		case z_axis:	return	dJointGetAMotorParam(angular_motor(),dParamHiStop3);
		default: NODEFAULT(return 0);
	}
}
static float get_stop_erp( dxJoint *amotor, axis axis_num  )
{
	switch(axis_num)
	{
		case x_axis:	return	dJointGetAMotorParam(amotor,dParamStopERP );
		case y_axis:	return	dJointGetAMotorParam(amotor,dParamStopERP2);
		case z_axis:	return	dJointGetAMotorParam(amotor,dParamStopERP3);
		default: NODEFAULT(return 0);
	}
}
static float get_stop_cfm( dxJoint *amotor, axis axis_num )
{
	switch(axis_num)
	{
		case x_axis:	return	dJointGetAMotorParam(amotor,dParamStopCFM  );
		case y_axis:	return	dJointGetAMotorParam(amotor,dParamStopCFM2 );
		case z_axis:	return	dJointGetAMotorParam(amotor,dParamStopCFM3 );
		default: NODEFAULT(return 0);
	}
}


static float set_stop_erp( dxJoint *amotor, axis axis_num, float erp  )
{
	switch(axis_num)
	{
		case x_axis:	dJointSetAMotorParam(amotor,dParamStopERP,  erp ); break;
		case y_axis:	dJointSetAMotorParam(amotor,dParamStopERP2, erp ); break;
		case z_axis:	dJointSetAMotorParam(amotor,dParamStopERP3, erp ); break;
		default: NODEFAULT();
	}
	return 0;
}

static float set_stop_cfm( dxJoint *amotor, axis axis_num, float cfm )
{
	switch(axis_num)
	{
		case x_axis:	dJointSetAMotorParam(amotor,dParamStopCFM,  cfm ); break;
		case y_axis:	dJointSetAMotorParam(amotor,dParamStopCFM2, cfm ); break;
		case z_axis:	dJointSetAMotorParam(amotor,dParamStopCFM3, cfm ); break;
		default: NODEFAULT();
	}
	return 0;
}



void	euler_hinge::set_stop_spring	( axis axis_num, float spring )
{
	const float dampg = get_stop_damping( axis_num );
	
	const float l_erp = erp( spring, dampg, base_param_step() );
	const float l_cfm = cfm( spring, dampg, base_param_step() );

	set_stop_erp( angular_motor(), axis_num, l_erp );
	set_stop_cfm( angular_motor(), axis_num, l_cfm );
}

void	euler_hinge::set_stop_damping( axis axis_num, float damping )
{
		
	const float sprng = get_stop_spring( axis_num );
	
	const float l_erp = erp( sprng, damping, base_param_step() );
	const float l_cfm = cfm( sprng, damping, base_param_step() );

	set_stop_erp( angular_motor(), axis_num, l_erp );
	set_stop_cfm( angular_motor(), axis_num, l_cfm );
}

float	euler_hinge::get_stop_spring	( axis axis_num )
{
	float cfm = get_stop_cfm( angular_motor(), axis_num );
	float erp = get_stop_erp( angular_motor(), axis_num );

	return ::spring( cfm, erp, base_param_step() );
	

}

float	euler_hinge::get_stop_damping( axis axis_num )		
{
	float cfm = get_stop_cfm( angular_motor(), axis_num );
	float erp = get_stop_erp( angular_motor(), axis_num );

	return damping( cfm, erp );
}

float set_normal_cfm( dxJoint *amotor, axis axis_num, float cfm )
{
	switch(axis_num)
	{
		case x_axis:	dJointSetAMotorParam(amotor,dParamCFM,  cfm ); break;
		case y_axis:	dJointSetAMotorParam(amotor,dParamCFM2, cfm ); break;
		case z_axis:	dJointSetAMotorParam(amotor,dParamCFM3, cfm ); break;
		default: NODEFAULT();
	}
	return 0;
}
float get_normal_cfm( dxJoint *amotor, axis axis_num )
{
	switch(axis_num)
	{
		case x_axis:	return dJointGetAMotorParam(amotor,dParamStopCFM );
		case y_axis:	return dJointGetAMotorParam(amotor,dParamStopCFM2);
		case z_axis:	return dJointGetAMotorParam(amotor,dParamStopCFM3);
		default: NODEFAULT(return 0);
	}
}

void	euler_hinge::set_normal_spring	( axis axis_num, float spring )
{
	const float l_damping = get_normal_damping( axis_num );
	
	const float l_cfm = cfm( spring, l_damping, base_param_step() );
	//const float l_erp = erp( spring, l_damping, base_param_step() );
	set_normal_cfm( angular_motor(), axis_num, l_cfm );

}
void	euler_hinge::set_normal_damping( axis axis_num, float dumping )		
{
	const float l_spring = get_normal_spring( axis_num );
	const float l_cfm = cfm( l_spring, dumping, base_param_step() );
	set_normal_cfm( angular_motor(), axis_num, l_cfm );
}

float	euler_hinge::get_normal_spring	( axis axis_num )
{
	const float l_erp = default_erp;
	const float l_cfm = get_normal_cfm( angular_motor(), axis_num );
	
	return spring( l_cfm, l_erp, base_param_step() );

}
float	euler_hinge::get_normal_damping( axis axis_num )	
{
	const float l_erp = default_erp;
	const float l_cfm = get_normal_cfm( angular_motor(), axis_num );
	return damping( l_cfm, l_erp );
}

