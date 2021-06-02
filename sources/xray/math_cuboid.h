////////////////////////////////////////////////////////////////////////////
//	Created		: 18.02.2009
//	Author		: Dmitriy Iassenev
//	Description	: a cuboid is a solid figure bounded by six faces, forming a convex polyhedron
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MATH_CUBOID_H_INCLUDED
#define XRAY_MATH_CUBOID_H_INCLUDED
	
#include <xray/math_aabb_plane.h>

namespace xray {
namespace math {

class XRAY_CORE_API cuboid {
public:
	enum {
		plane_count	= 6,
	}; // enum

	typedef aabb_plane const (planes_type)[plane_count];

public:
	inline					cuboid		( ) { }
							cuboid		( plane const (&planes)[plane_count] );
							cuboid		( aabb const& aabb, float4x4 const& matrix );
							cuboid		( cuboid const& other, float4x4 const& matrix );
			intersection	test_inexact( aabb const& aabb) const;
//.			intersection	test		( aabb const& aabb) const; not implemented
			intersection	test		( sphere const& sphere ) const;
	inline	planes_type const& planes	( ) const { return m_planes; }

protected:
	aabb_plane		m_planes[ plane_count ];
}; // class cuboid

} // namespace math
} // namespace xray

#endif // #ifndef XRAY_MATH_CUBOID_H_INCLUDED