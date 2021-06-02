////////////////////////////////////////////////////////////////////////////
//	Created 	: 11.02.2008
//	Author		: Konstantin Slipchenko
//	Description : joint_ball
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_PHYSICS_JOINT_BALL_H_INCLUDED
#define XRAY_PHYSICS_JOINT_BALL_H_INCLUDED
#include "joint_single_base_impl.h"

struct dxJoint;
class joint_ball:
		public joint_single_base_impl
{
public:
					joint_ball		( );
	virtual			~joint_ball		( );

private:
	virtual	void	set_anchor		( const float3	&anchor );
	virtual	void	get_anchor		( float3	&anchor )const;

private:
	static	dxJoint*new_ode_joint	( );
	//virtual	joint_ode_base&		get_joint ( )	{ return m_joint; }

}; // class joint_ball

#endif