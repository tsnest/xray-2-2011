////////////////////////////////////////////////////////////////////////////
//	Created		: 13.02.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COLLIDERS_AABB_BASE_H_INCLUDED
#define COLLIDERS_AABB_BASE_H_INCLUDED

namespace xray {
namespace collision {
namespace colliders {

class aabb_base : private boost::noncopyable {
public:
	inline					aabb_base			( math::aabb const& aabb );
	inline	bool			result				( ) const;

protected:
	static	inline	bool	completely_inside	( math::aabb const& self, float3 const& center, float3 const& extents );
	static	inline	bool	out_of_bounds		( math::aabb const& self, float3 const& center, float3 const& extents );

protected:
	math::aabb const&		m_aabb;
	bool					m_result;
}; // class aabb_base

} // namespace colliders
} // namespace collision
} // namespace xray

#include "colliders_aabb_base_inline.h"

#endif // #ifndef COLLIDERS_AABB_BASE_H_INCLUDED