 ////////////////////////////////////////////////////////////////////////////
//	Created		: 13.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if XRAY_PLATFORM_WINDOWS
#	include <xray/render/core/resource_manager.h>
#endif // #if XRAY_PLATFORM_WINDOWS

#include "post_process_parameters.h"

namespace xray {
namespace render {

post_process_parameters::post_process_parameters()
{
	// DOF part.
	dof_height_lights				= xray::math::float3(1,1,1);
	dof_focus_power					= 0.6f;
	dof_focus_region				= 20.0f;
	dof_focus_distance				= 30.0f;
	dof_far_blur_amount				= 0.0f;
	dof_near_blur_amount			= 0.0f;
	
	enable_dof						= true;
	use_bokeh_dof					= false;
	bokeh_dof_radius				= 8.0f;
	bokeh_dof_density				= 1.0f;
	use_bokeh_image					= false;

	// Bloom part.
	bloom_blurring_kernel			= 0.5f;
	bloom_blurring_intencity		= 0.25f;
	bloom_scale						= 0.5f;
	bloom_max_color					= 2.0f;
	enable_bloom					= false;
	
	// SSAO part.	
	enable_ssao						= true;
	ssao_saturation					= 1.0f;
	ssao_radius_scale				= 1.0f;
	
	// Color correction part.	
	frame_desaturation				= 0.0f;
	frame_height_lights				= xray::math::float3(1,1,1);
	frame_mid_tones					= xray::math::float3(1,1,1);
	frame_shadows					= xray::math::float3(0,0,0);
	frame_fade_color				= xray::math::float3(0,0,0);
	frame_fade_amount				= 0.0f;
	frame_gamma_correction_factor	= 1.0f;
	
	enable_scene					= true;
	
	environment_sun_position		= xray::math::float3(0,500,0);
	environment_far_fog_color		= xray::math::float3(1,1,1);
	environment_sun_color			= xray::math::float3(0.87f, 0.8f, 0.8f);
	
	environment_ambient_color		= xray::math::float3(0.15f, 0.15f, 0.15f);
//	environment_ambient_color		= xray::math::float3(1.f, 1.f, 1.f);
	environment_near_fog_distance	= 0.0f;
	environment_far_fog_distance	= 10000.0f;
	environment_shadow_transparency = 0.5f;
	


	environment_skylight_lower_color= xray::math::float4(0.0f, 0.0f, 0.0f, 0.0f);
	environment_skylight_upper_color= xray::math::float4(0.0f, 0.0f, 0.0f, 0.0f);
	environment_skylight_parameters	= xray::math::float4(2.0f, 2.0f, 0.0f, 0.0f);
	
	adaptation_speed				= 1.0f;
	
	wind_direction					= xray::math::float3(1.0f, 0.0f, 1.0f);
	wind_strength					= 0.1f;
	
	tonemap_bright_threshold		= 5.0f / 100.0f;
	tonemap_median					= 50.0f / 100.0f;
	tonemap_darkness_threshold		= 0.0f / 100.0f;
	tonemap_middle_gray				= 0.5f;
	
	use_color_grading_lut			= true;
	
	use_msaa						= true;
	
	use_environment_skycolor		= false;
	
#if XRAY_PLATFORM_WINDOWS
	color_grading_texture			= resource_manager::ref().get_color_grading_base_lut();
#endif // #if XRAY_PLATFORM_WINDOWS

	for (u32 i = 0; i < 6; i++)
		environment_skycolor[i]		= xray::math::float4(0.0f, 0.0f, 0.0f, 0.0f);
}

} // namespace render
} // namespace xray
