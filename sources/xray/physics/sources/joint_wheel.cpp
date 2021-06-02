////////////////////////////////////////////////////////////////////////////
//	Created		: 27.04.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "joint_wheel.h"
#include "joint_ode.h"
joint_wheel::joint_wheel():joint_single_base_impl( new_ode_hinge2() )
{

}

joint_wheel::~joint_wheel()
{
	detach( m_joint );
	delete_joint( m_joint );
}

void joint_wheel::set_anchor ( const float3	&anchor )		
{
	XRAY_UNREFERENCED_PARAMETER	( anchor );
}

void joint_wheel::get_anchor ( float3	&anchor )const		
{
	XRAY_UNREFERENCED_PARAMETER	( anchor );
}

