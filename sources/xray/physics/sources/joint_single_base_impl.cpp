////////////////////////////////////////////////////////////////////////////
//	Created		: 16.04.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "joint_single_base_impl.h"

void	joint_single_base_impl::connect( body	*b0, body	*b1 )
{
	joint_base_impl::connect( m_joint, b0, b1 );
}

void	joint_single_base_impl::connect( body	*b )
{
	joint_base_impl::connect( m_joint, b );
}

void	joint_single_base_impl::fill_island( island	&i )
{
	joint_base_impl::fill_island( m_joint, i );
}

void	joint_single_base_impl::render( xray::render::scene_ptr const& scene, xray::render::debug::renderer& renderer )const
{
	joint_base_impl::render( m_joint, scene, renderer );
}

void	joint_single_base_impl::integrate( float	time_delta )
{
	joint_base_impl::integrate( m_joint, time_delta );
}