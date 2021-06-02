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

struct aabb_a16 {
	float3_a16	min;
	float3_a16	max;
}; // struct aabb_a16

} // namespace sse

class ray_base : private boost::noncopyable {
public:
						ray_base			(
							float3 const& origin,
							float3 const& direction,
							float const max_distance
						);

protected:
			bool		intersects_aabb_sse		( sse::aabb_a16 const& aabb, float& distance ) const;

protected:
	sse::float3_a16		m_origin;

private:
	sse::float3_a16		m_inverted_direction;

protected:
	float3				m_direction;
	float				m_max_distance;
}; // class ray_base

} // namespace colliders
} // namespace collision
} // namespace xray

#endif // #ifndef COLLIDERS_RAY_BASE_H_INCLUDED