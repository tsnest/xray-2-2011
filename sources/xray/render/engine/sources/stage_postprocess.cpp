////////////////////////////////////////////////////////////////////////////
//	Created		: 03.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/custom_config.h>
#include "stage_postprocess.h"
#include <xray/render/core/backend.h>
#include <xray/render/core/effect_manager.h>
#include "effect_gather_bloom.h"
#include "effect_gather_luminance.h"
#include "effect_gather_luminance_histogram.h"
#include "effect_blur.h"
#include "effect_complex_post_process_blend.h"
#include "effect_copy_image.h"
#include "effect_eye_adaptation.h"
#include "effect_gather_sun_light_scattering_zone.h"
#include "material_manager.h"
#include <xray/render/core/render_target.h>
#include "vertex_formats.h"
#include "renderer_context.h"
#include <xray/render/core/res_geometry.h>
#include "scene_view.h"
#include "scene.h"
#include "lights_db.h"
#include "light.h"
#include "renderer_context_targets.h"
#include <xray/render/core/pix_event_wrapper.h>
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/res_effect.h>
#include <xray/render/core/effect_options_descriptor.h>
#include <xray/render/core/untyped_buffer.h>
#include "statistics.h"
#include <xray/render/core/options.h>
#include "effect_post_process_mlaa.h"
#include "effect_post_process_fxaa.h"

namespace xray {
namespace render {

static ref_texture create_color_grading_base_lut(u32 const size)
{
	struct pixel
	{
#	pragma warning(push)
#	pragma warning(disable:4201)
		union
		{
			u32		clr;
			struct
			{
				u8	r;
				u8	g;
				u8	b;
				u8	a;
			};
		};
#	pragma warning(pop)
	};
	
	u32 const data_size			= size * size * size * sizeof(pixel);
	pixel* temp_data			= (pixel*)ALLOCA(data_size);
	
	for (u32 z = 0; z < size; z++)
	{
		for (u32 y = 0; y < size; y++)
		{
			for (u32 x = 0; x < size; x++)
			{
				pixel* p	= temp_data + z * size * size + y * size + x;
				p->r		= static_cast_checked<u8>(x * size);
				p->g		= static_cast_checked<u8>(y * size);
				p->b		= static_cast_checked<u8>(z * size);
				p->a		= 255;
			}
		}
	}
	
	D3D_SUBRESOURCE_DATA	data;
	data.pSysMem			= temp_data;
	data.SysMemSlicePitch	= data_size / size;
	data.SysMemPitch		= data_size / (size * size);
	
	return					resource_manager::ref().create_texture3d(
		"$user$color_grading_base_3d_lut", 
		size, 
		size, 
		size, 
		&data, 
		DXGI_FORMAT_R8G8B8A8_UNORM, 
		D3D_USAGE_IMMUTABLE, 
		1
	);
}


bloom_shader_constants::bloom_shader_constants()
{
	m_bloom_parameters = backend::ref().register_constant_host( "bloom_parameters", rc_float );
}

void bloom_shader_constants::set(float bloom_scale, float bloom_max_color)
{
	float4 bloom_parameters(bloom_scale, bloom_max_color, 0.0f, 0.0f);
	
	backend::ref().set_ps_constant(m_bloom_parameters, bloom_parameters);
}

dof_shader_constants::dof_shader_constants()
{
	m_dof_height_lights		= backend::ref().register_constant_host( "dof_height_lights", rc_float );
	m_dof_parameters		= backend::ref().register_constant_host( "dof_parameters", rc_float );
	m_blurriness_amount		= backend::ref().register_constant_host( "blurriness_amount", rc_float );
	m_bokeh_dof_parameters	= backend::ref().register_constant_host( "bokeh_dof_parameters", rc_float );
}

void dof_shader_constants::set(xray::math::float3 const& blurriness_height_lights, float distance, float region, float power, float near_blur_amout, float far_blur_amout, float bokeh_dof_radius, float bokeh_dof_density)
{
	backend::ref().set_ps_constant(m_dof_height_lights, float4(blurriness_height_lights, 0));
	backend::ref().set_ps_constant(m_dof_parameters, float4(distance, region, power, 0));
	backend::ref().set_ps_constant(m_blurriness_amount, float4(near_blur_amout, far_blur_amout, 0, 0));
	backend::ref().set_ps_constant(m_bokeh_dof_parameters, float4(bokeh_dof_radius, bokeh_dof_density, 0, 0));
}


scene_shader_constants::scene_shader_constants()
{
	m_frame_height_lights_and_desaturation_parameters	= backend::ref().register_constant_host( "frame_height_lights_and_desaturation", rc_float );
	m_scene_mid_tones_parameters						= backend::ref().register_constant_host( "frame_mid_tones", rc_float );
	m_scene_shadows_parameters							= backend::ref().register_constant_host( "frame_shadows", rc_float );
	m_gamma_correction_factor							= backend::ref().register_constant_host( "gamma_correction_factor", rc_float );
	m_scene_fade_parameters								= backend::ref().register_constant_host( "frame_fade", rc_float );
}

void scene_shader_constants::set(float3 const& height_lights, float3 const& mid_tones, float3 const& shadows, float3 const& fade_color, float fade_amount, float gamma_correction_factor, float desaturation)
{
	float4 frame_height_lights_and_desaturation(height_lights, desaturation);
	float4 scene_fade(fade_color, fade_amount);
	
	backend::ref().set_ps_constant(m_frame_height_lights_and_desaturation_parameters, frame_height_lights_and_desaturation);
	backend::ref().set_ps_constant(m_scene_mid_tones_parameters, mid_tones);
	backend::ref().set_ps_constant(m_scene_shadows_parameters, shadows);
	backend::ref().set_ps_constant(m_gamma_correction_factor, gamma_correction_factor);
	backend::ref().set_ps_constant(m_scene_fade_parameters, scene_fade);
}


static float compute_gaussian_value(float x, float /*mean*/, float std_deviation)
{
	return ( 1.0f / xray::math::sqrt( 2.0f * xray::math::pi * std_deviation * std_deviation ) )
		* xray::math::exp( (-x*x)/(2.0f * std_deviation * std_deviation) );
}

static void get_gaussain_weights_offsets(float* out_weights, float* out_offsets, u32 buffer_size, float blur_scale, float blur_intencity, u32 num_samples)
{
	for (u32 i=0; i<num_samples; i++)
	{
		out_offsets[i] = (static_cast<float>(i)-4.0f)*(1.0f/static_cast<float>(buffer_size));
		float x = (static_cast<float>(i) - 4.0f) / 4.0f;
		out_weights[i] = blur_intencity * compute_gaussian_value( x, 0.0f, math::max(blur_scale,0.25f));
	}
}

struct screen_vertex
{
	xray::math::float4 position;
	xray::math::float2 tc;
	void set(xray::math::float4 const& in_position, xray::math::float2 const& in_tc)
	{
		position = in_position;
		tc		 = in_tc;
	}
}; // struct screen_vertex

bool stage_postprocess::is_effects_ready() const
{
	return 
		   m_sh_gather_bloom.c_ptr() != NULL
		&& m_sh_gather_luminance.c_ptr() != NULL
		&& m_sh_gather_luminance_histogram.c_ptr() != NULL
		&& m_sh_eye_adaptation.c_ptr() != NULL
		&& m_sh_blur.c_ptr() != NULL
		&& m_sh_complex_blend[0].c_ptr() != NULL
		&& m_sh_complex_blend[1].c_ptr() != NULL
		&& m_sh_complex_blend[2].c_ptr() != NULL
		&& m_sh_effect_copy_image.c_ptr() != NULL
		&& m_post_process_antialiasing_shader != NULL
		&& m_post_process_antialiasing_shader_fxaa != NULL
		&& m_sh_gather_sun_light_scattering_zone.c_ptr() != NULL;
}

stage_postprocess::stage_postprocess(renderer_context* context) : stage( context)
{
	effect_manager::ref().create_effect<effect_gather_bloom>(&m_sh_gather_bloom);
	effect_manager::ref().create_effect<effect_gather_luminance>(&m_sh_gather_luminance);
	effect_manager::ref().create_effect<effect_gather_luminance_histogram>(&m_sh_gather_luminance_histogram);
	effect_manager::ref().create_effect<effect_eye_adaptation>(&m_sh_eye_adaptation);
	effect_manager::ref().create_effect<effect_blur>(&m_sh_blur);
	effect_manager::ref().create_effect< effect_complex_post_process_blend<false> >(&m_sh_complex_blend[0]);
	effect_manager::ref().create_effect< effect_complex_post_process_blend<true, false> >(&m_sh_complex_blend[1]);
	effect_manager::ref().create_effect< effect_complex_post_process_blend<true, true> >(&m_sh_complex_blend[2]);
	effect_manager::ref().create_effect<effect_copy_image>(&m_sh_effect_copy_image);
	effect_manager::ref().create_effect<effect_post_process_mlaa>(&m_post_process_antialiasing_shader);
	effect_manager::ref().create_effect<effect_post_process_fxaa>(&m_post_process_antialiasing_shader_fxaa);
	
	effect_manager::ref().create_effect<effect_gather_sun_light_scattering_zone>(&m_sh_gather_sun_light_scattering_zone);
	

	m_blur_offsets_weights	= backend::ref().register_constant_host("offsets_weights", rc_float);
	m_kernel_offsets		= backend::ref().register_constant_host("kernel_offsets", rc_float);
	
	m_elapsed_time_parameter= backend::ref().register_constant_host("elapsed_time", rc_float);
	m_adaptation_factor		= backend::ref().register_constant_host("adaptation_factor", rc_float);
	
	m_sun_direction_parameter = backend::ref().register_constant_host("sun_direction_parameter", rc_float);

	m_frame_luminance_parameter = backend::ref().register_constant_host("frame_luminance_parameter", rc_float);
	
	m_luminance_range_parameter_parameter			 = backend::ref().register_constant_host( "luminance_range_parameter", rc_float );
	
	m_gamma_correction_factor							= backend::ref().register_constant_host( "gamma_correction_factor", rc_float );
	
	m_color_grading_base_lut = create_color_grading_base_lut(16);
	//resource_manager::ref().create_texture("resources/textures/color_grading/color_grading_base_lut");
	
	//m_material_post_effects.push_back(material_effects());
	
	//xray::resources::query_resource(
 	//	"default_post_process_and_environment", 
 	//	resources::material_class, 
 	//	boost::bind(&stage_postprocess::on_material_loaded, this, _1), 
 	//	render::g_allocator
 	//);
	m_textures.resize(10, NULL);

	const D3D_INPUT_ELEMENT_DESC screen_vertex_layout[] = 
	{
		{"POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	    0, 16,	D3D_INPUT_PER_VERTEX_DATA, 0},
	};
	
	u16 indices[6]			= { 0, 1, 2, 3, 2, 1 };
	m_screen_vertex_ib		= resource_manager::ref().create_buffer( 6*sizeof(u16), indices, enum_buffer_type_index, false);
	m_screen_vertex_geometry	= resource_manager::ref().create_geometry(
		screen_vertex_layout,
		sizeof(screen_vertex),
		backend::ref().vertex.buffer(),
		*m_screen_vertex_ib
	);

	m_enabled						= options::ref().m_enabled_post_process_stage;
	
	
}

//void stage_postprocess::on_material_loaded(xray::resources::queries_result& result)
//{
//	if (result.is_successful())
//	{
//		//m_test_material = static_cast_checked<xray::render::material*>(result[0].get_unmanaged_resource().c_ptr());
//		//xray::render::material_manager::ref().initialize_material_effects(m_material_post_effects[0], m_test_material, false);
//		//m_context->m_post_process_parameters = get_post_process_parameters();
//	}
//}

void stage_postprocess::fill_surface(ref_rt surf)
{
	float t_w = float(surf->width());
	float t_h = float(surf->height());
	
	backend::ref().set_render_targets( &*surf, 0, 0, 0);
	backend::ref().set_depth_stencil_target(0);//reset_depth_stencil_target();
	
	u32	    color = math::color_rgba( 255u, 255u, 255u, 255u);
	
	float2	p0( 0,0);
	float2	p1( 1,1);
	
	u32		offset;
	
	vertex_formats::TL* pv = ( vertex_formats::TL*)backend::ref().vertex.lock( 4, sizeof(vertex_formats::TL), offset);
	pv->set( 0,	  t_h, 0, 1.0, color, p0.x, p1.y); pv++;
	pv->set( 0,	  0,   0, 1.0, color, p0.x, p0.y); pv++;
	pv->set( t_w, t_h, 0, 1.0, color, p1.x, p1.y); pv++;
	pv->set( t_w, 0,   0, 1.0, color, p1.x, p0.y); pv++;
	backend::ref().vertex.unlock();
	
	m_context->m_g_quad_uv->apply();
	
	backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 2*3, 0, offset);
}

// TODO: rename
void stage_postprocess::fill_surface2(ref_rt surf)
{
	backend::ref().set_render_targets( &*surf, 0, 0, 0);
	backend::ref().set_depth_stencil_target(0);
	
	u32		offset;
	
	screen_vertex* pv = (screen_vertex*)backend::ref().vertex.lock(4, sizeof(screen_vertex), offset);
	pv->set( float4(-1.0f, -1.0f, 0.0f, 1.0f), float2(0.0f, 1.0f)); pv++;
	pv->set( float4(-1.0f,  1.0f, 0.0f, 1.0f), float2(0.0f, 0.0f)); pv++;
	pv->set( float4( 1.0f, -1.0f, 0.0f, 1.0f), float2(1.0f, 1.0f)); pv++;
	pv->set( float4( 1.0f,  1.0f, 0.0f, 1.0f), float2(1.0f, 0.0f)); pv++;
	backend::ref().vertex.unlock();
	
	m_screen_vertex_geometry->apply();
	
	backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 2*3, 0, offset);
}

//post_process_parameters const& stage_postprocess::get_post_process_parameters()
//{
//	return m_material_post_effects[0].m_post_process_stage_parameters;
//}

void stage_postprocess::buid_luminance_histogram(res_texture* scene_texture, u32 num_values, float min_value, float max_value, float* out_array)
{
	D3D_VIEWPORT orig_viewport;
	backend::ref().get_viewport			(orig_viewport);
	
	backend::ref().set_render_targets	(&*m_context->m_targets->m_rt_frame_luminance_histogram, 0, 0, 0);
	backend::ref().clear_render_targets	(0.0f, 0.0f, 0.0f, 0.0f);
	
	u32 const num_histogam_values		= math::min<u32>(num_values, NUM_HISTOGRAM_VALUES);
	float2* ranges						= (float2*)ALLOCA(sizeof(float2) * num_histogam_values);
	float const step					= (max_value - min_value) / num_histogam_values;
	float c								= min_value;
	
	for (u32 i=0; i<num_histogam_values; i++)
	{
		ranges[i] = float2(c, c + step);
		c += step;
		if (i==num_histogam_values-1)
			ranges[i].y = 100000.0f;
	}
	
	for (u32 i=0; i<num_histogam_values; i++)
	{
		measure_per_pixel_luminance_percentage(scene_texture, ranges[i].x, ranges[i].y);
		
		D3D_VIEWPORT tmp_viewport;
		tmp_viewport.TopLeftX	= float(i);
		tmp_viewport.TopLeftY	= 0.0f;
		tmp_viewport.Width		= 1;
		tmp_viewport.Height		= 1;
		tmp_viewport.MinDepth	= 0;
		tmp_viewport.MaxDepth	= 1.f;
		backend::ref().set_viewport( tmp_viewport);
		
		m_sh_gather_luminance_histogram->apply(effect_gather_luminance_histogram::gather_luminance_histogram);
		fill_surface2(m_context->m_targets->m_rt_frame_luminance_histogram);
		
		backend::ref().set_viewport( orig_viewport);
	}
	
	if (out_array)
	{
		resource_manager::ref().copy2D(
			&*m_context->m_targets->m_t_result_frame_luminance_histogram,
			0,
			0,
			&*m_context->m_targets->m_t_frame_luminance_histogram,
			0,
			0,
			m_context->m_targets->m_t_frame_luminance_histogram->width(),
			m_context->m_targets->m_t_frame_luminance_histogram->height()
		);
		
		u32 row_pitch = 0;
		
		float4* data = (float4*)m_context->m_targets->m_t_result_frame_luminance_histogram->map2D(D3D_MAP_READ, 0, row_pitch);
		
		for (u32 i=0; i<num_histogam_values; i++)
			out_array[i] = (*data++).x;
		
		m_context->m_targets->m_t_result_frame_luminance_histogram->unmap2D(0);
	}
}

void stage_postprocess::measure_per_pixel_luminance_percentage (res_texture* scene_texture, float min_luminanace, float max_luminanace)
{
//	post_process_parameters const& pp_parameters = m_context->scene_view()->post_process_parameters();
	
	s32 const last_target_index = NUM_TONEMAP_TEXTURES - 1;
	
	for (u32 lum_rt_index=0; lum_rt_index<NUM_TONEMAP_TEXTURES; lum_rt_index++)
	{
		backend::ref().set_render_targets	(&*m_context->m_targets->m_rt_frame_luminance[lum_rt_index], 0, 0, 0);
		backend::ref().clear_render_targets	(0.0f, 0.0f, 0.0f, 0.0f);
	}
	
	m_sh_gather_luminance_histogram->apply(effect_gather_luminance_histogram::gather_luminance_in_range);
	backend::ref().set_ps_constant(m_luminance_range_parameter_parameter, float4(min_luminanace, max_luminanace, 0.0f, 0.0f));
	backend::ref().set_ps_texture( "t_frame_color0", scene_texture);
	fill_surface(m_context->m_targets->m_rt_frame_luminance[last_target_index]);
	
	for (s32 lum_rt_index=last_target_index-1; lum_rt_index>=0; lum_rt_index--)
	{
		m_sh_gather_luminance_histogram->apply(effect_gather_luminance_histogram::gather_luminance_count);
		backend::ref().set_ps_texture( "t_frame_color1", &*m_context->m_targets->m_t_frame_luminance[lum_rt_index+1]);
		fill_surface(m_context->m_targets->m_rt_frame_luminance[lum_rt_index]);
	}
	
	resource_manager::ref().copy2D(
		&*m_context->m_targets->m_t_frame_luminance_current,
		0,
		0,
		&*m_context->m_targets->m_t_frame_luminance[0],
		0,
		0,
		m_context->m_targets->m_t_frame_luminance[0]->width(),
		m_context->m_targets->m_t_frame_luminance[0]->height()
	);
}

void stage_postprocess::clear_surface(ref_rt surf)
{
	backend::ref().set_render_targets	( &*surf, 0, 0, 0);
	backend::ref().clear_render_targets	(0.0f, 0.0f, 0.0f, 0.f);
}

void stage_postprocess::measure_per_pixel_luminance(res_texture* scene_texture, xray::math::float4& out_avrg_min_max)
{
	XRAY_UNREFERENCED_PARAMETER(out_avrg_min_max);
	
	s32 const last_target_index = NUM_TONEMAP_TEXTURES - 1;
	
	m_sh_gather_luminance->apply(effect_gather_luminance::gather_log_luminance);
	backend::ref().set_ps_texture( "t_frame_color0", scene_texture);
	fill_surface(m_context->m_targets->m_rt_frame_luminance[last_target_index]);
	
	for (s32 lum_rt_index=last_target_index-1; lum_rt_index>=0; lum_rt_index--)
	{
		if (lum_rt_index==0)
			m_sh_gather_luminance->apply(effect_gather_luminance::gather_exp_luminance);
		else
			m_sh_gather_luminance->apply(effect_gather_luminance::gather_luminance);
		
		backend::ref().set_ps_texture( "t_frame_color1", &*m_context->m_targets->m_t_frame_luminance[lum_rt_index+1]);
		fill_surface(m_context->m_targets->m_rt_frame_luminance[lum_rt_index]);
	}
	
/*	resource_manager::ref().copy2D(
		&*m_context->m_targets->m_t_frame_luminance_lockable,
		0,
		0,
		&*m_context->m_targets->m_t_frame_luminance[0],
		0,
		0,
		m_context->m_targets->m_t_frame_luminance[0]->width(),
		m_context->m_targets->m_t_frame_luminance[0]->height()
	);
	*/
	//u32 row_pitch = 0;
	//float4* data = (float4*)m_context->m_targets->m_t_frame_luminance_lockable->map2D(D3D_MAP_READ, 0, row_pitch);
	//out_avrg_min_max = *data;
	
	//m_context->m_targets->m_t_frame_luminance_lockable->unmap2D(0);
	
	post_process_parameters const& pp_parameters = m_context->scene_view()->post_process_parameters();
//	if not using eye adaptation
	if (math::is_similar(pp_parameters.adaptation_speed, 0.0f, 0.05f))
		resource_manager::ref().copy2D(
			&*m_context->m_targets->m_t_frame_luminance_current,
			0,
			0,
			&*m_context->m_targets->m_t_frame_luminance[0],
			0,
			0,
			m_context->m_targets->m_t_frame_luminance[0]->width(),
			m_context->m_targets->m_t_frame_luminance[0]->height()
		);	
}

void stage_postprocess::compute_per_pixel_eye_adaptated_luminance()
{
	post_process_parameters const& pp_parameters = m_context->scene_view()->post_process_parameters();//m_material_post_effects[0].m_post_process_stage_parameters;

	m_sh_eye_adaptation->apply();

	float time_delta					= m_context->get_time_delta();
	math::clamp							(time_delta, 0.0f, 1.0f / 30.0f);

	backend::ref().set_ps_constant( m_elapsed_time_parameter, time_delta );
	//backend::ref().set_ps_constant( m_adaptation_factor, (1.0f - math::pow(0.98f, pp_parameters.adaptation_rate * m_context->get_time_delta())) );
	backend::ref().set_ps_constant( m_adaptation_factor, time_delta * (1.0f / pp_parameters.adaptation_speed));

	static bool fist_pass = true;
	if (fist_pass)
	{
		backend::ref().set_ps_texture( "t_previous_luminance", &*m_context->m_targets->m_t_frame_luminance[0] );
		fist_pass = false;
	}
	else
	{
		backend::ref().set_ps_texture( "t_previous_luminance", &*m_context->m_targets->m_t_frame_luminance_previous );
	}
	
	backend::ref().set_ps_texture( "t_current_luminanace", &*m_context->m_targets->m_t_frame_luminance[0] );
	fill_surface2(m_context->m_targets->m_rt_frame_luminance_current);
	
	resource_manager::ref().copy2D(
		&*m_context->m_targets->m_t_frame_luminance_previous,
		0,
		0,
		&*m_context->m_targets->m_t_frame_luminance_current,
		0,
		0,
		m_context->m_targets->m_t_frame_luminance_current->width(),
		m_context->m_targets->m_t_frame_luminance_current->height()
	);	
}

template<class T> T linear_interpolation2(T a, T b, float alpha)
{
	return a * (1 - alpha) + b * alpha;
}

xray::math::float4 stage_postprocess::get_frame_luminance_parameters()
{
	// Average, Min, Max, Middle Gray
	xray::math::float4					parameters(0.5f, 0.0f, 1.0f, 0.18f);
	
	post_process_parameters const& pp_parameters = m_context->scene_view()->post_process_parameters();
	
	u32 const num_values				= NUM_HISTOGRAM_VALUES;
	float const min_luminance			= MIN_FRAME_LUMINANCE;
	float const max_luminance			= MAX_FRAME_LUMINANCE;
	float const luminance_range			= MAX_FRAME_LUMINANCE - MIN_FRAME_LUMINANCE;
	u32 const num_total_pixels			= NUM_TONEMAP_PIXELS;
	
	u32 const num_pixels_inv_bright		= u32((1.0f - pp_parameters.tonemap_bright_threshold) * num_total_pixels);
	u32 const num_pixels_median			= u32(pp_parameters.tonemap_median * num_total_pixels);
	u32 const num_pixels_darkness		= u32(pp_parameters.tonemap_darkness_threshold * num_total_pixels);
	
	float values						[num_values];
	buid_luminance_histogram			(
		&*m_context->m_targets->m_t_generic_0, 
		array_size(values), 
		min_luminance, 
		max_luminance, 
		values
	);
	
	float current_luminance				= min_luminance;
	u32   num_pixels					= 0;
	
	bool found_tonemap_bright			= false;
	bool found_tonemap_median			= false;
	bool found_tonemap_darkness			= false;
	
	for (s32 i = 0; i < num_values; i++)
	{
		u32 const num_in_next			= math::max<u32>(1, u32(values[i]));
		
		if (num_pixels + num_in_next >= num_pixels_inv_bright && !found_tonemap_bright)
		{
			parameters.z				= linear_interpolation2(
				current_luminance, 
				current_luminance + luminance_range / num_values, 
				float(num_pixels_inv_bright - num_pixels) / float(num_in_next)
			);
			found_tonemap_bright		= true;
		}
		if (num_pixels + num_in_next >= num_pixels_median && !found_tonemap_median)
		{
			parameters.x				= linear_interpolation2(
				current_luminance, 
				current_luminance + luminance_range / num_values, 
				float(num_pixels_median - num_pixels) / float(num_in_next)
			);
			found_tonemap_median		= true;
		}
		if (num_pixels + num_in_next >= num_pixels_darkness && !found_tonemap_darkness)
		{
			parameters.y				= linear_interpolation2(
				current_luminance, 
				current_luminance + luminance_range / num_values, 
				float(num_pixels_darkness - num_pixels) / float(num_in_next)
			);
			found_tonemap_darkness		= true;
		}
		current_luminance				+= luminance_range / num_values;
		num_pixels						+= num_in_next;
	}
	parameters.w						= pp_parameters.tonemap_middle_gray;
	return								parameters;
}

xray::math::float4 stage_postprocess::compute_luminance_parameters(u32 frame_delta)
{
//#define USE_LUMINANCE_FROM_HISTOGRAM 0
//#if USE_LUMINANCE_FROM_HISTOGRAM
	
	post_process_parameters const& pp_parameters = m_context->scene_view()->post_process_parameters();	
	
// 	if (math::is_similar(pp_parameters.adaptation_speed, 0.0f, 0.05f))
// 	{
// 		m_context->get_scene_view()->set_luminance_parameters( get_frame_luminance_parameters() );
// 		m_context->get_scene_view()->set_prev_luminance_parameters( get_frame_luminance_parameters() );
// 		
// 		return m_context->get_scene_view()->get_luminance_parameters();
// 	}
	
	float4								frame_luminance_parameter;
	
	if (m_context->scene_view()->get_render_frame_index() % frame_delta == 0)
	{
		frame_luminance_parameter		= get_frame_luminance_parameters();
		m_context->get_scene_view()->set_luminance_parameters(frame_luminance_parameter);

		if (m_context->scene_view()->get_render_frame_index() == frame_delta)
			m_context->get_scene_view()->set_prev_luminance_parameters(frame_luminance_parameter);
	}
	
	float time_delta					= m_context->get_time_delta();
	math::clamp							(time_delta, 1.0f / 300.0f, 1.0f / 30.0f);
	
	m_context->get_scene_view()->set_prev_luminance_parameters( 
			m_context->scene_view()->get_prev_luminance_parameters()
		+ 
			(
				m_context->get_scene_view()->get_luminance_parameters() - m_context->scene_view()->get_prev_luminance_parameters()
			) * time_delta / pp_parameters.adaptation_speed
		);
	
	frame_luminance_parameter			= m_context->scene_view()->get_prev_luminance_parameters();
	
//#else // #if USE_LUMINANCE_FROM_HISTOGRAM

	for (u32 lum_rt_index=0; lum_rt_index<NUM_TONEMAP_TEXTURES; lum_rt_index++)
	{
		backend::ref().set_render_targets	(&*m_context->m_targets->m_rt_frame_luminance[lum_rt_index], 0, 0, 0);
		backend::ref().clear_render_targets	(0.0f, 0.0f, 0.0f, 1.0f);
	}
	
// 	backend::ref().set_render_targets	(&*m_context->m_targets->m_rt_frame_luminance_current, 0, 0, 0);
// 	backend::ref().clear_render_targets	(0.0f, 0.0f, 0.0f, 1.0f);
// 	
// 	backend::ref().set_render_targets	(&*m_context->m_targets->m_rt_frame_luminance_previous, 0, 0, 0);
// 	backend::ref().clear_render_targets	(0.0f, 0.0f, 0.0f, 1.0f);
	
	float4 frame_luminance_parameter0;
	measure_per_pixel_luminance			(&*m_context->m_targets->m_t_generic_0, frame_luminance_parameter0);
	
	if (!math::is_similar(pp_parameters.adaptation_speed, 0.0f, 0.05f))
		compute_per_pixel_eye_adaptated_luminance();
	
	return frame_luminance_parameter;
//#endif // #if USE_LUMINANCE_FROM_HISTOGRAM
}

void stage_postprocess::execute_disabled()
{
	PIX_EVENT( stage_postprocess);
	
	if (!is_effects_ready())
		return;
	
	m_sh_effect_copy_image->apply	(effect_copy_image::copy_rewrite);
	backend::ref().set_ps_texture	( "t_base", &*m_context->m_targets->m_t_generic_0);
	backend::ref().set_ps_constant	(m_gamma_correction_factor, 1.0f);
	fill_surface					(m_context->m_targets->m_rt_present);
	m_context->set_w				(float4x4().identity());
}

void stage_postprocess::execute()
{
	PIX_EVENT( stage_postprocess);
	
	if (!is_effects_ready())
		return;
	
	if (!is_enabled())
	{
		execute_disabled();
		return;
	}
	
	BEGIN_CPUGPU_TIMER(statistics::ref().postprocess_stat_group.execute_time);
	
	//static bool saved				= false;
	
	if (!m_context->scene_view()->is_use_post_process())
	{
		m_sh_effect_copy_image->apply	(effect_copy_image::copy_rewrite);
		backend::ref().set_ps_texture	( "t_base", &*m_context->m_targets->m_t_generic_0);
		backend::ref().set_ps_constant	(m_gamma_correction_factor, 1.0f);
		fill_surface					(m_context->m_targets->m_rt_present);
		
		m_context->set_w				(float4x4().identity());
		
	
		//if (!saved)
		//	m_context->m_targets->m_t_generic_0->save_as("f:/hdr_screen.dds");
		//saved							= true;
		
		return;
	}
	//saved								= false;
	
	post_process_parameters const& pp_parameters = m_context->scene_view()->post_process_parameters();
	
	float3 sun_direction				(0.0f, 0.0f, 0.0f);
	
	// TODO: Move this effect to sun stage. Implement this effect for any local lights.
	light* the_sun = m_context->scene()->lights().get_sun().c_ptr();
	if (the_sun)
	{
		m_sh_gather_sun_light_scattering_zone->apply();
		sun_direction = the_sun->direction;
		backend::ref().set_ps_constant	(m_sun_direction_parameter, sun_direction);
		fill_surface(m_context->m_targets->m_rt_light_scattering);
	}
	
	float t_w							= float(m_context->m_targets->m_rt_blur_0->width());
	float t_h							= float(m_context->m_targets->m_rt_blur_0->height());
	float s_u							= 1.0f / backend::ref().target_width();
	float s_v							= 1.0f / backend::ref().target_height();
	
	kernel_offsets[0]					= float4( -s_u,  s_v, 0.0f, 0.0f ); kernel_offsets[1] = float4( s_u,  s_v, 0.0f, 0.0f );
	kernel_offsets[2]					= float4( -s_u, -s_v, 0.0f, 0.0f ); kernel_offsets[3] = float4( s_u, -s_v, 0.0f, 0.0f );
	kernel_offsets[4]					= float4( -s_u,  0,   0.0f, 0.0f ); kernel_offsets[5] = float4( s_u,  0,   0.0f, 0.0f );
	kernel_offsets[6]					= float4( 0,    -s_v, 0.0f, 0.0f ); kernel_offsets[7] = float4( 0,    s_v, 0.0f, 0.0f );
	
	backend::ref().flush_rt_views		();
	
	clear_surface						(m_context->m_targets->m_rt_blur_0);
	clear_surface						(m_context->m_targets->m_rt_blur_1);
	
	float4 frame_luminance_parameter	= compute_luminance_parameters(16);
	
	if (pp_parameters.enable_bloom)
	{
		m_sh_gather_bloom->apply		();
		backend::ref().set_ps_texture	( "t_frame_color", &*m_context->m_targets->m_t_generic_0);
		m_dof_shader_constants.set(
			pp_parameters.dof_height_lights, 
			pp_parameters.dof_focus_distance, 
			pp_parameters.dof_focus_region, 
			pp_parameters.dof_focus_power, 
			pp_parameters.dof_near_blur_amount, 
			pp_parameters.dof_far_blur_amount,
			pp_parameters.bokeh_dof_radius,
			pp_parameters.bokeh_dof_density
		);
		backend::ref().set_ps_constant	(m_kernel_offsets, kernel_offsets);
		m_bloom_shader_constants.set	(pp_parameters.bloom_scale, pp_parameters.bloom_max_color);
		backend::ref().set_ps_constant	(m_frame_luminance_parameter, frame_luminance_parameter);
		fill_surface					(m_context->m_targets->m_rt_blur_0);
	}
	
	float weights_h[27];
	float offsets_h[27];
	float weights_v[27];
	float offsets_v[27];
	
	float const blur_kernel				= pp_parameters.bloom_blurring_kernel;
	float const blur_intencity			= pp_parameters.bloom_blurring_intencity;
	
	get_gaussain_weights_offsets		(weights_h, offsets_h, u32(t_w), blur_kernel, blur_intencity, 27);
	get_gaussain_weights_offsets		(weights_v, offsets_v, u32(t_h), blur_kernel, blur_intencity, 27);
	
	for (u32 i = 0; i < 27; i++) 
		blur_offsets_weights[i]			= float4(offsets_h[i], weights_h[i], offsets_v[i], weights_v[i]);
	
	m_sh_blur->apply					(effect_blur::horizontally);
	m_textures[0]						= m_context->m_targets->m_t_blur_0;
	backend::ref().set_ps_texture		( "t_base", &*m_context->m_targets->m_t_blur_0);
	backend::ref().set_ps_constant		( m_blur_offsets_weights, blur_offsets_weights);
	fill_surface						(m_context->m_targets->m_rt_blur_1);
	
	backend::ref().flush_rt_shader_resources();
	
	m_sh_blur->apply					(effect_blur::vertically);
	m_textures[1]						= m_context->m_targets->m_t_blur_1;
	backend::ref().set_ps_texture		( "t_base", &*m_context->m_targets->m_t_blur_1);
	backend::ref().set_ps_constant		( m_blur_offsets_weights, blur_offsets_weights);
	fill_surface						(m_context->m_targets->m_rt_blur_0);
	
	u32 dof_type						= pp_parameters.use_bokeh_dof;
	
	if (dof_type==1)
		dof_type += pp_parameters.use_bokeh_image;
	
	m_sh_complex_blend[dof_type]->apply	();
	m_dof_shader_constants.set(
		pp_parameters.dof_height_lights, 
		pp_parameters.dof_focus_distance, 
		pp_parameters.dof_focus_region, 
		pp_parameters.dof_focus_power, 
		pp_parameters.dof_near_blur_amount, 
		pp_parameters.dof_far_blur_amount,
		pp_parameters.bokeh_dof_radius,
		pp_parameters.bokeh_dof_density
	);
	
	m_scene_shader_constants.set		(
		pp_parameters.frame_height_lights,
		pp_parameters.frame_mid_tones,
		pp_parameters.frame_shadows,
		pp_parameters.frame_fade_color,
		pp_parameters.frame_fade_amount,
		pp_parameters.frame_gamma_correction_factor,
		pp_parameters.frame_desaturation
	);
	
	if (pp_parameters.use_color_grading_lut && pp_parameters.color_grading_texture)
		backend::ref().set_ps_texture	("t_color_grading_lut", &*pp_parameters.color_grading_texture);
	else
		backend::ref().set_ps_texture	("t_color_grading_lut", &*m_color_grading_base_lut);
	
	backend::ref().set_ps_constant		(m_sun_direction_parameter, sun_direction);
	backend::ref().set_ps_constant		(m_frame_luminance_parameter, frame_luminance_parameter);
	
	fill_surface						(m_context->m_targets->m_rt_generic_1);
	
	if (pp_parameters.use_msaa && options::ref().m_enabled_mlaa)
	{
		clear_surface								(m_context->m_targets->m_rt_mlaa_edges);
		clear_surface								(m_context->m_targets->m_rt_mlaa_blended_weights);
		
		// MLAA.
		m_post_process_antialiasing_shader->apply	(effect_post_process_mlaa::color_edge_detection);
		fill_surface2								(m_context->m_targets->m_rt_mlaa_edges);
		
		m_post_process_antialiasing_shader->apply	(effect_post_process_mlaa::blending_weight_calculation);
		fill_surface2								(m_context->m_targets->m_rt_mlaa_blended_weights);
		
		m_post_process_antialiasing_shader->apply	(effect_post_process_mlaa::neighborhood_blending);
		fill_surface2								(m_context->m_targets->m_rt_present);
	}
	else if (options::ref().m_enabled_fxaa)
	{
		m_post_process_antialiasing_shader_fxaa->apply	();
		fill_surface2									(m_context->m_targets->m_rt_present);
	}
	else
	{
		// Convert to pc gamma space.
		m_sh_effect_copy_image->apply	(effect_copy_image::copy_rewrite);
		backend::ref().set_ps_texture	("t_base", &*m_context->m_targets->m_t_generic_1);
		backend::ref().set_ps_constant	(m_gamma_correction_factor, 1.0f);
		fill_surface					(m_context->m_targets->m_rt_present);
	}
	
	/*for (render::vector<material_effects>::const_iterator it=m_material_post_effects.begin(); it!=m_material_post_effects.end(); ++it)
	{
		if (!it->stage_enable[accumulate_distortion_render_stage] || !it->m_effects[accumulate_distortion_render_stage])
			continue;
		
		it->m_effects[accumulate_distortion_render_stage]->apply();
		
		resource_manager::ref().copy2D(m_context->m_targets->m_t_color.c_ptr(), 0, 0, m_context->m_targets->m_t_present.c_ptr(), 0, 0, m_context->m_targets->m_t_present->width(), m_context->m_targets->m_t_present->height());
		m_textures[4] = m_context->m_targets->m_t_color;
		backend::ref().set_ps_texture( "t_frame_color", &*m_context->m_targets->m_t_color);
		
		fill_surface(m_context->m_targets->m_rt_present);
	}
	
	// Add "post_process" resource?
	// Apply material post process effects.
	for (render::vector<material_effects>::const_iterator it=m_material_post_effects.begin(); it!=m_material_post_effects.end(); ++it)
	{
		if (!it->stage_enable[post_process_render_stage] || !it->m_effects[post_process_render_stage])
			continue;
		
		it->m_effects[post_process_render_stage]->apply();
		
		resource_manager::ref().copy2D(m_context->m_targets->m_t_color.c_ptr(), 0, 0, m_context->m_targets->m_t_present.c_ptr(), 0, 0, m_context->m_targets->m_t_present->width(), m_context->m_targets->m_t_present->height());
		m_textures[5] = m_context->m_targets->m_t_color;
		backend::ref().set_ps_texture( "t_frame_color", &*m_context->m_targets->m_t_color);
		
		fill_surface(m_context->m_targets->m_rt_present);
	}
	*/
	
	END_CPUGPU_TIMER;
	
	m_context->set_w				(float4x4().identity());

	backend::ref().reset_render_targets();
	backend::ref().reset_depth_stencil_target();
}

} // namespace render
} // namespace xray
