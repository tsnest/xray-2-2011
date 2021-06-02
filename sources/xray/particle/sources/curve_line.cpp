////////////////////////////////////////////////////////////////////////////
//	Created		: 03.08.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "curve_line.h"

namespace xray {
namespace particle {

float get_tangent_from_2d_vector(xray::math::float2 const& vec)
{
	float t = vec.y / xray::math::max(vec.x, xray::math::epsilon_5);
	return t;
}

} // namespace particle
} // namespace xray
