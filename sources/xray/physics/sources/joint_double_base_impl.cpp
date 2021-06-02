////////////////////////////////////////////////////////////////////////////
//	Created		: 27.04.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "joint_double_base_impl.h"

joint_double_base_impl::joint_double_base_impl( dxJoint	*joint0, dxJoint *joint1 ):
 m_joint0( joint0 ), m_joint1(joint1)
{

}

joint_double_base_impl::~joint_double_base_impl( )
{
	ASSERT( !m_joint0 );
	ASSERT( !m_joint1 );
}


void	joint_double_base_impl::connect( body	*b0, body	*b1 )
{
	joint_base_impl::connect( m_joint0, b0, b1 );
	joint_base_impl::connect( m_joint1, b0, b1 );
}

void	joint_double_base_impl::connect( body	*b )
{
	joint_base_impl::connect( m_joint0, b );
	joint_base_impl::connect( m_joint1, b );
}

void	joint_double_base_impl::integrate( float	time_delta )
{
	joint_base_impl::integrate( m_joint0, time_delta ); // :connect( m_joint0, b0, b1 );
	joint_base_impl::integrate( m_joint1, time_delta );
	//joint_base_impl::connect( m_joint1, b0, b1 );
}

void	joint_double_base_impl::fill_island( island	&i )
{
	joint_base_impl::fill_island( m_joint0, i );
	joint_base_impl::fill_island( m_joint1, i );
}

void	joint_double_base_impl::render( xray::render::scene_ptr const& scene, xray::render::debug::renderer& renderer ) const
{
	joint_base_impl::render( m_joint0, scene, renderer );
	joint_base_impl::render( m_joint1, scene, renderer );
}