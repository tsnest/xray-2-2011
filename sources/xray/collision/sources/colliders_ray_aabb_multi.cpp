////////////////////////////////////////////////////////////////////////////
//	Created		: 26.10.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "colliders_ray_aabb.h"

using xray::collision::colliders::ray_aabb_collider;
using xray::math::float3;

static inline float invert	( float const value )
{
	if ( xray::math::is_zero( value ) )
		return				( 0.f );

	return					( 1.f / value );
}

ray_aabb_collider::ray_aabb_collider			(
		float3 const& origin,
		float3 const& direction
	) :
	m_direction				( direction )
{
	( float3& )m_origin				= origin;
	m_origin.padding				= 0.f;

	m_direction.normalize			( );

	// division by zero here is acceptable!
	(float3&)m_inverted_direction	= float3( invert( m_direction.x ), invert( m_direction.y ), invert( m_direction.z ) );
	m_inverted_direction.padding	= 0.f;
}

using xray::collision::colliders::sse::aabb_a16;

bool ray_aabb_collider::intersects_aabb_sse	( aabb_a16 const& aabb, float& distance ) const
{
	XRAY_UNREFERENCED_PARAMETERS	( &aabb, distance );
	return false;
}