////////////////////////////////////////////////////////////////////////////
//	Created		: 03.09.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_COLLISION_RAY_COLLISION_UTILS_INLINE_H_INCLUDED
#define XRAY_COLLISION_RAY_COLLISION_UTILS_INLINE_H_INCLUDED

namespace xray {
namespace collision {

inline bool test_triangle		(
		float3 const& v0,
		float3 const& v1,
		float3 const& v2,
		float3 const& position,
		float3 const& direction,
		float const max_distance,
		float& range
	)
{
	float3 const edge1		= v1 - v0;
	float3 const edge2		= v2 - v0;
	float3 normal			= direction ^ edge2;
	float const determinant	= edge1 | normal;

	// not culling branch
	if ( math::is_zero( determinant, math::epsilon_5 ) )
		return				( false );

	float3 const			temp = position - v0;
	float const inverted_determinant = 1.f/determinant;
	float const u			= temp.dot_product(normal)*inverted_determinant;
	if ( u < 0.f )
		return				( false );

	if ( u > 1.f )
		return				( false );

	normal					= temp ^ edge1;
	float const v			= direction.dot_product(normal)*inverted_determinant;
	if ( v < 0.f )
		return				( false );

	if ( u + v > 1.f )
		return				( false );

	range					= (edge2 | normal) * inverted_determinant;
	if ( range <= 0.f )
		return				( false );

	if ( range > max_distance )
		return				( false );

	return					( true );
}

} // namespace collision
} // namespace xray

#endif // #ifndef XRAY_COLLISION_RAY_COLLISION_UTILS_INLINE_H_INCLUDED