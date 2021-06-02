////////////////////////////////////////////////////////////////////////////
//	Created		: 03.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_POSTPROCESS_H_INCLUDED
#define STAGE_POSTPROCESS_H_INCLUDED

#include "stage.h"
#include <xray/render/core/res_texture_list.h>
#include "material_effects.h"

namespace xray {
namespace render {

class shader_constant_host;

class render_target;
class resource_intrusive_base;
typedef intrusive_ptr<render_target, resource_intrusive_base, threading::single_threading_policy> ref_rt;

struct post_process_parameters;


class untyped_buffer;
class resource_intrusive_base;
typedef	intrusive_ptr<
	untyped_buffer,
	resource_intrusive_base,
	threading::single_threading_policy
> untyped_buffer_ptr;

class res_geometry;
typedef	intrusive_ptr<res_geometry, resource_intrusive_base, threading::single_threading_policy>	ref_geometry;

class res_effect;
typedef	xray::resources::resource_ptr < xray::render::res_effect, xray::resources::unmanaged_intrusive_base > ref_effect;

struct bloom_shader_constants
{
	bloom_shader_constants();
	void set(float bloom_scale, float bloom_max_color);
	
private:
	shader_constant_host* m_bloom_parameters;
}; // struct bloom_shader_constants

struct dof_shader_constants
{
	dof_shader_constants();
	void set(xray::math::float3 const& blurriness_height_lights, float distance, float region, float power, float near_blur_amout, float far_blur_amout, float bokeh_dof_radius, float bokeh_dof_density);
	
private:
	shader_constant_host* m_dof_parameters;
	shader_constant_host* m_dof_height_lights;
	shader_constant_host* m_blurriness_amount;
	shader_constant_host* m_bokeh_dof_parameters;
}; // struct dof_shader_constants

struct scene_shader_constants
{
	scene_shader_constants();
	void set(xray::math::float3 const& height_lights, xray::math::float3 const& mid_tones, xray::math::float3 const& shadows, xray::math::float3 const& fade_color, float fade_amount, float gamma_correction_factor, float desaturation);
		
private:
	shader_constant_host* m_frame_height_lights_and_desaturation_parameters;
	shader_constant_host* m_scene_mid_tones_parameters;
	shader_constant_host* m_scene_shadows_parameters;
	shader_constant_host* m_gamma_correction_factor;
	shader_constant_host* m_scene_fade_parameters;
}; // struct scene_shader_constants


class stage_postprocess: public stage
{
public:
	stage_postprocess					(renderer_context* context);
	virtual void execute				();
	virtual void execute_disabled		();
	
private:
	bool is_effects_ready				() const;
	void fill_surface					(ref_rt surf);
	void fill_surface2					(ref_rt surf);
	void clear_surface					(ref_rt surf);
	
	void measure_per_pixel_luminance				(res_texture* scene_texture, xray::math::float4& out_avrg_min_max);
	void compute_per_pixel_eye_adaptated_luminance();
	void measure_per_pixel_luminance_percentage	(res_texture* scene_texture, float min_luminanace, float max_luminanace);
	
	xray::math::float4 compute_luminance_parameters(u32 frame_delta);
	
	void buid_luminance_histogram		(res_texture* scene_texture, u32 num_values, float min_value, float max_value, float* out_array);
	xray::math::float4 get_frame_luminance_parameters	();
	
	//post_process_parameters const&		get_post_process_parameters();
	
	ref_effect							m_sh_gather_bloom;
	ref_effect							m_sh_gather_luminance;
	ref_effect							m_sh_gather_luminance_histogram;
	
	ref_effect							m_sh_eye_adaptation;

	ref_effect							m_sh_blur;
	ref_effect							m_sh_complex_blend[3];
	ref_effect							m_sh_effect_copy_image;
	ref_effect							m_post_process_antialiasing_shader;
	ref_effect							m_post_process_antialiasing_shader_fxaa;

	ref_effect							m_sh_gather_sun_light_scattering_zone;

	untyped_buffer_ptr					m_screen_vertex_ib;
	ref_geometry						m_screen_vertex_geometry;
	
	shader_constant_host*				m_kernel_offsets;
	shader_constant_host*				m_blur_offsets_weights;
	shader_constant_host*				m_luminance_range_parameter_parameter;
	shader_constant_host*				m_elapsed_time_parameter;
	shader_constant_host*				m_adaptation_factor;
	shader_constant_host*				m_gamma_correction_factor;
	
	shader_constant_host*				m_sun_direction_parameter;
	shader_constant_host*				m_frame_luminance_parameter;
	
	xray::math::float4					kernel_offsets[8];
	xray::math::float4					blur_offsets_weights[27];
	
	res_texture_list					m_textures;
	
	ref_texture							m_color_grading_base_lut;
	bloom_shader_constants				m_bloom_shader_constants;
	dof_shader_constants				m_dof_shader_constants;
	scene_shader_constants				m_scene_shader_constants;
	
	render::vector<material_effects>	m_material_post_effects;
	xray::render::material_ptr			m_test_material;
	
}; // class stage_postprocess

} // namespace render
} // namespace xray
#endif // #ifndef STAGE_POSTPROCESS_H_INCLUDED
