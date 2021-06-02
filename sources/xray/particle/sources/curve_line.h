////////////////////////////////////////////////////////////////////////////
//	Created		: 03.08.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef CURVE_LINE_H_INCLUDED
#define CURVE_LINE_H_INCLUDED

#include "help_functions.h"
#include "evaluate_type.h"
#include <xray/math_randoms_generator.h>

namespace xray {
namespace particle {

float get_tangent_from_2d_vector(xray::math::float2 const& vec);

enum enum_evaluate_time_type
{
	linear_time_type,
	range_time_type,
};

enum enum_interpolation_type
{
	linear_interp_type,
	curve_interp_type,
	none_interp_type,
};

template<class T> struct curve_point
{
	T upper_value;
	T lower_value;
	T tangent_in;
	T tangent_out;
	
	float time;
	enum_interpolation_type interp_type;
};

// TODO:
enum enum_curve_line_type
{
	float_curve_line  = 0,
	color_curve_line,
	custom_curve_line,
};

// Rename to curve_line_points!
template<class T, enum_curve_line_type> 
struct curve_line_points
{
	typedef curve_point<T>  point_type;
	
	typedef typename xray::platform_pointer<point_type, xray::platform_pointer_default>::type point_ptr_type;
	
	~curve_line_points();
	
	void recalculate_ranges	();
	void sort_points_by_time();
	
	void set_defaults		();
	u32  save_binary		(xray::mutable_buffer& buffer, bool calc_size = false);
	void load_binary		(xray::mutable_buffer& buffer);
	
	template < class ConfigValueType >
	void load				(ConfigValueType config);
	
	void reserve			(u32 num, bool mt_alloc = true);
	
	void clear				();
	
	void set_point			(u32 index, point_type point);
	void get_point			(u32 index, point_type& out_point) const;
	void set_interp_type	(u32 index, enum_interpolation_type type);
	void set_point			(u32 index, T value, T tangent_in, T tangent_out, float time, enum_interpolation_type type = curve_interp_type);
	
	u32 get_left_point_index(float time);
	
	// range_alpha:			[0, 1]
	T evaluate				(float time, T default_value, enum_evaluate_time_type time_type = range_time_type, float left_range_alpha = 0.0f, float right_range_alpha = 0.0f) const;
	
	void calc_tangents		();
	
	float					curve_time_min,
							curve_time_max;
	
	T						curve_value_min,
							curve_value_max;
private:
	void calc_tangent		(T const& prev_value, T const& value, T const& next_value, T& out_new_tangent);
	
	point_ptr_type			points;
	u32						num_points;
}; // struct curve_line_points
 
typedef curve_line_points<float, float_curve_line>						curve_line_float;
typedef curve_line_points<xray::math::float3_pod, float_curve_line>		curve_line_float3;
typedef curve_line_points<xray::math::float4_pod, float_curve_line>		curve_line_float4;
typedef curve_line_points<xray::math::float4_pod, color_curve_line>		curve_line_color_base;

struct curve_line_color: public curve_line_color_base
{
	curve_line_color()
	{
		m_evaluate_type = age_evaluate_type;
	}
	template<class ConfigValueType>
	inline void load(ConfigValueType config)
	{
		if (config.value_exists("Input"))
			m_evaluate_type = string_to_evaluate_type(pcstr(config["Input"]));
		
		if (config.value_exists("source") && 
			config["source"].value_exists("data") &&
			config["source"]["data"].value_exists("ramp"))
		curve_line_color_base::load<ConfigValueType>(config["source"]["data"]["ramp"]);
	}
	xray::math::float4 evaluate(float time, xray::math::float4 default_value, enum_evaluate_time_type time_type = range_time_type) const
	{
		switch (m_evaluate_type)
		{
			case age_evaluate_type:
				return curve_line_color_base::evaluate(time, default_value, time_type);
			case random_evaluate_type:
				return curve_line_color_base::evaluate(
					random_float(curve_time_min, curve_time_max),
					default_value,
					range_time_type
					);
			default: NODEFAULT( return default_value );
		}
	}
private:
	enum_evaluate_type m_evaluate_type;
}; // struct color_curve

// TODO: optimize!
template<class T, enum_curve_line_type value_type> T evaluate_value(curve_line_points<T, value_type> const& upper, 
																	curve_line_points<T, value_type> const& lower, 
																	float time, 
																	T default_value, 
																	enum_evaluate_type evaluate_type, 
																	enum_evaluate_time_type time_type = range_time_type)
{
	switch (evaluate_type)
	{
		case age_evaluate_type:
			return linear_interpolation(
				upper.evaluate(time, default_value, time_type), 
				lower.evaluate(time, default_value, time_type), 
				random_float(0.0f, 1.0f)
				);
		case random_evaluate_type:
			time = linear_interpolation(
						xray::math::min(upper.curve_time_min, lower.curve_time_min),
						xray::math::max(upper.curve_time_max, lower.curve_time_max), 
						random_float(0.0f, 1.0f)
						);
			return linear_interpolation(
				upper.evaluate(time, default_value, range_time_type), 
				lower.evaluate(time, default_value, range_time_type), 
				random_float(0.0f, 1.0f)
				);
		default: NODEFAULT( return default_value );
	}
}

struct curve_line_ranged_base
{
private:
	friend struct curve_line_ranged_float;
	friend struct curve_line_ranged_xyz_float;
	
	curve_line_ranged_base() {}
	inline void set_defaults()
	{
		m_upper.set_defaults();
		m_lower.set_defaults();
	}
	inline u32 save_binary(xray::mutable_buffer& buffer, bool calc_size = false)
	{
		return m_upper.save_binary(buffer, calc_size) +
			   m_lower.save_binary(buffer, calc_size);
	}
	inline void load_binary(xray::mutable_buffer& buffer)
	{
		m_upper.load_binary(buffer);
		m_lower.load_binary(buffer);
	}
	template<class ConfigValueType>
	inline void load(ConfigValueType config)
	{
		ConfigValueType const& data = config["data"];
		if (data.value_exists("curve0"))
			m_upper.load(data["curve0"]);
		
		//if (data.value_exists("curve1"))
		//	m_lower.load(data["curve1"]);		
		//else
		//	m_lower.load(data["curve0"]);	
	}
	inline void reserve(u32 num_points)
	{
		m_upper.reserve(num_points);
	}
	inline void clear()
	{
		m_lower.clear();
	}
	inline float evaluate(float time, float default_value, enum_evaluate_type evaluate_type, enum_evaluate_time_type time_type, u32 seed)
	{
		xray::math::random32 rnd;
		rnd.seed( seed );
		
		u32 const index = m_upper.get_left_point_index(time);
		
		// Must be!!!
#if 0
		float temp_value;
		for (u32 random_index=0; random_index<index; random_index++)
			temp_value		= float(rnd.random(65535)) / 65535.0f;
		XRAY_UNREFERENCED_PARAMETER	( temp_value );
#else // #if 0
		for (u32 random_index=0; random_index<index; random_index++)
			rnd.random		( 65535 );
#endif // #if 0

		float const alpha0 = float(rnd.random(65535)) / 65535.0f;
		float const alpha1 = float(rnd.random(65535)) / 65535.0f;
		
		switch (evaluate_type)
		{
			case age_evaluate_type:
			return m_upper.evaluate(time, default_value, time_type, alpha0, alpha1);
			/*
			case random_evaluate_type:
				time = linear_interpolation(
							xray::math::min(upper.curve_time_min, lower.curve_time_min),
							xray::math::max(upper.curve_time_max, lower.curve_time_max), 
							random_float(0.0f, 1.0f)
							);
				return linear_interpolation(
					upper.evaluate(time, default_value, range_time_type), 
					lower.evaluate(time, default_value, range_time_type), 
					random_float(0.0f, 1.0f)
					);
			*/
			default: NODEFAULT( return default_value );
		}

//		return evaluate_value<float>(m_upper, m_lower, time, default_value, evaluate_type, time_type);
	}
public:
	
private:
	curve_line_float m_upper;
	curve_line_float m_lower;
};

struct curve_line_ranged_float
{
	inline curve_line_ranged_float()
	{
		m_evaluate_type = age_evaluate_type;
	}
	inline void set_defaults()
	{
		m_line.set_defaults();
	}
	inline u32 save_binary(xray::mutable_buffer& buffer, bool calc_size = false)
	{
		return m_line.save_binary(buffer, calc_size);
	}
	inline void load_binary(xray::mutable_buffer& buffer)
	{
		m_line.load_binary(buffer);
	}
	template<class ConfigValueType>
	inline void load(ConfigValueType config)
	{
 		if (config.value_exists("Input"))
 			m_evaluate_type = string_to_evaluate_type(pcstr(config["Input"]));
		
		if (config.value_exists("source"))
			m_line.load(config["source"]);
	}
	inline void reserve(u32 num_points)
	{
		m_line.reserve(num_points);
	}
	inline void clear()
	{
		m_line.clear();
	}
	inline float evaluate(float time, float default_value, enum_evaluate_time_type time_type, u32 seed)
	{
		return m_line.evaluate(time, default_value, m_evaluate_type, time_type, seed);
	}
	float get_max_value() const
	{
		return xray::math::max(m_line.m_upper.curve_value_max, m_line.m_lower.curve_value_max);
	}
private:
	enum_evaluate_type		m_evaluate_type;
	curve_line_ranged_base	m_line;
}; // struct curve_line_ranged_float

struct curve_line_ranged_xyz_float
{
	inline curve_line_ranged_xyz_float()
	{
		m_evaluate_type = age_evaluate_type;
	}
	inline void set_defaults()
	{
		m_line_x.set_defaults();
		m_line_y.set_defaults();
		m_line_z.set_defaults();
	}
	inline u32 save_binary(xray::mutable_buffer& buffer, bool calc_size = false)
	{
		return m_line_x.save_binary(buffer, calc_size) +
			   m_line_y.save_binary(buffer, calc_size) +
			   m_line_z.save_binary(buffer, calc_size);
	}
	inline void load_binary(xray::mutable_buffer& buffer)
	{
		m_line_x.load_binary(buffer);
		m_line_y.load_binary(buffer);
		m_line_z.load_binary(buffer);
	}
	template<class ConfigValueType>
	inline void load(ConfigValueType config)
	{
 		if (config.value_exists("Input"))
 			m_evaluate_type = string_to_evaluate_type(pcstr(config["Input"]));
		
		if (config.value_exists("source0"))
			m_line_x.load(config["source0"]);
		
		if (config.value_exists("source1"))
			m_line_y.load(config["source1"]);
		
		if (config.value_exists("source2"))
			m_line_z.load(config["source2"]);
	}
	inline void reserve(u32 num_points_x, u32 num_points_y, u32 num_points_z)
	{
		m_line_x.reserve(num_points_x);
		m_line_y.reserve(num_points_y);
		m_line_z.reserve(num_points_z);
	}
	inline void clear()
	{
		m_line_x.clear();
		m_line_y.clear();
		m_line_z.clear();
	}
	inline float3 evaluate(float time, math::float3 const& default_value, enum_evaluate_time_type time_type, u32 seed)
	{
		float x = m_line_x.evaluate(time, default_value.x, m_evaluate_type, time_type, seed);
		float y = m_line_y.evaluate(time, default_value.y, m_evaluate_type, time_type, seed);
		float z = m_line_z.evaluate(time, default_value.z, m_evaluate_type, time_type, seed);
		
		return math::float3(x, y, z);
	}
	float get_max_value() const
	{
		return math::max(
			math::max(m_line_x.m_upper.curve_value_max, m_line_x.m_lower.curve_value_max), 
				math::max(
					math::max(m_line_x.m_upper.curve_value_max, m_line_x.m_lower.curve_value_max), 
					math::max(m_line_x.m_upper.curve_value_max, m_line_x.m_lower.curve_value_max)
					)
				);
	}
private:
	enum_evaluate_type		m_evaluate_type;
	curve_line_ranged_base	m_line_x;
	curve_line_ranged_base	m_line_y;
	curve_line_ranged_base	m_line_z;
}; // struct curve_line_ranged_xyz_float

} // namespace particle
} // namespace xray

#include "curve_line_inline.h"

#endif // #ifndef CURVE_LINE_H_INCLUDED