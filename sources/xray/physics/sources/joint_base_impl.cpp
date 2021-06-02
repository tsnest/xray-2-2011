////////////////////////////////////////////////////////////////////////////
//	Created 	: 25.02.2008
//	Author		: Konstantin Slipchenko
//	Description : joint_base_impl
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "joint_base_impl.h"
#include "joint_ode_base.h"
#include "body.h"
#include <xray/render/facade/debug_renderer.h>

joint_base_impl::~joint_base_impl( )	
{ 
	ASSERT( !m_body_1 );
	ASSERT( !m_body_0 );
}

void	joint_base_impl::connect( dxJoint *j, body	*b0, body	*b1 )
{
	//ASSERT( !m_body_1 );
	//ASSERT( !m_body_0 );
	m_body_0 = b0;
	m_body_1 = b1;
	::connect( j, b0, b1 ); 
}

void	joint_base_impl::connect( dxJoint *j, body	*b )
{
	ASSERT( !m_body_1 );
	ASSERT( !m_body_0 );
	m_body_0 = b;
	::connect( j, b ); 
}

void	joint_base_impl::integrate( dxJoint *j, float time_delta )
{
	XRAY_UNREFERENCED_PARAMETERS(j, time_delta);
	//::integrate( j, time_delta );
}

void	joint_base_impl::fill_island( dxJoint *j, island	&i )
{
	::fill_island( j, i );
}
void	joint_base_impl::detach( dxJoint *j )
{
	//no virtual calls
	m_body_0 = m_body_1 = 0;
	::detach( j );
}


void joint_base_impl::render		( const dxJoint *j, render::scene_ptr const& scene, xray::render::debug::renderer& renderer ) const
{
	XRAY_UNREFERENCED_PARAMETER(j);
	if( m_body_0 && m_body_1 )
	{
		const float3 &p0 = m_body_0->get_pose().c.xyz();
		const float3 &p1 = m_body_1->get_pose().c.xyz();
		renderer.draw_arrow( scene, p0, p1, math::color( 0u, 255u, 255u, 255u ) );
	}
}