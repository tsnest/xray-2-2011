////////////////////////////////////////////////////////////////////////////
//	Created		: 22.09.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "damage_sensor_parameters.h"

namespace xray {
namespace ai {

damage_sensor_parameters::damage_sensor_parameters	( ) :
	enabled											( memory::uninitialized_value< bool >() )
{
}

void damage_sensor_parameters::deserialize			( configs::binary_config_value const& options )
{
	enabled											= options["enabled"];
}

} // namespace ai
} // namespace xray