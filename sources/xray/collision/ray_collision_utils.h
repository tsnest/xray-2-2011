////////////////////////////////////////////////////////////////////////////
//	Created		: 03.09.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_COLLISION_RAY_COLLISION_UTILS_H_INCLUDED
#define XRAY_COLLISION_RAY_COLLISION_UTILS_H_INCLUDED

namespace xray {
namespace collision {

inline	bool	test_triangle			(
						float3 const& v0,
						float3 const& v1,
						float3 const& v2,
						float3 const& position,
						float3 const& direction,
						float const max_distance,
						float& range
					);

} // namespace collision
} // namespace xray
#include <xray/collision/ray_collision_utils_inline.h>
#endif // #ifndef XRAY_COLLISION_RAY_COLLISION_UTILS_H_INCLUDED