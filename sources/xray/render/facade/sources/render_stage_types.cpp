////////////////////////////////////////////////////////////////////////////
//	Created		: 16.09.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/facade/render_stage_types.h>

namespace xray {
namespace render {

enum_render_stage_type string_to_stage_type(pcstr stage_name)
{
	if		(strings::equal(stage_name, "g_stage"))						return geometry_render_stage;
	else if (strings::equal(stage_name, "distortion"))					return accumulate_distortion_render_stage;
	else if (strings::equal(stage_name, "forward"))						return forward_render_stage;
	else if (strings::equal(stage_name, "shadow"))						return shadow_render_stage;
	else if (strings::equal(stage_name, "ambient_occlusion"))			return ambient_occlusion_render_stage;
	else if (strings::equal(stage_name, "light_propagation_volumes"))	return light_propagation_volumes_render_stage;
	else if (strings::equal(stage_name, "lighting"))					return lighting_render_stage;
	else if (strings::equal(stage_name, "post_process"))				return post_process_render_stage;
	else if (strings::equal(stage_name, "debug_post_process"))			return debug_post_process_render_stage;
	else if (strings::equal(stage_name, "decals"))						return decals_accumulate_render_stage;
	else if (strings::equal(stage_name, "debug"))						return debug_render_stage;
	else																return unknown_render_stage;
}

pcstr stage_type_to_string(enum_render_stage_type stage_type)
{
	switch (stage_type)
	{
		case geometry_render_stage:										return "g_stage";
		case accumulate_distortion_render_stage:						return "distortion";
		case forward_render_stage:										return "forward";
		case shadow_render_stage:										return "shadow";
		case ambient_occlusion_render_stage:							return "ambient_occlusion";
		case light_propagation_volumes_render_stage:					return "light_propagation_volumes";
		case lighting_render_stage:										return "lighting";
		case post_process_render_stage:									return "post_process";
		case debug_post_process_render_stage:							return "debug_post_process";
		case debug_render_stage:										return "debug";
		case decals_accumulate_render_stage:							return "decals";
		default:														return "unknown";
	};
}

} // namespace render
} // namespace xray

