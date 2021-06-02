////////////////////////////////////////////////////////////////////////////
//	Created		: 31.05.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/math_curve.h>

namespace xray {
namespace math {

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

float get_tangent_from_2d_vector( float2 const& vec )
{
	float t = vec.y / xray::math::max(vec.x, xray::math::epsilon_5);
	return t;
}


enum_evaluate_type string_to_evaluate_type(pcstr name)
{
		 if	(xray::strings::compare(name, "Age")==0)	return age_evaluate_type;
	else if (xray::strings::compare(name, "Random")==0)	return random_evaluate_type;
	
	return age_evaluate_type;
}

} // namespace math
} // namespace xray
