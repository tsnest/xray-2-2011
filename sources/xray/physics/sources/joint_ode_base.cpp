////////////////////////////////////////////////////////////////////////////
//	Created 	: 23.02.2008
//	Author		: Konstantin Slipchenko
//	Description : joint_ode_base
////////////////////////////////////////////////////////////////////////////
#include	"pch.h"
#include	"joint_ode_base.h"
//#include	"joint_ode.h"
#include	"ode_include.h"
#include	"body.h"
#include	"island.h"

void	connect( dxJoint* dx_joint, body	*b0, body	*b1 )	
{
	
	dxBody	*d_b0 = b0->ode( );
	ASSERT( d_b0, "b0 has no ode!" );
	dxBody	*d_b1 = b1->ode( );
	ASSERT( d_b1, "b1 has no ode!" );

	dJointAttach( dx_joint, d_b0, d_b1 );

}

void	connect( dxJoint* dx_joint, body	*b )
{
	ASSERT( b );
	dxBody	*d_b = b->ode( );
	ASSERT( d_b, "b has no ode!" );

	dJointAttach( dx_joint, 0, d_b );
	//dJointAttach( dx_joint, d_b, 0 ); //?
}

//void	joint_ode_base::integrate	( float	time_delta )
//{
//
//}


void	fill_island( dxJoint* dx_joint, island	&i )
{
	i.ode().add_joint( dx_joint );
}

void detach( dxJoint* dx_joint )
{
	removeJointReferencesFromAttachedBodies( dx_joint );
}



