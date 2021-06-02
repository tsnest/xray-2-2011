////////////////////////////////////////////////////////////////////////////
//	Created		: 03.08.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "help_functions.h"
#include <xray/math_randoms_generator.h>

namespace xray {
namespace particle {

static xray::math::random32 randomizer(100);
static u32 const random_max = 65535;

float random_float(float min_value, float max_value)
{
	float alpha = float(randomizer.random(random_max)) / float(random_max);
	return linear_interpolation(min_value,max_value,alpha);
}

float3 random_float3(float3 const& min_value, float3 const& max_value)
{
	return float3(random_float(min_value.x, max_value.x),
				  random_float(min_value.y, max_value.y),
				  random_float(min_value.z, max_value.z));
}

} // namespace particle
} // namespace xray
