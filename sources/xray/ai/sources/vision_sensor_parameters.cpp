////////////////////////////////////////////////////////////////////////////
//	Created		: 22.09.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "vision_sensor_parameters.h"

namespace xray {
namespace ai {

vision_sensor_parameters::vision_sensor_parameters	( ) :
	vertical_fov									( memory::uninitialized_value< float >() ),
	min_indirect_view_factor						( memory::uninitialized_value< float >() ),
	far_plane_distance								( memory::uninitialized_value< float >() ),
	near_plane_distance								( memory::uninitialized_value< float >() ),
	aspect_ratio									( memory::uninitialized_value< float >() ),
	time_quant										( memory::uninitialized_value< float >() ),
	decrease_factor									( memory::uninitialized_value< float >() ),
	velocity_factor									( memory::uninitialized_value< float >() ),
	transparency_threshold							( memory::uninitialized_value< float >() ),
	luminosity_factor								( memory::uninitialized_value< float >() ),
	max_visibility									( memory::uninitialized_value< float >() ),
	visibility_threshold							( memory::uninitialized_value< float >() ),
	visibility_inertia								( memory::uninitialized_value< u32 >() ),
	enabled											( memory::uninitialized_value< bool >() )
{
}

void vision_sensor_parameters::deserialize			( configs::binary_config_value const& options )
{
	enabled											= options["enabled"];
	vertical_fov									= options["vertical_fov"];
	min_indirect_view_factor						= options["min_indirect_view_factor"];
	far_plane_distance								= options["far_plane_distance"];
	near_plane_distance								= options["near_plane_distance"];
	float const horizontal_dimension				= options["aspect_ratio_horizontal_dimension"];
	float const vertical_dimension					= options["aspect_ratio_vertical_dimension"];
	aspect_ratio									= horizontal_dimension / vertical_dimension;
	time_quant										= options["time_quant"];
	decrease_factor									= options["decrease_factor"];
	velocity_factor									= options["velocity_factor"];
	transparency_threshold							= options["transparency_threshold"];
	luminosity_factor								= options["luminosity_factor"];
	visibility_inertia								= options["visibility_inertia"];
	visibility_threshold							= options["visibility_threshold"];
	max_visibility									= visibility_threshold + visibility_inertia / time_quant * decrease_factor;
}

} // namespace ai
} // namespace xray