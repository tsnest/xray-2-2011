////////////////////////////////////////////////////////////////////////////
//	Created 	: 23.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MATH_FRUSTUM_H_INCLUDED
#define XRAY_MATH_FRUSTUM_H_INCLUDED

#include <xray/math_aabb_plane.h>
#include <xray/math_cuboid.h>

namespace xray {
namespace math {

class XRAY_CORE_API frustum : public cuboid {
public:
	inline				frustum		( ) { }
						frustum		( float4x4 const& view_multiplied_by_projection );
}; // class frustum

} // namespace math
} // namespace xray

#endif // #ifndef XRAY_MATH_FRUSTUM_H_INCLUDED