////////////////////////////////////////////////////////////////////////////
//	Created		: 25.09.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "renderer_context_targets.h"
#include "shared_names.h"
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/render_target.h>
#include <xray/render/core/backend.h>

namespace xray {
namespace render {

u32		renderer_context_targets::s_new_id = 0;

renderer_context_targets::renderer_context_targets( math::uint2 size)
{
	// zero m_size!
	m_size = math::uint2(0, 0);
	create_targets( size);
}

void	renderer_context_targets::create_targets( math::uint2 size)
{
	if( m_size == size)
		return;

	m_size = size;

	m_id = s_new_id++;
	ASSERT( s_new_id > 0, "This means we reached u32 max !");
	

/*
	ref_rt				m_rt_gbuffer_position_downsamped;
	ref_texture			m_t_gbuffer_position_downsamped;
	fixed_string<64>	m_rt_gbuffer_position_downsamped_name;
	
	ref_rt				m_rt_gbuffer_normal_downsamped;
	ref_texture			m_t_gbuffer_normal_downsamped;
	fixed_string<64>	m_rt_gbuffer_normal_downsamped_name;
*/
	
	m_rt_gbuffer_position_downsampled_name.assignf( "%s_%d", r2_rt_gbuffer_position_downsampled, m_id);
	m_rt_gbuffer_position_downsampled	= resource_manager::ref().create_render_target( m_rt_gbuffer_position_downsampled_name.get_buffer(), size.width / 4, size.height / 4, DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);
	
	m_rt_gbuffer_normal_downsampled_name.assignf( "%s_%d", r2_rt_gbuffer_normal_downsampled, m_id);
	m_rt_gbuffer_normal_downsampled	= resource_manager::ref().create_render_target( m_rt_gbuffer_normal_downsampled_name.get_buffer(), size.width / 4, size.height / 4, DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);
	
	//m_rt_indirect_lighting_name.assignf( "%s_%d", r2_rt_indirect_lighting, m_id);
	//m_rt_indirect_lighting	= resource_manager::ref().create_render_target( m_rt_indirect_lighting_name.get_buffer(), size.width, size.height, DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);
		
	m_apply_indirect_lighting_ds_name.assignf( "$user$apply_indirect_lighting_ds_%d", m_id);
	m_apply_indirect_lighting_ds	= resource_manager::ref().create_render_target(m_apply_indirect_lighting_ds_name.get_buffer(), size.width, size.height, DXGI_FORMAT_R24G8_TYPELESS, enum_rt_usage_depth_stencil);
	
	m_rt_position_ex_name.assignf( "%s_%d", r2_rt_p_ex, m_id);
	m_rt_precision_ex	= resource_manager::ref().create_render_target( m_rt_position_ex_name.get_buffer(), size.width, size.height,	DXGI_FORMAT_R16G16_UNORM, enum_rt_usage_render_target);

	//m_rt_position_name.assignf( "%s_%d", r2_rt_p, m_id);
	//m_rt_position		= resource_manager::ref().create_render_target( m_rt_position_name.get_buffer(), size.width, size.height,		DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);

	//m_rt_normal_name.assignf( "%s_%d", r2_rt_n, m_id);
	//m_rt_normal			= resource_manager::ref().create_render_target( m_rt_normal_name.get_buffer(), size.width, size.height,		DXGI_FORMAT_R16G16B16A16_FLOAT,	enum_rt_usage_render_target);

	//m_rt_color_name.assignf( "%s_%d", r2_rt_albedo, m_id);
	//m_rt_color			= resource_manager::ref().create_render_target( m_rt_color_name.get_buffer(), size.width, size.height, DXGI_FORMAT_R8G8B8A8_UNORM,		enum_rt_usage_render_target);//take into account mixed_depth!!!!!!!!
	
	m_rt_tangents_name.assignf( "%s_%d", r2_rt_tangents, m_id);
	m_rt_tangents		= resource_manager::ref().create_render_target( m_rt_tangents_name.get_buffer(), size.width, size.height,		DXGI_FORMAT_R8G8B8A8_UNORM, enum_rt_usage_render_target);
	
	m_rt_position_name.assignf( "%s_%d", r2_rt_p, m_id);
	m_rt_position		= resource_manager::ref().create_render_target( m_rt_position_name.get_buffer(), size.width, size.height,		DXGI_FORMAT_R16G16B16A16_UNORM, enum_rt_usage_render_target);

	m_rt_normal_name.assignf( "%s_%d", r2_rt_n, m_id);
	m_rt_normal			= resource_manager::ref().create_render_target( m_rt_normal_name.get_buffer(), size.width, size.height,		DXGI_FORMAT_R16G16B16A16_FLOAT,	enum_rt_usage_render_target);

	m_rt_color_name.assignf( "%s_%d", r2_rt_albedo, m_id);
	m_rt_color			= resource_manager::ref().create_render_target( m_rt_color_name.get_buffer(), size.width, size.height, DXGI_FORMAT_R8G8B8A8_UNORM,		enum_rt_usage_render_target);//take into account mixed_depth!!!!!!!!

	m_rt_emissive_name.assignf( "%s_%d", r2_rt_emissive, m_id);
	m_rt_emissive		= resource_manager::ref().create_render_target( m_rt_emissive_name.get_buffer(), size.width, size.height, DXGI_FORMAT_R10G10B10A2_UNORM, enum_rt_usage_render_target);

	m_rt_distortion_name.assignf( "%s_%d", r2_rt_distortion, m_id);
	m_rt_distortion		= resource_manager::ref().create_render_target( m_rt_distortion_name.get_buffer(), size.width, size.height, DXGI_FORMAT_R11G11B10_FLOAT, enum_rt_usage_render_target);
	
	m_rt_ssao_accumulator_name.assignf( "%s_%d", r2_rt_ssao_accumulator, m_id);
	m_rt_ssao_accumulator		= resource_manager::ref().create_render_target( m_rt_ssao_accumulator_name.get_buffer(), size.width, size.height, DXGI_FORMAT_R8_UNORM, enum_rt_usage_render_target);
	
	m_rt_ssao_accumulator_small_name.assignf( "%s_%d", r2_rt_ssao_accumulator_small, m_id);
	m_rt_ssao_accumulator_small	= resource_manager::ref().create_render_target( m_rt_ssao_accumulator_small_name.get_buffer(), size.width, size.height, DXGI_FORMAT_R8_UNORM, enum_rt_usage_render_target);
	
	//name.assignf( "%s_%d", r2_rt_p_ex, m_id);
	//m_rt_accumulator			= resource_manager::ref().create_render_target( name.get_buffer(), size.width, size.height,	DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);
	m_rt_accumulator_diffuse_name.assignf( "%s_%d", r2_rt_accum_diffuse, m_id);
	m_rt_accumulator_diffuse	= resource_manager::ref().create_render_target( m_rt_accumulator_diffuse_name.get_buffer(), size.width, size.height,	DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);

	m_rt_accumulator_specular_name.assignf( "%s_%d", r2_rt_accum_specular, m_id);
	m_rt_accumulator_specular	= resource_manager::ref().create_render_target( m_rt_accumulator_specular_name.get_buffer(), size.width, size.height,	DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);

	
	m_rt_light_scattering_name.assignf( "%s_%d", r2_rt_light_scattering, m_id);
	m_rt_light_scattering	= resource_manager::ref().create_render_target( m_rt_light_scattering_name.get_buffer(), size.width/4, size.height/4,	DXGI_FORMAT_R8G8B8A8_UNORM, enum_rt_usage_render_target);
	

	m_rt_present_name.assignf( "%s_%d", r2_rt_present, m_id);
	m_rt_present			= resource_manager::ref().create_render_target( m_rt_present_name.get_buffer(), size.width, size.height, DXGI_FORMAT_R8G8B8A8_UNORM, enum_rt_usage_render_target);
	
	// generic( LDR) RTs
	m_rt_generic_0_name.assignf( "%s_%d", r2_rt_generic0, m_id);
	m_rt_generic_0			= resource_manager::ref().create_render_target( m_rt_generic_0_name.get_buffer(), size.width, size.height, DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);
	
	m_rt_generic_1_name.assignf( "%s_%d", r2_rt_generic1, m_id);
	m_rt_generic_1			= resource_manager::ref().create_render_target( m_rt_generic_1_name.get_buffer(), size.width, size.height, DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);

	
	// blur RTs
	m_rt_blur_0_name.assignf( "%s_%d", r2_rt_blur0, m_id);
	m_rt_blur_0				= resource_manager::ref().create_render_target( m_rt_blur_0_name.get_buffer(), size.width/4, size.height/4, DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);
	
	m_rt_blur_1_name.assignf( "%s_%d", r2_rt_blur1, m_id);
	m_rt_blur_1				= resource_manager::ref().create_render_target( m_rt_blur_1_name.get_buffer(), size.width/4, size.height/4, DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);	
	
	m_rt_decals_diffuse_name.assignf( "%s_%d", r2_rt_decals_diffuse, m_id);
	m_rt_decals_diffuse		= resource_manager::ref().create_render_target( m_rt_decals_diffuse_name.get_buffer(), size.width, size.height, DXGI_FORMAT_R8G8B8A8_UNORM, enum_rt_usage_render_target);
	
	m_rt_decals_normal_name.assignf( "%s_%d", r2_rt_decals_normal, m_id);
	m_rt_decals_normal		= resource_manager::ref().create_render_target( m_rt_decals_normal_name.get_buffer(), size.width, size.height, DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);
	
	u32 lum_rt_size			= 1;
	
	for (u32 i=0; i<NUM_TONEMAP_TEXTURES; i++)
	{
		m_rt_frame_luminance_name[i].assignf( "%s%d_%d", r2_rt_frame_luminance, i, m_id);
		m_rt_frame_luminance[i]	= resource_manager::ref().create_render_target( m_rt_frame_luminance_name[i].get_buffer(), lum_rt_size, lum_rt_size, DXGI_FORMAT_R32G32B32A32_FLOAT, enum_rt_usage_render_target);	
		lum_rt_size *= 2;
	}
	
	m_rt_frame_luminance_current_name.assignf( "%s_%d", r2_rt_frame_luminance, m_id);
	m_rt_frame_luminance_current	= resource_manager::ref().create_render_target( m_rt_frame_luminance_current_name.get_buffer(), 1, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, enum_rt_usage_render_target);
	
	m_rt_frame_luminance_previous_name.assignf( "%s_%d", r2_rt_frame_luminance_previous, m_id);
	m_rt_frame_luminance_previous	= resource_manager::ref().create_render_target( m_rt_frame_luminance_previous_name.get_buffer(), 1, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, enum_rt_usage_render_target);

	m_rt_frame_luminance_histogram_name.assignf("%s_%d", r2_rt_frame_luminance_histogram, m_id);
	m_rt_frame_luminance_histogram	= resource_manager::ref().create_render_target( m_rt_frame_luminance_histogram_name.get_buffer(), NUM_HISTOGRAM_VALUES, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, enum_rt_usage_render_target, 0, 0);	

	// Textures to render targets
	m_t_position			= resource_manager::ref().create_texture( m_rt_position_name.get_buffer());
	
	m_t_position_ex			= resource_manager::ref().create_texture( m_rt_position_ex_name.get_buffer());
	
	m_t_normal				= resource_manager::ref().create_texture( m_rt_normal_name.get_buffer());
	
	m_t_color				= resource_manager::ref().create_texture( m_rt_color_name.get_buffer());
	
	m_t_emissive			= resource_manager::ref().create_texture( m_rt_emissive_name.get_buffer());
	
	m_t_distortion			= resource_manager::ref().create_texture( m_rt_distortion_name.get_buffer());

	m_t_ssao_accumulator	= resource_manager::ref().create_texture( m_rt_ssao_accumulator_name.get_buffer());
	m_t_ssao_accumulator_small	= resource_manager::ref().create_texture( m_rt_ssao_accumulator_small_name.get_buffer());
	
	
	m_t_light_scattering	= resource_manager::ref().create_texture( m_rt_light_scattering_name.get_buffer());
	
	//m_t_accumulator			= resource_manager::ref().create_texture( m_rt_color_name.get_buffer());
	
	m_t_accumulator_diffuse	= resource_manager::ref().create_texture( m_rt_accumulator_diffuse_name.get_buffer());
	
	m_t_accumulator_specular = resource_manager::ref().create_texture( m_rt_accumulator_specular_name.get_buffer());
	
	m_t_present				= resource_manager::ref().create_texture( m_rt_present_name.get_buffer());
	
	m_t_generic_0			= resource_manager::ref().create_texture( m_rt_generic_0_name.get_buffer());
	m_t_generic_1			= resource_manager::ref().create_texture( m_rt_generic_1_name.get_buffer());
	
	m_t_blur_0				= resource_manager::ref().create_texture( m_rt_blur_0_name.get_buffer());
	m_t_blur_1				= resource_manager::ref().create_texture( m_rt_blur_1_name.get_buffer());
	
	m_t_decals_diffuse		= resource_manager::ref().create_texture( m_rt_decals_diffuse_name.get_buffer());
	m_t_decals_normal		= resource_manager::ref().create_texture( m_rt_decals_normal_name.get_buffer());
	
	for (u32 i=0; i<NUM_TONEMAP_TEXTURES; i++)
		m_t_frame_luminance[i]	= resource_manager::ref().create_texture( m_rt_frame_luminance_name[i].get_buffer());

	m_t_frame_luminance_current	= resource_manager::ref().create_texture( m_rt_frame_luminance_current_name.get_buffer());
	
	m_t_frame_luminance_previous = resource_manager::ref().create_texture( m_rt_frame_luminance_previous_name.get_buffer());
	
	m_t_frame_luminance_histogram = resource_manager::ref().create_texture( m_rt_frame_luminance_histogram_name.get_buffer());
	
	m_t_result_frame_luminance_histogram_name.assignf("$user$result_frame_luminance_histogram_%d", m_id);
	m_t_result_frame_luminance_histogram = resource_manager::ref().create_texture2d( m_t_result_frame_luminance_histogram_name.get_buffer(), NUM_HISTOGRAM_VALUES, 1, 0, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D_USAGE_STAGING, 0, 1 );
	
	m_t_frame_luminance_lockable_name.assignf("$user$frame_luminance_lockable_%d", m_id);
	m_t_frame_luminance_lockable = resource_manager::ref().create_texture2d( m_t_frame_luminance_lockable_name.get_buffer(), 1, 1, 0, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D_USAGE_STAGING, 0, 1 );
	
	m_t_tangents = resource_manager::ref().create_texture( m_rt_tangents_name.get_buffer());
	
	
	// MLAA render targets.
	m_rt_mlaa_edges_name.assignf			("%s_%d", r2_rt_mlaa_edges, m_id);
	m_rt_mlaa_blended_weights_name.assignf	("%s_%d", r2_rt_mlaa_blended_weights, m_id);
	
	m_rt_mlaa_edges				= resource_manager::ref().create_render_target(m_rt_mlaa_edges_name.get_buffer(), size.width, size.height, DXGI_FORMAT_R8G8B8A8_UNORM, enum_rt_usage_render_target);
	m_rt_mlaa_blended_weights	= resource_manager::ref().create_render_target(m_rt_mlaa_blended_weights_name.get_buffer(), size.width, size.height, DXGI_FORMAT_R8G8B8A8_UNORM, enum_rt_usage_render_target);
	
	m_t_mlaa_edges				= resource_manager::ref().create_texture(m_rt_mlaa_edges_name.get_buffer());
	m_t_mlaa_blended_weights	= resource_manager::ref().create_texture(m_rt_mlaa_blended_weights_name.get_buffer());
	
	m_t_gbuffer_position_downsampled	= resource_manager::ref().create_texture(m_rt_gbuffer_position_downsampled_name.get_buffer());
	m_t_gbuffer_normal_downsampled	= resource_manager::ref().create_texture(m_rt_gbuffer_normal_downsampled_name.get_buffer());

	//m_t_indirect_lighting	= resource_manager::ref().create_texture(m_rt_indirect_lighting_name.get_buffer());
	
	backend::ref().set_render_targets	( &*m_rt_frame_luminance_previous, 0, 0, 0);
	backend::ref().clear_render_targets	(0.0f, 0.0f, 0.0f, 0.f);
	
	backend::ref().set_render_targets	( &*m_rt_frame_luminance_current, 0, 0, 0);
	backend::ref().clear_render_targets	(0.0f, 0.0f, 0.0f, 0.f);










}

void	renderer_context_targets::resize	( math::uint2 size)
{
	create_targets( size);
}


} // namespace render
} // namespace xray
