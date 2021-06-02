////////////////////////////////////////////////////////////////////////////
//	Created		: 27.04.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef JOINT_DOUBLE_BASE_IMPL_H_INCLUDED
#define JOINT_DOUBLE_BASE_IMPL_H_INCLUDED

#include "joint_base_impl.h"
#include "joint.h"

struct dxJoint;
class  body;
class  island;

class joint_double_base_impl: 
	public joint,
	protected joint_base_impl
{

protected:

dxJoint	*m_joint0;
dxJoint	*m_joint1;

public:
					joint_double_base_impl	( dxJoint	*joint0, dxJoint *joint1 );
					~joint_double_base_impl	();

public:
	virtual	void	connect		( body	*b0, body	*b1 )					;
	virtual	void	connect		( body	*b )								;

private:
	virtual	void	integrate	( float	time_delta )						;
	virtual	void	fill_island ( island	&i )							;

private:
	virtual	void	render		( xray::render::scene_ptr const& scene, xray::render::debug::renderer& renderer ) const	;

}; // class joint_double_base_imple

#endif // #ifndef JOINT_DOUBLE_BASE_IMPLE_H_INCLUDED