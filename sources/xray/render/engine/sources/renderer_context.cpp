////////////////////////////////////////////////////////////////////////////
//	Created		: 19.01.2010
//	Author		: Armen Abroyan
//	Copyright ( C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "renderer_context.h"
#include <xray/console_command.h>
#include <xray/render/core/backend.h>
#include <xray/render/core/resource_manager.h>
#include "scene_manager.h"
#include "shared_names.h"
#include <xray/render/core/render_target.h>
#include "scene_view.h"
#include "renderer_context_targets.h"
#include <xray/render/core/utils.h>
#include <xray/render/core/res_geometry.h>

#include "register_effect_descriptors.h"


static u32 s_lighting_material_strategy = 0;
static xray::console_commands::cc_u32 s_cc_lighting_material_strategy ("lighting_material_strategy", s_lighting_material_strategy, 0, 4, true, xray::console_commands::command_type_engine_internal);

static float s_shadow_map_z_bias = 0.0001f;
static xray::console_commands::cc_float s_cc_shadow_map_z_bias("shadow_map_z_bias", s_shadow_map_z_bias, 0, 4, true, xray::console_commands::command_type_engine_internal);

namespace xray {
namespace render {

renderer_context::renderer_context	() :
	m_scene				( NULL),
	m_scene_view		( NULL),
	m_targets			( NULL),
	m_view_pos			( 0.f, 0.f, 0.f, 1.f ),
	m_view_dir			( 0.f, 0.f, 1.f, 0.1f ),
	m_w					( math::float4x4().identity() ),
	m_w_transposed		( math::float4x4().identity() ),
	m_v					( math::float4x4().identity() ),
	m_v_transposed		( math::float4x4().identity() ),
	m_v_inverted		( math::float4x4().identity() ),
	m_v_inverted_transposed	( math::float4x4().identity() ),
	m_p					( math::float4x4().identity() ),
	m_p_transposed		( math::float4x4().identity() ),
	m_p_inverted		( math::float4x4().identity() ),
	m_wv				( math::float4x4().identity() ),
	m_wv_transposed		( math::float4x4().identity() ),
	m_vp				( math::float4x4().identity() ),
	m_vp_transposed		( math::float4x4().identity() ),
	m_wvp				( math::float4x4().identity() ),
	m_wvp_transposed	( math::float4x4().identity() ),
	m_v2shadow0			( math::float4x4().identity() ),
	m_v2shadow1			( math::float4x4().identity() ),
	m_v2shadow2			( math::float4x4().identity() ),
	m_v2shadow3			( math::float4x4().identity() ),
	m_c_w				( NULL),
	m_c_w_inv			( NULL),
	m_c_v				( NULL),
	m_c_p				( NULL),
	m_c_wv				( NULL),
	m_c_vp				( NULL),
	m_c_wvp				( NULL),
	m_c_v2w				( NULL),
	m_time_delta		( 0.0f),
	m_solid_color_specular			( 0.f, 0.f, 0.f, 0.f),
	m_solid_material_parameters		( 10.f, 1.f, 0.f, 0.f)
{
	memset( &m_eye_rays, 0, sizeof( m_eye_rays));
	register_effect_descriptors		();
	
	sun_cascade cascades[sun_cascade::num_max_sun_shadow_cascades];
	
	cascades[0].size = 10.0f;
	cascades[0].bias = 0.0001f;
	
	cascades[1].size = 30.0f;
	cascades[1].bias = 0.00015f;
	
	cascades[2].size = 90.0f;
	cascades[2].bias = 0.0002f;
	
	cascades[3].size = 270.0f;
	cascades[3].bias = 0.00035f;
	
	for (u32 cascade_index=0; cascade_index<sun_cascade::num_max_sun_shadow_cascades; cascade_index++)
		m_sun_cascades.push_back(cascades[cascade_index]);
	
	memset( &m_eye_rays, 0, sizeof( m_eye_rays));
	//if ( marker!=Device.dwFrame)
	{
		// Near/Far
		float	n = 6000/*g_pGamePersistent->Environment().CurrentEnv->fog_near*/;
		float	f = 10000/*g_pGamePersistent->Environment().CurrentEnv->fog_far*/;
		float	r = 1/( f-n);
		m_fog_params.set		( -n*r, r, r, r);
	}
	//backend::ref().set_constant( c, m_fog_params);

//	m_screen_resolution = float4( (float)backend::ref().target_width(), (float)backend::ref().target_height(), 1.f/backend::ref().target_width(), 1.f/backend::ref().target_height());
	m_screen_resolution = float4( 1.f, 1.f, 1.f, 1.f );

	reset_matrices();
	//////////////////////////////////////////////////////////////////////////
	m_c_w	= resource_manager::ref().register_constant_binding( shader_constant_binding( "m_W",		&m_w_transposed));
	m_c_v	= resource_manager::ref().register_constant_binding( shader_constant_binding( "m_V",		&m_v_transposed));
	m_c_p	= resource_manager::ref().register_constant_binding( shader_constant_binding( "m_P",		&m_p_transposed));
	m_c_v2w = resource_manager::ref().register_constant_binding( shader_constant_binding( "m_V2W",		&m_v_inverted_transposed));
	m_c_wv	= resource_manager::ref().register_constant_binding( shader_constant_binding( "m_WV",		&m_wv_transposed));
	m_c_vp	= resource_manager::ref().register_constant_binding( shader_constant_binding( "m_VP",		&m_vp_transposed));
	m_c_wvp	= resource_manager::ref().register_constant_binding( shader_constant_binding( "m_WVP",		&m_wvp_transposed));
	m_c_near_far = resource_manager::ref().register_constant_binding( shader_constant_binding( "near_far_invn_invf",		&m_near_far_invn_invf));
	
	m_c_fog_params			=	resource_manager::ref().register_constant_binding( shader_constant_binding( "fog_params",	&m_fog_params));
	m_c_screen_resolution	=	resource_manager::ref().register_constant_binding( shader_constant_binding( "screen_res",	&m_screen_resolution));
	m_c_view_pos			=	resource_manager::ref().register_constant_binding( shader_constant_binding( "eye_position", &m_view_pos));
	m_c_view_dir			=	resource_manager::ref().register_constant_binding( shader_constant_binding( "eye_direction", &m_view_dir));
	m_c_cascade_shadow_map_size	=	resource_manager::ref().register_constant_binding( shader_constant_binding( "cascade_shadow_map_size", &m_cascade_shadow_map_size));
	
	m_c_solid_color_specular		=	resource_manager::ref().register_constant_binding( shader_constant_binding( "solid_color_specular",	&m_solid_color_specular));
	m_c_solid_material_parameters	=	resource_manager::ref().register_constant_binding( shader_constant_binding( "solid_material_params",	&m_solid_material_parameters));
	m_c_solid_emission_color		=	resource_manager::ref().register_constant_binding( shader_constant_binding( "solid_emission_color",	&m_solid_emission_color));

	m_c_scene_time		=	resource_manager::ref().register_constant_binding( shader_constant_binding( "scene_time",	&m_current_time));
	
	
	// Common usage render targets and their textures
	create_casceded_shadow_map_buffers(1024);
	
	//m_t_indirect_lighting		= resource_manager::ref().create_texture( r2_rt_indirect_lighting);
	

	// Output dependent render target textures
	m_t_position			= resource_manager::ref().create_texture( r2_rt_p);
	m_t_position_ex			= resource_manager::ref().create_texture( r2_rt_p_ex);
	m_t_normal				= resource_manager::ref().create_texture( r2_rt_n);
	m_t_color				= resource_manager::ref().create_texture( r2_rt_albedo);
	m_t_emissive			= resource_manager::ref().create_texture( r2_rt_emissive);
	m_t_distortion			= resource_manager::ref().create_texture( r2_rt_distortion);

	m_t_ssao_accumulator	= resource_manager::ref().create_texture( r2_rt_ssao_accumulator);
	m_t_ssao_accumulator_small	= resource_manager::ref().create_texture( r2_rt_ssao_accumulator_small);

	//m_t_accumulator			= resource_manager::ref().create_texture( r2_rt_accum);
	m_t_accumulator_diffuse		= resource_manager::ref().create_texture( r2_rt_accum_diffuse);
	m_t_accumulator_specular	= resource_manager::ref().create_texture( r2_rt_accum_specular);
	
	m_t_present			= resource_manager::ref().create_texture( r2_rt_present);
	m_t_generic_0		= resource_manager::ref().create_texture( r2_rt_generic0);
	m_t_generic_1		= resource_manager::ref().create_texture( r2_rt_generic1);
	
	m_t_null			= resource_manager::ref().create_texture( r2_t_null);

	m_t_blur_0			= resource_manager::ref().create_texture( r2_rt_blur0);
	m_t_blur_1			= resource_manager::ref().create_texture( r2_rt_blur1);

	m_t_decals_diffuse			= resource_manager::ref().create_texture( r2_rt_decals_diffuse);
	m_t_decals_normal			= resource_manager::ref().create_texture( r2_rt_decals_normal);
	
	
	m_t_light_scattering= resource_manager::ref().create_texture( r2_rt_light_scattering);
	
	for (u32 i=0; i<NUM_TONEMAP_TEXTURES; i++)
	{
		names[i].assignf( "%s%d", r2_rt_frame_luminance, i);
		m_t_frame_luminance[i] = resource_manager::ref().create_texture( names[i].get_buffer());
	}
	
	m_t_frame_luminance_previous = resource_manager::ref().create_texture( r2_rt_frame_luminance_previous );
	m_t_frame_luminance_current = resource_manager::ref().create_texture( r2_rt_frame_luminance );
	
	m_t_frame_luminance_histogram = resource_manager::ref().create_texture( r2_rt_frame_luminance_histogram);

	m_t_tangents = resource_manager::ref().create_texture( r2_rt_tangents);
	
	m_t_mlaa_edges				= resource_manager::ref().create_texture(r2_rt_mlaa_edges);
	m_t_mlaa_blended_weights	= resource_manager::ref().create_texture(r2_rt_mlaa_blended_weights);

	
	m_t_gbuffer_position_downsampled	= resource_manager::ref().create_texture(r2_rt_gbuffer_position_downsampled);
	m_t_gbuffer_normal_downsampled	= resource_manager::ref().create_texture(r2_rt_gbuffer_normal_downsampled);
	
	//set_target_context( m_default_targets);
};

void renderer_context::create_casceded_shadow_map_buffers(u32 shadow_map_size)
{
	//u32 cascade_shadow_map_size = shadow_map_size_option->value();
	m_cascade_shadow_map_size = float(shadow_map_size);
	const pcstr cascade_names[] = { r2_rt_shadow_map0, r2_rt_shadow_map1, r2_rt_shadow_map2, r2_rt_shadow_map3 };
	for (u32 cascade_index=0; cascade_index<sun_cascade::num_max_sun_shadow_cascades; cascade_index++)
	{
		m_rt_shadow_map[cascade_index]  = 0;
		m_t_shadow_map[cascade_index]	= 0;
		m_rt_shadow_map[cascade_index]	= resource_manager::ref().create_render_target	( cascade_names[cascade_index], shadow_map_size, shadow_map_size, DXGI_FORMAT_R24G8_TYPELESS, enum_rt_usage_depth_stencil);
		m_t_shadow_map[cascade_index]	= resource_manager::ref().create_texture		( cascade_names[cascade_index]);
	}
}

void renderer_context::set_scene( render::scene * scene)
{
	m_scene = scene;
}

scene_view const *	renderer_context::scene_view				()	const
{ 
	return static_cast_checked<xray::render::scene_view const*>(m_scene_view.c_ptr());
}

scene_view *	renderer_context::get_scene_view				()
{ 
	return static_cast_checked<xray::render::scene_view*>(m_scene_view.c_ptr());
}

void renderer_context::set_scene_view( scene_view_ptr view_ptr)
{
	m_scene_view = view_ptr;
	
	xray::render::scene_view* view = static_cast_checked<xray::render::scene_view*>(view_ptr.c_ptr());
	
	set_v( view->camera().get_view_transform());
#ifndef MASTER_GOLD
	set_culling_v( view->camera().get_culling_view_transform());
#endif // #ifndef MASTER_GOLD
	set_p( view->camera().get_projection_transform());
}

void renderer_context::set_view2shadow	( float4x4 const & view2shadow, u32 index)
{ 
	switch (index)
	{
		case 0:
			m_v2shadow0	= view2shadow;
			break;
		case 1:
			m_v2shadow1	= view2shadow;
			break;
		case 2:
			m_v2shadow2	= view2shadow;
			break;
		case 3:
			m_v2shadow3	= view2shadow;
			break;
	}
}

const float4x4& renderer_context::get_view2shadow	( u32 index) const
{ 
	switch (index)
	{
		case 0:
			return m_v2shadow0;
		case 1:
			return m_v2shadow1;
		case 2:
			return m_v2shadow2;
		case 3:
			return m_v2shadow3;
	}
	return m_v2shadow0;
}

void renderer_context::set_target_context		( renderer_context_targets const * targets_context )
{
	m_targets	= targets_context;

	if( !m_targets || !m_targets->m_t_position)
	{
// 		m_t_skin_scattering->clone		( &*m_t_null);
// 		m_t_skin_scattering_small->clone			( &*m_t_null);
// 		m_t_skin_scattering_blurred_0->clone		( &*m_t_null);
// 		m_t_skin_scattering_blurred_1->clone		( &*m_t_null);
// 		m_t_skin_scattering_blurred_2->clone		( &*m_t_null);
// 		m_t_skin_scattering_blurred_3->clone		( &*m_t_null);
// 		m_t_skin_scattering_blurred_4->clone		( &*m_t_null);
	
		m_t_mlaa_edges->clone				( &*m_t_null);
		m_t_mlaa_blended_weights->clone				( &*m_t_null);

		m_t_position->clone				( &*m_t_null);
		m_t_position_ex->clone			( &*m_t_null);
		m_t_normal->clone				( &*m_t_null);
		m_t_color->clone				( &*m_t_null);
		m_t_emissive->clone				( &*m_t_null);
		m_t_distortion->clone			( &*m_t_null);
		m_t_ssao_accumulator->clone		( &*m_t_null);
		m_t_ssao_accumulator_small->clone( &*m_t_null);
		//m_t_accumulator->clone		( &*m_t_null);
		m_t_accumulator_diffuse->clone	( &*m_t_null);
		m_t_accumulator_specular->clone	( &*m_t_null);
		
		//m_t_indirect_lighting->clone	( &*m_t_null);
		m_t_present->clone				( &*m_t_null);
		
		m_t_generic_0->clone			( &*m_t_null);
		m_t_generic_1->clone			( &*m_t_null);

		m_t_gbuffer_position_downsampled->clone			( &*m_t_null);
		m_t_gbuffer_normal_downsampled->clone			( &*m_t_null);
		
		m_t_blur_0->clone			( &*m_t_null);
		m_t_blur_1->clone			( &*m_t_null);
		
		for (u32 i=0; i<NUM_TONEMAP_TEXTURES; i++)
			m_t_frame_luminance[i]->clone( &*m_t_null);

		m_t_frame_luminance_current->clone( &*m_t_null);
		
		m_t_frame_luminance_previous->clone( &*m_t_null);
		m_t_frame_luminance_histogram->clone(&*m_t_null);
		
		m_t_light_scattering->clone(&*m_t_null);
		
		m_t_decals_diffuse->clone		(&*m_t_null);
		m_t_decals_normal->clone		(&*m_t_null);

		m_t_tangents->clone		(&*m_t_null);
		m_t_bitangents->clone		(&*m_t_null);

		

		return;
	}

	m_screen_resolution				= float4( (float)m_targets->size().x,  (float)m_targets->size().y, 1.f/m_targets->size().x, 1.f/m_targets->size().y);

	m_t_position->clone				( &*m_targets->m_t_position);
	m_t_position_ex->clone			( &*m_targets->m_t_position_ex);
	m_t_normal->clone				( &*m_targets->m_t_normal);
	m_t_color->clone				( &*m_targets->m_t_color);
	m_t_emissive->clone				( &*m_targets->m_t_emissive);
	m_t_distortion->clone			( &*m_targets->m_t_distortion);
	m_t_ssao_accumulator->clone		( &*m_targets->m_t_ssao_accumulator);
	m_t_ssao_accumulator_small->clone( &*m_targets->m_t_ssao_accumulator_small);
	//m_t_accumulator->clone		( &*m_targets->m_t_accumulator);
	m_t_accumulator_diffuse->clone	( &*m_targets->m_t_accumulator_diffuse);
	m_t_accumulator_specular->clone	( &*m_targets->m_t_accumulator_specular);

	m_t_decals_diffuse->clone		( &*m_targets->m_t_decals_diffuse);
	m_t_decals_normal->clone		( &*m_targets->m_t_decals_normal);
	
	m_t_light_scattering->clone		( &*m_targets->m_t_light_scattering);
	
	m_t_present->clone				( &*m_targets->m_t_present);
	
	m_t_generic_0->clone			( &*m_targets->m_t_generic_0);
	m_t_generic_1->clone			( &*m_targets->m_t_generic_1);
	
	m_t_blur_0->clone				( &*m_targets->m_t_blur_0);
	m_t_blur_1->clone				( &*m_targets->m_t_blur_0);

	m_t_tangents->clone				( &*m_targets->m_t_tangents);

	m_t_mlaa_edges->clone			( &*m_targets->m_t_mlaa_edges);
	m_t_mlaa_blended_weights->clone	( &*m_targets->m_t_mlaa_blended_weights);
	
	m_t_gbuffer_position_downsampled->clone	( &*m_targets->m_t_gbuffer_position_downsampled);
	m_t_gbuffer_normal_downsampled->clone	( &*m_targets->m_t_gbuffer_normal_downsampled);
	
	//m_t_indirect_lighting->clone	( &*m_targets->m_t_indirect_lighting);
		
	for (u32 i=0; i<NUM_TONEMAP_TEXTURES; i++)
		m_t_frame_luminance[i]->clone( &*m_targets->m_t_frame_luminance[i]);
	
	m_t_frame_luminance_current->clone( &*m_targets->m_t_frame_luminance_current);
	
	m_t_frame_luminance_previous->clone( &*m_targets->m_t_frame_luminance_previous);
	
	m_t_frame_luminance_histogram->clone( &*m_targets->m_t_frame_luminance_histogram);
}

void renderer_context::set_time_delta	( float const time_delta )
{
	m_time_delta					= time_delta;
}

void renderer_context::set_current_time ( float const current_time)
{
	m_current_time					= current_time;
}

void renderer_context::set_w(  const float4x4& m)
{
	m_w_transposed		= transpose( m_w = m );
	m_wv_transposed		= transpose( m_wv = m_w * m_v );
	m_wvp_transposed	= transpose( m_wvp = mul4x4( m_wv, m_p) );

	backend::ref().set_vs_constant( m_c_w, m_w_transposed);
	backend::ref().set_vs_constant( m_c_wv, m_wv_transposed);
	backend::ref().set_vs_constant( m_c_wvp, m_wvp_transposed);

	backend::ref().set_ps_constant( m_c_w, m_w_transposed);
	backend::ref().set_ps_constant( m_c_wv, m_wv_transposed);
	backend::ref().set_ps_constant( m_c_wvp, m_wvp_transposed);


	//m_bInvWValid	= false;
	//if ( c_invw)		apply_invw();
}

void renderer_context::set_v( const float4x4& m)
{
	m_v_transposed		= transpose( m_v = m );
	
	m_v_inverted.try_invert	( m );
	m_v_inverted_transposed	= transpose( m_v_inverted );

	m_wv_transposed		= transpose( m_wv = m_w * m_v );
	m_vp_transposed		= transpose( m_vp = mul4x4( m_v, m_p ) );
	m_wvp_transposed	= transpose( m_wvp = mul4x4( m_wv, m_p) );

	m_view_pos			= float4( m_v_inverted.c.xyz(), 1); 
	m_view_dir			= float4( m_v_inverted.k.xyz(), 0); 

	// ??????????????
	backend::ref().set_ps_constant( m_c_view_pos, m_view_pos);
	backend::ref().set_ps_constant( m_c_view_dir, m_view_dir);
	
	backend::ref().set_vs_constant( m_c_v, m_v_transposed);
	backend::ref().set_vs_constant( m_c_vp, m_vp_transposed);
	backend::ref().set_vs_constant( m_c_wv, m_wv_transposed);
	backend::ref().set_vs_constant( m_c_wvp, m_wvp_transposed);

	backend::ref().set_ps_constant( m_c_v, m_v_transposed);
	backend::ref().set_ps_constant( m_c_vp, m_vp_transposed);
	backend::ref().set_ps_constant( m_c_wv, m_wv_transposed);
	backend::ref().set_ps_constant( m_c_wvp, m_wvp_transposed);
}

#ifndef MASTER_GOLD
void renderer_context::set_culling_v( const float4x4& m )
{
	m_culling_v					= m;
	m_culling_v_inverted		= math::invert4x3( m_culling_v );
	m_culling_vp				= mul4x4( m_culling_v, m_p );
	m_culling_view_pos			= m_culling_v_inverted.c;
	m_culling_view_dir			= m_culling_v_inverted.k;
}
#endif // #ifndef MASTER_GOLD

void renderer_context::set_p( const float4x4& m)
{
	m_p		= m;
	m_p_transposed		= transpose( m_p = m );
	m_p		= m;
	
	

	m_p_inverted		= math::invert4x4(m);

	//m_p_inverted		= utils::get_projection_invert( m );
//	m_p_inverted_transposed	= transpose( m_p_inverted );

	m_vp_transposed		= transpose( m_vp = mul4x4( m_v, m_p ) );

#ifndef MASTER_GOLD
	m_culling_vp		= mul4x4( m_culling_v, m_p );
#endif // #ifndef MASTER_GOLD

	m_wvp_transposed	= transpose( m_wvp = mul4x4( m_wv, m_p) );

	update_near_far		( );
	update_eye_rays		( );

	backend::ref().set_vs_constant( m_c_p, m_p_transposed);
	backend::ref().set_vs_constant( m_c_vp, m_vp_transposed);
	backend::ref().set_vs_constant( m_c_wvp, m_wvp_transposed);

	backend::ref().set_ps_constant( m_c_p, m_p_transposed);
	backend::ref().set_ps_constant( m_c_vp, m_vp_transposed);
	backend::ref().set_ps_constant( m_c_wvp, m_wvp_transposed);
}

void	renderer_context::update_near_far()
{
	// Near plane distance
	float4	tmp_transformed = m_p_inverted.transform( float4( 0.f, 0.f, 0.f, 1.f));
	m_near_far_invn_invf.x	= tmp_transformed.z/tmp_transformed.w;

	// Far plane distance
	tmp_transformed			= m_p_inverted.transform( float4( 0.f, 0.f, 1.f, 1.f));
	m_near_far_invn_invf.y	= tmp_transformed.z/tmp_transformed.w;

	// Invert Near and far distances
	m_near_far_invn_invf.z = 1.f/m_near_far_invn_invf.x;
	m_near_far_invn_invf.w = 1.f/m_near_far_invn_invf.y;
}

void renderer_context::reset_matrices()
{
	m_w.identity();
	m_w_transposed.identity();
	m_v.identity();
	m_v_transposed.identity();
	m_v_inverted.identity();
	m_v_inverted_transposed.identity();
	m_p.identity();
	m_p_transposed.identity();
	m_p_inverted.identity();
	m_wv.identity();
	m_wv_transposed.identity();
	m_vp.identity();
	m_vp_transposed.identity();
	m_wvp.identity();
	m_wvp_transposed.identity();

	m_near_far_invn_invf	= float4( 0.f, 0.f, 0.f, 0.f);
}

u32 renderer_context::get_lighting_material_strtegy()
{
	return s_lighting_material_strategy;
}

float renderer_context::get_shadow_map_z_bias()
{
	return s_shadow_map_z_bias;
}


void renderer_context::update_eye_rays()
{
	float4x4 const& inv_proj = m_p_inverted;

	float4 eye_00	( -1.f,  1.f, 1.f, 1.f);

//		float4 eye_00	( -1.f,  1.f, 1.f, 1.f);
//  	float4 eye_01	( -1.f, -1.f, 1.f, 1.f);
//  	float4 eye_10	(  1.f,  1.f, 1.f, 1.f);
//  	float4 eye_11	(  1.f, -1.f, 1.f, 1.f);

	eye_00 = inv_proj.transform( eye_00);
//  	eye_01 = inv_proj.transform( eye_01);
//  	eye_10 = inv_proj.transform( eye_10);
//  	eye_11 = inv_proj.transform( eye_11);

	float inv_w  = 1.f/eye_00.w;
	eye_00.x *= inv_w;
	eye_00.y *= inv_w;
	eye_00.z *= inv_w;
	m_eye_rays[0] = float3(  eye_00.x,  eye_00.y, eye_00.z);
	m_eye_rays[1] = float3(  eye_00.x, -eye_00.y, eye_00.z);
	m_eye_rays[2] = float3( -eye_00.x,  eye_00.y, eye_00.z);
	m_eye_rays[3] = float3( -eye_00.x, -eye_00.y, eye_00.z);

// 	m_eye_rays[0] = float3( eye_00.x*inv_w, eye_00.y*inv_w, eye_00.z*inv_w);
// 	m_eye_rays[1] = float3( eye_01.x*inv_w, eye_01.y*inv_w, eye_01.z*inv_w);
// 	m_eye_rays[2] = float3( eye_10.x*inv_w, eye_10.y*inv_w, eye_10.z*inv_w);
// 	m_eye_rays[3] = float3( eye_11.x*inv_w, eye_11.y*inv_w, eye_11.z*inv_w);
}

void renderer_context::set_solid_diffusecolor_specularintensity	( float3 color, float specular_intensity )			
{ 
	m_solid_color_specular.x	= color.x;
	m_solid_color_specular.y	= color.y;
	m_solid_color_specular.z	= color.z;
	m_solid_color_specular.w	= specular_intensity;
}

void renderer_context::set_solid_material_parameters			( float specular_power, float diffuse_power, float tranclucensy, float material_id )	
{ 
	m_solid_material_parameters.x 	= specular_power;
	m_solid_material_parameters.y 	= diffuse_power;
	m_solid_material_parameters.z 	= tranclucensy;
	m_solid_material_parameters.w 	= material_id;
}
void renderer_context::set_solid_emission_color				( float3 emission_color)
{
	m_solid_emission_color.x		= emission_color.x;
	m_solid_emission_color.y		= emission_color.y;
	m_solid_emission_color.z		= emission_color.z;
}

// void renderer_context::setup_fog_params( shader_constant* c)
// {
// 	float4	result;
// 	//if ( marker!=Device.dwFrame)
// 	{
// 		// Near/Far
// 		float	n = 600/*g_pGamePersistent->Environment().CurrentEnv->fog_near*/;
// 		float	f = 1000/*g_pGamePersistent->Environment().CurrentEnv->fog_far*/;
// 		float	r = 1/( f-n);
// 		result.set		( -n*r, r, r, r);
// 	}
// 	backend::ref().set_constant( c,result);
// }
// 
// void renderer_context::setup_eye_position( shader_constant* c)
// {
// 	backend::ref().set_constant( c, float4( get_view_pos(), 1));
// }

} // namespace render
} // namespace xray
