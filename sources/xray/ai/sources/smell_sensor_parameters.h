////////////////////////////////////////////////////////////////////////////
//	Created		: 22.09.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SMELL_SENSOR_PARAMETERS_H_INCLUDED
#define SMELL_SENSOR_PARAMETERS_H_INCLUDED

namespace xray {
namespace ai {

struct smell_sensor_parameters
{
			smell_sensor_parameters	( );

	void	deserialize				( configs::binary_config_value const& options );

	float	max_smelling_distance;
	float	min_intensity;
	float	max_intensity;
	float	always_recognized_distance;
	float	decreasing_time_quant;
	float	decrease_factor;
	float	last_smell_time;
	u32		max_smells_count;
	bool	enabled;
}; // struct smell_sensor_parameters

} // namespace ai
} // namespace xray

#endif // #ifndef SMELL_SENSOR_PARAMETERS_H_INCLUDED