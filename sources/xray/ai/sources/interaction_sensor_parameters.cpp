////////////////////////////////////////////////////////////////////////////
//	Created		: 22.09.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "interaction_sensor_parameters.h"

namespace xray {
namespace ai {

interaction_sensor_parameters::interaction_sensor_parameters	( ) :
	enabled			( memory::uninitialized_value< bool >() )
{
}

void interaction_sensor_parameters::deserialize					( configs::binary_config_value const& options )
{
	enabled			= options["enabled"];
}

} // namespace ai
} // namespace xray