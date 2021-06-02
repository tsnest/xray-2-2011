////////////////////////////////////////////////////////////////////////////
//	Created		: 22.09.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "hearing_sensor_parameters.h"

namespace xray {
namespace ai {

hearing_sensor_parameters::hearing_sensor_parameters	( ) :
	max_sound_distance			( memory::uninitialized_value< float >() ),
	min_sound_threshold			( memory::uninitialized_value< float >() ),
	always_recognized_distance	( memory::uninitialized_value< float >() ),		
	decreasing_time_quant		( memory::uninitialized_value< float >() ),
	decrease_factor				( memory::uninitialized_value< float >() ),
	last_sound_time				( memory::uninitialized_value< float >() ),
	max_sounds_count			( memory::uninitialized_value< u32 >() ),
	enabled						( memory::uninitialized_value< bool >() )
{
}

void hearing_sensor_parameters::deserialize				( configs::binary_config_value const& options )
{
	enabled						= options["enabled"];
	max_sounds_count			= options["max_sounds_count"];
	max_sound_distance			= options["max_sound_distance"];
	min_sound_threshold			= options["min_sound_threshold"];
	always_recognized_distance	= options["always_recognized_distance"];
	decreasing_time_quant		= options["decreasing_time_quant"];
	decrease_factor				= options["decrease_factor"];
	last_sound_time				= options["last_sound_time"];
}

} // namespace ai
} // namespace xray