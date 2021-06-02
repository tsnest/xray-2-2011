////////////////////////////////////////////////////////////////////////////
//	Created		: 16.04.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef JOINT_SINGLE_BASE_IMPL_H_INCLUDED
#define JOINT_SINGLE_BASE_IMPL_H_INCLUDED

#include "joint_base_impl.h"
#include "joint.h"

class joint_single_base_impl:

	public		joint,
	protected	joint_base_impl
{

protected:
	dxJoint*		m_joint;

public:
	joint_single_base_impl( dxJoint *j ): m_joint( j ){ ASSERT(j); }

public:
	virtual	void	connect		( body	*b0, body	*b1 )					;
	virtual	void	connect		( body	*b )								;

private:
	virtual	void	integrate	( float	time_delta )						;
	virtual	void	fill_island ( island	&i )							;

private:
	virtual	void	render		( xray::render::scene_ptr const& scene, xray::render::debug::renderer& renderer ) const	;

}; // class joint_single_base_impl

#endif // #ifndef JOINT_SINGLE_BASE_IMPL_H_INCLUDED