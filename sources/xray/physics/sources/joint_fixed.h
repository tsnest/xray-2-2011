////////////////////////////////////////////////////////////////////////////
//	Created		: 22.05.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef JOINT_FIXED_H_INCLUDED
#define JOINT_FIXED_H_INCLUDED
#include "joint_single_base_impl.h"

class joint_fixed :
	public joint_single_base_impl
{

public:
					joint_fixed		( );
	virtual			~joint_fixed	( );

private:
	virtual	void	set_anchor		( float3 const& anchor );
	virtual	void	get_anchor		( float3& anchor ) const;

private:
	static	dxJoint* new_ode_joint	( );

}; // class joint_fixed

#endif // #ifndef JOINT_FIXED_H_INCLUDED