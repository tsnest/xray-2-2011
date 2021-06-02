////////////////////////////////////////////////////////////////////////////
//	Created		: 13.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_POSTPROCESS_PARAMETERS_H_INCLUDED
#define XRAY_RENDER_POSTPROCESS_PARAMETERS_H_INCLUDED

#if XRAY_PLATFORM_WINDOWS
#	include <xray/render/core/resource_intrusive_base.h>
#	include <xray/render/core/res_texture.h>
#endif // #if XRAY_PLATFORM_WINDOWS

namespace xray {
namespace render {

#define NUM_TONEMAP_TEXTURES			9
#define NUM_HISTOGRAM_VALUES			16
#define MAX_TONEMAP_TEXTURE_DIMENSION	256
#define NUM_TONEMAP_PIXELS				(MAX_TONEMAP_TEXTURE_DIMENSION * MAX_TONEMAP_TEXTURE_DIMENSION)
// TODO: 0.5f
#define MIN_FRAME_LUMINANCE				0.0f
#define MAX_FRAME_LUMINANCE				4.0f

#if XRAY_PLATFORM_WINDOWS
class res_texture;
typedef intrusive_ptr<res_texture, resource_intrusive_base, threading::single_threading_policy> ref_texture;
#endif // #if XRAY_PLATFORM_WINDOWS

struct post_process_parameters
{
	post_process_parameters();
	
	// DOF part.
	xray::math::float3	dof_height_lights;
	float				dof_focus_power;
	float				dof_focus_region;
	float				dof_focus_distance;
	float				dof_far_blur_amount;
	float				dof_near_blur_amount;
	bool				enable_dof;
	bool				use_bokeh_dof;
	float				bokeh_dof_radius;
	float				bokeh_dof_density;
	bool				use_bokeh_image;
	
	// Bloom part.
	float			    bloom_scale;
	float			    bloom_blurring_kernel;
	float			    bloom_blurring_intencity;
	float				bloom_max_color;
	bool				enable_bloom;
	
	// SSAO part.
	bool				enable_ssao;
	float				ssao_saturation;
	float				ssao_radius_scale;

	// Color correction part.
	float				frame_desaturation;
	xray::math::float3	frame_height_lights;
	xray::math::float3	frame_mid_tones;
	xray::math::float3	frame_shadows;
	xray::math::float3	frame_fade_color;
	float				frame_fade_amount;
	float				frame_gamma_correction_factor;
	bool				enable_scene;
	
	
	//Temporary. Environment.
	xray::math::float3  environment_sun_position;
	xray::math::float3  environment_far_fog_color;
	xray::math::float3  environment_ambient_color;
	xray::math::float3  environment_sun_color;
	
	float			  	environment_far_fog_distance;
	float			  	environment_near_fog_distance;
	float			  	environment_shadow_transparency;
	
	xray::math::float4	environment_skylight_lower_color;
	xray::math::float4	environment_skylight_upper_color;
	
	// upper darkness, power, lower darkness, power
	xray::math::float4	environment_skylight_parameters;
	
	float			  	adaptation_speed;
	
	float				tonemap_bright_threshold;
	float				tonemap_median;
	float				tonemap_darkness_threshold;
	float				tonemap_middle_gray;
	
	
	// TODO: remove from post_process_parameters!
	xray::math::float3  wind_direction;
	float			  	wind_strength;
	
	bool				use_color_grading_lut;
	
	bool				use_msaa;
	
	xray::math::float4 environment_skycolor[6];
	bool				use_environment_skycolor;
	
#if XRAY_PLATFORM_WINDOWS
	ref_texture			color_grading_texture;
#endif // #if XRAY_PLATFORM_WINDOWS
	
}; // struct post_process_parameters

} // namespace render
} // namespace xray

#endif // #ifndef POSTPROCESS_PARAMETERS_H_INCLUDED