////////////////////////////////////////////////////////////////////////////
//	Created		: 11.02.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
//	Description	: http://www.flipcode.com/archives/SSE_RayBox_Intersection_Test.shtml
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

// turn those verbose intrinsics into something readable.
#define LOADPS(mem)			_mm_load_ps((const float * const)(mem))
#define STORESS(ss,mem)		_mm_store_ss((float * const)(mem),(ss))
#define MINSS				_mm_min_ss
#define MAXSS				_mm_max_ss
#define MINPS				_mm_min_ps
#define MAXPS				_mm_max_ps
#define MULPS				_mm_mul_ps
#define SUBPS				_mm_sub_ps
#define ROTATELPS(ps)		_mm_shuffle_ps((ps),(ps), 0x39)	// a,b,c,d -> b,c,d,a
#define MUXHPS(low,high)	_mm_movehl_ps((low),(high))		// low{a,b,c,d}|high{e,f,g,h} = {c,d,g,h}

static const float XRAY_ALIGN(16) ps_cst_plus_inf[4] = {
	xray::math::infinity, 
	xray::math::infinity,
	xray::math::infinity,
	xray::math::infinity
};
static const float XRAY_ALIGN(16) ps_cst_minus_inf[4] = {
	-xray::math::infinity,
	-xray::math::infinity,
	-xray::math::infinity,
	-xray::math::infinity
};

using xray::collision::colliders::sse::aabb_a16;

bool ray_aabb_collider::intersects_aabb_sse	( aabb_a16 const& aabb, float& distance ) const
{
#pragma message(XRAY_TODO("Lain 2 Arkom: link error with bbox.intersect( m_origin, direction, intersection_point ) "))

#if 0
	//That function return incorrect result with following arguments
	//AABB:	center		{x=0.00000000 y=1.6431688 z=0.00000000}		xray::math::float3
	//		extents		{x=3.6015866 y=1.6431690 z=2.2099221}		xray::math::float3
	//Ray:	origin		{ x=3.6265864 y=0.025000000 z=1.4092371}	xray::math::float3
	//		direction	{ x=-1.0000000 y=0.00000000 z=0.00000000}	xray::math::float3


	// you may already have those values hanging around somewhere
	const __m128
		plus_inf	= LOADPS(ps_cst_plus_inf),
		minus_inf	= LOADPS(ps_cst_minus_inf);

	// use whatever's appropriate to load.
	const __m128
		box_min	= LOADPS(&aabb.min),
		box_max	= LOADPS(&aabb.max),
		pos	= LOADPS(&m_origin),
		inv_dir	= LOADPS(&m_inverted_direction);

	// use a div if inverted directions aren't available
	const __m128 l1 = MULPS(SUBPS(box_min, pos), inv_dir);
	const __m128 l2 = MULPS(SUBPS(box_max, pos), inv_dir);

	// the order we use for those min/max is vital to filter out
	// NaNs that happens when an inv_dir is +/- inf and
	// (box_min - pos) is 0. inf * 0 = NaN
	const __m128 filtered_l1a = MINPS(l1, plus_inf);
	const __m128 filtered_l2a = MINPS(l2, plus_inf);

	const __m128 filtered_l1b = MAXPS(l1, minus_inf);
	const __m128 filtered_l2b = MAXPS(l2, minus_inf);

	// now that we're back on our feet, test those slabs.
	__m128 lmax = MAXPS(filtered_l1a, filtered_l2a);
	__m128 lmin = MINPS(filtered_l1b, filtered_l2b);

	// unfold back. try to hide the latency of the shufps & co.
	const __m128 lmax0 = ROTATELPS(lmax);
	const __m128 lmin0 = ROTATELPS(lmin);
	lmax = MINSS(lmax, lmax0);
	lmin = MAXSS(lmin, lmin0);

	const __m128 lmax1 = MUXHPS(lmax,lmax);
	const __m128 lmin1 = MUXHPS(lmin,lmin);
	lmax = MINSS(lmax, lmax1);
	lmin = MAXSS(lmin, lmin1);

	const bool ret = !!_mm_comige_ss(lmax, _mm_setzero_ps()) & _mm_comige_ss(lmax,lmin);

//	STORESS(lmin, &rs.t_near);
//	STORESS(lmax, &rs.t_far);
	STORESS(lmin, &distance);

	return  ret;
#else // #if 1
	math::aabb bbox							= math::create_aabb_min_max( aabb.min, aabb.max );
	math::float3 const& direction			= float3( invert( m_inverted_direction.x ), invert( m_inverted_direction.y ), invert( m_inverted_direction.z ) );
	math::float3							intersection_point;
	math::intersection intersection_result	= bbox.intersect( m_origin, direction, intersection_point );
	if ( intersection_result != math::intersection_none )
	{
		distance = intersection_result == math::intersection_inside ? 0.0f : ( intersection_point - m_origin ).length();
	}
	return									intersection_result != math::intersection_none;
#endif // #if 1
}
