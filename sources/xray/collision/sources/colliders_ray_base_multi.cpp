////////////////////////////////////////////////////////////////////////////
//	Created		: 26.10.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "colliders_ray_base.h"

using xray::collision::colliders::ray_base;
using xray::math::float3;

static inline float invert	( float const value )
{
	if ( xray::math::is_zero( value ) )
		return				( 0.f );

	return					( 1.f / value );
}

ray_base::ray_base			(
		float3 const& origin,
		float3 const& direction,
		float const max_distance
	) :
	m_direction				( direction ),
	m_max_distance			( max_distance )
{
	( float3& )m_origin				= origin;
	m_origin.padding				= 0.f;

	m_direction.normalize			( );

	// division by zero here is acceptable!
	(float3&)m_inverted_direction	= float3( invert( m_direction.x ), invert( m_direction.y ), invert( m_direction.z ) );
	m_inverted_direction.padding	= 0.f;
}

// turn those verbose intrinsics into something readable.
//#define LOADPS(mem)			_mm_load_ps((const float * const)(mem))
//#define STORESS(ss,mem)		_mm_store_ss((float * const)(mem),(ss))
//#define MINSS				_mm_min_ss
//#define MAXSS				_mm_max_ss
//#define MINPS				_mm_min_ps
//#define MAXPS				_mm_max_ps
//#define MULPS				_mm_mul_ps
//#define SUBPS				_mm_sub_ps
//#define ROTATELPS(ps)		_mm_shuffle_ps((ps),(ps), 0x39)	// a,b,c,d -> b,c,d,a
//#define MUXHPS(low,high)	_mm_movehl_ps((low),(high))		// low{a,b,c,d}|high{e,f,g,h} = {c,d,g,h}
//
//static const float XRAY_ALIGN(16) ps_cst_plus_inf[4] = {
//	xray::math::infinity, 
//	xray::math::infinity,
//	xray::math::infinity,
//	xray::math::infinity
//};
//static const float XRAY_ALIGN(16) ps_cst_minus_inf[4] = {
//	-xray::math::infinity,
//	-xray::math::infinity,
//	-xray::math::infinity,
//	-xray::math::infinity
//};

using xray::collision::colliders::sse::aabb_a16;

bool ray_base::intersects_aabb_sse	( aabb_a16 const& aabb, float& distance ) const
{
	XRAY_UNREFERENCED_PARAMETERS	( &aabb, distance );
	return false;
}