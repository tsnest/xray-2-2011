////////////////////////////////////////////////////////////////////////////
//	Created 	: 11.02.2008
//	Author		: Konstantin Slipchenko
//	Description : joint_ode
////////////////////////////////////////////////////////////////////////////

#include	"pch.h"
#include	"joint_ode.h"
#include	"ode_include.h"

//#include	"body.h"
//#include	"island.h"
//joint_ode::~joint_ode()

void	delete_joint	( dxJoint	* &j )
{
	ASSERT( j );
	dJointDestroy( j );
	j = 0;
}

dxJoint	*new_ode_ball()
{
	return dJointCreateBall(0,0);
}

dxJoint *new_ode_hinge()
{
	return dJointCreateHinge(0,0);
}
dxJoint *new_ode_angular_motor()
{
	return dJointCreateAMotor( 0,0);
}
dxJoint	*new_ode_hinge2()
{
	return dJointCreateHinge2(0,0);
}

dxJoint	*new_ode_slider()
{
	return dJointCreateSlider(0,0);
}

dxJoint	*new_ode_fixed()
{
	return dJointCreateFixed(0,0);
}
