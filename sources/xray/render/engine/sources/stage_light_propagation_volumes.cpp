////////////////////////////////////////////////////////////////////////////
//	Created		: 19.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/backend.h>
#include "stage_light_propagation_volumes.h"
#include "renderer_context.h"
#include "renderer_context_targets.h"
#include <xray/render/core/pix_event_wrapper.h>
#include "scene.h"
#include "render_model.h"
#include <xray/render/core/res_effect.h>
#include <xray/render/core/res_geometry.h>
#include "statistics.h"
#include "scene_view.h"

#include "effect_copy_image.h"
#include <xray/render/core/effect_manager.h>
#include <xray/render/core/render_target.h>
#include "vertex_formats.h"
#include <xray/render/core/effect_options_descriptor.h>
#include <xray/render/core/untyped_buffer.h>

#include <xray/render/core/res_effect.h>
#include "material.h"

#include <xray/collision/space_partitioning_tree.h>

#include <xray/render/core/options.h>

#include <xray/render/core/resource_manager.h>
#include "shared_names.h"
#include "lights_db.h"
#include "light.h"
#include "effect_fill_reflective_shadow_map.h"
#include <xray/render/core/effect_descriptor.h>
#include <xray/render/facade/vertex_input_type.h>
#include <xray/render/core/destroy_data_helper.h>

#include "effect_downsample_reflective_shadow_map.h"
#include "effect_downsample_gbuffer.h"
#include "effect_apply_indirect_lighting.h"
#include "effect_copy_image.h"

namespace xray {
namespace render {
	
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

stage_light_propagation_volumes::stage_light_propagation_volumes(renderer_context* context):
	stage(context),
	m_has_indirect_lighting(false)
{	
	m_num_cascades				= options::ref().m_lpv_num_cascades;
	
	math::clamp<u32>			(m_num_cascades, 1, 4);
	
	m_grid_size					= options::ref().m_num_radiance_volume_cells;
	
	m_rsm_source_size			= options::ref().m_light_propagation_volumes_rsm_size;
	m_rsm_downsampled_size		= options::ref().m_light_propagation_volumes_rsm_size / 2;
	
	m_radiance_volume			= ALLOC(radiance_volume, m_num_cascades);
	
	float cascade_scales			[] = {1.0f, 2.0f, 4.0f, 6.0f, };
	float cascade_cells_scales		[] = {1.0f, 1.0f, 1.0f, 1.0f, };
	float cascade_flux_scales		[] = {1.0f, 1.0f, 1.0f, 1.0f, };
	float cascade_iteration_scales	[] = {1.0f, 1.0f / 2.0f, 1.0f / 4.0f, 1.0f / 8.0f, };
	
	for (u32 cascade_index = 0; cascade_index < m_num_cascades; cascade_index++)
	{
		new(&m_radiance_volume[cascade_index])radiance_volume(
			m_rsm_downsampled_size,
			u32(options::ref().m_num_radiance_volume_cells * cascade_cells_scales[cascade_index]),
			u32(options::ref().m_num_propagate_iterations * cascade_iteration_scales[cascade_index]),
			options::ref().m_radiance_volume_scale * cascade_scales[cascade_index],
			options::ref().m_lpv_flux_amplifier * cascade_flux_scales[cascade_index]
		);
	}
	
	//m_rt_rms_albedo_source		= resource_manager::ref().create_render_target(r2_rt_lpv_rsm_albedo_source, m_rsm_source_size, m_rsm_source_size, DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);
	

	m_rt_rms_albedo_source		= resource_manager::ref().create_render_target(r2_rt_lpv_rsm_albedo_source, m_rsm_source_size, m_rsm_source_size, DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);
	m_t_rms_albedo_source		= resource_manager::ref().create_texture(r2_rt_lpv_rsm_albedo_source);
	m_rt_rms_normal_source		= resource_manager::ref().create_render_target(r2_rt_lpv_rsm_normal_source, m_rsm_source_size, m_rsm_source_size, DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);
	m_t_rms_normal_source		= resource_manager::ref().create_texture(r2_rt_lpv_rsm_normal_source);
	m_rt_rms_position_source	= resource_manager::ref().create_render_target(r2_rt_lpv_rsm_position_source, m_rsm_source_size, m_rsm_source_size, DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);
	m_t_rms_position_source		= resource_manager::ref().create_texture(r2_rt_lpv_rsm_position_source);
	
	m_rms_depth_stencil_source	= resource_manager::ref().create_render_target("$user$rms_depth_stencil", m_rsm_source_size, m_rsm_source_size, DXGI_FORMAT_R24G8_TYPELESS, enum_rt_usage_depth_stencil);
	
	m_rt_rms_albedo				= resource_manager::ref().create_render_target(r2_rt_lpv_rsm_albedo, m_rsm_downsampled_size, m_rsm_downsampled_size, DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);
	m_t_rms_albedo				= resource_manager::ref().create_texture(r2_rt_lpv_rsm_albedo);
	m_rt_rms_normal				= resource_manager::ref().create_render_target(r2_rt_lpv_rsm_normal, m_rsm_downsampled_size, m_rsm_downsampled_size, DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);
	m_t_rms_normal				= resource_manager::ref().create_texture(r2_rt_lpv_rsm_normal);
	m_rt_rms_position			= resource_manager::ref().create_render_target(r2_rt_lpv_rsm_position, m_rsm_downsampled_size, m_rsm_downsampled_size, DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);
	m_t_rms_position			= resource_manager::ref().create_texture(r2_rt_lpv_rsm_position);
	
	m_c_interreflection_contribution		= backend::ref().register_constant_host("interreflection_contribution", rc_float);
	m_c_cascade_index						= backend::ref().register_constant_host("cascade_index", rc_int);
	m_c_num_cascades						= backend::ref().register_constant_host("num_cascades", rc_int);
	
	m_c_smaller_cascade_grid_cell_size		= backend::ref().register_constant_host("smaller_cascade_grid_cell_size", rc_float);
	m_c_smaller_cascade_grid_size			= backend::ref().register_constant_host("smaller_cascade_grid_size", rc_float);
	m_c_smaller_cascade_grid_origin			= backend::ref().register_constant_host("smaller_cascade_grid_origin", rc_float);
	m_c_radiance_blend_factor				= backend::ref().register_constant_host("radiance_blend_factor", rc_float);
	
	register_rsm_constans		();
	register_light_constans		();
	
	m_c_eye_ray_corner			= backend::ref().register_constant_host("s_eye_ray_corner", rc_float);
	
	for (u32 i = 0; i < num_vertex_input_types; i++)
	{
		if (i == post_process_vertex_input_type)
			continue;
		
		byte						data[Kb];
		effect_options_descriptor	desc(data, Kb);
		desc["vertex_input_type"]	= (enum_vertex_input_type)i;
		
		effect_manager::ref().create_effect<effect_fill_reflective_shadow_map>(&m_fill_rsm_effect[i], desc);
	}
	
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
	
	effect_manager::ref().create_effect<effect_downsample_reflective_shadow_map>(&m_downsample_rsm_effect);
	effect_manager::ref().create_effect<effect_apply_indirect_lighting>(&m_apply_indirect_lighting_effect);
	effect_manager::ref().create_effect<effect_downsample_gbuffer>(&m_downsample_gbuffer_effect);
	
	m_enabled					= options::ref().m_enabled_light_propagation_volumes_stage;
}

bool stage_light_propagation_volumes::is_effects_ready() const
{
	for (u32 i = 0; i < num_vertex_input_types; i++)
	{
		if (i == post_process_vertex_input_type)
			continue;
		
		if (m_fill_rsm_effect[i].c_ptr() == NULL)
			return false;
	}
	
	for (u32 cascade_index = 0; cascade_index < m_num_cascades; cascade_index++)
	{
		if (!m_radiance_volume[cascade_index].is_effects_ready())
			return false;
	}
	
	return m_downsample_rsm_effect.c_ptr() != NULL 
		&& m_apply_indirect_lighting_effect.c_ptr() != NULL
		&& m_downsample_gbuffer_effect.c_ptr() != NULL;
}

stage_light_propagation_volumes::~stage_light_propagation_volumes()
{
	FREE(m_radiance_volume);
}

void stage_light_propagation_volumes::set_rsm_contants(xray::math::float3 const& light_direction, 
													   xray::math::float3 const& grid_origin, 
													   float grid_scale)
{
	backend::ref().set_ps_constant	(m_c_grid_origin, grid_origin);
	backend::ref().set_ps_constant	(m_c_grid_cell_size, grid_scale / float(m_grid_size));
	backend::ref().set_ps_constant	(m_c_invert_rsm_size, 1.0f / float(m_rsm_downsampled_size));
	backend::ref().set_ps_constant	(m_c_light_direction, light_direction);
}

void stage_light_propagation_volumes::register_rsm_constans()
{
	m_c_grid_origin					= backend::ref().register_constant_host("grid_origin",  rc_float );
	m_c_grid_cell_size				= backend::ref().register_constant_host("grid_cell_size",  rc_float );
	m_c_grid_size					= backend::ref().register_constant_host("grid_size",  rc_float );
	m_c_invert_rsm_size				= backend::ref().register_constant_host("invert_rsm_size",  rc_float );
}

void stage_light_propagation_volumes::register_light_constans()
{
	m_c_view_to_light_matrix		= backend::ref().register_constant_host("view_to_light_matrix",  rc_float );
	m_c_light_color					= backend::ref().register_constant_host("light_color", rc_float );
	m_c_light_intensity				= backend::ref().register_constant_host("light_intensity", rc_float );
	m_c_light_position				= backend::ref().register_constant_host("light_position", rc_float );
	m_c_light_direction				= backend::ref().register_constant_host("light_direction", rc_float );
	m_c_light_attenuation_power		= backend::ref().register_constant_host("light_attenuation_power", rc_float );
	m_c_light_range					= backend::ref().register_constant_host("light_range", rc_float );
	m_c_lighting_model				= backend::ref().register_constant_host("lighting_model", rc_int );
	m_c_diffuse_influence_factor	= backend::ref().register_constant_host("light_diffuse_influence_factor", rc_float );
	m_c_specular_influence_factor	= backend::ref().register_constant_host("light_specular_influence_factor", rc_float );
	m_c_light_spot_penumbra_half_angle_cosine	= backend::ref().register_constant_host( "light_spot_penumbra_half_angle_cosine", rc_float );
	m_c_light_spot_umbra_half_angle_cosine		= backend::ref().register_constant_host( "light_spot_umbra_half_angle_cosine", rc_float );
	m_c_light_spot_inversed_umbra_half_angle_cosine_minus_penumbra_half_angle_cosine
		= backend::ref().register_constant_host( "light_spot_inversed_umbra_half_angle_cosine_minus_penumbra_half_angle_cosine", rc_float );
	m_c_light_spot_falloff			= backend::ref().register_constant_host("light_spot_falloff", rc_float );
	m_c_light_type					= backend::ref().register_constant_host("light_type", rc_int );
	m_c_light_capsule_half_width	= backend::ref().register_constant_host("light_capsule_half_width", rc_float );
	m_c_light_capsule_radius			= backend::ref().register_constant_host("light_capsule_radius", rc_float );
	m_c_light_sphere_radius			= backend::ref().register_constant_host("light_sphere_radius", rc_float );
	m_c_light_local_to_world		= backend::ref().register_constant_host("light_local_to_world", rc_float );
	m_c_light_sphere_radius			= backend::ref().register_constant_host("light_sphere_radius", rc_float );
}

void stage_light_propagation_volumes::set_light_constans(xray::render::light* l)
{
	float light_range				= l->range;
	float3 light_color				= l->color;
	float3 const light_position		= m_context->get_v().transform_position( l->position );
	float3 const light_direction	= m_context->get_v().transform_direction( l->direction );
	backend::ref().set_ps_constant	(m_c_light_position, light_position );
	backend::ref().set_ps_constant	(m_c_light_direction, light_direction );
	
	if (l->get_type() == light_type_spot)
	{
		backend::ref().set_ps_constant	(m_c_light_range, light_range/math::sin(l->spot_penumbra_angle/2.f) );
		float const penumbra_half_angle_cosine	= math::cos( l->spot_penumbra_angle/2.f );
		backend::ref().set_ps_constant	(m_c_light_spot_penumbra_half_angle_cosine, penumbra_half_angle_cosine );
		float const umbra_half_angle_cosine	= math::cos( l->spot_umbra_angle/2.f );
		backend::ref().set_ps_constant	(m_c_light_spot_umbra_half_angle_cosine, umbra_half_angle_cosine );
		backend::ref().set_ps_constant	(m_c_light_spot_inversed_umbra_half_angle_cosine_minus_penumbra_half_angle_cosine,
			1.f/( math::max(umbra_half_angle_cosine - penumbra_half_angle_cosine, 0.0001f) )
			);
		backend::ref().set_ps_constant	(m_c_light_spot_falloff, l->spot_falloff );
	}
	
	backend::ref().set_ps_constant	(m_c_light_attenuation_power, l->attenuation_power );
	backend::ref().set_ps_constant	(m_c_diffuse_influence_factor, l->diffuse_influence_factor );
	backend::ref().set_ps_constant	(m_c_specular_influence_factor, l->specular_influence_factor );
	backend::ref().set_ps_constant	(m_c_light_color, light_color );
	backend::ref().set_ps_constant	(m_c_light_intensity, l->intensity );
	backend::ref().set_ps_constant	(m_c_light_type, s32(l->get_type()));
	backend::ref().set_ps_constant	(m_c_lighting_model, l->lighting_model );
}

void stage_light_propagation_volumes::render_to_rms(//xray::render::light* in_light,
													xray::math::float3 const& light_color,
													float const light_intensity,
													xray::math::float4x4 const& view_matrix, 
													xray::math::float4x4 const& projection_matrix)
{
	D3D_VIEWPORT orig_viewport;
	backend::ref().get_viewport				(orig_viewport);
	
	backend::ref().set_render_targets		(&*m_rt_rms_albedo_source, &*m_rt_rms_normal_source, &*m_rt_rms_position_source, 0);
	backend::ref().clear_render_targets		(0.0f, 0.0f, 0.0f, 0.0f);
	backend::ref().set_depth_stencil_target (&*m_rms_depth_stencil_source);
	backend::ref().clear_depth_stencil		(D3D_CLEAR_DEPTH | D3D_CLEAR_STENCIL, 1.0f, 0);
	
	D3D_VIEWPORT tmp_viewport;
	
	tmp_viewport.TopLeftX					= 0.0f;
	tmp_viewport.TopLeftY					= 0.0f;
	tmp_viewport.Width						= float(m_rsm_source_size);
	tmp_viewport.Height						= float(m_rsm_source_size);
	tmp_viewport.MinDepth					= 0;
	tmp_viewport.MaxDepth					= 1.f;
	
	backend::ref().set_viewport				(tmp_viewport);
	
	float3 view_position					= m_context->get_v_inverted().c.xyz();
	
	m_context->push_set_v					(view_matrix);
	m_context->push_set_p					(projection_matrix);
	
	render_surface_instances				visible_render_models;
	
	m_context->scene()->select_models		(m_context->get_culling_vp(), visible_render_models);
	
	render_surface_instances::iterator			it_d  = visible_render_models.begin();
	render_surface_instances::const_iterator	end_d = visible_render_models.end();
	
	for (; it_d != end_d; ++it_d)
	{
		render_surface_instance& instance	= *(*it_d);
		material_effects& me				= instance.m_render_surface->get_material_effects();
		render_geometry& geometry			= instance.m_render_surface->m_render_geometry;
		
		if (!me.m_effects[geometry_render_stage])
			continue;
		
		me.m_effects[geometry_render_stage]->apply(2);
		//m_fill_rsm_effect[static_mesh_vertex_input_type]->apply();
		
		backend::ref().set_ps_constant	(m_c_light_color, light_color );
		backend::ref().set_ps_constant	(m_c_light_intensity, light_intensity );
		
		//set_light_constans					(in_light);
		
		m_context->set_w					(*instance.m_transform);
		instance.set_constants				();
		geometry.geom->apply				();
		
		backend::ref().render_indexed		(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, geometry.primitive_count * 3, 0, 0);
	}
	
	backend::ref().reset_render_targets		();
	backend::ref().reset_depth_stencil_target();
	backend::ref().set_viewport				(orig_viewport);
	m_context->pop_v						();
	m_context->pop_p						();
}


static float3 compute_aligment(float3 const& lightXZshift, float4x4 const& light_space_transform, float smap_res)
{
	float3 align_origin				(0.0f, 0.0f, 0.0f); 
	
	align_origin					-= lightXZshift;
	
	float4x4 viewport(	float4(smap_res/2.f,	0.0f,				0.0f,		0.0f),
						float4(0.0f,			-smap_res/2.f,		0.0f,		0.0f),
						float4(0.0f,			0.0f,				1.0f,		0.0f),
						float4(smap_res/2.f,	smap_res/2.f,		0.0f,		1.0f));
	
	float4x4 viewport_invert				= math::invert4x3(viewport);
	float4x4 light_space_transform_invert	= math::invert4x3(light_space_transform);
	
	float4	origin_pixel			= light_space_transform.transform(float4(align_origin, 1.f));
	origin_pixel					*= (1.f/origin_pixel.w);
	origin_pixel					= viewport.transform(origin_pixel);
	
	float const align_granularity	= 4.f;
	
	origin_pixel.x					= origin_pixel.x / align_granularity - floorf	(origin_pixel.x / align_granularity);
	origin_pixel.y					= origin_pixel.y / align_granularity - floorf	(origin_pixel.y / align_granularity);
	origin_pixel.x					*= align_granularity;
	origin_pixel.y					*= align_granularity;
	origin_pixel.z					= 0;
	
	origin_pixel.xyz()				= viewport_invert.transform_direction(origin_pixel.xyz());
	origin_pixel.xyz()				= light_space_transform_invert.transform_direction(origin_pixel.xyz());
	
	return origin_pixel.xyz();
}

void stage_light_propagation_volumes::render_to_sun_rms(xray::render::light* sun, u32 const cascade_index)
{
	float max_scale					= m_radiance_volume[cascade_index].get_scale();
	
	float3 sun_position				= m_context->get_view_pos() - sun->direction * max_scale * 1.41421f * 2.0f;// * 20.0f;
	
	float4x4 view_matrix			= create_camera_direction(sun_position, sun->direction, float3(1, 0, 0));
	float4x4 projection_matrix		= math::create_orthographic_projection(
		max_scale, 
		max_scale,
		0.01f, 
		1.41421f * max_scale * 20.0f
	);
	
	float3 adjastment				= compute_aligment(float3(0.0f, 0.0f, 0.0f), view_matrix * projection_matrix, m_rsm_source_size);
	view_matrix						= create_camera_direction(sun_position + adjastment, sun->direction, float3(1, 0, 0));
	
	render_to_rms					(sun->color, sun->intensity, view_matrix, projection_matrix);
}

static xray::math::float3 view_matrix_parameters[6][3] = {	
	{xray::math::float3(0.0f, 0.0f, 0.0f), xray::math::float3(1.0f, 0.0f, 0.0f),	xray::math::float3(0.0f, 1.0f, 0.0f)},	//  x
	{xray::math::float3(0.0f, 0.0f, 0.0f), xray::math::float3(-1.0f, 0.0f, 0.0f),	xray::math::float3(0.0f, 1.0f, 0.0f)},	// -x
	{xray::math::float3(0.0f, 0.0f, 0.0f), xray::math::float3(0.0f, 1.0f, 0.0f),	xray::math::float3(0.0f, 0.0f, -1.0f)},	//  y
	{xray::math::float3(0.0f, 0.0f, 0.0f), xray::math::float3(0.0f, -1.0f, 0.0f),	xray::math::float3(0.0f, 0.0f, 1.0f)},	// -y
	{xray::math::float3(0.0f, 0.0f, 0.0f), xray::math::float3(0.0f, 0.0f, 1.0f),	xray::math::float3(0.0f, 1.0f, 0.0f)},	//  z
	{xray::math::float3(0.0f, 0.0f, 0.0f), xray::math::float3(0.0f, 0.0f, -1.0f),	xray::math::float3(0.0f, 1.0f, 0.0f)},	// -z
};

void stage_light_propagation_volumes::render_to_point_rms(xray::render::light* l, u32 const face_index)
{
	math::float4x4 face_view_matrix = math::create_camera_at(
		l->position + view_matrix_parameters[face_index][0],
		l->position + view_matrix_parameters[face_index][1],
		view_matrix_parameters[face_index][2]
	);
	
	math::float4x4 face_projection_matrix = math::create_perspective_projection(math::pi_d2, 1.0f, 0.1f, l->range);
	
	render_to_rms					(l->color, l->intensity, face_view_matrix, face_projection_matrix);
}

void stage_light_propagation_volumes::render_to_sky_rms	(u32 const face_index, u32 const cascade_index)
{
	float3 const light_color		= m_context->scene_view()->post_process_parameters().environment_skycolor[face_index].xyz();
	float const light_intensity		= 1.0f;
	
//	math::float4x4 face_projection_matrix = math::create_perspective_projection(math::pi_d2, 1.0f, 0.1f, l->range);
//	
//	render_to_rms					(l->color, l->intensity, face_view_matrix, face_projection_matrix);
	
	float const max_scale			= m_radiance_volume[cascade_index].get_scale();
	
	float3 const direction			= view_matrix_parameters[face_index][1];
	
	float3 const sky_position		= m_context->get_view_pos() - direction * max_scale * 1.41421f * 2.0f;// * 20.0f;
	
	float4x4 view_matrix			= create_camera_direction(sky_position, direction, view_matrix_parameters[face_index][2]);
	float4x4 const projection_matrix= math::create_orthographic_projection(
										max_scale, 
										max_scale,
										0.01f, 
										1.41421f * max_scale * 20.0f
									);
	
	float3 const adjastment			= compute_aligment(float3(0.0f, 0.0f, 0.0f), view_matrix * projection_matrix, m_rsm_source_size);
	view_matrix						= create_camera_direction(sky_position + adjastment, direction, view_matrix_parameters[face_index][2]);
	
	render_to_rms					(light_color, light_intensity, view_matrix, projection_matrix);
}

void stage_light_propagation_volumes::render_to_spot_rms(xray::render::light* l)
{
	float const max_angle			 = math::max(l->spot_umbra_angle, l->spot_penumbra_angle);
	
	math::float4x4 projection_matrix = math::create_perspective_projection(max_angle, 1.0f, l->range / 1000.0f, l->range);
	
	math::float4x4 view_matrix		 = math::create_camera_at(l->position, 
															  l->position + l->direction, 
															  math::normalize(math::cross_product(l->direction, l->right)));
	
	render_to_rms					(l->color, l->intensity, view_matrix, projection_matrix);
}

// void stage_light_propagation_volumes::process_cascade(u32 const cascade_index)
// {
// 	lights_db::lights_type const&	e_lights = m_context->scene()->lights().get_lights();
// 	
// 	collision::objects_type objects		= g_allocator;
// 	objects.reserve						( e_lights.size() );
// 	
// 	math::frustum frustum				= m_context->get_culling_vp();
// 	m_context->scene()->lights().lights_tree().cuboid_query(u32(-1), frustum, objects );
// 	
// 	bool has_indirect_lighting			= false;
// 	
// 	for ( collision::objects_type::const_iterator i = objects.begin(), e = objects.end(); i != e; ++i )
// 	{
// 		light* l = static_cast<light*>((*i)->user_data());
// 		
// 		if (!l->use_with_lpv)
// 			continue;
// 		switch (l->get_type())
// 		{
// 		case light_type_spot:
// 			process_spot_light			(cascade_index, l);
// 			has_indirect_lighting		= true;
// 			
// 			break;
// 		default:
// 			NOT_IMPLEMENTED();
// 		}
// 	}
// 	
// 	if (has_indirect_lighting)
// 		propagate_lighting				(cascade_index);
// }

void stage_light_propagation_volumes::propagate_lighting(u32 const cascade_index)
{
	m_radiance_volume[cascade_index].propagate_lighting(cascade_index);
}

void stage_light_propagation_volumes::render_quad()
{
	u32	offset;
	
	screen_vertex* pv = (screen_vertex*)backend::ref().vertex.lock(4, sizeof(screen_vertex), offset);
	pv->set(float4(-1.0f, -1.0f, 0.0f, 1.0f), float2(0.0f, 1.0f)); pv++;
	pv->set(float4(-1.0f,  1.0f, 0.0f, 1.0f), float2(0.0f, 0.0f)); pv++;
	pv->set(float4( 1.0f, -1.0f, 0.0f, 1.0f), float2(1.0f, 1.0f)); pv++;
	pv->set(float4( 1.0f,  1.0f, 0.0f, 1.0f), float2(1.0f, 0.0f)); pv++;
	backend::ref().vertex.unlock			();
	
	m_screen_vertex_geometry->apply			();
	backend::ref().render_indexed			(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 2*3, 0, offset);
}

void stage_light_propagation_volumes::downsample_rsm(xray::float3 const& light_direction, 
													 xray::float3 const& grid_origin, 
													 float grid_scale)
{
	m_downsample_rsm_effect->apply			();
	set_rsm_contants						(light_direction, grid_origin, grid_scale);
	
	backend::ref().set_render_targets		(&*m_rt_rms_albedo, &*m_rt_rms_normal, &*m_rt_rms_position, 0);
	backend::ref().set_depth_stencil_target	(0);
	
	D3D_VIEWPORT orig_viewport;
	backend::ref().get_viewport				(orig_viewport);
	
	D3D_VIEWPORT tmp_viewport;
	
	tmp_viewport.TopLeftX					= 0.0f;
	tmp_viewport.TopLeftY					= 0.0f;
	tmp_viewport.Width						= float(m_rsm_downsampled_size);
	tmp_viewport.Height						= float(m_rsm_downsampled_size);
	tmp_viewport.MinDepth					= 0;
	tmp_viewport.MaxDepth					= 1.f;
	
	backend::ref().set_viewport				(tmp_viewport);
	
	render_quad								();
	
	backend::ref().reset_render_targets		();
	backend::ref().reset_depth_stencil_target();
	backend::ref().set_viewport				(orig_viewport);
}

void stage_light_propagation_volumes::downsample_gbuffer()
{
	m_downsample_gbuffer_effect->apply		();
	
	u32 const width							= m_context->m_targets->m_t_gbuffer_position_downsampled->width();
	u32 const height						= m_context->m_targets->m_t_gbuffer_position_downsampled->height();
	
	float3 const* const eye_rays			= m_context->get_eye_rays();
	backend::ref().set_ps_constant			(m_c_eye_ray_corner, ((float4*)eye_rays)[0]);
	
	backend::ref().set_render_targets		(&*m_context->m_targets->m_rt_gbuffer_position_downsampled, &*m_context->m_targets->m_rt_gbuffer_normal_downsampled, 0, 0);
	backend::ref().set_depth_stencil_target	(0);
	
	D3D_VIEWPORT orig_viewport;
	backend::ref().get_viewport				(orig_viewport);
	
	D3D_VIEWPORT tmp_viewport;
	
	tmp_viewport.TopLeftX					= 0.0f;
	tmp_viewport.TopLeftY					= 0.0f;
	tmp_viewport.Width						= float(width);
	tmp_viewport.Height						= float(height);
	tmp_viewport.MinDepth					= 0;
	tmp_viewport.MaxDepth					= 1.f;
	
	backend::ref().set_viewport				(tmp_viewport);
	
	render_quad								();
	
	backend::ref().reset_render_targets		();
	backend::ref().reset_depth_stencil_target();
	backend::ref().set_viewport				(orig_viewport);
}

void stage_light_propagation_volumes::inject_lighting(u32 const cascade_index, 
													  xray::math::float3 const& light_position, 
													  xray::math::float3 const& light_direction,
													  float light_fov)
{
	m_radiance_volume[cascade_index].inject_lighting(light_position, light_direction, light_fov, m_rsm_downsampled_size);
}

void stage_light_propagation_volumes::inject_occluders(u32 const cascade_index, 
													   xray::math::float3 const& light_position, 
													   xray::math::float3 const& light_direction)
{
	m_radiance_volume[cascade_index].inject_occluders(m_context, light_position, light_direction, m_rsm_downsampled_size);
}

static float blend_alpha = 0.0f;

void stage_light_propagation_volumes::execute()
{
	PIX_EVENT(stage_ambient_occlusion);
	
	if (!is_effects_ready())
		return;
	
	if (!is_enabled())
	{
		execute_disabled();
		return;
	}
	
	light*	sun							= &*m_context->scene()->lights().get_sun().c_ptr();
	
	lights_db::lights_type const&		e_lights = m_context->scene()->lights().get_lights();
	
	collision::objects_type objects		= g_allocator;
	objects.reserve						( e_lights.size() );
	
	math::frustum frustum				= m_context->get_culling_vp();
	m_context->scene()->lights().lights_tree().cuboid_query(u32(-1), frustum, objects );
	
	// Refresh once per 5 frames
	bool is_need_refresh						= m_context->scene_view()->get_render_frame_index() % options::ref().m_lpv_refresh_once_per_frames == 0;
	
	m_has_indirect_lighting						= false;
	
	if (sun && sun->use_with_lpv)
	{
		m_has_indirect_lighting					= true;
	}
	
	for ( collision::objects_type::const_iterator i = objects.begin(), e = objects.end(); i != e; ++i )
	{
		light* l = static_cast<light*>((*i)->user_data());
		
		if (!l->use_with_lpv)
			continue;
		
		switch (l->get_type())
		{
		case light_type_spot:
		case light_type_point:
			m_has_indirect_lighting				= true;
			break;
		};
	}
	
	bool sky_lighting							= m_context->scene_view()->post_process_parameters().use_environment_skycolor;
	
	m_has_indirect_lighting = m_has_indirect_lighting || sky_lighting;
	
	if (is_need_refresh && m_has_indirect_lighting)
	{
		for (s32 cascade_index = m_num_cascades - 1; cascade_index >= 0; cascade_index--)
			m_radiance_volume[cascade_index].prepare_gv();
		
		downsample_gbuffer						();
		
		backend::ref().set_depth_stencil_target		(&*m_context->m_targets->m_apply_indirect_lighting_ds);
		backend::ref().clear_depth_stencil			(D3D_CLEAR_DEPTH | D3D_CLEAR_STENCIL, 1.0f, 0);
		backend::ref().reset_depth_stencil_target	();
		
		for (s32 cascade_index = m_num_cascades - 1; cascade_index >= 0; cascade_index--)
			m_radiance_volume[cascade_index].prepare(
				m_context->get_view_pos(), 
				m_context->get_view_dir(), 
				0.4f //cascade_index == 0 ? 0.3f : 0.5f
			);
		
		for (s32 cascade_index = m_num_cascades - 1; cascade_index >= 0; cascade_index--)
			m_radiance_volume[cascade_index].inject_camera_occluders(m_context);
		
		if (sky_lighting)
		{
			for (int face_index = 0; face_index < 6; face_index++)
			{
				float3 const light_color		= m_context->scene_view()->post_process_parameters().environment_skycolor[face_index].xyz();
				
				if (face_index == 2 || math::length(light_color) < math::epsilon_5)
					continue;
				
				float3 const direction	= view_matrix_parameters[face_index][1];
				float3 const position	= -direction * 1000.0f;
				
				BEGIN_CPUGPU_TIMER(statistics::ref().lpv_stat_group.rsm_rendering_time);
				if (!options::ref().m_lpv_disable_rsm_generating)
					render_to_sky_rms	(face_index, m_num_cascades - 1);
				END_CPUGPU_TIMER;
				
				for (s32 cascade_index = m_num_cascades - 1; cascade_index >= 0; cascade_index--)
				{
					BEGIN_CPUGPU_TIMER(statistics::ref().lpv_stat_group.rsm_downsample_time);
					if (!options::ref().m_lpv_disable_rsm_downsampling)
						downsample_rsm		(direction, m_radiance_volume[cascade_index].get_origin(), m_radiance_volume[cascade_index].get_scale());
					END_CPUGPU_TIMER;
					
					BEGIN_CPUGPU_TIMER(statistics::ref().lpv_stat_group.vpl_injection_time);
					if (!options::ref().m_lpv_disable_vpl_injection)
						inject_lighting					(
							cascade_index, 
							position, 
							direction, 
							1.0f
						);
					END_CPUGPU_TIMER;
					
					BEGIN_CPUGPU_TIMER(statistics::ref().lpv_stat_group.gv_injection_time);
					if (!options::ref().m_lpv_disable_gv_injection)
						inject_occluders				(cascade_index, position, direction);
					END_CPUGPU_TIMER;
				}
			}
		}
		
		if (sun && sun->use_with_lpv)
		{
			for (s32 cascade_index = m_num_cascades - 1; cascade_index >= 0; cascade_index--)
			{
				//backend::ref().flush_rt_shader_resources();
				
				//backend::ref().flush_rt_shader_resources();

				BEGIN_CPUGPU_TIMER(statistics::ref().lpv_stat_group.rsm_rendering_time);
				if (!options::ref().m_lpv_disable_rsm_generating)
					render_to_sun_rms			(sun, cascade_index/*m_num_cascades - 1*/);
				END_CPUGPU_TIMER;

				BEGIN_CPUGPU_TIMER(statistics::ref().lpv_stat_group.rsm_downsample_time);
				if (!options::ref().m_lpv_disable_rsm_downsampling)
					downsample_rsm				(sun->direction, m_radiance_volume[cascade_index].get_origin(), m_radiance_volume[cascade_index].get_scale());
				END_CPUGPU_TIMER;
				BEGIN_CPUGPU_TIMER(statistics::ref().lpv_stat_group.vpl_injection_time);
				if (!options::ref().m_lpv_disable_vpl_injection)
					inject_lighting					(
						cascade_index, 
						sun->position, 
						sun->direction, 
						1.0f//math::max(sun->spot_umbra_angle, sun->spot_penumbra_angle)
					);
				END_CPUGPU_TIMER;
				
				BEGIN_CPUGPU_TIMER(statistics::ref().lpv_stat_group.gv_injection_time);
				if (!options::ref().m_lpv_disable_gv_injection)
					inject_occluders				(cascade_index, sun->position, sun->direction);
				END_CPUGPU_TIMER;
			}
			m_has_indirect_lighting		= true;
		}
		
		for ( collision::objects_type::const_iterator i = objects.begin(), e = objects.end(); i != e; ++i )
		{
			light* l = static_cast<light*>((*i)->user_data());
			
			if (!l->use_with_lpv)
				continue;
			switch (l->get_type())
			{
			case light_type_spot:
				
				BEGIN_CPUGPU_TIMER(statistics::ref().lpv_stat_group.rsm_rendering_time);
				if (!options::ref().m_lpv_disable_rsm_generating)
					render_to_spot_rms				(l);
				END_CPUGPU_TIMER;
				
				for (s32 cascade_index = m_num_cascades - 1; cascade_index >= 0; cascade_index--)
				{
					backend::ref().flush_rt_shader_resources();
					
					BEGIN_CPUGPU_TIMER(statistics::ref().lpv_stat_group.rsm_downsample_time);
					if (!options::ref().m_lpv_disable_rsm_downsampling)
						downsample_rsm					(l->direction, m_radiance_volume[cascade_index].get_origin(), m_radiance_volume[cascade_index].get_scale());
					END_CPUGPU_TIMER;
					
					BEGIN_CPUGPU_TIMER(statistics::ref().lpv_stat_group.vpl_injection_time);
					if (!options::ref().m_lpv_disable_vpl_injection)
						inject_lighting					(
							cascade_index, 
							l->position, 
							l->direction, 
							math::max(l->spot_umbra_angle, l->spot_penumbra_angle)
						);
					END_CPUGPU_TIMER;
					
					BEGIN_CPUGPU_TIMER(statistics::ref().lpv_stat_group.gv_injection_time);
					if (!options::ref().m_lpv_disable_gv_injection)
						inject_occluders				(cascade_index, l->position, l->direction);
					END_CPUGPU_TIMER;
				}
				
				m_has_indirect_lighting		= true;
				
				break;
			case light_type_point:
				
				for (int face_index = 0; face_index < 6; face_index++)
				{
					float3 face_direction				= view_matrix_parameters[face_index][1];
					
					BEGIN_CPUGPU_TIMER(statistics::ref().lpv_stat_group.rsm_rendering_time);
					if (!options::ref().m_lpv_disable_rsm_generating)
						render_to_point_rms				(l, face_index);
					END_CPUGPU_TIMER;
					
					for (s32 cascade_index = m_num_cascades - 1; cascade_index >= 0; cascade_index--)
					{
						backend::ref().flush_rt_shader_resources();
						
						BEGIN_CPUGPU_TIMER(statistics::ref().lpv_stat_group.rsm_downsample_time);
						if (!options::ref().m_lpv_disable_rsm_downsampling)
							downsample_rsm					(face_direction, m_radiance_volume[cascade_index].get_origin(), m_radiance_volume[cascade_index].get_scale());
						END_CPUGPU_TIMER;
						
						BEGIN_CPUGPU_TIMER(statistics::ref().lpv_stat_group.vpl_injection_time);
						if (!options::ref().m_lpv_disable_vpl_injection)
							inject_lighting					(
								cascade_index, 
								l->position, 
								face_direction, 
								math::pi_d2
							);
						END_CPUGPU_TIMER;
						
						BEGIN_CPUGPU_TIMER(statistics::ref().lpv_stat_group.gv_injection_time);
						if (!options::ref().m_lpv_disable_gv_injection)
							inject_occluders				(cascade_index, l->position, face_direction);
						END_CPUGPU_TIMER;
					}
					
					m_has_indirect_lighting		= true;
				}
				break;
			default:
				break;//NOT_IMPLEMENTED();
			}
		}
		
		BEGIN_CPUGPU_TIMER(statistics::ref().lpv_stat_group.propagation_time);
		if (!options::ref().m_lpv_disable_propagation)
			for (s32 cascade_index = m_num_cascades - 1; cascade_index >= 0; cascade_index--)
				propagate_lighting(cascade_index);
		END_CPUGPU_TIMER;
		
		if (blend_alpha > 1.0f)
		{
			//for (s32 cascade_index = m_num_cascades - 1; cascade_index >= 0; cascade_index--)
			//	m_radiance_volume[cascade_index].fill_previous_result();
			//blend_alpha = 0.0f;
		}
	}
	
	if (m_has_indirect_lighting && !options::ref().m_lpv_disable_lpv_lookup)
	{
		BEGIN_CPUGPU_TIMER(statistics::ref().lpv_stat_group.lpv_lookup_time);
		
		for (s32 cascade_index = m_num_cascades - 1; cascade_index >= 0; cascade_index--)
		{
			// Apply to scene.
			backend::ref().set_render_targets		(&*m_context->m_targets->m_rt_accumulator_diffuse, 0, 0, 0);
			
			backend::ref().set_depth_stencil_target	(NULL);
			
			m_apply_indirect_lighting_effect->apply	(0);
			
			backend::ref().set_ps_texture			("t_radiance_r", &*m_radiance_volume[cascade_index].get_accumulated_propagation_r());
			backend::ref().set_ps_texture			("t_radiance_g", &*m_radiance_volume[cascade_index].get_accumulated_propagation_g());
			backend::ref().set_ps_texture			("t_radiance_b", &*m_radiance_volume[cascade_index].get_accumulated_propagation_b());
			//backend::ref().set_ps_texture			("t_previous_radiance_r", &*m_radiance_volume[cascade_index].get_previous_radiance_r_texture());
			//backend::ref().set_ps_texture			("t_previous_radiance_g", &*m_radiance_volume[cascade_index].get_previous_radiance_g_texture());
			//backend::ref().set_ps_texture			("t_previous_radiance_b", &*m_radiance_volume[cascade_index].get_previous_radiance_b_texture());
			//backend::ref().set_ps_texture			("t_occluders", &*m_radiance_volume[cascade_index].get_occluders_texture());
			
			backend::ref().set_ps_constant			(m_c_cascade_index, cascade_index);
			backend::ref().set_ps_constant			(m_c_num_cascades, m_num_cascades);
			
			backend::ref().set_ps_constant			(m_c_grid_cell_size, m_radiance_volume[cascade_index].get_scale() / float(m_grid_size));
			backend::ref().set_ps_constant			(m_c_grid_size, float(m_grid_size));
			backend::ref().set_ps_constant			(m_c_grid_origin, m_radiance_volume[cascade_index].get_origin());
			
			blend_alpha								+= m_context->get_time_delta() * (1.0f / 1.0f);
			
			backend::ref().set_ps_constant			(m_c_radiance_blend_factor, blend_alpha);
			
			bool first								= cascade_index == 0;
			
			float smaller_cascade_grid_cell_size	= m_radiance_volume[cascade_index - (first ? 0 : 1)].get_scale() / float(m_grid_size);
			float smaller_cascade_grid_size			= float(m_grid_size);
			float3 smaller_cascade_grid_origin		= m_radiance_volume[cascade_index - (first ? 0 : 1)].get_origin();
			
			backend::ref().set_ps_constant			(m_c_smaller_cascade_grid_cell_size, smaller_cascade_grid_cell_size);
			backend::ref().set_ps_constant			(m_c_smaller_cascade_grid_size, smaller_cascade_grid_size);
			backend::ref().set_ps_constant			(m_c_smaller_cascade_grid_origin, smaller_cascade_grid_origin);
			
			backend::ref().set_ps_constant			(m_c_interreflection_contribution, options::ref().m_lpv_interreflection_contribution);
			
			float3 const* const eye_rays			= m_context->get_eye_rays();
			backend::ref().set_ps_constant			(m_c_eye_ray_corner,	((float4*)eye_rays)[0]);
			
			render_quad								();
			
			//m_apply_indirect_lighting_effect->apply	(1);
			//backend::ref().set_render_targets		(&*m_context->m_targets->m_rt_accumulator_diffuse, 0, 0, 0);
			//render_quad								();
			
			backend::ref().reset_render_targets			();
			backend::ref().reset_depth_stencil_target	();
			
			
		}
		END_CPUGPU_TIMER;
	}
	
	/*
	backend::ref().set_depth_stencil_target		(&*m_context->m_targets->m_apply_indirect_lighting_ds);
	backend::ref().clear_depth_stencil			(D3D_CLEAR_DEPTH | D3D_CLEAR_STENCIL, 1.0f, 0);
	backend::ref().reset_depth_stencil_target	();

	for (s32 cascade_index = m_num_cascades - 1; cascade_index >= 0; cascade_index--)
		m_radiance_volume[cascade_index].prepare(m_context->get_view_pos(), m_context->get_view_dir());
	
	for (s32 cascade_index = m_num_cascades - 1; cascade_index >= 0; cascade_index--)
	{
		process_cascade							(cascade_index);
		
		// Apply to scene.
		backend::ref().set_render_targets		(&*m_context->m_targets->m_rt_accumulator_diffuse, &*m_context->m_targets->m_rt_accumulator_specular, 0, 0);
		backend::ref().set_depth_stencil_target	(&*m_context->m_targets->m_apply_indirect_lighting_ds);
		
		m_apply_indirect_lighting_effect->apply	();
		
		backend::ref().set_ps_texture			("t_radiance_r", &*m_radiance_volume[cascade_index].get_radiance_r_texture());
		backend::ref().set_ps_texture			("t_radiance_g", &*m_radiance_volume[cascade_index].get_radiance_g_texture());
		backend::ref().set_ps_texture			("t_radiance_b", &*m_radiance_volume[cascade_index].get_radiance_b_texture());
		
		backend::ref().set_ps_constant			(m_c_grid_cell_size, m_radiance_volume[cascade_index].get_scale() / float(m_grid_size));
		backend::ref().set_ps_constant			(m_c_grid_size, float(m_grid_size));
		backend::ref().set_ps_constant			(m_c_grid_origin, m_radiance_volume[cascade_index].get_origin());
		
		backend::ref().set_ps_constant			(m_c_interreflection_contribution, options::ref().m_lpv_interreflection_contribution);
		
		float3 const* const eye_rays			= m_context->get_eye_rays();
		backend::ref().set_ps_constant			(m_c_eye_ray_corner,	((float4*)eye_rays)[0]);
		
		render_quad								();
		
		backend::ref().reset_render_targets			();
		backend::ref().reset_depth_stencil_target	();
	}
	*/
}

void stage_light_propagation_volumes::draw_debug()
{
#ifndef MASTER_GOLD
	m_radiance_volume[0].draw_debug		(m_context);
	m_radiance_volume[1].draw_debug		(m_context);
	m_radiance_volume[2].draw_debug		(m_context);
#endif // #ifndef MASTER_GOLD
}

} // namespace render
} // namespace xray
