////////////////////////////////////////////////////////////////////////////
//	Created		: 27.04.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "joint_slider_hinge.h"
#include "joint_ode.h"
joint_slider_hinge::joint_slider_hinge	( ):
joint_double_base_impl(
					   new_ode_slider( ),
					   new_ode_angular_motor( )
					   )
{
}

void joint_slider_hinge::set_anchor ( const float3	&anchor )		
{
	XRAY_UNREFERENCED_PARAMETER( anchor );
}

void joint_slider_hinge::get_anchor ( float3	&anchor )const		
{
	XRAY_UNREFERENCED_PARAMETER( anchor );
}

joint_slider_hinge::~joint_slider_hinge	()
{
 	detach( m_joint0 );
	delete_joint( m_joint0 );

	detach( m_joint1 );
	delete_joint( m_joint1 );
}