////////////////////////////////////////////////////////////////////////////
//	Created		: 25.09.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_RENDERER_CONTEXT_TARGETS_H_INCLUDED
#define XRAY_RENDER_ENGINE_RENDERER_CONTEXT_TARGETS_H_INCLUDED

#include "post_process_parameters.h"

namespace xray {
namespace render {

class render_target;
class resource_intrusive_base;
typedef intrusive_ptr<render_target, resource_intrusive_base, threading::single_threading_policy> ref_rt;

class res_texture;
typedef intrusive_ptr<res_texture, resource_intrusive_base, threading::single_threading_policy> ref_texture;

class renderer_context_targets 
{
	friend class renderer_context;
public:
	renderer_context_targets( math::uint2 size);

	math::uint2		size	() const				{ return m_size; }
	void			resize	( math::uint2 size);
	
	ref_rt				m_rt_gbuffer_position_downsampled;
	ref_texture			m_t_gbuffer_position_downsampled;
	fixed_string<64>	m_rt_gbuffer_position_downsampled_name;
	
	ref_rt				m_rt_gbuffer_normal_downsampled;
	ref_texture			m_t_gbuffer_normal_downsampled;
	fixed_string<64>	m_rt_gbuffer_normal_downsampled_name;
	
	//ref_rt				m_rt_indirect_lighting;
	//ref_texture			m_t_indirect_lighting;
	//fixed_string<64>	m_rt_indirect_lighting_name;
	
	ref_rt				m_rt_mlaa_edges;
	ref_texture			m_t_mlaa_edges;
	fixed_string<64>	m_rt_mlaa_edges_name;
	
	ref_rt				m_rt_mlaa_blended_weights;
	ref_texture			m_t_mlaa_blended_weights;
	fixed_string<64>	m_rt_mlaa_blended_weights_name;
	
	ref_rt			m_rt_light_scattering;
	
	ref_rt			m_rt_position;
	ref_rt			m_rt_precision_ex;
	ref_rt			m_rt_normal;
	ref_rt			m_rt_color;
	ref_rt			m_rt_emissive;
	ref_rt			m_rt_distortion;
	ref_rt			m_rt_ssao_accumulator;
	ref_rt			m_rt_ssao_accumulator_small;
	
	ref_rt			m_rt_decals_diffuse;
	ref_rt			m_rt_decals_normal;
	
	//ref_rt			m_rt_accumulator;
	ref_rt			m_rt_accumulator_diffuse;
	ref_rt			m_rt_accumulator_specular;
	ref_rt			m_rt_shadow_map;
	//ref_rt			m_rt_color_null;	
	
	ref_rt			m_rt_blur_0;
	ref_rt			m_rt_blur_1;
	
	ref_rt			m_rt_present;
	ref_rt			m_rt_present_0;
	
	ref_rt			m_rt_generic_0;
	ref_rt			m_rt_generic_1;
	
	ref_rt				m_rt_frame_luminance[NUM_TONEMAP_TEXTURES];
	ref_texture			m_t_frame_luminance[NUM_TONEMAP_TEXTURES];
	fixed_string<64>	m_rt_frame_luminance_name[NUM_TONEMAP_TEXTURES];
	
	ref_rt				m_rt_frame_luminance_previous;
	ref_texture			m_t_frame_luminance_previous;
	fixed_string<64>	m_rt_frame_luminance_previous_name;
	
	ref_rt				m_rt_frame_luminance_current;
	ref_texture			m_t_frame_luminance_current;
	fixed_string<64>	m_rt_frame_luminance_current_name;
	
	ref_rt				m_rt_frame_luminance_histogram;
	ref_texture			m_t_frame_luminance_histogram;
	fixed_string<64>	m_rt_frame_luminance_histogram_name;
	
	ref_texture			m_t_result_frame_luminance_histogram;
	
	ref_texture			m_t_frame_luminance_lockable;
	
	ref_rt				m_rt_tangents;
	ref_texture			m_t_tangents;
	fixed_string<64>	m_rt_tangents_name;
	
	ref_rt				m_apply_indirect_lighting_ds;
	fixed_string<64>	m_apply_indirect_lighting_ds_name;
	
public:
	math::uint2		m_size;
	
	// Textures of render targets
	ref_texture		m_t_position;
	ref_texture		m_t_position_ex;
	ref_texture 	m_t_normal;
	ref_texture 	m_t_color;
	ref_texture 	m_t_emissive;
	ref_texture 	m_t_distortion;
	ref_texture 	m_t_ssao_accumulator;
	ref_texture 	m_t_ssao_accumulator_small;
	
	
	ref_texture		m_t_decals_diffuse;
	ref_texture		m_t_decals_normal;
	
	
	ref_texture		m_t_light_scattering;
	//ref_texture		m_t_accumulator;
	ref_texture		m_t_accumulator_diffuse;
	ref_texture		m_t_accumulator_specular;

	ref_texture		m_t_present;
	ref_texture		m_t_generic_0;
	ref_texture		m_t_generic_1;

	ref_texture		m_t_blur_0;
	ref_texture		m_t_blur_1;
	
	fixed_string<64>		m_rt_light_scattering_name;
	
	fixed_string<64>		m_rt_decals_diffuse_name;
	fixed_string<64>		m_rt_decals_normal_name;

	fixed_string<64>		m_rt_present_name;
	
	fixed_string<64>		m_rt_position_name;
	fixed_string<64>		m_rt_position_ex_name;
	fixed_string<64> 		m_rt_normal_name;
	fixed_string<64> 		m_rt_color_name;
	fixed_string<64> 		m_rt_emissive_name;
	fixed_string<64> 		m_rt_distortion_name;
	fixed_string<64> 		m_rt_ssao_accumulator_name;
	fixed_string<64> 		m_rt_ssao_accumulator_small_name;
	//fixed_string<64>		m_rt_accumulator_name;
	fixed_string<64>		m_rt_accumulator_diffuse_name;
	fixed_string<64>		m_rt_accumulator_specular_name;
	fixed_string<64>		m_rt_generic_0_name;
	fixed_string<64>		m_rt_generic_1_name;
	fixed_string<64>		m_rt_blur_0_name;
	fixed_string<64>		m_rt_blur_1_name;
	fixed_string<64>		m_t_result_frame_luminance_histogram_name;
	fixed_string<64>		m_t_frame_luminance_lockable_name;
	
private:
	void	create_targets	( math::uint2 size);
	
	static u32		s_new_id;
	int				m_id;

}; // class renderer_context_targets

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_ENGINE_RENDERER_CONTEXT_TARGETS_H_INCLUDED