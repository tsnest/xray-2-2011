////////////////////////////////////////////////////////////////////////////
//	Created		: 03.08.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef HELP_FUNCTIONS_H_INCLUDED
#define HELP_FUNCTIONS_H_INCLUDED

namespace xray {
namespace particle {

template<class T> T linear_interpolation(T a, T b, float alpha)
{
	return a * (1 - alpha) + b * alpha;
}

template<class T> T bilinear_interpolation(T a0, T b0, T a1, T b1, float alpha0, float alpha1)
{
	T const upper_result	= linear_interpolation(a0, b0, alpha0);
	T const lower_result	= linear_interpolation(a1, b1, alpha0);
	T const result			= linear_interpolation(upper_result, lower_result, alpha1);
	
	return					result;
}

//http://en.wikipedia.org/wiki/Cubic_Hermite_spline
template<class T, class T2> T cubic_interpolation( T P0, T M0, T P1, T M1, T2 t )
{
	const T2 t_pow_2 = t * t,
			 t_pow_3 = t * t * t;
	
	return  P0 * (2.0f * t_pow_3 - 3.0f * t_pow_2 + 1.0f)	+
			M0 * (t_pow_3 - 2.0f * t_pow_2 + t)				+
			P1 * ( - 2.0f * t_pow_3 + 3.0f * t_pow_2)		+
			M1 * (t_pow_3 - t_pow_2);
}

float random_float(float min_value, float max_value);
float3 random_float3(float3 const& min_value, float3 const& max_value);

template<class T, class ConfigValueType> T read_config_value(ConfigValueType const& config_value, pcstr name, T const& default_value)
{
	if (config_value.value_exists(name))
		return (T)config_value[name];
	else
		return default_value;
}

} // namespace particle
} // namespace xray

#endif // #ifndef HELP_FUNCTIONS_H_INCLUDED