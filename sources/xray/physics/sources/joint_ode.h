////////////////////////////////////////////////////////////////////////////
//	Created 	: 11.02.2008
//	Author		: Konstantin Slipchenko
//	Description : joint_ball
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_PHYSICS_JOINT_ODE_H_INCLUDED
#define XRAY_PHYSICS_JOINT_ODE_H_INCLUDED

struct dxJoint;

dxJoint	*new_ode_ball			( );
dxJoint *new_ode_hinge			( );
dxJoint *new_ode_angular_motor	( );
dxJoint	*new_ode_hinge2			( );
dxJoint	*new_ode_slider			( );
dxJoint	*new_ode_fixed			( );
void	delete_joint			( dxJoint	* &j );


#endif