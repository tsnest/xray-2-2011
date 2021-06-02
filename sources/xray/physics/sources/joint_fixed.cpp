////////////////////////////////////////////////////////////////////////////
//	Created		: 22.05.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "joint_fixed.h"

#include	"ode_include.h"
#include	"ode_convert.h"
#include	"joint_ode.h"

dxJoint* joint_fixed::new_ode_joint()
{
	return new_ode_fixed();
}

joint_fixed::joint_fixed():
	joint_single_base_impl( new_ode_joint() )
{
	
}

joint_fixed::~joint_fixed()
{
	detach( m_joint );
	delete_joint( m_joint );
}



void	joint_fixed::set_anchor  ( const float3	&/*anchor*/ )
{
	//dJointSetFixedAnchor( m_joint, anchor.x, anchor.y, anchor.z ) ;
	dJointSetFixed( m_joint );
	//R_ASSERT(false);
}

void	joint_fixed::get_anchor  ( float3	&anchor )const
{
	XRAY_UNREFERENCED_PARAMETER	(anchor);
	//dVector3 res;
	//dJointGetFixedAnchor( dJointID( m_joint ), res );
	//anchor = cast_xr( res );
	R_ASSERT(false);
}