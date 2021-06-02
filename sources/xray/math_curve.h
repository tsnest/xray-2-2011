////////////////////////////////////////////////////////////////////////////
//	Created		: 03.08.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MATH_CURVE_H_INCLUDED
#define MATH_CURVE_H_INCLUDED

#include "math_randoms_generator.h"

namespace xray {
namespace math {

enum enum_evaluate_type
{
	age_evaluate_type,
	random_evaluate_type,
};

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

static random32 randomizer(100);
static u32 const random_max = 65535;

float random_float(float min_value, float max_value);
float3 random_float3(float3 const& min_value, float3 const& max_value);

enum_evaluate_type string_to_evaluate_type(pcstr name);
float XRAY_CORE_API get_tangent_from_2d_vector( float2 const& vec );

// Rename to curve_line_points!
template<class T, enum_curve_line_type> 
struct curve_line_points
{
	typedef curve_point<T>  point_type;
	
	typedef typename xray::platform_pointer<point_type, xray::platform_pointer_default>::type point_ptr_type;
	
	~curve_line_points();
	
	u32  get_num_points		() const { return num_points; }

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
 
typedef curve_line_points<float, float_curve_line>			curve_line_float;
typedef curve_line_points<float3_pod, float_curve_line>		curve_line_float3;
typedef curve_line_points<float4_pod, float_curve_line>		curve_line_float4;
typedef curve_line_points<float4_pod, color_curve_line>		curve_line_color_base;

// aliases
typedef curve_line_points<float, float_curve_line>			float_curve;
typedef curve_line_points<float3_pod, float_curve_line>		float3_curve;
typedef curve_line_points<float4_pod, float_curve_line>		float4_curve;
typedef curve_line_points<float4_pod, color_curve_line>		color_curve_base;

template struct XRAY_CORE_API curve_line_points<float, float_curve_line>;
template struct XRAY_CORE_API curve_line_points<float3_pod, float_curve_line>;
template struct XRAY_CORE_API curve_line_points<float4_pod, float_curve_line>;
template struct XRAY_CORE_API curve_line_points<float4_pod, color_curve_line>;

struct curve_line_color: public curve_line_color_base
{
	inline						curve_line_color	( );

	template<class ConfigValueType>
	inline void					load				( ConfigValueType config );

	inline float4				evaluate			(	float time,
														float4 default_value,
														enum_evaluate_time_type time_type = range_time_type
													) const;
private:
	enum_evaluate_type m_evaluate_type;
}; // struct color_curve


struct XRAY_CORE_API curve_line_ranged_base
{
private:
	friend struct curve_line_ranged_float;
	friend struct curve_line_ranged_xyz_float;
	
	inline			curve_line_ranged_base		( );

	inline void		set_defaults				( );
	
	inline u32		save_binary					(xray::mutable_buffer& buffer, bool calc_size = false);
	inline void		load_binary					(xray::mutable_buffer& buffer);

	template<class ConfigValueType>
	inline void		load						(ConfigValueType config);
	
	inline void		reserve						(u32 num_points);
	inline void		clear						( );
	inline float	evaluate					(float time, float default_value, enum_evaluate_type evaluate_type, enum_evaluate_time_type time_type, u32 seed);
private:
	curve_line_float m_upper;
	curve_line_float m_lower;
};

struct XRAY_CORE_API curve_line_ranged_float
{
	inline			curve_line_ranged_float		( );

	inline void		set_defaults				( );
	inline u32		save_binary					( mutable_buffer& buffer, bool calc_size = false );
	inline void		load_binary					( mutable_buffer& buffer );
	template<class ConfigValueType>
	inline void		load						( ConfigValueType config );

	inline void		reserve						( u32 num_points );
	inline void		clear						( );
	inline float	evaluate					( float time, float default_value, enum_evaluate_time_type time_type, u32 seed );
	inline float	get_max_value				( )	const;
private:
	enum_evaluate_type		m_evaluate_type;
	curve_line_ranged_base	m_line;
}; // struct curve_line_ranged_float

struct XRAY_CORE_API curve_line_ranged_xyz_float
{
	inline					curve_line_ranged_xyz_float	( );
	
	inline void				set_defaults				( );
	
	inline u32				save_binary					( mutable_buffer& buffer, bool calc_size = false );
	inline void				load_binary					( mutable_buffer& buffer );
	template<class ConfigValueType>
	inline void				load						( ConfigValueType config );

	inline void				reserve						( u32 num_points_x, u32 num_points_y, u32 num_points_z );
	inline void				clear						( );
	inline float3			evaluate					( float time, float3 const& default_value, enum_evaluate_time_type time_type, u32 seed );
	inline float			get_max_value				( ) const;
private:
	enum_evaluate_type		m_evaluate_type;
	curve_line_ranged_base	m_line_x;
	curve_line_ranged_base	m_line_y;
	curve_line_ranged_base	m_line_z;
}; // struct curve_line_ranged_xyz_float

} // namespace math
} // namespace xray

#include "math_curve_inline.h"

#endif // #ifndef MATH_CURVE_H_INCLUDED