////////////////////////////////////////////////////////////////////////////
//	Created 	: 22.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MATH_SPHERE_H_INCLUDED
#define XRAY_MATH_SPHERE_H_INCLUDED

namespace xray {
namespace math {

class XRAY_CORE_API sphere {
public:
	typedef float			type;

public:
#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable:4201)
#endif // #if defined(_MSC_VER)
	union {
		float4_pod			vector;
		struct {
			float3_pod		center;
			type			radius;
		};
	};
#if defined(_MSC_VER)
#	pragma warning(pop)
#endif // #if defined(_MSC_VER)

	inline					sphere			( );
	inline					sphere			( float3 const& center, type radius );
			intersection	intersect_ray	( float3 const& position, float3 const& direction, type range, u32& count, type (&results)[2] ) const;
			intersection	intersect_ray	( float3 const& position, float3 const& direction, type& range ) const;
	inline	bool			intersect_ray	( float3 const& position, float3 const& direction ) const;
	inline	bool			intersect		( sphere const& other ) const;
	inline	bool			contains		( float3 const& position );
	inline	bool			contains		( sphere const& other );
	inline	bool			similar			( sphere const& other ) const;

	#ifndef MASTER_GOLD
	inline	bool			valid			( ) const;
	#endif
}; // class sphere

} // namespace math
} // namespace xray

#include <xray/math_sphere_inline.h>

#endif // #ifndef XRAY_MATH_SPHERE_H_INCLUDED