////////////////////////////////////////////////////////////////////////////
//	Created		: 03.03.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef COLOR_MATRIX_H_INCLUDED
#define COLOR_MATRIX_H_INCLUDED

#include "help_functions.h"
#include "evaluate_type.h"

namespace xray {
namespace particle {

struct color_matrix_point_type
{
	xray::math::float4_pod			color;
	xray::math::float2_pod			position;
}; // struct color_matrix_point

struct color_matrix
{
	typedef xray::platform_pointer<color_matrix_point_type, xray::platform_pointer_default>::type point_ptr_type;
	
					    ~color_matrix();
	void				set_defaults ();
	
	u32					save_binary	(xray::mutable_buffer& buffer, bool calc_size = false);
	void				load_binary	(xray::mutable_buffer& buffer);
	
	template<class ConfigValueType>
	void				load		(ConfigValueType config);
	
	xray::math::float4	evaluate	(float time, xray::math::float4 const& default_value) const;
	xray::math::float4	evaluate	(float x, float y, xray::math::float4 const& default_value) const;
	
	bool				is_on_column	(float time, float time_delta, s32& out_column_index, bool& out_is_last) const;
	float				get_local_time  (float time) const;
	float				get_time		(s32 column_index) const;
	
private:
	
	void				reserve		(u32 num_rows, u32 num_columns);
	void				clear		();
	
	color_matrix_point_type const& get_point(u32 row_index, u32 column_index) const;
	
	// Row-major order.
	point_ptr_type		m_points;
	u32					m_num_rows;
	u32					m_num_columns;
	enum_evaluate_type	m_evaluate_type;
}; // struct color_matrix

} // namespace particle
} // namespace xray

#include "color_matrix_inline.h" 

#endif // #ifndef COLOR_MATRIX_H_INCLUDED