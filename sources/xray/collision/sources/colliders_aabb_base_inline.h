////////////////////////////////////////////////////////////////////////////
//	Created		: 13.02.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COLLIDERS_AABB_BASE_INLINE_H_INCLUDED
#define COLLIDERS_AABB_BASE_INLINE_H_INCLUDED

namespace xray {
namespace collision {
namespace colliders {

inline aabb_base::aabb_base				( math::aabb const& aabb ) :
	m_aabb		( aabb )
{
}

inline bool aabb_base::result			( ) const
{
	return		m_result;
}

inline bool aabb_base::completely_inside( math::aabb const& self, float3 const& center, float3 const& extents )
{
	return
		( center.x - extents.x >= self.min.x ) &&
		( center.y - extents.y >= self.min.y ) &&
		( center.z - extents.z >= self.min.z ) &&
		( center.x + extents.x <= self.max.x ) &&
		( center.y + extents.y <= self.max.y ) &&
		( center.z + extents.z <= self.max.z )
	;
}

inline bool aabb_base::out_of_bounds	( math::aabb const& self, float3 const& center, float3 const& extents )
{
	if ( self.max.x < center.x - extents.x )
		return	true;

	if ( self.max.y < center.y - extents.y )
		return	true;

	if ( self.max.z < center.z - extents.z )
		return	true;

	if ( self.min.x > center.x + extents.x )
		return	true;

	if ( self.min.y > center.y + extents.y )
		return	true;

	if ( self.min.z > center.z + extents.z )
		return	true;

	return		false;
}

} // namespace colliders
} // namespace collision
} // namespace xray

#endif // #ifndef COLLIDERS_AABB_BASE_INLINE_H_INCLUDED