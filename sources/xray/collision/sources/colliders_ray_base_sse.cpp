////////////////////////////////////////////////////////////////////////////
//	Created 	: 09.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "colliders_ray_sse.h"
#include "colliders_ray.h"

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

static const float flt_plus_inf = xray::math::infinity;		// let's keep C and C++ compilers happy.
static const float _MM_ALIGN16 ps_cst_plus_inf[4] = {
	flt_plus_inf, 
	flt_plus_inf,
	flt_plus_inf,
	flt_plus_inf
};
static const float _MM_ALIGN16 ps_cst_minus_inf[4] = {
	-flt_plus_inf,
	-flt_plus_inf,
	-flt_plus_inf,
	-flt_plus_inf
};

using xray::collision::colliders::ray_base;
using xray::collision::colliders::sse::aabb_a16;

bool ray_base::intersects_aabb_sse	( aabb_a16 const& aabb, float& distance )
{
	// you may already have those values hanging around somewhere
	const __m128
		plus_inf	= LOADPS(ps_cst_plus_inf),
		minus_inf	= LOADPS(ps_cst_minus_inf);

	// use whatever's apropriate to load.
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
}