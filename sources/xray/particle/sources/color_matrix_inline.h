////////////////////////////////////////////////////////////////////////////
//	Created		: 03.03.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

namespace xray {
namespace particle {


inline float color_matrix::get_time(s32 column_index) const
{
	color_matrix_point_type const& point = get_point(0, column_index);
	return point.position.x;
}

inline float color_matrix::get_local_time(float time) const
{
	for (u32 column_index=0; column_index<m_num_columns-1; column_index++)
	{
		color_matrix_point_type& point		= m_points[column_index];
		color_matrix_point_type& next_point = m_points[column_index+1];
		
		float const left  = point.position.x;
		float const right = next_point.position.x;
		
		if (left<=time && time<=right)
		{
			return (time - left) / (right - left);
		}
	}
	return 0.0f;
}

inline bool color_matrix::is_on_column(float time, float time_delta, s32& out_column_index, bool& out_is_last) const
{
	for (u32 row_index=0; row_index<m_num_rows; row_index++)
	{
		for (u32 column_index=0; column_index<m_num_columns; column_index++)
		{
			color_matrix_point_type& point = m_points[row_index * m_num_columns + column_index];
			
			if (time <= point.position.x && time + time_delta > point.position.x)
			{
				out_column_index = column_index;
				out_is_last		 = column_index == m_num_columns - 1;
				return true;
			}
		}
	}
	return false;
}

inline xray::math::float4 color_matrix::evaluate(float time, xray::math::float4 const& default_value) const
{
	return evaluate(time, random_float(0.0f, 1.0f), default_value);
}

inline xray::math::float4 color_matrix::evaluate(float x, float y, xray::math::float4 const& default_value) const
{
	xray::math::clamp(x, 0.0f, 1.0f);
	xray::math::clamp(y, 0.0f, 1.0f);
	
	if (m_evaluate_type==random_evaluate_type)
	{
		x = random_float(0.0f, 1.0f);
	}
	
	float const x_scaled = x * 1.0f;
	float const y_scaled = y * 1.0f;
	
	u32 left_column  = 0;
	u32 right_column = 0;
	
	bool found_columns = false;
	for (u32 column_index=1; column_index<m_num_columns; column_index++)
	{
		color_matrix_point_type const& curr  = get_point(0, column_index);
		color_matrix_point_type const& prev  = get_point(0, column_index - 1);
		
		if (prev.position.x <= x_scaled && x_scaled <= curr.position.x)
		{
			left_column		= column_index - 1;
			right_column	= column_index;
			found_columns	= true;
			break;
		}
	}
	
	if (!found_columns)
		return default_value;
	
	u32 upper_row	= 0;
	u32 lower_row	= 0;
	
	bool found_rows = false;
	for (u32 row_index=1; row_index<m_num_rows; row_index++)
	{
		color_matrix_point_type const& curr  = get_point(row_index, 0);
		color_matrix_point_type const& prev  = get_point(row_index - 1, 0);
		
		if (prev.position.y <= y_scaled && y_scaled <= curr.position.y)
		{
			upper_row	= row_index - 1;
			lower_row	= row_index;
			found_rows	= true;
			break;
		}
	}
	
	if (!found_rows)
		return default_value;
	
	color_matrix_point_type const& upper_left_point		= get_point(upper_row, left_column);
	color_matrix_point_type const& upper_right_point	= get_point(upper_row, right_column);
	color_matrix_point_type const& lower_left_point		= get_point(lower_row, left_column);
	color_matrix_point_type const& lower_right_point	= get_point(lower_row, right_column);
	
	float dist_x = upper_right_point.position.x - upper_left_point.position.x;
	float dist_y = lower_right_point.position.y - upper_right_point.position.y;
	
	if (xray::math::abs(dist_x)<=xray::math::epsilon_5)
		dist_x = 1.0f;
	
	if (xray::math::abs(dist_y)<=xray::math::epsilon_5)
		dist_y = 1.0f;
	
	float const alpha_x = (x_scaled - upper_left_point.position.x) / dist_x;
	float const alpha_y = (y_scaled - upper_right_point.position.y) / dist_y;
	
	xray::math::float4 const result	= bilinear_interpolation(
		upper_left_point.color, 
		upper_right_point.color, 
		lower_left_point.color, 
		lower_right_point.color, 
		alpha_x, 
		alpha_y
		);
	
	return result;
}

inline color_matrix_point_type const& color_matrix::get_point(u32 row_index, u32 column_index) const
{
	ASSERT_CMP(row_index, <, m_num_rows);
	ASSERT_CMP(column_index, <, m_num_columns);
	
	return m_points[row_index * m_num_columns + column_index];
}

inline color_matrix::~color_matrix()
{
	clear();
}

template <class ConfigValueType>
inline void color_matrix::load				(ConfigValueType config)
{
	if (config.value_exists("Input"))
		m_evaluate_type = string_to_evaluate_type(pcstr(config["Input"]));
	
	u32 num_rows		= 0;
	u32 num_columns		= 0;
	
	u32 row_index		= 0;
	u32 column_index	= 0;
	
	xray::fixed_string<16> row_name;
	xray::fixed_string<16> column_name;
	
	if (!config.value_exists("source") || !config["source"].value_exists("data"))
		return;
	
	ConfigValueType const& rows_config = config["source"]["data"];
	
	for (;;)
	{
		row_name.assignf("row%d", row_index);
		
		if (!rows_config.value_exists(pcstr(row_name.c_str())))
			break;
		
		ConfigValueType row_it = rows_config[ pcstr(row_name.c_str()) ];
		
		if (!row_it.size())
			break;
		
		ConfigValueType const& columns_config = row_it;
		
		for (;;)
		{
			column_name.assignf("element%d", column_index);
			
			if (!columns_config.value_exists(pcstr(column_name.c_str())))
				break;
			
			ConfigValueType column_it = columns_config[ pcstr(column_name.c_str()) ];
			
			if (!column_it.size())
				break;
			
			column_index++;
		}		
		row_index++;
	}
	
	num_rows	= row_index;
	num_columns = column_index;
	
	if (!num_rows || !num_columns)
		return;
	
	reserve(num_rows, num_columns);
	
	for (row_index=0; row_index<num_rows; row_index++)
	{
		row_name.assignf("row%d", row_index);
		ConfigValueType const& columns_config = rows_config[ pcstr(row_name.c_str()) ];
		
		for (column_index=0; column_index<num_columns; column_index++)
		{
			column_name.assignf("element%d", column_index);
			ConfigValueType const& element = columns_config[ pcstr(column_name.c_str()) ];
			
			color_matrix_point_type& point = m_points[row_index * num_columns + column_index];
			
			point.position	= xray::math::float2(element["position"]);
			point.color		= xray::math::float4(element["color"]);
		}
	}
}

inline void color_matrix::set_defaults		()
{
	m_points		= 0;
	m_num_rows		= 0;
	m_num_columns	= 0;
	m_evaluate_type = age_evaluate_type;
}
	
inline u32 color_matrix::save_binary		(xray::mutable_buffer& buffer, bool calc_size)
{
	u32 save_size = m_num_rows * m_num_columns * sizeof(color_matrix_point_type);
	if (!calc_size)
	{
		xray::memory::copy(buffer.c_ptr(), save_size, m_points, save_size);
		buffer+=save_size;
	}
	return save_size;
}

inline void color_matrix::load_binary		(xray::mutable_buffer& buffer)
{
	m_points = (color_matrix_point_type*)buffer.c_ptr();
	buffer += m_num_rows * m_num_columns * sizeof(color_matrix_point_type);	
}

inline void color_matrix::reserve			(u32 num_rows, u32 num_columns)
{
	ASSERT_CMP(num_rows, !=, 0);
	ASSERT_CMP(num_columns, !=, 0);
	
	clear			();
	
	m_num_rows		= num_rows;
	m_num_columns	= num_columns;
	
	m_points		= MT_ALLOC(color_matrix_point_type, num_rows * num_columns);
	
	color_matrix_point_type* point_to_init = m_points;
	
	for (u32 i=0; i<m_num_rows * m_num_columns; i++)
		new(point_to_init++)color_matrix_point_type;
}

inline void color_matrix::clear				()
{
	if (!m_num_rows || !m_num_columns)
		return;
	
	color_matrix_point_type* pointer = &*m_points;
	
	MT_FREE				( pointer );
	m_points			= 0;
	m_num_rows			= 0;
	m_num_columns		= 0;
}

} // namespace particle
} // namespace xray