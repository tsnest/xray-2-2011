////////////////////////////////////////////////////////////////////////////
//	Created		: 23.12.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "colliders_ray_aabb.h"

using xray::collision::colliders::ray_aabb_collider;
using xray::math::float3;

bool ray_aabb_collider::intersects_aabb_vertical	(
		float3 const& center, 
		float3 const& extents, 
		float& distance
	) const
{
#if 1
	if ( center.x + extents.x < m_origin.x )
		return				false;  			   	
												   
	if ( center.z + extents.z < m_origin.z )
		return				false;
	
	if (center.x - extents.x > m_origin.x)
		return				false;  			   	
												   
	if (center.z - extents.z > m_origin.z)
		return				false;

	if ( m_origin.y >= center.y + extents.y ) {
		if ( m_direction.y < 0.f )
			distance		= math::abs( m_origin.y - (center.y + extents.y)  );
		else
			return			false;
	}
	else if ( m_origin.y <= center.y - extents.y ) {
		if ( m_direction.y > 0.f )
			distance		= math::abs( center.y - (m_origin.y + extents.y)  );
		else
			return			false;
	}
	else
		distance			= 0.f;

	return					true;
#else // #if 1
	// the same, but using generic intersection routine (for test/debug purposes)
	math::aabb const& bbox	= math::create_aabb_center_radius( center, extents );
	math::float3 intersection;
	if ( bbox.intersect( m_origin, m_direction, intersection ) )
	{
		distance			= bbox.contains( m_origin ) ? 0.f : ( intersection - m_origin ).length();
		return				true;
	}

	return					false;
#endif // #if 1
}