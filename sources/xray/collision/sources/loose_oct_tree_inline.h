////////////////////////////////////////////////////////////////////////////
//	Created		: 10.02.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef LOOSE_OCT_TREE_INLINE_H_INCLUDED
#define LOOSE_OCT_TREE_INLINE_H_INCLUDED

namespace xray {
namespace collision {

inline u8 octant_index										( float3 const& signs )
{
	return	(signs.x  < 0.f ? 0 : 1) | ( (signs.y < 0.f ? 0 : 1) << 1 ) | ( (signs.z < 0.f ? 0 : 1) << 2 );
}

inline float3 octant_vector									( ptrdiff_t const octant_index )
{
	return	float3( (octant_index & 0x1) ? 1.f : -1.f, (octant_index & 0x2) ? 1.f : -1.f, (octant_index & 0x4) ? 1.f : -1.f );
}

inline non_null<oct_node const>::ptr loose_oct_tree::root	( ) const
{
	ASSERT	( m_initialized );
	return	m_root;
}

inline float3 const& loose_oct_tree::aabb_center			( ) const
{
	return	m_aabb_center;
}

inline float loose_oct_tree::aabb_extents					( ) const
{
	return	m_aabb_extents;
}

template < typename P >
inline void loose_oct_tree::for_each_iterate				(
		P const& predicate,
		non_null< oct_node const>::ptr const node,
		float3 const& aabb_center,
		float const aabb_extents
	) const
{
	ASSERT					( m_initialized );

	float const octant_radius = aabb_extents/2.f;

	oct_node const* const* const b	= node->octants;
	oct_node const* const* i		= b;
	oct_node const* const* const e	= b + 8;
	for ( ; i != e; ++i ) {
		if ( !*i )
			continue;

		for_each_iterate	( predicate, *i, aabb_center + octant_vector(i - b)*octant_radius, octant_radius );
	}

	object* object			= node->objects;
	while ( object ) {
		predicate			( object );
		object				= object->m_next;
	}
}

template < typename P >
inline void loose_oct_tree::for_each						( P const& predicate ) const
{
	for_each_iterate		( predicate, m_root, m_aabb_center, m_aabb_extents );
}

} // namespace collision
} // namespace xray

#endif // #ifndef LOOSE_OCT_TREE_INLINE_H_INCLUDED