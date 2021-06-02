////////////////////////////////////////////////////////////////////////////
//	Created		: 11.02.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COLLIDERS_RAY_BASE_H_INCLUDED
#define COLLIDERS_RAY_BASE_H_INCLUDED

#include <xray/collision/object.h>

namespace xray {
namespace collision {
namespace colliders {

namespace sse {

struct XRAY_ALIGN(16) float3_a16 : public math::float3 {
	float		padding;
}; // struct float3_a16

struct  aabb_a16 {
	float3_a16	min;
	float3_a16	max;
}; // struct aabb_a16


inline void	construct_aabb_a16( aabb_a16 &result, float3 const& center, float3 const& extents )
{
	(float3&)result.min		= center - extents;
	result.min.padding		= 0.f;

	(float3&)result.max		= center + extents;
	result.max.padding		= 0.f;
}

} // namespace sse



class ray_aabb_collider : private boost::noncopyable {

public:
					ray_aabb_collider	(
											float3 const& origin,
											float3 const& direction
										);

public:
	bool		intersects_aabb_sse		( sse::aabb_a16 const& aabb, float& distance ) const;
	bool		intersects_aabb_vertical( float3 const&	center, float3 const& extents, float& distance ) const;

	sse::float3_a16 const&	origin		()const { return m_origin; }
	float3			const&	direction	()const { return m_direction; }

protected:
	sse::float3_a16		m_origin;

private:
	sse::float3_a16		m_inverted_direction;

protected:
	float3				m_direction;
}; // class rray_aabb_collider

} // namespace colliders
} // namespace collision
} // namespace xray

#endif // #ifndef COLLIDERS_RAY_BASE_H_INCLUDED