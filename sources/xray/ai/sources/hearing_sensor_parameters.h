////////////////////////////////////////////////////////////////////////////
//	Created		: 22.09.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef HEARING_SENSOR_PARAMETERS_H_INCLUDED
#define HEARING_SENSOR_PARAMETERS_H_INCLUDED

namespace xray {
namespace ai {

struct hearing_sensor_parameters
{
			hearing_sensor_parameters	( );

	void	deserialize					( configs::binary_config_value const& options );

	float	max_sound_distance;
	float	min_sound_threshold;
	float	always_recognized_distance;
	float	decreasing_time_quant;
	float	decrease_factor;
	float	last_sound_time;
	u32		max_sounds_count;
	bool	enabled;
}; // struct hearing_sensor_parameters

} // namespace ai
} // namespace xray

#endif // #ifndef HEARING_SENSOR_PARAMETERS_H_INCLUDED