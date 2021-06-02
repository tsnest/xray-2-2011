////////////////////////////////////////////////////////////////////////////
//	Created 	: 09.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COLLIDERS_RAY_H_INCLUDED
#define COLLIDERS_RAY_H_INCLUDED

#include "colliders_ray_sse.h"

namespace Opcode {
	class AABBNoLeafNode;
} // namespace Opcode

namespace xray {
namespace collision {

namespace detail {
	class cell;
} // namespace detail

namespace colliders {

class ray : private boost::noncopyable
{
public:
						ray						(
							query_type const query_type,
							detail::cell const& cell,
							float3 const& origin,
							float3 const& direction,
							float const max_range,
							Results& results,
							Predicate const& predicate
						);
			bool		query					();
	static	bool		test_triangle			(
							float3 const& v0,
							float3 const& v1,
							float3 const& v2,
							float3 const& position,
							float3 const& direction,
							float const max_range,
							float& range
						);

private:
			bool		intersects_aabb_sse		(sse::aabb_a16 const& aabb, float& distance) const;
			bool		intersects_aabb			(Opcode::AABBNoLeafNode const* node, float& distance);
			bool		test_triangle			(u32 const& triangle, float& range);
			void		test_primitive			(u32 const& triangle);
			void		query					(Opcode::AABBNoLeafNode const* node);

private:
			bool		intersects_aabb_vertical(Opcode::AABBNoLeafNode const* node);
			void		query_vertical			(Opcode::AABBNoLeafNode const* node);

private:
	_MM_ALIGN16
		sse::float3_a16	m_origin;

	_MM_ALIGN16
		sse::float3_a16	m_inverted_direction;

	float3				m_direction;
	detail::cell const&	m_cell;
	Results&			m_results;
	Predicate const&	m_predicate;
	float				m_max_range;
}; // class ray

} // namespace colliders
} // namespace collision
} // namespace xray

#endif // #ifndef COLLIDERS_RAY_H_INCLUDED