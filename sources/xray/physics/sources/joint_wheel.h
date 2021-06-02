////////////////////////////////////////////////////////////////////////////
//	Created		: 27.04.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef JOINT_WHEEL_H_INCLUDED
#define JOINT_WHEEL_H_INCLUDED
#include "joint_single_base_impl.h"
class joint_wheel:
	public joint_single_base_impl
{

public:
					joint_wheel	();
	virtual			~joint_wheel();
	virtual	void	set_anchor  ( const float3	&anchor );
	virtual	void	get_anchor  ( float3	&anchor )const;

}; // class joint_wheel

#endif // #ifndef JOINT_WHEEL_H_INCLUDED