////////////////////////////////////////////////////////////////////////////
//	Created		: 22.09.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "smell_sensor_parameters.h"

namespace xray {
namespace ai {

smell_sensor_parameters::smell_sensor_parameters	( ) :
	max_smelling_distance		( memory::uninitialized_value< float >() ),
	min_intensity				( memory::uninitialized_value< float >() ),
	max_intensity				( memory::uninitialized_value< float >() ),		
	always_recognized_distance	( memory::uninitialized_value< float >() ),
	decreasing_time_quant		( memory::uninitialized_value< float >() ),
	decrease_factor				( memory::uninitialized_value< float >() ),
	last_smell_time				( memory::uninitialized_value< float >() ),
	max_smells_count			( memory::uninitialized_value< u32 >() ),
	enabled						( memory::uninitialized_value< bool >() )
{
}

void smell_sensor_parameters::deserialize			( configs::binary_config_value const& options )
{
	enabled						= options["enabled"];
	max_smells_count			= options["max_smells_count"];
	max_smelling_distance		= options["max_smelling_distance"];
	min_intensity				= options["min_intensity"];
	max_intensity				= options["max_intensity"];
	always_recognized_distance	= options["always_recognized_distance"];
	decreasing_time_quant		= options["decreasing_time_quant"];
	decrease_factor				= options["decrease_factor"];
	last_smell_time				= options["last_smell_time"];
}

} // namespace ai
} // namespace xray