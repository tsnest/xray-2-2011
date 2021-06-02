////////////////////////////////////////////////////////////////////////////
//	Created		: 13.08.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MATH_CURVE_INLINE_H_INCLUDED
#define MATH_CURVE_INLINE_H_INCLUDED

namespace xray {
namespace math {

#define TEMPLATE_SIGNATURE	template<class T, enum_curve_line_type curve_line_type>
#define CURVE_LINE			curve_line_points<T,curve_line_type>

template<class T> T linear_interpolation(T a, T b, float alpha)
{
	return a * (1 - alpha) + b * alpha;
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

template<class T, class ConfigValueType> T read_config_value(ConfigValueType const& config_value, pcstr name, T const& default_value)
{
	if (config_value.value_exists(name))
		return (T)config_value[name];
	else
		return default_value;
}

template < class T>
T get_tangent_from_2d_vector( T x, T y )
{
	T t = y / xray::math::max(x, xray::math::epsilon_5);
	return t;
}

template < class T >
static void smooth_tangents( T& curve )
{

}

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
						min(upper.curve_time_min, lower.curve_time_min),
						max(upper.curve_time_max, lower.curve_time_max), 
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

TEMPLATE_SIGNATURE
inline CURVE_LINE::~curve_line_points()
{
	clear			();
}

TEMPLATE_SIGNATURE
inline void CURVE_LINE::set_defaults()
{
	points			= 0;
	num_points		= 0;
	
	memory::zero(&curve_value_min, sizeof(curve_value_min));
	memory::zero(&curve_value_max, sizeof(curve_value_max));
	
	memory::zero(&curve_time_min, sizeof(curve_time_min));
	memory::zero(&curve_time_max, sizeof(curve_time_max));
}

TEMPLATE_SIGNATURE
inline u32 CURVE_LINE::save_binary( xray::mutable_buffer& buffer, bool calc_size)
{
	u32 save_size = num_points * sizeof(point_type);
	if (!calc_size)
	{
		xray::memory::copy(buffer.c_ptr(), save_size, points, save_size);
		buffer+=save_size;
	}
	return save_size;
}

TEMPLATE_SIGNATURE
inline void CURVE_LINE::load_binary( xray::mutable_buffer& buffer )
{
	points = (point_type*)buffer.c_ptr();
	buffer += num_points * sizeof(point_type);	

	recalculate_ranges();
}

TEMPLATE_SIGNATURE
inline void CURVE_LINE::sort_points_by_time()
{
	if (num_points)
	{
		struct predicate
		{
			static inline bool compare	( point_type const& left, point_type const& right )
			{
				return left.time < right.time;
			}
		}; // struct predicate
		point_type*& points_to_sort = (point_type*&)points;
		std::sort(points_to_sort, points_to_sort + num_points, &predicate::compare);
	}
}

TEMPLATE_SIGNATURE
template <class ConfigValueType>
inline void CURVE_LINE::load(ConfigValueType keys_config)
{
	u32 num_keys = 0;
	
	xray::fixed_string<8> key_name;
	u32 key_index	  = 0;
	for (;;)
	{
		key_name.assignf("key%d", key_index);

		if (!keys_config.value_exists(pcstr(key_name.c_str())))
			break;

		ConfigValueType it = keys_config[ pcstr(key_name.c_str()) ];
		
		if (!it.size())
			break;
		
		key_index++;
	}

	num_keys = key_index;
	reserve( num_keys, true );
	
	u32 point_index = 0;
	
	key_index		= 0;
	
	for (;;)
	{
		key_name.assignf("key%d", key_index);
		
		if (!keys_config.value_exists(pcstr(key_name.c_str())))
			break;

		ConfigValueType it = keys_config[ pcstr(key_name.c_str()) ];
		
		if (!it.size())
			break;
		
		key_index++;

		point_type point;
		
		if (identity(curve_line_type==float_curve_line))
		{
			// TODO: load interp type
			float2 left_tangent		= float2(it["left_tangent"]);
			float2 position			= float2(it["position"]);
			float2 right_tangent	= float2(it["right_tangent"]);
			float  delta			= it.value_exists("delta") ? float(it["delta"]) : 0.0f;
			
			float upper_value		= position.y + delta;
			float lower_value		= position.y - delta;
			
			point.upper_value		= *(T*)&upper_value;
			point.lower_value		= *(T*)&lower_value;
			
			point.time				= position.x;
			
			float t0				= get_tangent_from_2d_vector(position - left_tangent);
			float t1				= get_tangent_from_2d_vector(right_tangent - position);
			
			point.tangent_in		= *(T*)&t0;
			point.tangent_out		= *(T*)&t1;
			
			// TODO: key_type!
			if (it.value_exists("key_type"))
				point.interp_type	= (u32(it["key_type"])==3) ? linear_interp_type : curve_interp_type;
			else
				point.interp_type	= curve_interp_type;
			
			set_point(point_index, point);
		}
		else if (identity(curve_line_type==color_curve_line))
		{
			float4 RGBA				= float4(it["value"]);
			
			point.upper_value		= *(T*)&RGBA;
			point.lower_value		= *(T*)&RGBA;
			point.time				= float(it["key"]);
			point.interp_type		= linear_interp_type;
			
			set_point(point_index, point);
		}
		else if (identity(curve_line_type==custom_curve_line))
		{
		}
		else NODEFAULT( );
		
		point_index++;
	}
	
	recalculate_ranges();
	sort_points_by_time();
}

TEMPLATE_SIGNATURE
inline void CURVE_LINE::recalculate_ranges()
{
	if (num_points==0)
		return;

	curve_time_min  = points[0].time;
	curve_time_max  = points[0].time;
	curve_value_min = points[0].lower_value;
	curve_value_max = points[0].upper_value;

	for (u32 i=1; i<num_points; i++)
	{
		point_type& point = points[i];
		
		if (point.time<curve_time_min)   curve_time_min  = point.time;
		if (point.time>curve_time_max)   curve_time_max  = point.time;
		if (point.lower_value<curve_value_min) curve_value_min = point.lower_value;
		if (point.upper_value>curve_value_max) curve_value_max = point.upper_value;
	}
}

TEMPLATE_SIGNATURE
inline void CURVE_LINE::reserve(u32 num, bool)
{
	if (!num)
		return;
	
	clear		( );
	num_points	= num;

	points		= MT_ALLOC(point_type,num);
	
	point_type* point_to_init = points;
	for (u32 i=0; i<num; i++)
		new(point_to_init++)point_type;
	
	recalculate_ranges();
}

TEMPLATE_SIGNATURE
inline void CURVE_LINE::clear()
{
	if (!num_points)
		return;
	
	point_type* pointer = &*points;
	MT_FREE				( pointer );
	points				= 0;
	num_points			= 0;
}

TEMPLATE_SIGNATURE
inline void CURVE_LINE::set_point(u32 index, point_type point)
{
	ASSERT_CMP(index, <, num_points);

	points[index] = point;
}

TEMPLATE_SIGNATURE
inline void CURVE_LINE::get_point(u32 index, point_type& out_point) const
{
	ASSERT_CMP(index, <, num_points);
	
	out_point = points[index];
}

TEMPLATE_SIGNATURE
inline void CURVE_LINE::set_interp_type(u32 index, enum_interpolation_type type)
{
	ASSERT_CMP(index, <, num_points);
	
	points[index].interp_type = type;
}

TEMPLATE_SIGNATURE
inline void CURVE_LINE::set_point(u32 index, T value, T tangent_in, T tangent_out, float time, enum_interpolation_type type)
{
	ASSERT_CMP(index, <, num_points);
	
	point_type point;
	
	point.upper_value = value;
	point.lower_value = value;
	point.tangent_in  = tangent_in;
	point.tangent_out = tangent_out;
	point.time		  = time;
	point.interp_type = type;
	
	points[index] = point;
}

TEMPLATE_SIGNATURE
inline u32 CURVE_LINE::get_left_point_index(float time)
{
	if (!num_points || time <= points[0].time)
		return 0;
	
	if (time >= points[num_points-1].time)
		return num_points-1;
	
	for (u32 i=1; i<num_points; i++)
	{
		point_type& a = points[i-1];
		point_type& b = points[i];
		
		if (time<a.time || time>b.time)
			continue;
		
		return i;
	}
	return 0;
}

TEMPLATE_SIGNATURE
inline T CURVE_LINE::evaluate(float time, T default_value, enum_evaluate_time_type time_type, float left_range_alpha, float right_range_alpha) const
{
	if (!num_points)
		return default_value;
	
	point_type& first_point = points[0];
	point_type& last_point  = points[num_points-1];
	
	if (num_points==1)
		return linear_interpolation(first_point.lower_value, first_point.upper_value, left_range_alpha);
	
	if (time_type==linear_time_type)
	{
		time = linear_interpolation(curve_time_min, curve_time_max, time);
	}
	
	if (time<=first_point.time)
		return linear_interpolation(first_point.lower_value, first_point.upper_value, left_range_alpha);
	
	if (time>=last_point.time)
		return linear_interpolation(last_point.lower_value, last_point.upper_value, right_range_alpha);
	
	for (u32 i=1; i<num_points; i++)
	{
		point_type& a = points[i-1];
		point_type& b = points[i];
		
		if (time<a.time || time>b.time)
			continue;
		
		float dist = (b.time - a.time);
		
		if (abs(dist)<=epsilon_5)
			continue;
		
		T a_value = linear_interpolation(a.lower_value, a.upper_value, left_range_alpha);
		T b_value = linear_interpolation(b.lower_value, b.upper_value, right_range_alpha);
		
		if (a.interp_type==linear_interp_type && b.interp_type==linear_interp_type)
		{
			return linear_interpolation(a_value, b_value, (time - a.time) / dist);			
		}
		else
		{
			return cubic_interpolation(a_value, a.tangent_out, b_value, b.tangent_in, (time - a.time) / dist);
		}
	}
	return default_value;
}

TEMPLATE_SIGNATURE
inline void CURVE_LINE::calc_tangent(T const& prev_value, T const& value, T const& next_value, T& out_new_tangent)
{
	out_new_tangent = 0.5f * ( (value-prev_value) + (next_value-value) );
}

TEMPLATE_SIGNATURE
inline void CURVE_LINE::calc_tangents()
{
	for (u32 i=0; i<num_points; i++)
	{
		T& tangent_in  = points[i].tangent_in;
		T& tangent_out = points[i].tangent_out;
		
		if (i==0)
		{
			if (i<num_points-1)
				xray::memory::zero(&tangent_out, sizeof(T));
			else
				xray::memory::zero(&tangent_out, sizeof(T));
		}
		else
		{
			if (i<num_points-1)
			{
				calc_tangent( points[i-1].upper_value, points[i].upper_value, points[i+1].upper_value, tangent_in );
				tangent_out = tangent_in;
			}
			else
				xray::memory::zero(&tangent_in, sizeof(T));
		}
	}
}

// curve_line_color

inline curve_line_color::curve_line_color( )
{
	m_evaluate_type = age_evaluate_type;
}

template<class ConfigValueType>
inline void curve_line_color::load(ConfigValueType config)
{
	if (config.value_exists("Input"))
		m_evaluate_type = string_to_evaluate_type(pcstr(config["Input"]));

	if (config.value_exists("source") && 
		config["source"].value_exists("data") &&
		config["source"]["data"].value_exists("ramp"))
		curve_line_color_base::load<ConfigValueType>(config["source"]["data"]["ramp"]);
}

inline float4 curve_line_color::evaluate(float time, float4 default_value, enum_evaluate_time_type time_type ) const
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
	default: NODEFAULT();
	}
	return default_value;
}

// curve_line_ranged_base

curve_line_ranged_base::curve_line_ranged_base() 
{

}

inline void curve_line_ranged_base::set_defaults()
{
	m_upper.set_defaults();
	m_lower.set_defaults();
}

inline u32 curve_line_ranged_base::save_binary(xray::mutable_buffer& buffer, bool calc_size)
{
	return m_upper.save_binary(buffer, calc_size) +
		m_lower.save_binary(buffer, calc_size);
}

inline void curve_line_ranged_base::load_binary(xray::mutable_buffer& buffer)
{
	m_upper.load_binary(buffer);
	m_lower.load_binary(buffer);
}

template<class ConfigValueType>
inline void curve_line_ranged_base::load(ConfigValueType config)
{
	ConfigValueType const& data = config["data"];
	if (data.value_exists("curve0"))
		m_upper.load(data["curve0"]);

	//if (data.value_exists("curve1"))
	//	m_lower.load(data["curve1"]);		
	//else
	//	m_lower.load(data["curve0"]);	
}

inline void curve_line_ranged_base::reserve(u32 num_points)
{
	m_upper.reserve(num_points);
}

inline void curve_line_ranged_base::clear()
{
	m_lower.clear();
}

inline float curve_line_ranged_base::evaluate(float time, float default_value, enum_evaluate_type evaluate_type, enum_evaluate_time_type time_type, u32 seed)
{
	random32 rnd;
	rnd.seed( seed );

//	u32 const index = m_upper.get_left_point_index(time);

	//float temp_value = 0.0f;
	//for (u32 random_index=0; random_index<index; random_index++)
	//{
	//	temp_value = float(rnd.random(65535)) / 65535.0f;
	//}

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

// curve_line_ranged_float

inline curve_line_ranged_float::curve_line_ranged_float()
{
	m_evaluate_type = age_evaluate_type;
}

inline void curve_line_ranged_float::set_defaults()
{
	m_line.set_defaults();
}

inline u32 curve_line_ranged_float::save_binary(mutable_buffer& buffer, bool calc_size)
{
	return m_line.save_binary(buffer, calc_size);
}

inline void curve_line_ranged_float::load_binary(mutable_buffer& buffer)
{
	m_line.load_binary(buffer);
}

template<class ConfigValueType>
inline void curve_line_ranged_float::load(ConfigValueType config)
{
	if (config.value_exists("Input"))
		m_evaluate_type = string_to_evaluate_type(pcstr(config["Input"]));

	if (config.value_exists("source"))
		m_line.load(config["source"]);
}

inline void curve_line_ranged_float::reserve(u32 num_points)
{
	m_line.reserve(num_points);
}

inline void curve_line_ranged_float::clear()
{
	m_line.clear();
}

inline float curve_line_ranged_float::evaluate(float time, float default_value, enum_evaluate_time_type time_type, u32 seed)
{
	return m_line.evaluate(time, default_value, m_evaluate_type, time_type, seed);
}

inline float curve_line_ranged_float::get_max_value() const
{
	return max(m_line.m_upper.curve_value_max, m_line.m_lower.curve_value_max);
}

// curve_line_ranged_xyz_float

inline curve_line_ranged_xyz_float::curve_line_ranged_xyz_float()
{
	m_evaluate_type = age_evaluate_type;
}

inline void curve_line_ranged_xyz_float::set_defaults()
{
	m_line_x.set_defaults();
	m_line_y.set_defaults();
	m_line_z.set_defaults();
}

inline u32 curve_line_ranged_xyz_float::save_binary(mutable_buffer& buffer, bool calc_size )
{
	return m_line_x.save_binary(buffer, calc_size) +
		m_line_y.save_binary(buffer, calc_size) +
		m_line_z.save_binary(buffer, calc_size);
}

inline void curve_line_ranged_xyz_float::load_binary(mutable_buffer& buffer)
{
	m_line_x.load_binary(buffer);
	m_line_y.load_binary(buffer);
	m_line_z.load_binary(buffer);
}

template<class ConfigValueType>
inline void curve_line_ranged_xyz_float::load(ConfigValueType config)
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

inline void curve_line_ranged_xyz_float::reserve(u32 num_points_x, u32 num_points_y, u32 num_points_z)
{
	m_line_x.reserve(num_points_x);
	m_line_y.reserve(num_points_y);
	m_line_z.reserve(num_points_z);
}

inline void curve_line_ranged_xyz_float::clear()
{
	m_line_x.clear();
	m_line_y.clear();
	m_line_z.clear();
}

inline float3 curve_line_ranged_xyz_float::evaluate(float time, float3 const& default_value, enum_evaluate_time_type time_type, u32 seed)
{
	float x = m_line_x.evaluate(time, default_value.x, m_evaluate_type, time_type, seed);
	float y = m_line_y.evaluate(time, default_value.y, m_evaluate_type, time_type, seed);
	float z = m_line_z.evaluate(time, default_value.z, m_evaluate_type, time_type, seed);

	return float3(x, y, z);
}

inline float curve_line_ranged_xyz_float::get_max_value() const
{
	return max(
		max(m_line_x.m_upper.curve_value_max, m_line_x.m_lower.curve_value_max), 
		max(
		max(m_line_x.m_upper.curve_value_max, m_line_x.m_lower.curve_value_max), 
		max(m_line_x.m_upper.curve_value_max, m_line_x.m_lower.curve_value_max)
		)
		);
}

} // namespace math
} // namespace xray

#undef TEMPLATE_SIGNATURE
#undef CURVE_LINE	


#endif // #ifndef MATH_CURVE_INLINE_H_INCLUDED