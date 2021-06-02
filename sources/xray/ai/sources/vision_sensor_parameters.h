////////////////////////////////////////////////////////////////////////////
//	Created		: 22.09.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VISION_SENSOR_PARAMETERS_H_INCLUDED
#define VISION_SENSOR_PARAMETERS_H_INCLUDED

namespace xray {
namespace ai {

struct vision_sensor_parameters
{
			vision_sensor_parameters	( );

	void	deserialize					( configs::binary_config_value const& options );
	
	float	vertical_fov;
	float	min_indirect_view_factor;
	float	far_plane_distance;
	float	near_plane_distance;
	float	aspect_ratio;
	float	time_quant;
	float	decrease_factor;
	float	velocity_factor;
	float	transparency_threshold;
	float	luminosity_factor;
	float	max_visibility;
	float	visibility_threshold;
	u32		visibility_inertia;
	bool	enabled;
}; // struct vision_sensor_parameters

} // namespace ai
} // namespace xray

#endif // #ifndef VISION_SENSOR_PARAMETERS_H_INCLUDED