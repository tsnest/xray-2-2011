////////////////////////////////////////////////////////////////////////////
//	Created		: 27.08.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "stage_lights.h"
#include <xray/render/core/effect_manager.h>
#include "effect_light_mask.h"
#include <xray/render/core/resource_manager.h>
#include "du_sphere.h"

#include "point_light_effect.h"
#include "spot_light_effect.h"
#include "capsule_light_effect.h"
#include "obb_light_effect.h"
#include "sphere_light_effect.h"
#include "plane_spot_light_effect.h"

#include <xray/render/core/render_target.h>
#include "vertex_formats.h"

#include "effect_shadow_map.h"
#include <xray/render/core/untyped_buffer.h>
#include <xray/render/core/backend.h>
#include <xray/render/core/res_geometry.h>
#include "light.h"
#include "renderer_context.h"
#include "render_model.h"
#include <xray/render/core/pix_event_wrapper.h>
#include "lights_db.h"
#include "scene.h"
#include "renderer_context_targets.h"
#include <xray/render/core/render_target.h>
#include <xray/render/core/effect_options_descriptor.h>
#include <xray/collision/space_partitioning_tree.h>
#include "system_renderer.h"
#include "render_particle_emitter_instance.h"
#include "stage_particles.h"
#include <xray/render/facade/particles.h>
#include <xray/particle/particle_data_type.h>
#include <xray/particle/world.h>
#include "scene_view.h"
#include <xray/geometry_primitives.h>

#include <xray/console_command.h>
#include <xray/console_command_processor.h>

#include "engine_options.h"

#include "statistics.h"

#include "speedtree_forest.h"
#include "speedtree_tree.h"

#include "material.h"
#include "render_model.h"

#include "effect_downsample_skin_irradiance_texture.h"
#include "effect_lighting_stage_organic_base_materials.h"
#include "effect_fix_irradiance_texture.h"

#include "organic_lighting_passes.h"

#include <xray/render/core/options.h>

static float s_shadow_z_near_value = 0.025f;
static xray::console_commands::cc_float s_shadow_z_near("shadow_z_near", s_shadow_z_near_value, 0.0f, 1.0f, true, xray::console_commands::command_type_engine_internal);

using xray::render::stage_lights;
using xray::render::renderer_context;
using xray::render::render_model_instance;
using xray::render::light;

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

struct make_lookup_vcm_vertex
{
	xray::math::float4 position;
	xray::math::float2 tc;
	void set(xray::math::float4 const& in_position, xray::math::float2 const& in_tc)
	{
		position = in_position;
		tc		 = in_tc;
	}
};

void stage_lights::new_sphere_geometry	( )
{
	m_sphere_geometry.vertex_buffer	= resource_manager::ref().create_buffer( DU_SPHERE_NUMVERTEX*sizeof( float3), du_sphere_vertices, enum_buffer_type_vertex, false);
	m_sphere_geometry.index_buffer	= resource_manager::ref().create_buffer( DU_SPHERE_NUMFACES*3*sizeof( u16), du_sphere_faces, enum_buffer_type_index, false);

	D3D_INPUT_ELEMENT_DESC desc[]	= {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0 }
	};
	m_sphere_geometry.geometry		= resource_manager::ref().create_geometry( desc, sizeof(float3), *m_sphere_geometry.vertex_buffer, *m_sphere_geometry.index_buffer);
}

void stage_lights::create_pyramid_geometry	( )
{
	float3 vertices[]					= {
		float3(0.f, 0.f, 0.f),
		float3(-1.f,-1.f,1.f),
		float3(-1.f, 1.f,1.f),
		float3( 1.f,-1.f,1.f),
		float3( 1.f, 1.f,1.f),
	};
	u16 indices[]						= {
		0, 1, 2,
		0, 2, 4,
		0, 4, 3,
		0, 3, 1,
		1, 3, 2,
		2, 3, 4,
	};
	
	m_pyramid_geometry.vertex_buffer	= resource_manager::ref().create_buffer( array_size(vertices)*sizeof(vertices[0]), vertices, enum_buffer_type_vertex, false);
	m_pyramid_geometry.index_buffer		= resource_manager::ref().create_buffer( array_size(indices)*sizeof(indices[0]), indices, enum_buffer_type_index, false);
	
	D3D_INPUT_ELEMENT_DESC desc[]		= {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0 }
	};
	
	m_pyramid_geometry.geometry			= resource_manager::ref().create_geometry( desc, sizeof(float3), *m_pyramid_geometry.vertex_buffer, *m_pyramid_geometry.index_buffer);
}

void stage_lights::create_obb_geometry	( )
{
	m_obb_geometry.vertex_buffer	=
		resource_manager::ref().create_buffer(
			geometry_utils::cube_solid::vertex_count*sizeof(float3),
			geometry_utils::cube_solid::vertices,
			enum_buffer_type_vertex,
			false
		);
	u32 const buffer_size	= geometry_utils::cube_solid::index_count*sizeof(u16);
	u16* const indices		= static_cast<u16*>( ALLOCA( buffer_size ) );
	std::copy				( geometry_utils::cube_solid::faces, geometry_utils::cube_solid::faces + geometry_utils::cube_solid::index_count, indices );
	m_obb_geometry.index_buffer		=
		resource_manager::ref().create_buffer(
			geometry_utils::cube_solid::index_count*sizeof(u16),
			indices,
			enum_buffer_type_index,
			false
		);

	D3D_INPUT_ELEMENT_DESC desc[]	= {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0 }
	};
	m_obb_geometry.geometry			= resource_manager::ref().create_geometry( desc, sizeof(float3), *m_obb_geometry.vertex_buffer, *m_obb_geometry.index_buffer);
}

stage_lights::stage_lights( renderer_context* context, bool is_forward_lighting_pass): 
	stage							( context ), 
	m_is_forward_lighting_pass		( is_forward_lighting_pass )
{
	
	m_enabled						= options::ref().m_enabled_lighting_stage;
	
	if (m_enabled)
	{
		if (is_forward_lighting_pass)
			m_enabled				= options::ref().m_enabled_forward_lighting_stage;
		else
			m_enabled				= options::ref().m_enabled_deferred_lighting_stage;
	}
	
	if (options::ref().m_enabled_local_light_shadows)
	{
		m_shadow_depth_stencil[0] = resource_manager::ref().create_render_target(r2_rt_shadow_map_size_1024, 1024, 1024, DXGI_FORMAT_R24G8_TYPELESS, enum_rt_usage_depth_stencil);
		m_shadow_depth_stencil[1] = resource_manager::ref().create_render_target(r2_rt_shadow_map_size_512,   512,  512, DXGI_FORMAT_R24G8_TYPELESS, enum_rt_usage_depth_stencil);
		m_shadow_depth_stencil[2] = resource_manager::ref().create_render_target(r2_rt_shadow_map_size_256,   256,  256, DXGI_FORMAT_R24G8_TYPELESS, enum_rt_usage_depth_stencil);
		m_shadow_depth_stencil_texture[0] = resource_manager::ref().create_texture(r2_rt_shadow_map_size_1024);
		m_shadow_depth_stencil_texture[1] = resource_manager::ref().create_texture(r2_rt_shadow_map_size_512);
		m_shadow_depth_stencil_texture[2] = resource_manager::ref().create_texture(r2_rt_shadow_map_size_256);
	}
	
	if (!is_forward_lighting_pass)
	{

#if 0
		// In high quality.
		m_cubemap_texture				= resource_manager::ref().create_texture_cube( "shadow_cubemap_texture_low", 1024, 1024, 0, DXGI_FORMAT_R16_FLOAT, D3D_USAGE_DEFAULT, 1, false);
		m_depth_stencil_cubemap_texture	= resource_manager::ref().create_texture_cube( "shadow_cubemap_texture_low_ds", 1024, 1024, 0, DXGI_FORMAT_R24G8_TYPELESS, D3D_USAGE_DEFAULT, 1, true);
		
		for (u32 cubemap_face_index=0; cubemap_face_index<6; cubemap_face_index++)
		{
			m_cubemap_face_render_target_names[cubemap_face_index].assignf("shadow_cubemap_render_target_low_%d", cubemap_face_index);
			m_cubemap_face_render_target[cubemap_face_index] = resource_manager::ref().create_render_target( 
				m_cubemap_face_render_target_names[cubemap_face_index].get_buffer(), 
				1024,
				1024,
				DXGI_FORMAT_R16_FLOAT, 
				enum_rt_usage_render_target, 
				m_cubemap_texture, 
				cubemap_face_index
			);
			m_cubemap_face_depth_stencil_names[cubemap_face_index].assignf("shadow_cubemap_depth_stencil_low_%d", cubemap_face_index);
			m_cubemap_face_depth_stencil[cubemap_face_index] = resource_manager::ref().create_render_target( 
				m_cubemap_face_depth_stencil_names[cubemap_face_index].get_buffer(),
				1024,
				1024,
				DXGI_FORMAT_R24G8_TYPELESS, 
				enum_rt_usage_depth_stencil,
				m_depth_stencil_cubemap_texture, 
				cubemap_face_index
			);
		}
#endif // #if 0
	
#if 0
		m_vcm_render_target = resource_manager::ref().create_render_target("vcm_texture", 3072, 2048, DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);
		m_vcm_depth_stencil = resource_manager::ref().create_render_target("vcm_texture_ds", 3072, 2048, DXGI_FORMAT_R24G8_TYPELESS, enum_rt_usage_depth_stencil);
		
		m_lookup_vcm_texture				= resource_manager::ref().create_texture_cube( "lookup_vcm_texture", 64, 64, 0, DXGI_FORMAT_R16G16B16A16_FLOAT, D3D_USAGE_DEFAULT, 1, false);
		m_lookup_vcm_depth_stencil_texture	= resource_manager::ref().create_texture_cube( "lookup_vcm_texture_ds", 64, 64, 0, DXGI_FORMAT_R24G8_TYPELESS, D3D_USAGE_DEFAULT, 1, true);
		
		for (u32 cubemap_face_index=0; cubemap_face_index<6; cubemap_face_index++)
		{
			m_lookup_vcm_render_target_names[cubemap_face_index].assignf("lookup_vcm_render_target_%d", cubemap_face_index);
			m_lookup_vcm_render_target[cubemap_face_index] = resource_manager::ref().create_render_target( 
				m_lookup_vcm_render_target_names[cubemap_face_index].get_buffer(), 
				64,
				64,
				DXGI_FORMAT_R16G16B16A16_FLOAT, 
				enum_rt_usage_render_target, 
				m_lookup_vcm_texture, 
				cubemap_face_index
			);
			m_lookup_vcm_depth_stencil_names[cubemap_face_index].assignf("lookup_vcm_depth_stencil_%d", cubemap_face_index);
			m_lookup_vcm_depth_stencil[cubemap_face_index] = resource_manager::ref().create_render_target( 
				m_lookup_vcm_depth_stencil_names[cubemap_face_index].get_buffer(),
				64,
				64,
				DXGI_FORMAT_R24G8_TYPELESS, 
				enum_rt_usage_depth_stencil,
				m_lookup_vcm_depth_stencil_texture, 
				cubemap_face_index
			);
		}
		
		const D3D_INPUT_ELEMENT_DESC make_lookup_vcm_layout[] = 
		{
			{"POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	    0, 16,	D3D_INPUT_PER_VERTEX_DATA, 0},
		};
		
		u16 indices[6]			= { 0, 1, 2, 3, 2, 1 };
		m_lookup_vcm_ib			= resource_manager::ref().create_buffer( 6*sizeof(u16), indices, enum_buffer_type_index, false);
		m_lookup_vcm_geometry	= resource_manager::ref().create_geometry(
			make_lookup_vcm_layout,
			sizeof(make_lookup_vcm_vertex),
			backend::ref().vertex.buffer(),
			*m_lookup_vcm_ib
		);
#endif // #if 0

	}
	else // if forward lighting
	{
		u32 const irradiance_texture_size			= options::ref().m_organic_irradiance_texture_size;
		u32 const blurred_irradiance_texture_size	= irradiance_texture_size / 4;
		
		m_rt_skin_scattering_temp		= resource_manager::ref().create_render_target(r2_rt_skin_scattering_temp, irradiance_texture_size, irradiance_texture_size, DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);
		m_t_skin_scattering_temp		= resource_manager::ref().create_texture(r2_rt_skin_scattering_temp);
		
		//m_rt_skin_scattering_position	= resource_manager::ref().create_render_target(r2_rt_skin_scattering_position, irradiance_texture_size, irradiance_texture_size, DXGI_FORMAT_R32G32B32A32_FLOAT, enum_rt_usage_render_target);
		//m_t_skin_scattering_position	= resource_manager::ref().create_texture(r2_rt_skin_scattering_position);

		m_rt_skin_scattering_stretch	= resource_manager::ref().create_render_target(r2_rt_skin_scattering_stretch, irradiance_texture_size, irradiance_texture_size, DXGI_FORMAT_R16G16_FLOAT, enum_rt_usage_render_target);
		m_t_skin_scattering_stretch		= resource_manager::ref().create_texture(r2_rt_skin_scattering_stretch);

		m_rt_skin_scattering			= resource_manager::ref().create_render_target(r2_rt_skin_scattering, irradiance_texture_size, irradiance_texture_size, DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);
		m_t_skin_scattering				= resource_manager::ref().create_texture(r2_rt_skin_scattering);
		
		m_rt_skin_scattering_small		= resource_manager::ref().create_render_target(r2_rt_skin_scattering_small, blurred_irradiance_texture_size, blurred_irradiance_texture_size, DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);
		m_t_skin_scattering_small		= resource_manager::ref().create_texture(r2_rt_skin_scattering_small);
		
		m_rt_skin_scattering_blurred_0	= resource_manager::ref().create_render_target(r2_rt_skin_scattering_blurred_0, blurred_irradiance_texture_size, blurred_irradiance_texture_size, DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);
		m_t_skin_scattering_blurred_0	= resource_manager::ref().create_texture(r2_rt_skin_scattering_blurred_0);
		
		m_rt_skin_scattering_blurred_1	= resource_manager::ref().create_render_target(r2_rt_skin_scattering_blurred_1, blurred_irradiance_texture_size, blurred_irradiance_texture_size, DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);
		m_t_skin_scattering_blurred_1	= resource_manager::ref().create_texture(r2_rt_skin_scattering_blurred_1);
		
		m_rt_skin_scattering_blurred_2	= resource_manager::ref().create_render_target(r2_rt_skin_scattering_blurred_2, blurred_irradiance_texture_size, blurred_irradiance_texture_size, DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);
		m_t_skin_scattering_blurred_2	= resource_manager::ref().create_texture(r2_rt_skin_scattering_blurred_2);
		
		m_rt_skin_scattering_blurred_3	= resource_manager::ref().create_render_target(r2_rt_skin_scattering_blurred_3, blurred_irradiance_texture_size, blurred_irradiance_texture_size, DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);
		m_t_skin_scattering_blurred_3	= resource_manager::ref().create_texture(r2_rt_skin_scattering_blurred_3);
		
		m_rt_skin_scattering_blurred_4	= resource_manager::ref().create_render_target(r2_rt_skin_scattering_blurred_4, blurred_irradiance_texture_size, blurred_irradiance_texture_size, DXGI_FORMAT_R16G16B16A16_FLOAT, enum_rt_usage_render_target);
		m_t_skin_scattering_blurred_4	= resource_manager::ref().create_texture(r2_rt_skin_scattering_blurred_4);
	}
	
	effect_manager::ref().create_effect<effect_shadow_map>(&m_shadow_effect);
	effect_manager::ref().create_effect<effect_light_mask>(&m_effect_accum_mask);
	effect_manager::ref().create_effect< point_light_effect<false, false> >(&m_point_light_accumulator);
	effect_manager::ref().create_effect< point_light_effect<false, true> >(&m_point_light_shadower);
	effect_manager::ref().create_effect< point_light_effect<true, false> >(&m_shadowed_point_light_accumulator);
	effect_manager::ref().create_effect< spot_light_effect<false> >(&m_spot_light_accumulator);
	effect_manager::ref().create_effect< spot_light_effect<true> >(&m_shadowed_spot_light_accumulator);
	effect_manager::ref().create_effect< capsule_light_effect >(&m_capsule_light_accumulator);
	effect_manager::ref().create_effect< obb_light_effect<false> >(&m_obb_light_accumulator);
	effect_manager::ref().create_effect< obb_light_effect<true> >(&m_shadowed_obb_light_accumulator);
	effect_manager::ref().create_effect< sphere_light_effect<false> >(&m_sphere_light_accumulator);
	effect_manager::ref().create_effect< sphere_light_effect<true> >(&m_shadowed_sphere_light_accumulator);
	effect_manager::ref().create_effect< plane_spot_light_effect<false> >(&m_plane_spot_light_accumulator);
	effect_manager::ref().create_effect< plane_spot_light_effect<true> >(&m_shadowed_plane_spot_light_accumulator);
	effect_manager::ref().create_effect<effect_downsample_skin_irradiance_texture>(&m_sh_downsample_skin_irradiance_texture);
	effect_manager::ref().create_effect<effect_fix_irradiance_texture>(&m_sh_fix_irradiance_texture);
	
	new_sphere_geometry				( );
	create_pyramid_geometry			( );
	create_obb_geometry				( );
	
	m_c_view_to_light_matrix		= backend::ref().register_constant_host( "view_to_light_matrix",  rc_float );
	m_c_shadow_z_bias				= backend::ref().register_constant_host( "shadow_z_bias",  rc_float );
	m_c_shadow_map_size				= backend::ref().register_constant_host( "shadow_map_size",  rc_float );
	m_c_shadow_transparency			= backend::ref().register_constant_host( "shadow_transparency",  rc_float );
	
	m_c_light_color					= backend::ref().register_constant_host( "light_color", rc_float );
	m_c_light_intensity				= backend::ref().register_constant_host( "light_intensity", rc_float );
	m_c_light_position				= backend::ref().register_constant_host( "light_position", rc_float );
	m_c_light_direction				= backend::ref().register_constant_host( "light_direction", rc_float );
	m_c_light_attenuation_power		= backend::ref().register_constant_host( "light_attenuation_power", rc_float );
	m_c_light_range					= backend::ref().register_constant_host( "light_range", rc_float );
	m_c_lighting_model				= backend::ref().register_constant_host( "lighting_model", rc_int );
	
	m_c_diffuse_influence_factor	= backend::ref().register_constant_host( "light_diffuse_influence_factor", rc_float );
	m_c_specular_influence_factor	= backend::ref().register_constant_host( "light_specular_influence_factor", rc_float );
	
	m_c_is_shadower					= backend::ref().register_constant_host( "is_shadower", rc_float );
	
	m_c_light_spot_penumbra_half_angle_cosine	= backend::ref().register_constant_host( "light_spot_penumbra_half_angle_cosine", rc_float );
	m_c_light_spot_umbra_half_angle_cosine		= backend::ref().register_constant_host( "light_spot_umbra_half_angle_cosine", rc_float );
	m_c_light_spot_inversed_umbra_half_angle_cosine_minus_penumbra_half_angle_cosine
		= backend::ref().register_constant_host( "light_spot_inversed_umbra_half_angle_cosine_minus_penumbra_half_angle_cosine", rc_float );
	m_c_light_spot_falloff			= backend::ref().register_constant_host( "light_spot_falloff", rc_float );
	
	m_c_light_type					= backend::ref().register_constant_host( "light_type", rc_int );
	
	m_c_light_capsule_half_width	= backend::ref().register_constant_host( "light_capsule_half_width", rc_float );
	m_c_light_capsule_radius			= backend::ref().register_constant_host( "light_capsule_radius", rc_float );

	m_c_light_sphere_radius			= backend::ref().register_constant_host( "light_sphere_radius", rc_float );
	m_c_light_local_to_world		= backend::ref().register_constant_host( "light_local_to_world", rc_float );

	m_c_light_sphere_radius			= backend::ref().register_constant_host( "light_sphere_radius", rc_float );
	
	m_c_eye_ray_corner				= backend::ref().register_constant_host( "s_eye_ray_corner", rc_float );
	m_c_near_far					= backend::ref().register_constant_host( "s_near_far", rc_float );
	
	m_far_fog_color_and_distance	= backend::ref().register_constant_host( "far_fog_color_and_distance", rc_float );
	m_near_fog_distance				= backend::ref().register_constant_host( "near_fog_distance", rc_float );
	
	m_c_is_unwrap_pass				= backend::ref().register_constant_host( "is_unwrap_pass", rc_float );

	m_blur_offsets_weights			= backend::ref().register_constant_host("offsets_weights", rc_float);
	m_kernel_offsets				= backend::ref().register_constant_host("kernel_offsets", rc_float);

	m_c_use_shadows					= backend::ref().register_constant_host("use_shadows", rc_int);
	
	m_ambient_color					= backend::ref().register_constant_host("ambient_color", rc_float );
	
	m_gamma_correction_factor		= backend::ref().register_constant_host( "gamma_correction_factor", rc_float );
	
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
}

static xray::math::float3 view_matrix_parameters[6][3] = {	
	{xray::math::float3(0.0f, 0.0f, 0.0f), xray::math::float3(1.0f, 0.0f, 0.0f),	xray::math::float3(0.0f, 1.0f, 0.0f)},	//  x
	{xray::math::float3(0.0f, 0.0f, 0.0f), xray::math::float3(-1.0f, 0.0f, 0.0f),	xray::math::float3(0.0f, 1.0f, 0.0f)},	// -x
	{xray::math::float3(0.0f, 0.0f, 0.0f), xray::math::float3(0.0f, 1.0f, 0.0f),	xray::math::float3(0.0f, 0.0f, -1.0f)},	//  y
	{xray::math::float3(0.0f, 0.0f, 0.0f), xray::math::float3(0.0f, -1.0f, 0.0f),	xray::math::float3(0.0f, 0.0f, 1.0f)},	// -y
	{xray::math::float3(0.0f, 0.0f, 0.0f), xray::math::float3(0.0f, 0.0f, 1.0f),	xray::math::float3(0.0f, 1.0f, 0.0f)},	//  z
	{xray::math::float3(0.0f, 0.0f, 0.0f), xray::math::float3(0.0f, 0.0f, -1.0f),	xray::math::float3(0.0f, 1.0f, 0.0f)},	// -z
};

bool stage_lights::is_effects_ready() const
{
	return	
		   m_shadow_effect.c_ptr() != NULL
		&& m_effect_accum_mask.c_ptr() != NULL
		&& m_point_light_accumulator.c_ptr() != NULL
		&& m_point_light_shadower.c_ptr() != NULL
		&& m_shadowed_point_light_accumulator.c_ptr() != NULL
		&& m_spot_light_accumulator.c_ptr() != NULL
		&& m_shadowed_spot_light_accumulator.c_ptr() != NULL
		&& m_capsule_light_accumulator.c_ptr() != NULL
		&& m_obb_light_accumulator.c_ptr() != NULL
		&& m_shadowed_obb_light_accumulator.c_ptr() != NULL
		&& m_sphere_light_accumulator.c_ptr() != NULL
		&& m_shadowed_sphere_light_accumulator.c_ptr() != NULL
		&& m_shadowed_plane_spot_light_accumulator.c_ptr() != NULL
		&& m_sh_downsample_skin_irradiance_texture.c_ptr() != NULL
		&& m_sh_fix_irradiance_texture.c_ptr() != NULL
		&& m_plane_spot_light_accumulator.c_ptr() != NULL;
}

void stage_lights::render_to_cubemap_face( u32 face_index, xray::math::float3 const& eye_position, float far_plane )
{
	math::float4x4 face_view_matrix = math::create_camera_at(
		eye_position + view_matrix_parameters[face_index][0], 
		eye_position + view_matrix_parameters[face_index][1], 
		view_matrix_parameters[face_index][2]
	);
	
	//float const N = 1024.0f;
	//float const b = 4.0f;
	
	float fov = math::pi_d2;//.98f * 2 * math::atan( (N / 2.0f) / (N / 2.0f - b) );
	
	math::float4x4 face_projection_matrix = math::create_perspective_projection(fov, 1.0f, far_plane / 100.0f, far_plane);
	
	m_context->push_set_v( face_view_matrix);
	m_context->push_set_p( face_projection_matrix);
	
	m_context->scene()->select_models( m_context->get_culling_vp(), m_dynamic_visuals_to_shadow);
	
	render_surface_instances::iterator it_d			= m_dynamic_visuals_to_shadow.begin();
	render_surface_instances::const_iterator	end_d	= m_dynamic_visuals_to_shadow.end();
	
	for ( ; it_d != end_d; ++it_d)
	{
		render_surface_instance& instance = *(*it_d);
		render_geometry& geometry	= instance.m_render_surface->m_render_geometry;
		
		//if ( !instance->get_material_effects().stage_enable[shadow_render_stage] || 
		//	 !instance->get_material_effects().m_effects[shadow_render_stage])
		//	continue;
		
		m_context->set_w(*instance.m_transform);
		geometry.geom->apply();
		//instance->get_material_effects().m_effects[shadow_render_stage]->apply();
		m_shadow_effect->apply();
		backend::ref().set_ps_constant	( m_c_light_position, eye_position);
		
		backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, geometry.primitive_count*3, 0, 0);		
	}
	
	m_context->pop_v();
	m_context->pop_p();
}

void stage_lights::make_spot_light_shadowmap( u32 shadow_quality, light* l )
{
	float const max_angle			 = math::max(l->spot_umbra_angle, l->spot_penumbra_angle);
	
	math::float4x4 projection_matrix = math::create_perspective_projection(max_angle, 1.0f, l->range / 1000.0f, l->range);
	
	math::float4x4 view_matrix		 = math::create_camera_at(l->position, l->position + l->direction, math::normalize(math::cross_product(l->direction, l->right)));
	
	u32 const spot_shadow_map_size	 = index_to_shadow_size(l->shadow_map_size_index);
	render_to_hw_shadowmap			 (shadow_quality, l->shadow_z_bias, spot_shadow_map_size, l->shadow_map_size_index, view_matrix, projection_matrix, 0);
}

void stage_lights::make_plane_spot_light_shadowmap( u32 shadow_quality, light* l )
{
	float const max_angle			 = math::max(l->spot_umbra_angle, l->spot_penumbra_angle);
	float const max_scale			 = math::max(l->scale.x, l->scale.z);
	
	float const inv_distance		 = max_scale / math::tan(max_angle / 2.0f);
	
	float3 const direction			 = -math::normalize(math::cross_product(l->direction, l->right));
	float3 const up				     = l->direction;
	float3 const new_position		 = l->position - inv_distance * direction;
	
	math::float4x4 projection_matrix = math::create_perspective_projection(max_angle, 1.0f, inv_distance, inv_distance + l->range);
	
	math::float4x4 view_matrix		 = math::create_camera_at(new_position, new_position + direction, up);
	
	u32 const spot_shadow_map_size	 = index_to_shadow_size(l->shadow_map_size_index);
	render_to_hw_shadowmap			 (shadow_quality, l->shadow_z_bias, spot_shadow_map_size, l->shadow_map_size_index, view_matrix, projection_matrix, 0);
}

void stage_lights::render_to_hw_shadowmap(
		u32 const shadow_quality,
		float const z_bias,
		u32 const smap_size,
		u32 const smap_size_index,
		xray::math::float4x4 const& view_matrix,
		xray::math::float4x4 const& projection_matrix,
		u32 const marge
	)
{
	XRAY_UNREFERENCED_PARAMETERS(shadow_quality, marge);
	
	BEGIN_CPUGPU_TIMER(statistics::ref().lights_stat_group.shadow_map_time);
	
	D3D_VIEWPORT orig_viewport;
	backend::ref().get_viewport( orig_viewport);
	
	backend::ref().set_render_targets		(0, 0, 0, 0);
	backend::ref().set_depth_stencil_target (&*m_shadow_depth_stencil[smap_size_index]);
	backend::ref().clear_depth_stencil		(D3D_CLEAR_DEPTH|D3D_CLEAR_STENCIL, 1.0f, 0);
	
	D3D_VIEWPORT tmp_viewport;
	
	tmp_viewport.TopLeftX	= 0.0f;//float(marge);
	tmp_viewport.TopLeftY	= 0.0f;//float(marge);
	tmp_viewport.Width		= float(smap_size);// - marge * 2);
	tmp_viewport.Height		= float(smap_size);// - marge * 2);
	tmp_viewport.MinDepth	= 0;
	tmp_viewport.MaxDepth	= 1.f;
	
	backend::ref().set_viewport( tmp_viewport);
	
	float3 view_position	= m_context->get_v_inverted().c.xyz();
	
	m_context->push_set_v( view_matrix);
	m_context->push_set_p( projection_matrix);
	
	m_context->scene()->select_models( m_context->get_culling_vp(), m_dynamic_visuals_to_shadow);
	
	render_surface_instances::iterator it_d			= m_dynamic_visuals_to_shadow.begin();
	render_surface_instances::const_iterator	end_d	= m_dynamic_visuals_to_shadow.end();
	
	for ( ; it_d != end_d; ++it_d)
	{
		render_surface_instance& instance = *(*it_d);
		material_effects& me		= instance.m_render_surface->get_material_effects();
		render_geometry& geometry	= instance.m_render_surface->m_render_geometry;
		
		if (!me.is_cast_shadow || !geometry.geom.c_ptr())
		{
			continue;
		}
		
		if (!me.stage_enable[shadow_render_stage] || !me.m_effects[shadow_render_stage])
		{
			m_shadow_effect->apply();
		}
		else
		{
			me.m_effects[shadow_render_stage]->apply();
		}
		
		m_context->set_w(*instance.m_transform);
		instance.set_constants		( );
		geometry.geom->apply();
		
		backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, geometry.primitive_count*3, 0, 0);
	}
	
	if (options::ref().m_enabled_draw_speedtree)
	{
		// SpeedTree
		speedtree_forest::tree_render_info_array_type visible_trees;
		m_context->scene()->get_speedtree_forest()->get_visible_tree_components(m_context, view_position, true, visible_trees);
		
		for (speedtree_forest::tree_render_info_array_type::iterator it=visible_trees.begin(); it!=visible_trees.end(); ++it)
		{
			if ( !it->tree_component->get_material_effects().stage_enable[shadow_render_stage] || 
				 !it->tree_component->get_material_effects().m_effects[shadow_render_stage] ||
				 !it->tree_component->get_material_effects().is_cast_shadow)
				continue;

			// TODO: fix
			//if (it->tree_component->get_geometry_type()==SpeedTree::GEOMETRY_TYPE_VERTICAL_BILLBOARDS)
			//	continue;		

			it->tree_component->get_material_effects().m_effects[shadow_render_stage]->apply();
			
			m_context->scene()->get_speedtree_forest()->get_speedtree_wind_parameters().set		(it->tree_component->m_parent->GetWind());
			m_context->scene()->get_speedtree_forest()->get_speedtree_common_parameters().set	(m_context, it->tree_component, view_position);
			if (it->instance)
				m_context->set_w																	(
					m_context->scene()->get_speedtree_forest()->get_instance_transform(*it->instance)
				);
			else
				m_context->set_w																	(math::float4x4().identity());

			
			if (it->tree_component->get_geometry_type()==SpeedTree::GEOMETRY_TYPE_VERTICAL_BILLBOARDS)
			{
				m_context->scene()->get_speedtree_forest()->get_speedtree_billboard_parameters().set(m_context, it->tree_component);
				it->tree_component->m_render_geometry.geom->apply();
				backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, it->tree_component->m_render_geometry.index_count, 0, 0);
			}
			else
			{
				m_context->scene()->get_speedtree_forest()->get_speedtree_tree_parameters().set(it->tree_component, it->instance, it->instance_lod);
				it->tree_component->m_render_geometry.geom->apply();
				backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, it->lod->num_indices, it->lod->start_index, 0);
				
				statistics::ref().visibility_stat_group.num_triangles.value += it->lod->num_indices / 3;
				statistics::ref().visibility_stat_group.num_speedtree_instances.value++;
			}
		}
	}
	
	float4x4 shadow_transform = m_context->get_vp();
	
	m_context->pop_v();
	m_context->pop_p();
	
	m_view_to_light_matrix		= shadow_transform;//mul4x3(m_context->get_v_inverted(), shadow_transform);
	m_shadow_z_bias				= 0.1f * z_bias;
	m_shadow_map_size			= float(smap_size);
	
	backend::ref().set_viewport( orig_viewport);
	
	END_CPUGPU_TIMER;
}

void stage_lights::render_to_cubemap( u32 shadow_quality, light* in_light )
{
	XRAY_UNREFERENCED_PARAMETER(shadow_quality);
	
//   	math::color clear_colors[] = {	
//   		math::color( 1.f, 0.f, 0.f, 1.f), math::color( 0.3f, 0.f, 0.f, 1.f),
//   		math::color( 0.f, 1.f, 0.f, 1.f), math::color( 0.f, 0.3f, 0.f, 1.f),
//   		math::color( 0.f, 0.f, 1.f, 1.f), math::color( 0.f, 0.f, 0.3f, 1.f),
//   	};
	
	D3D_VIEWPORT orig_viewport;
	backend::ref().get_viewport( orig_viewport);
	
	backend::ref().set_render_targets		(&*m_vcm_render_target, 0, 0, 0);
	backend::ref().set_depth_stencil_target (&*m_vcm_depth_stencil);
	backend::ref().clear_render_targets		(10000.f, 0.f, 0.f, 1.f);
	backend::ref().clear_depth_stencil		(D3D_CLEAR_DEPTH|D3D_CLEAR_STENCIL, 1.0f, 0);
			
	for (u32 face_index=0; face_index<6; face_index++)
	{
		//backend::ref().set_render_targets		(&*m_cubemap_face_render_target[face_index], 0, 0, 0);
		//backend::ref().set_depth_stencil_target (&*m_cubemap_face_depth_stencil[face_index]);
		//backend::ref().clear_depth_stencil		(D3D_CLEAR_DEPTH|D3D_CLEAR_STENCIL, 1.0f, 0);
		//backend::ref().clear_render_targets		(10000.0f, 0.0f, 0.0f, 1.0f);
		D3D_VIEWPORT tmp_viewport;
		
		tmp_viewport.TopLeftX	= float((face_index / 2) * 1024);
		tmp_viewport.TopLeftY	= float((face_index % 2) * 1024);
		tmp_viewport.Width		= 1024.0f;
		tmp_viewport.Height		= 1024.0f;
		tmp_viewport.MinDepth	= 0;
		tmp_viewport.MaxDepth	= 1.f;
		
		backend::ref().set_viewport( tmp_viewport);

		if (in_light->is_cast_shadows_in(light::shadow_distribution_side(face_index)))
			render_to_cubemap_face					(face_index, in_light->position, in_light->range);
		
	}
	
	backend::ref().set_viewport( orig_viewport);
}

stage_lights::~stage_lights()
{
}

void stage_lights::render_particle_lighting(xray::render::render_particle_emitter_instance* instance, light* l, u32 num_particles)
{
	float light_range				= l->range;
	float3 light_color				= l->color;
	float3 const light_position		= m_context->get_v().transform_position( l->position );
	float3 const light_direction	= m_context->get_v().transform_direction( l->direction );
	
	bool draw = false;
	
	switch ( l->get_type() ) {
		case light_type_point : {
			instance->get_material_effects().m_effects[lighting_render_stage]->apply( /*light_type_point*/ );
			backend::ref().set_ps_constant	( m_c_light_position,		light_position );
			backend::ref().set_ps_constant	( m_c_light_range,			light_range );
			backend::ref().set_ps_constant	( m_c_light_attenuation_power,	l->attenuation_power );

			draw = true;
			break;
		}
		case light_type_spot : {
			instance->get_material_effects().m_effects[lighting_render_stage]->apply( /*light_type_spot*/ );
			backend::ref().set_ps_constant	( m_c_light_position,		light_position );
			backend::ref().set_ps_constant	( m_c_light_direction,		light_direction );
			backend::ref().set_ps_constant	( m_c_light_range,			light_range );
			backend::ref().set_ps_constant	( m_c_light_attenuation_power,	l->attenuation_power );
	
			
			float const penumbra_half_angle_cosine	= math::cos( l->spot_penumbra_angle/2.f );
			backend::ref().set_ps_constant	( m_c_light_spot_penumbra_half_angle_cosine, penumbra_half_angle_cosine );
			backend::ref().set_ps_constant	( m_c_light_spot_inversed_umbra_half_angle_cosine_minus_penumbra_half_angle_cosine,
				1.f/( math::max(math::cos( l->spot_umbra_angle/2.f ) - penumbra_half_angle_cosine, 0.0001f) )
			);
			backend::ref().set_ps_constant	( m_c_light_spot_falloff, l->spot_falloff );
			draw = true;
			break;
		}
		case light_type_obb : {
			instance->get_material_effects().m_effects[lighting_render_stage]->apply( /*light_type_obb*/ );

			backend::ref().set_ps_constant	( m_c_light_position,		light_position );
			backend::ref().set_ps_constant	( m_c_light_range,			light_range );
			backend::ref().set_ps_constant	( m_c_light_attenuation_power,	l->attenuation_power );

			float4x4 obb_world				= l->m_xform;
			obb_world.set_scale				( l->scale );
			backend::ref().set_ps_constant	( m_c_light_local_to_world,	obb_world * m_context->get_v() );

			backend::ref().set_ps_constant	( m_c_light_color, light_color );
			backend::ref().set_ps_constant	( m_c_light_intensity, l->intensity );
			backend::ref().set_ps_constant	( m_c_lighting_model, l->lighting_model );

			float3 const* const eye_rays	= m_context->get_eye_rays();
			backend::ref().set_ps_constant	( m_c_eye_ray_corner,	((float4*)eye_rays)[0] );
			backend::ref().set_vs_constant	( m_c_near_far, m_context->get_near_far());
			draw = true;
			break;
		}
		case light_type_capsule : {
			instance->get_material_effects().m_effects[lighting_render_stage]->apply( /*light_type_capsule*/ );

			backend::ref().set_ps_constant	( m_c_light_position,		light_position );
			backend::ref().set_ps_constant	( m_c_light_direction,		light_direction );
			backend::ref().set_ps_constant	( m_c_light_range,			light_range );
			backend::ref().set_ps_constant	( m_c_light_attenuation_power,	l->attenuation_power );

			backend::ref().set_ps_constant	( m_c_light_capsule_half_width,	l->scale.z );
			backend::ref().set_ps_constant	( m_c_light_capsule_radius,	l->scale.x );

			backend::ref().set_ps_constant	( m_c_light_color, light_color );
			backend::ref().set_ps_constant	( m_c_light_intensity, l->intensity );
			backend::ref().set_ps_constant	( m_c_lighting_model, l->lighting_model );

			float3 const* const eye_rays	= m_context->get_eye_rays();
			backend::ref().set_ps_constant	( m_c_eye_ray_corner,	((float4*)eye_rays)[0] );
			backend::ref().set_vs_constant	( m_c_near_far, m_context->get_near_far());
			draw = true;
			break;
		}
		case light_type_parallel : {
			if (instance->get_material_effects().m_effects[lighting_render_stage])
			{
				instance->get_material_effects().m_effects[lighting_render_stage]->apply( /*light_type_parallel*/ );
				backend::ref().set_ps_constant	( m_c_light_direction, light_direction );
				draw = true;
			}
			break;
		}
		case render::light_type_sphere : {
			instance->get_material_effects().m_effects[lighting_render_stage]->apply( /*light_type_sphere*/ );
			backend::ref().set_ps_constant	( m_c_light_position,		light_position );
			backend::ref().set_ps_constant	( m_c_light_range,			light_range );
			backend::ref().set_ps_constant	( m_c_light_attenuation_power,	l->attenuation_power );

			backend::ref().set_ps_constant	( m_c_light_sphere_radius,	l->scale.x );

			backend::ref().set_ps_constant	( m_c_light_color, light_color );
			backend::ref().set_ps_constant	( m_c_light_intensity, l->intensity );
			backend::ref().set_ps_constant	( m_c_lighting_model, l->lighting_model );

			float3 const* const eye_rays	= m_context->get_eye_rays();
			backend::ref().set_ps_constant	( m_c_eye_ray_corner,	((float4*)eye_rays)[0] );
			backend::ref().set_vs_constant	( m_c_near_far, m_context->get_near_far());
			draw = true;
			break;
		}
		case render::light_type_plane_spot : {
			instance->get_material_effects().m_effects[lighting_render_stage]->apply( /*light_type_plane_spot*/ );
			backend::ref().set_ps_constant	( m_c_light_position,		light_position );
			backend::ref().set_ps_constant	( m_c_light_direction,		light_direction );
			backend::ref().set_ps_constant	( m_c_light_range,			light_range/math::sin(l->spot_penumbra_angle/2.f) );
			backend::ref().set_ps_constant	( m_c_light_attenuation_power,	l->attenuation_power );

			float const penumbra_half_angle_cosine	= math::cos( l->spot_penumbra_angle/2.f );
			backend::ref().set_ps_constant	( m_c_light_spot_penumbra_half_angle_cosine, penumbra_half_angle_cosine );
			float const umbra_half_angle_cosine	= math::cos( l->spot_umbra_angle/2.f );
			backend::ref().set_ps_constant	( m_c_light_spot_umbra_half_angle_cosine, umbra_half_angle_cosine );
			backend::ref().set_ps_constant	( m_c_light_spot_inversed_umbra_half_angle_cosine_minus_penumbra_half_angle_cosine,
				1.f/( math::max(umbra_half_angle_cosine - penumbra_half_angle_cosine, 0.0001f) )
			);
			backend::ref().set_ps_constant	( m_c_light_spot_falloff, l->spot_falloff );

			// plane spot light specific
			backend::ref().set_ps_constant	( m_c_light_local_to_world,	l->m_plane_spot_xform * m_context->get_v() );

			backend::ref().set_ps_constant	( m_c_light_color, light_color );
			backend::ref().set_ps_constant	( m_c_light_intensity, l->intensity );
			backend::ref().set_ps_constant	( m_c_lighting_model, l->lighting_model );

			float3 const* const eye_rays	= m_context->get_eye_rays();
			backend::ref().set_ps_constant	( m_c_eye_ray_corner,	((float4*)eye_rays)[0] );
			backend::ref().set_vs_constant	( m_c_near_far, m_context->get_near_far());
			draw = true;
			break;
		}
		default : NODEFAULT( );
	}
	
	if (draw)
	{
		backend::ref().set_ps_constant	( m_c_light_color, light_color );
		backend::ref().set_ps_constant	( m_c_light_intensity, l->intensity );
		backend::ref().set_ps_constant	( m_c_lighting_model, l->lighting_model );
		backend::ref().set_ps_constant	( m_c_light_type, s32(l->get_type()));
		backend::ref().set_ps_constant	( m_c_diffuse_influence_factor, l->diffuse_influence_factor );
		backend::ref().set_ps_constant	( m_c_specular_influence_factor, l->specular_influence_factor );
		
		particle_shader_constants::ref().set(
			m_context->get_v_inverted().transform_direction(float3(0,1000,0)).normalize(),
			m_context->get_v_inverted().transform_direction(float3(1000,0,0)).normalize(),
			m_context->get_v_inverted().lines[3].xyz(),
			instance->locked_axis(),
			instance->screen_alignment()
			);
		particle_shader_constants::ref().set_time(m_context->m_current_time);
		
		m_context->set_w( instance->transform() );
		
		instance->render(m_context->get_v_inverted().lines[3].xyz(), num_particles);	
	}
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
		out_weights[i] = blur_intencity * compute_gaussian_value( x, 0.0f, xray::math::max(blur_scale,0.25f));
	}
}

void stage_lights::fill_surface(xray::render::ref_rt surf)
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

void stage_lights::make_skin_scattering_texture(xray::render::render_surface_instance* instance, light* l)
{
	PIX_EVENT(render_skin);
	
/*
	if (identity(0))
	{
		backend::ref().set_render_targets		(&*m_rt_skin_scattering_position, 0, 0, 0);
		backend::ref().set_depth_stencil_target (0);
		backend::ref().clear_render_targets		(0.0f, 0.f, 0.f, 0.f);
		
		D3D_VIEWPORT							orig_viewport;
		D3D_VIEWPORT							tmp_viewport;
		
		backend::ref().get_viewport				(orig_viewport);
		
		tmp_viewport.TopLeftX					= 0.0f;
		tmp_viewport.TopLeftY					= 0.0f;
		tmp_viewport.Width						= options::ref().m_organic_irradiance_texture_size;
		tmp_viewport.Height						= options::ref().m_organic_irradiance_texture_size;
		tmp_viewport.MinDepth					= 0;
		tmp_viewport.MaxDepth					= 1.f;
		
		backend::ref().set_viewport				(tmp_viewport);
		
		render_geometry& geometry				= instance->m_render_surface->m_render_geometry;
		
		
		instance->m_render_surface->get_material_effects().m_effects[lighting_render_stage]->apply(organic_lighting_position_pass);
		geometry.geom->apply();
		m_context->set_w						(*instance->m_transform);
		backend::ref().render_indexed			(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, instance->m_render_surface->m_render_geometry.primitive_count*3, 0, 0);

		backend::ref().set_viewport( orig_viewport);
	}
*/	
	
	math::float4 clear_color				= float4(-1.0f, 0.0f, 0.0f, 0.0f);//float4(m_context->scene_view()->post_process_parameters().environment_ambient_color, 0.0f) + instance->m_render_surface->get_material_effects().organic_clear_color;
	backend::ref().set_render_targets		(&*m_rt_skin_scattering_temp, &*m_rt_skin_scattering_stretch, 0, 0);
	backend::ref().set_depth_stencil_target (0);
	backend::ref().clear_render_targets		(clear_color.x, clear_color.y, clear_color.z, 0.0f);
	
	D3D_VIEWPORT							orig_viewport;
	D3D_VIEWPORT							tmp_viewport;
	
	backend::ref().get_viewport				(orig_viewport);
	
	tmp_viewport.TopLeftX					= 0.0f;
	tmp_viewport.TopLeftY					= 0.0f;
	tmp_viewport.Width						= float(options::ref().m_organic_irradiance_texture_size);
	tmp_viewport.Height						= float(options::ref().m_organic_irradiance_texture_size);
	tmp_viewport.MinDepth					= 0;
	tmp_viewport.MaxDepth					= 1.f;
	
	backend::ref().set_viewport				(tmp_viewport);
	
	float light_range						= l->range;
	float3 light_color						= l->color;
	float3 const light_position				= m_context->get_v().transform_position( l->position );
	float3 const light_direction			= m_context->get_v().transform_direction( l->direction );
	
	render_geometry& geometry				= instance->m_render_surface->m_render_geometry;
	
	bool draw								= false;
	
	switch ( l->get_type() ) {
		case light_type_point : {
			instance->m_render_surface->get_material_effects().m_effects[lighting_render_stage]->apply(organic_lighting_lighting_pass);
			//backend::ref().set_ps_texture("shadowmap_texture", &*m_shadow_depth_stencil_texture[l->shadow_map_size_index]);
			backend::ref().set_ps_constant	(m_c_light_position, light_position);
			backend::ref().set_ps_constant	(m_c_light_range, light_range );
			backend::ref().set_ps_constant	(m_c_light_attenuation_power, l->attenuation_power);
			backend::ref().set_ps_constant	(m_c_is_shadower, 0.0f);
			
 			if (l->is_cast_shadows() && options::ref().m_enabled_local_light_shadows)
 			{
 				backend::ref().set_ps_constant(m_c_shadow_transparency, l->shadow_transparency);
 				backend::ref().set_ps_constant(m_c_view_to_light_matrix, transpose(m_view_to_light_matrix));
 				backend::ref().set_ps_constant(m_c_shadow_z_bias, m_shadow_z_bias);
 				backend::ref().set_ps_constant(m_c_shadow_map_size, m_shadow_map_size);
 				backend::ref().set_ps_constant(m_c_use_shadows, u32(1));
 				backend::ref().set_ps_texture ("shadowmap_texture", &*m_shadow_depth_stencil_texture[l->shadow_map_size_index]);
 			}
 			else
			{
				backend::ref().set_ps_constant(m_c_use_shadows, u32(0));
			}
			draw = true;
			break;
		}
		case light_type_spot : {
			instance->m_render_surface->get_material_effects().m_effects[lighting_render_stage]->apply(organic_lighting_lighting_pass);
			
			backend::ref().set_ps_constant	(m_c_is_shadower, 0.0f);
			
			backend::ref().set_ps_constant	(m_c_light_position, light_position);
			backend::ref().set_ps_constant	(m_c_light_direction, light_direction);
			backend::ref().set_ps_constant	(m_c_light_range, light_range/math::sin(l->spot_penumbra_angle/2.f));
			backend::ref().set_ps_constant	(m_c_light_attenuation_power, l->attenuation_power);
			
			backend::ref().set_ps_constant	(m_c_diffuse_influence_factor, l->diffuse_influence_factor);
			backend::ref().set_ps_constant	(m_c_specular_influence_factor, l->specular_influence_factor);
			
			float const penumbra_half_angle_cosine	= math::cos( l->spot_penumbra_angle/2.f);
			backend::ref().set_ps_constant	(m_c_light_spot_penumbra_half_angle_cosine, penumbra_half_angle_cosine);
			float const umbra_half_angle_cosine	= math::cos( l->spot_umbra_angle/2.f);
			backend::ref().set_ps_constant	(m_c_light_spot_umbra_half_angle_cosine, umbra_half_angle_cosine);
			backend::ref().set_ps_constant	(m_c_light_spot_inversed_umbra_half_angle_cosine_minus_penumbra_half_angle_cosine,
				1.f/( math::max(umbra_half_angle_cosine - penumbra_half_angle_cosine, 0.0001f))
				);
			backend::ref().set_ps_constant	(m_c_light_spot_falloff, l->spot_falloff);
			
			backend::ref().set_ps_constant	(m_c_light_color, light_color);
			backend::ref().set_ps_constant	(m_c_light_intensity, l->intensity);
			backend::ref().set_ps_constant	(m_c_lighting_model, l->lighting_model);
			
			float3 const* const eye_rays	= m_context->get_eye_rays();
			backend::ref().set_ps_constant	(m_c_eye_ray_corner,	((float4*)eye_rays)[0]);
			//backend::ref().set_vs_constant(m_c_near_far, m_context->get_near_far());
			
			if (l->is_cast_shadows() && options::ref().m_enabled_local_light_shadows)
			{
				backend::ref().set_ps_constant(m_c_shadow_transparency, l->shadow_transparency);
				backend::ref().set_ps_constant(m_c_view_to_light_matrix, transpose(m_view_to_light_matrix));
				backend::ref().set_ps_constant(m_c_shadow_z_bias, m_shadow_z_bias);
				backend::ref().set_ps_constant(m_c_shadow_map_size, m_shadow_map_size);
				backend::ref().set_ps_constant(m_c_use_shadows, u32(1));
				backend::ref().set_ps_texture ("shadowmap_texture", &*m_shadow_depth_stencil_texture[l->shadow_map_size_index]);
			}
			else
			{
				backend::ref().set_ps_constant(m_c_use_shadows, u32(0));
			}
			draw = true;
			break;
		}
		case light_type_obb : {
			NOT_IMPLEMENTED();
			break;
		}
		case light_type_capsule : {
			NOT_IMPLEMENTED();
			break;
		}
		case light_type_parallel : {
			//if (instance->get_material_effects().stage_enable[lighting_render_stage])
			//{
			//	instance->get_material_effects().m_effects[lighting_render_stage]->apply( light_type_parallel );
			//	backend::ref().set_ps_constant	( m_c_light_direction, light_direction );
			//	draw = true;
			//}
			instance->m_render_surface->get_material_effects().m_effects[lighting_render_stage]->apply(organic_lighting_lighting_pass);
			backend::ref().set_ps_constant	( m_c_light_direction, light_direction );
			backend::ref().set_ps_constant(m_c_use_shadows, u32(0));
			backend::ref().set_ps_constant	(m_c_is_shadower, 0.0f);
			draw = true;
			break;
		}
		case render::light_type_sphere : {
			NOT_IMPLEMENTED();
			break;
		}
		case render::light_type_plane_spot : {
			NOT_IMPLEMENTED();
			break;
		}
		default : NODEFAULT( );
	}
	
	if (draw)
	{
		backend::ref().set_ps_constant		(m_far_fog_color_and_distance, float4(m_context->scene_view()->post_process_parameters().environment_far_fog_color, m_context->scene_view()->post_process_parameters().environment_far_fog_distance));
		backend::ref().set_ps_constant		(m_near_fog_distance, m_context->scene_view()->post_process_parameters().environment_near_fog_distance);
		backend::ref().set_ps_constant		(m_c_light_color, light_color);
		backend::ref().set_ps_constant		(m_c_light_intensity, l->intensity);
		backend::ref().set_ps_constant		(m_c_lighting_model, l->lighting_model);
		backend::ref().set_ps_constant		(m_c_light_type, s32(l->get_type()));
		backend::ref().set_ps_constant		(m_c_diffuse_influence_factor, l->diffuse_influence_factor);
		backend::ref().set_ps_constant		(m_c_specular_influence_factor, l->specular_influence_factor);
		
		backend::ref().set_ps_constant(
			m_ambient_color, 
			float4(
				m_context->scene_view()->post_process_parameters().environment_ambient_color,
				0
			)
		);
		
		geometry.geom->apply();
		m_context->set_w					(*instance->m_transform);
		backend::ref().render_indexed		(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, instance->m_render_surface->m_render_geometry.primitive_count*3, 0, 0);
		//fill_surface						(m_rt_skin_scattering);
		//bool is_save = true;
		//if (is_save)
		//	m_t_skin_scattering->save_as("f:/skin.dds");
		
		m_sh_fix_irradiance_texture->apply	();
		fill_surface						(m_rt_skin_scattering);
		
		float weights_h[9];
		float offsets_h[9];
		float weights_v[9];
		float offsets_v[9];
		
		float const blur_kernel				= 0.5f;
		float const blur_intencity			= 0.5f;
		
		float t_w							= float(options::ref().m_organic_irradiance_texture_size);
		float t_h							= float(options::ref().m_organic_irradiance_texture_size);
		
		get_gaussain_weights_offsets		(weights_h, offsets_h, u32(t_w), blur_kernel, blur_intencity, 9);
		get_gaussain_weights_offsets		(weights_v, offsets_v, u32(t_h), blur_kernel, blur_intencity, 9);
		
		xray::math::float4					kernel_offsets[8];
		xray::math::float4					blur_offsets_weights[9];
		
		for (u32 i = 0; i < 9; i++) 
			blur_offsets_weights[i]			= float4(offsets_h[i], weights_h[i], offsets_v[i], weights_v[i]);
		
		m_sh_downsample_skin_irradiance_texture->apply();
		backend::ref().set_ps_texture		("t_base", &*m_t_skin_scattering);
		backend::ref().set_ps_constant		(m_ambient_color, float4(m_context->scene_view()->post_process_parameters().environment_ambient_color, 0));
		fill_surface						(m_rt_skin_scattering_small);
		
		instance->m_render_surface->get_material_effects().m_effects[lighting_render_stage]->apply(organic_lighting_blurring_pass);
		backend::ref().set_ps_texture		("t_base", &*m_t_skin_scattering_small);
		backend::ref().set_ps_constant		(m_ambient_color, float4(m_context->scene_view()->post_process_parameters().environment_ambient_color, 0));
		backend::ref().set_ps_constant		(m_blur_offsets_weights, blur_offsets_weights);
		fill_surface						(m_rt_skin_scattering_blurred_0);
		
		backend::ref().flush_rt_shader_resources();
		instance->m_render_surface->get_material_effects().m_effects[lighting_render_stage]->apply(organic_lighting_blurring_pass);
		backend::ref().set_ps_texture		("t_base", &*m_t_skin_scattering_blurred_0);
		backend::ref().set_ps_constant		(m_ambient_color, float4(m_context->scene_view()->post_process_parameters().environment_ambient_color, 0));
		backend::ref().set_ps_constant		(m_blur_offsets_weights, blur_offsets_weights);
		fill_surface						(m_rt_skin_scattering_blurred_1);
		
		backend::ref().flush_rt_shader_resources();
		instance->m_render_surface->get_material_effects().m_effects[lighting_render_stage]->apply(organic_lighting_blurring_pass);
		backend::ref().set_ps_texture		("t_base", &*m_t_skin_scattering_blurred_1);
		backend::ref().set_ps_constant		(m_ambient_color, float4(m_context->scene_view()->post_process_parameters().environment_ambient_color, 0));
		backend::ref().set_ps_constant		(m_blur_offsets_weights, blur_offsets_weights);
		fill_surface						(m_rt_skin_scattering_blurred_2);
		
		backend::ref().flush_rt_shader_resources();
		instance->m_render_surface->get_material_effects().m_effects[lighting_render_stage]->apply(organic_lighting_blurring_pass);
		backend::ref().set_ps_texture		("t_base", &*m_t_skin_scattering_blurred_2);
		backend::ref().set_ps_constant		(m_ambient_color, float4(m_context->scene_view()->post_process_parameters().environment_ambient_color, 0));
		backend::ref().set_ps_constant		(m_blur_offsets_weights, blur_offsets_weights);
		fill_surface						(m_rt_skin_scattering_blurred_3);
		
		backend::ref().flush_rt_shader_resources();
		instance->m_render_surface->get_material_effects().m_effects[lighting_render_stage]->apply(organic_lighting_blurring_pass);
		backend::ref().set_ps_texture		("t_base", &*m_t_skin_scattering_blurred_3);
		backend::ref().set_ps_constant		(m_blur_offsets_weights, blur_offsets_weights);
		fill_surface						(m_rt_skin_scattering_blurred_4);
		
		//backend::ref().flush_rt_shader_resources();
		
		// Set old. TODO: get_render_targets(), set_render_targets()
		backend::ref().set_render_targets( &*m_context->m_targets->m_rt_generic_0, 0, 0, 0); 
		backend::ref().reset_depth_stencil_target();
		backend::ref().set_viewport( orig_viewport);
		
		instance->m_render_surface->get_material_effects().m_effects[lighting_render_stage]->apply(organic_lighting_combine_pass);
		backend::ref().set_ps_constant	( m_c_light_position,		light_position );
		backend::ref().set_ps_constant	( m_c_light_range,			light_range );
		backend::ref().set_ps_constant	( m_c_light_color,			light_color );
		
		backend::ref().set_ps_constant(
			m_ambient_color, 
			float4(
				m_context->scene_view()->post_process_parameters().environment_ambient_color,
				0
			)
		);
		
		geometry.geom->apply();
		m_context->set_w(*instance->m_transform);
		backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, instance->m_render_surface->m_render_geometry.primitive_count*3, 0, 0);
	}
	
	// Set old. TODO: get_render_targets(), set_render_targets()
	backend::ref().set_render_targets( &*m_context->m_targets->m_rt_generic_0, 0, 0, 0);
	backend::ref().reset_depth_stencil_target();
	backend::ref().set_viewport( orig_viewport);
}

void stage_lights::render_model_lighting(xray::render::render_surface_instance* instance, light* l)
{
	float light_range				= l->range;
	
	XRAY_UNREFERENCED_PARAMETER(light_range);
	
	float3 light_color				= l->color;
	float3 const light_position		= m_context->get_v().transform_position( l->position );
	float3 const light_direction	= m_context->get_v().transform_direction( l->direction );

	material_effects& me			= instance->m_render_surface->get_material_effects();
	
	//bool draw = false;
	
	bool const is_organic				= me.is_organic;
	
	if (is_organic && l->get_type() == light_type_spot)
	{
		if (l->is_cast_shadows() && options::ref().m_enabled_local_light_shadows)
			make_spot_light_shadowmap(0, l);
		
		make_skin_scattering_texture	(instance, l);
		
		return;
	}
	if (is_organic && l->get_type() == light_type_point)
	{
		/*for (u32 face_index=0; face_index<6; face_index++)
		{
			bool const distribute_shadow = l->is_cast_shadows_in(light::shadow_distribution_side(face_index));

			math::float4x4 face_view_matrix = math::create_camera_at(
				l->position + view_matrix_parameters[face_index][0], 
				l->position + view_matrix_parameters[face_index][1], 
				view_matrix_parameters[face_index][2]
			);
			
			if (distribute_shadow)
			{
				math::float4x4 face_projection_matrix	= math::create_perspective_projection(math::pi_d2, 1.0f, s_shadow_z_near_value, light_range);

				u32 const spot_shadow_map_size			= index_to_shadow_size(l->shadow_map_size_index);//spot_shadow_map_size_option->value();
				u32 const marge							= 0; // TODO: get from point light filter settings , and pass to shader
				
				backend::ref().flush_rt_shader_resources();
				
				render_to_hw_shadowmap					(
					0,
					l->shadow_z_bias,
					spot_shadow_map_size,
					l->shadow_map_size_index,
					face_view_matrix,
					face_projection_matrix, marge
					);
			}
			
			make_skin_scattering_texture	(instance, l);
		}
		*/
		make_skin_scattering_texture	(instance, l);
		return;
	}
	if (is_organic && l->get_type() == light_type_parallel)
	{
		make_skin_scattering_texture	(instance, l);
		return;
	}
	
#if 0
	switch ( l->get_type() ) {
		case light_type_point : {
			me.m_effects[lighting_render_stage]->apply( /*light_type_point*/ );
			backend::ref().set_ps_constant	( m_c_light_position,		light_position );
			backend::ref().set_ps_constant	( m_c_light_range,			light_range );
			backend::ref().set_ps_constant	( m_c_light_attenuation_power,	l->attenuation_power );
			draw = true;
			
			break;
		}
		case light_type_spot : {
			me.m_effects[lighting_render_stage]->apply( /*light_type_spot*/ );
			backend::ref().set_ps_constant	( m_c_light_position,		light_position );
			backend::ref().set_ps_constant	( m_c_light_direction,		light_direction );
			backend::ref().set_ps_constant	( m_c_light_range,			light_range );
			backend::ref().set_ps_constant	( m_c_light_attenuation_power,	l->attenuation_power );
			
			float const penumbra_half_angle_cosine	= math::cos( l->spot_penumbra_angle/2.f );
			backend::ref().set_ps_constant	( m_c_light_spot_penumbra_half_angle_cosine, penumbra_half_angle_cosine );
			backend::ref().set_ps_constant	( m_c_light_spot_inversed_umbra_half_angle_cosine_minus_penumbra_half_angle_cosine,
				1.f/( math::max(math::cos( l->spot_umbra_angle/2.f ) - penumbra_half_angle_cosine, 0.0001f) )
			);
			backend::ref().set_ps_constant	( m_c_light_spot_falloff, l->spot_falloff );
			draw = true;
			break;
		}
		case light_type_obb : {
			me.m_effects[lighting_render_stage]->apply( /*light_type_obb*/ );
			
			backend::ref().set_ps_constant	( m_c_light_position,		light_position );
			backend::ref().set_ps_constant	( m_c_light_range,			light_range );
			backend::ref().set_ps_constant	( m_c_light_attenuation_power,	l->attenuation_power );

			float4x4 obb_world				= l->m_xform;
			obb_world.set_scale				( l->scale );
			backend::ref().set_ps_constant	( m_c_light_local_to_world,	obb_world * m_context->get_v() );

			backend::ref().set_ps_constant	( m_c_light_color, light_color );
			backend::ref().set_ps_constant	( m_c_light_intensity, l->intensity );
			backend::ref().set_ps_constant	( m_c_lighting_model, l->lighting_model );
			
			float3 const* const eye_rays	= m_context->get_eye_rays();
			backend::ref().set_ps_constant	( m_c_eye_ray_corner,	((float4*)eye_rays)[0] );
			backend::ref().set_vs_constant	( m_c_near_far, m_context->get_near_far());
			draw = true;
			break;
		}
		case light_type_capsule : {
			me.m_effects[lighting_render_stage]->apply( /*light_type_capsule*/ );
			
			backend::ref().set_ps_constant	( m_c_light_position,		light_position );
			backend::ref().set_ps_constant	( m_c_light_direction,		light_direction );
			backend::ref().set_ps_constant	( m_c_light_range,			light_range );
			backend::ref().set_ps_constant	( m_c_light_attenuation_power,	l->attenuation_power );

			backend::ref().set_ps_constant	( m_c_light_capsule_half_width,	l->scale.z );
			backend::ref().set_ps_constant	( m_c_light_capsule_radius,	l->scale.x );

			backend::ref().set_ps_constant	( m_c_light_color, light_color );
			backend::ref().set_ps_constant	( m_c_light_intensity, l->intensity );
			backend::ref().set_ps_constant	( m_c_lighting_model, l->lighting_model );

			float3 const* const eye_rays	= m_context->get_eye_rays();
			backend::ref().set_ps_constant	( m_c_eye_ray_corner,	((float4*)eye_rays)[0] );
			backend::ref().set_vs_constant	( m_c_near_far, m_context->get_near_far());
			draw = true;
			break;
		}
		case light_type_parallel : {
			if (me.m_effects[lighting_render_stage])
			{
				me.m_effects[lighting_render_stage]->apply( /*light_type_parallel*/ );
				backend::ref().set_ps_constant	( m_c_light_direction, light_direction );
				draw = true;
			}
			break;
		}
		case render::light_type_sphere : {
			me.m_effects[lighting_render_stage]->apply( /*light_type_sphere*/ );
			backend::ref().set_ps_constant	( m_c_light_position,		light_position );
			backend::ref().set_ps_constant	( m_c_light_range,			light_range );
			backend::ref().set_ps_constant	( m_c_light_attenuation_power,	l->attenuation_power );

			backend::ref().set_ps_constant	( m_c_light_sphere_radius,	l->scale.x );

			backend::ref().set_ps_constant	( m_c_light_color, light_color );
			backend::ref().set_ps_constant	( m_c_light_intensity, l->intensity );
			backend::ref().set_ps_constant	( m_c_lighting_model, l->lighting_model );

			float3 const* const eye_rays	= m_context->get_eye_rays();
			backend::ref().set_ps_constant	( m_c_eye_ray_corner,	((float4*)eye_rays)[0] );
			backend::ref().set_vs_constant	( m_c_near_far, m_context->get_near_far());
			draw = true;
			break;
		}
		case render::light_type_plane_spot : {
			me.m_effects[lighting_render_stage]->apply( /*light_type_plane_spot*/ );
			backend::ref().set_ps_constant	( m_c_light_position,		light_position );
			backend::ref().set_ps_constant	( m_c_light_direction,		light_direction );
			backend::ref().set_ps_constant	( m_c_light_range,			light_range/math::sin(l->spot_penumbra_angle/2.f) );
			backend::ref().set_ps_constant	( m_c_light_attenuation_power,	l->attenuation_power );

			float const penumbra_half_angle_cosine	= math::cos( l->spot_penumbra_angle/2.f );
			backend::ref().set_ps_constant	( m_c_light_spot_penumbra_half_angle_cosine, penumbra_half_angle_cosine );
			float const umbra_half_angle_cosine	= math::cos( l->spot_umbra_angle/2.f );
			backend::ref().set_ps_constant	( m_c_light_spot_umbra_half_angle_cosine, umbra_half_angle_cosine );
			backend::ref().set_ps_constant	( m_c_light_spot_inversed_umbra_half_angle_cosine_minus_penumbra_half_angle_cosine,
				1.f/( math::max(umbra_half_angle_cosine - penumbra_half_angle_cosine, 0.0001f) )
			);
			backend::ref().set_ps_constant	( m_c_light_spot_falloff, l->spot_falloff );

			// plane spot light specific
			backend::ref().set_ps_constant	( m_c_light_local_to_world,	l->m_plane_spot_xform * m_context->get_v() );

			backend::ref().set_ps_constant	( m_c_light_color, light_color );
			backend::ref().set_ps_constant	( m_c_light_intensity, l->intensity );
			backend::ref().set_ps_constant	( m_c_lighting_model, l->lighting_model );
			
			float3 const* const eye_rays	= m_context->get_eye_rays();
			backend::ref().set_ps_constant	( m_c_eye_ray_corner,	((float4*)eye_rays)[0] );
			backend::ref().set_vs_constant	( m_c_near_far, m_context->get_near_far());
			draw = true;
			break;
		}
		default : NODEFAULT( );
	}
	
	if (draw)
	{
		backend::ref().set_ps_constant	( m_far_fog_color_and_distance, float4(m_context->scene_view()->post_process_parameters().environment_far_fog_color, m_context->scene_view()->post_process_parameters().environment_far_fog_distance));
		backend::ref().set_ps_constant	( m_near_fog_distance, m_context->scene_view()->post_process_parameters().environment_near_fog_distance);	
		backend::ref().set_ps_constant	( m_c_light_color, light_color );
		backend::ref().set_ps_constant	( m_c_light_intensity, l->intensity );
		backend::ref().set_ps_constant	( m_c_lighting_model, l->lighting_model );
		backend::ref().set_ps_constant	( m_c_light_type, s32(l->get_type()));
		backend::ref().set_ps_constant	( m_c_diffuse_influence_factor, l->diffuse_influence_factor );
		backend::ref().set_ps_constant	( m_c_specular_influence_factor, l->specular_influence_factor );
		backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, instance->m_render_surface->m_render_geometry.primitive_count*3, 0, 0);
	}
#endif // #if 0
}

void stage_lights::render_speedtree_lighting (xray::render::lod_entry const* lod,
									 SpeedTree::CInstance const* instance,
									 SpeedTree::SInstanceLod const*	instance_lod,
									 xray::render::speedtree_tree_component* tree_component, 
									 light* l )
{
	float light_range				= l->range;
	float3 light_color				= l->color;
	float3 const light_position		= m_context->get_v().transform_position( l->position );
	float3 const light_direction	= m_context->get_v().transform_direction( l->direction );
	
	//if (tree_component->get_geometry_type()==SpeedTree::GEOMETRY_TYPE_VERTICAL_BILLBOARDS)
	//	return;


	bool draw = false;
		
	switch ( l->get_type() ) {
		case light_type_point : {
			tree_component->get_material_effects().m_effects[lighting_render_stage]->apply( /*light_type_point*/ );
			backend::ref().set_ps_constant	( m_c_light_position,		light_position );
			backend::ref().set_ps_constant	( m_c_light_range,			light_range );
			backend::ref().set_ps_constant	( m_c_light_attenuation_power,	l->attenuation_power );
			draw = true;
			break;
		}
		case light_type_spot : {
			tree_component->get_material_effects().m_effects[lighting_render_stage]->apply( /*light_type_spot*/ );
			backend::ref().set_ps_constant	( m_c_light_position,		light_position );
			backend::ref().set_ps_constant	( m_c_light_direction,		light_direction );
			backend::ref().set_ps_constant	( m_c_light_range,			light_range );
			backend::ref().set_ps_constant	( m_c_light_attenuation_power,	l->attenuation_power );

			float const penumbra_half_angle_cosine	= math::cos( l->spot_penumbra_angle/2.f );
			backend::ref().set_ps_constant	( m_c_light_spot_penumbra_half_angle_cosine, penumbra_half_angle_cosine );
			backend::ref().set_ps_constant	( m_c_light_spot_inversed_umbra_half_angle_cosine_minus_penumbra_half_angle_cosine,
				1.f/( math::max(math::cos( l->spot_umbra_angle/2.f ) - penumbra_half_angle_cosine, 0.0001f) )
			);
			backend::ref().set_ps_constant	( m_c_light_spot_falloff, l->spot_falloff );
			draw = true;
			break;
		}
		case light_type_obb : {
			tree_component->get_material_effects().m_effects[lighting_render_stage]->apply( /*light_type_obb*/ );
			
			backend::ref().set_ps_constant	( m_c_light_position,		light_position );
			backend::ref().set_ps_constant	( m_c_light_range,			light_range );
			backend::ref().set_ps_constant	( m_c_light_attenuation_power,	l->attenuation_power );

			float4x4 obb_world				= l->m_xform;
			obb_world.set_scale				( l->scale );
			backend::ref().set_ps_constant	( m_c_light_local_to_world,	obb_world * m_context->get_v() );

			backend::ref().set_ps_constant	( m_c_light_color, light_color );
			backend::ref().set_ps_constant	( m_c_light_intensity, l->intensity );
			backend::ref().set_ps_constant	( m_c_lighting_model, l->lighting_model );
			
			float3 const* const eye_rays	= m_context->get_eye_rays();
			backend::ref().set_ps_constant	( m_c_eye_ray_corner,	((float4*)eye_rays)[0] );
			backend::ref().set_vs_constant	( m_c_near_far, m_context->get_near_far());
			draw = true;
			break;
		}
		case light_type_capsule : {
			tree_component->get_material_effects().m_effects[lighting_render_stage]->apply( /*light_type_capsule*/ );

			backend::ref().set_ps_constant	( m_c_light_position,		light_position );
			backend::ref().set_ps_constant	( m_c_light_direction,		light_direction );
			backend::ref().set_ps_constant	( m_c_light_range,			light_range );
			backend::ref().set_ps_constant	( m_c_light_attenuation_power,	l->attenuation_power );

			backend::ref().set_ps_constant	( m_c_light_capsule_half_width,	l->scale.z );
			backend::ref().set_ps_constant	( m_c_light_capsule_radius,	l->scale.x );

			backend::ref().set_ps_constant	( m_c_light_color, light_color );
			backend::ref().set_ps_constant	( m_c_light_intensity, l->intensity );
			backend::ref().set_ps_constant	( m_c_lighting_model, l->lighting_model );

			float3 const* const eye_rays	= m_context->get_eye_rays();
			backend::ref().set_ps_constant	( m_c_eye_ray_corner,	((float4*)eye_rays)[0] );
			backend::ref().set_vs_constant	( m_c_near_far, m_context->get_near_far());
			draw = true;
			break;
		}
		case light_type_parallel : {
			if (tree_component->get_material_effects().m_effects[lighting_render_stage])
			{
				tree_component->get_material_effects().m_effects[lighting_render_stage]->apply( /*light_type_parallel*/ );
				backend::ref().set_ps_constant	( m_c_light_direction, light_direction );
				draw = true;
			}
			break;
		}
		case render::light_type_sphere : {
			tree_component->get_material_effects().m_effects[lighting_render_stage]->apply( /*light_type_sphere*/ );
			backend::ref().set_ps_constant	( m_c_light_position,		light_position );
			backend::ref().set_ps_constant	( m_c_light_range,			light_range );
			backend::ref().set_ps_constant	( m_c_light_attenuation_power,	l->attenuation_power );

			backend::ref().set_ps_constant	( m_c_light_sphere_radius,	l->scale.x );

			backend::ref().set_ps_constant	( m_c_light_color, light_color );
			backend::ref().set_ps_constant	( m_c_light_intensity, l->intensity );
			backend::ref().set_ps_constant	( m_c_lighting_model, l->lighting_model );

			float3 const* const eye_rays	= m_context->get_eye_rays();
			backend::ref().set_ps_constant	( m_c_eye_ray_corner,	((float4*)eye_rays)[0] );
			backend::ref().set_vs_constant	( m_c_near_far, m_context->get_near_far());
			draw = true;
			break;
		}
		case render::light_type_plane_spot : {
			tree_component->get_material_effects().m_effects[lighting_render_stage]->apply( /*light_type_plane_spot*/ );
			backend::ref().set_ps_constant	( m_c_light_position,		light_position );
			backend::ref().set_ps_constant	( m_c_light_direction,		light_direction );
			backend::ref().set_ps_constant	( m_c_light_range,			light_range/math::sin(l->spot_penumbra_angle/2.f) );
			backend::ref().set_ps_constant	( m_c_light_attenuation_power,	l->attenuation_power );

			float const penumbra_half_angle_cosine	= math::cos( l->spot_penumbra_angle/2.f );
			backend::ref().set_ps_constant	( m_c_light_spot_penumbra_half_angle_cosine, penumbra_half_angle_cosine );
			float const umbra_half_angle_cosine	= math::cos( l->spot_umbra_angle/2.f );
			backend::ref().set_ps_constant	( m_c_light_spot_umbra_half_angle_cosine, umbra_half_angle_cosine );
			backend::ref().set_ps_constant	( m_c_light_spot_inversed_umbra_half_angle_cosine_minus_penumbra_half_angle_cosine,
				1.f/( math::max(umbra_half_angle_cosine - penumbra_half_angle_cosine, 0.0001f) )
			);
			backend::ref().set_ps_constant	( m_c_light_spot_falloff, l->spot_falloff );

			// plane spot light specific
			backend::ref().set_ps_constant	( m_c_light_local_to_world,	l->m_plane_spot_xform * m_context->get_v() );

			backend::ref().set_ps_constant	( m_c_light_color, light_color );
			backend::ref().set_ps_constant	( m_c_light_intensity, l->intensity );
			backend::ref().set_ps_constant	( m_c_lighting_model, l->lighting_model );

			float3 const* const eye_rays	= m_context->get_eye_rays();
			backend::ref().set_ps_constant	( m_c_eye_ray_corner,	((float4*)eye_rays)[0] );
			backend::ref().set_vs_constant	( m_c_near_far, m_context->get_near_far());
			draw = true;
			break;
		}
		default : NODEFAULT( );
	}
	
	if (draw)
	{
		backend::ref().set_ps_constant	( m_far_fog_color_and_distance, float4(m_context->scene_view()->post_process_parameters().environment_far_fog_color, m_context->scene_view()->post_process_parameters().environment_far_fog_distance));
		backend::ref().set_ps_constant	( m_near_fog_distance, m_context->scene_view()->post_process_parameters().environment_near_fog_distance);	
		backend::ref().set_ps_constant	( m_c_light_color, light_color );
		backend::ref().set_ps_constant	( m_c_light_intensity, l->intensity );
		backend::ref().set_ps_constant	( m_c_lighting_model, l->lighting_model );
		backend::ref().set_ps_constant	( m_c_diffuse_influence_factor, l->diffuse_influence_factor );
		backend::ref().set_ps_constant	( m_c_specular_influence_factor, l->specular_influence_factor );

		m_context->scene()->get_speedtree_forest()->get_speedtree_wind_parameters().set		(tree_component->m_parent->GetWind());
		m_context->scene()->get_speedtree_forest()->get_speedtree_common_parameters().set	(m_context, tree_component, m_context->get_v_inverted().c.xyz());
		
		backend::ref().set_ps_constant	( m_c_light_type, s32(l->get_type()));
		
		if (instance)
			m_context->set_w																(
				m_context->scene()->get_speedtree_forest()->get_instance_transform(*instance)
			);
		else
			m_context->set_w																(math::float4x4().identity());
		
		if (tree_component->get_geometry_type()==SpeedTree::GEOMETRY_TYPE_VERTICAL_BILLBOARDS)
		{
			m_context->scene()->get_speedtree_forest()->get_speedtree_billboard_parameters().set(m_context, tree_component);
			tree_component->m_render_geometry.geom->apply();
			backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, tree_component->m_render_geometry.index_count, 0, 0);
		}
		else
		{
			m_context->scene()->get_speedtree_forest()->get_speedtree_tree_parameters().set(tree_component, instance, instance_lod);
			tree_component->m_render_geometry.geom->apply();
			backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, lod->num_indices, lod->start_index, 0);
		}
	}
}

void stage_lights::execute_disabled()
{
	backend::ref().set_render_targets			(&*m_context->m_targets->m_rt_accumulator_diffuse, &*m_context->m_targets->m_rt_accumulator_specular, 0, 0);
	backend::ref().reset_depth_stencil_target	();
	
	backend::ref().clear_render_targets			( 
		math::color( 1.0f, 1.0f, 1.0f, 1.0f),
		math::color( 0.0f, 0.0f, 0.0f, 0.0f),
		math::color( 0.0f, 0.0f, 0.0f, 0.0f),
		math::color( 0.0f, 0.0f, 0.0f, 0.0f)
	);
}

void stage_lights::execute()
{
	PIX_EVENT( stage_lights);
	
	if (!is_effects_ready())
		return;
	
	if (!is_enabled())
	{
		execute_disabled();
		return;
	}
	
	float4x4 v = m_context->get_v();
	float4x4 p = m_context->get_p();

	lights_db::lights_type const&	e_lights = m_context->scene()->lights().get_lights();
	
	if (!identity(m_is_forward_lighting_pass))
	{
		collision::objects_type objects		= g_allocator;
		objects.reserve						( e_lights.size() );
		
		math::frustum frustum				= m_context->get_culling_vp();
		m_context->scene()->lights().lights_tree().cuboid_query(u32(-1), frustum, objects );

		for ( collision::objects_type::const_iterator i = objects.begin(), e = objects.end(); i != e; ++i )
		{
			render_light						( static_cast<light*>( (*i)->user_data() ), false );
			statistics::ref().visibility_stat_group.num_lights.value++;
		}
		
// 		for ( collision::objects_type::const_iterator i = objects.begin(), e = objects.end(); i != e; ++i )
// 		{
// 			render_light						( static_cast<light*>( (*i)->user_data() ), true );
// 			statistics::ref().visibility_stat_group.num_lights.value++;
// 		}
		
		m_context->set_w					( float4x4().identity() );
		return;
	}
	
	BEGIN_CPUGPU_TIMER(statistics::ref().lights_stat_group.forward_lighting_time);
	
	backend::ref().set_render_targets( &*m_context->m_targets->m_rt_generic_0, 0, 0, 0);
	
	m_context->scene()->select_models( m_context->get_culling_vp(), m_dynamic_visuals);
	
	render_surface_instances::iterator it_d			= m_dynamic_visuals.begin();
	render_surface_instances::const_iterator	end_d	= m_dynamic_visuals.end();
	
	for ( ; it_d != end_d; ++it_d)
	{
		render_surface_instance& instance = *(*it_d);
		material_effects& me		= instance.m_render_surface->get_material_effects();
		render_geometry& geometry	= instance.m_render_surface->m_render_geometry;
		
		if ( !me.stage_enable[lighting_render_stage])
			continue;
		
		m_context->set_w(*instance.m_transform);
		geometry.geom->apply();
		
		light* L = m_context->scene()->lights().get_sun().c_ptr();
		
		if (L)
			render_model_lighting( &instance, L );
		
		for (lights_db::lights_type::const_iterator e_it=e_lights.begin() ; e_it!=e_lights.end(); ++e_it)
		{
			render_model_lighting( &instance, &*e_it->light);
		}
	}
	
	// SpeedTree
	if (options::ref().m_enabled_draw_speedtree)
	{
		speedtree_forest::tree_render_info_array_type visible_trees;
		
		m_context->scene()->get_speedtree_forest()->get_visible_tree_components(m_context, m_context->get_v_inverted().c.xyz(), true, visible_trees);
		
		for (speedtree_forest::tree_render_info_array_type::iterator it = visible_trees.begin(); it != visible_trees.end(); ++it)
		{
			m_context->set_w(xray::math::float4x4().identity());
			
			if (!it->tree_component->get_material_effects().stage_enable[lighting_render_stage] || !it->tree_component->get_material_effects().m_effects[lighting_render_stage])
				continue;
			
			light* L = m_context->scene()->lights().get_sun().c_ptr();
			
			if (L)
				render_speedtree_lighting(it->lod, it->instance, it->instance_lod, it->tree_component, L);
			
			for (lights_db::lights_type::const_iterator e_it=e_lights.begin() ; e_it!=e_lights.end(); ++e_it)
			{
				render_speedtree_lighting(it->lod, it->instance, it->instance_lod, it->tree_component, &*e_it->light);
			}
		}
	}

	// Particles
	xray::particle::world* part_world = m_context->scene()->particle_world();
	
	if (!part_world)
	{
		m_context->set_w					( float4x4().identity() );
		return;
	}
	
	particle::render_particle_emitter_instances_type emitters( g_allocator );
	
	part_world->get_render_emitter_instances(m_context->get_culling_vp(), emitters);
	
	for (particle::render_particle_emitter_instances_type::const_iterator it=emitters.begin(); it!=emitters.end(); ++it)
	{
		render::render_particle_emitter_instance* instance	= static_cast< render::render_particle_emitter_instance* >( *it );
		u32 const						  num_particles	= instance->get_num_particles();
		
		if (!num_particles)
			continue;
		
		xray::particle::enum_particle_render_mode particle_render_mode = m_context->scene_view()->get_particles_render_mode();
		
		light* L = m_context->scene()->lights().get_sun().c_ptr();
		
		if (L)
			render_particle_lighting(instance, L, num_particles);
		
		if (particle_render_mode==particle::normal_particle_render_mode && instance->get_material_effects().stage_enable[lighting_render_stage])
		{
			for (lights_db::lights_type::const_iterator e_it=e_lights.begin() ; e_it!=e_lights.end(); ++e_it)
			{
				render_particle_lighting(instance, &*e_it->light, num_particles);
			}
		}
	}
	
	END_CPUGPU_TIMER;
	
	m_context->set_w					( float4x4().identity() );

	backend::ref().reset_render_targets();
	backend::ref().reset_depth_stencil_target();
}


u32	stage_lights::index_to_shadow_size( u32 size_index ) const
{
	switch (size_index)
	{
		case 0:
			return 1024;
		case 1:
			return 512;
		case 2:
			return 256;
		default: NODEFAULT( return 1024 );
	}
}

void stage_lights::render_shadowed_light( light* l )
{
	float light_range								= l->range;
	float3 light_color								= l->color;
	float3 const light_position						= m_context->get_v().transform_position( l->position );
	

	ref_geometry geometry							= m_pyramid_geometry.geometry;
	
	for (u32 face_index=0; face_index<6; face_index++)
	{
		bool const distribute_shadow = l->is_cast_shadows_in(light::shadow_distribution_side(face_index));
		
		math::float4x4 face_view_matrix = math::create_camera_at(
			l->position + view_matrix_parameters[face_index][0], 
			l->position + view_matrix_parameters[face_index][1], 
			view_matrix_parameters[face_index][2]
		);
		
		float const range_X_tan_penumbra_angle_div_2= light_range * math::tan(math::pi_d4);
		float4x4	scale_matrix					= math::create_scale( float3( range_X_tan_penumbra_angle_div_2, range_X_tan_penumbra_angle_div_2, light_range ) );
		
		float4x4 local_to_world						= face_view_matrix;
		local_to_world.try_invert(local_to_world);
		local_to_world								= scale_matrix * local_to_world;
		
		if (distribute_shadow)
		{
			math::float4x4 face_projection_matrix	= math::create_perspective_projection(math::pi_d2, 1.0f, s_shadow_z_near_value, light_range);
			
			u32 const spot_shadow_map_size			= index_to_shadow_size(l->shadow_map_size_index);//spot_shadow_map_size_option->value();
			u32 const marge							= 0; // TODO: get from point light filter settings , and pass to shader
			
			backend::ref().flush_rt_shader_resources();
			
			render_to_hw_shadowmap					(
				0,
				l->shadow_z_bias,
				spot_shadow_map_size,
				l->shadow_map_size_index,
				face_view_matrix,
				face_projection_matrix, marge
			);
		}
		
		m_context->set_w							( local_to_world );
		
		BEGIN_CPUGPU_TIMER(statistics::ref().lights_stat_group.accumulate_lighting_time);
		
		backend::ref().set_render_targets			(&*m_context->m_targets->m_rt_accumulator_diffuse, &*m_context->m_targets->m_rt_accumulator_specular, 0, 0); 
		backend::ref().reset_depth_stencil_target	();
		
		// Mask opaque geometry.
		m_effect_accum_mask->apply					(effect_light_mask::tech_mask_local_light);
		geometry->apply								();
		backend::ref().render_indexed				( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 6*3, 0, 0);
		
		float3 const light_direction				= m_context->get_v().transform_direction( l->direction );
		
		switch ( l->get_type() ) {
			case light_type_point : {
				for (u32 tech_index=0; tech_index<2; tech_index++)
				{
					if (distribute_shadow)
					{
						m_shadowed_point_light_accumulator->apply( tech_index );
						backend::ref().set_ps_constant( m_c_shadow_transparency, l->shadow_transparency );
						backend::ref().set_ps_constant( m_c_view_to_light_matrix, transpose(m_view_to_light_matrix));
						backend::ref().set_ps_constant( m_c_shadow_z_bias, m_shadow_z_bias);
						backend::ref().set_ps_constant( m_c_shadow_map_size, m_shadow_map_size);
						backend::ref().set_ps_texture("shadowmap_texture", &*m_shadow_depth_stencil_texture[l->shadow_map_size_index]);
					}
					else
					{
						m_point_light_accumulator->apply( tech_index );
					}
					
					backend::ref().set_ps_constant	( m_c_light_position,		light_position );
					backend::ref().set_ps_constant	( m_c_light_range,			l->range );
					backend::ref().set_ps_constant	( m_c_light_attenuation_power,	l->attenuation_power );
					
					backend::ref().set_ps_constant	( m_c_diffuse_influence_factor, l->diffuse_influence_factor );
					backend::ref().set_ps_constant	( m_c_specular_influence_factor, l->specular_influence_factor );
					backend::ref().set_ps_constant	( m_c_is_shadower, 0.0f );
					
					backend::ref().set_ps_constant	( m_c_light_color, light_color );
					backend::ref().set_ps_constant	( m_c_light_intensity, l->intensity );
					backend::ref().set_ps_constant	( m_c_lighting_model, l->lighting_model );
					
					float3 const* const eye_rays	= m_context->get_eye_rays();
					backend::ref().set_ps_constant	( m_c_eye_ray_corner,	((float4*)eye_rays)[0] );
					backend::ref().set_vs_constant	( m_c_near_far, m_context->get_near_far());
					geometry->apply					( );
					backend::ref().render_indexed	( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 6*3, 0, 0);
				}
				break;
			}
			case light_type_obb : {
				for (u32 tech_index=0; tech_index<2; tech_index++)
				{
					if (distribute_shadow)
					{
						m_shadowed_obb_light_accumulator->apply( tech_index );
						backend::ref().set_ps_constant( m_c_shadow_transparency, l->shadow_transparency );
						backend::ref().set_ps_constant( m_c_view_to_light_matrix, transpose(m_view_to_light_matrix));
						backend::ref().set_ps_constant( m_c_shadow_z_bias, m_shadow_z_bias);
						backend::ref().set_ps_constant( m_c_shadow_map_size, m_shadow_map_size);
						backend::ref().set_ps_texture ("shadowmap_texture", &*m_shadow_depth_stencil_texture[l->shadow_map_size_index]);
					}
					else
					{
						m_obb_light_accumulator->apply( tech_index );
					}
					
					backend::ref().set_ps_constant	( m_c_light_position,		light_position );
					backend::ref().set_ps_constant	( m_c_light_range,			light_range );
					backend::ref().set_ps_constant	( m_c_light_attenuation_power,	l->attenuation_power );

					backend::ref().set_ps_constant	( m_c_diffuse_influence_factor, l->diffuse_influence_factor );
					backend::ref().set_ps_constant	( m_c_specular_influence_factor, l->specular_influence_factor );

					float4x4 obb_world				= l->m_xform;
					obb_world.set_scale				( l->scale );
					backend::ref().set_ps_constant	( m_c_light_local_to_world,	obb_world * m_context->get_v() );

					backend::ref().set_ps_constant	( m_c_light_color, light_color );
					backend::ref().set_ps_constant	( m_c_light_intensity, l->intensity );
					backend::ref().set_ps_constant	( m_c_lighting_model, l->lighting_model );
					
					float3 const* const eye_rays	= m_context->get_eye_rays();
					backend::ref().set_ps_constant	( m_c_eye_ray_corner,	((float4*)eye_rays)[0] );
					backend::ref().set_vs_constant	( m_c_near_far, m_context->get_near_far());
					geometry->apply					( );
					backend::ref().render_indexed	( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 6*3, 0, 0);
				}
				break;
			}
			case light_type_sphere : {
				for (u32 tech_index=0; tech_index<2; tech_index++)
				{
					if (distribute_shadow)
					{
						m_shadowed_sphere_light_accumulator->apply( tech_index );
						backend::ref().set_ps_constant( m_c_shadow_transparency, l->shadow_transparency );
						backend::ref().set_ps_constant( m_c_view_to_light_matrix, transpose(m_view_to_light_matrix));
						backend::ref().set_ps_constant( m_c_shadow_z_bias, m_shadow_z_bias);
						backend::ref().set_ps_constant( m_c_shadow_map_size, m_shadow_map_size);
						backend::ref().set_ps_texture("shadowmap_texture", &*m_shadow_depth_stencil_texture[l->shadow_map_size_index]);
					}
					else
					{
						m_sphere_light_accumulator->apply( tech_index );
					}
					backend::ref().set_ps_constant	( m_c_light_position,		light_position );
					backend::ref().set_ps_constant	( m_c_light_range,			light_range );
					backend::ref().set_ps_constant	( m_c_light_attenuation_power,	l->attenuation_power );
					
					backend::ref().set_ps_constant	( m_c_diffuse_influence_factor, l->diffuse_influence_factor );
					backend::ref().set_ps_constant	( m_c_specular_influence_factor, l->specular_influence_factor );
					
					backend::ref().set_ps_constant	( m_c_light_sphere_radius,	l->scale.x );
					
					backend::ref().set_ps_constant	( m_c_light_color, light_color );
					backend::ref().set_ps_constant	( m_c_light_intensity, l->intensity );
					backend::ref().set_ps_constant	( m_c_lighting_model, l->lighting_model );
					
					float3 const* const eye_rays	= m_context->get_eye_rays();
					backend::ref().set_ps_constant	( m_c_eye_ray_corner,	((float4*)eye_rays)[0] );
					backend::ref().set_vs_constant	( m_c_near_far, m_context->get_near_far());
					geometry->apply					( );
					backend::ref().render_indexed	( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 6*3, 0, 0);
				}
				break;
			}
			default : NODEFAULT( );
		}
		END_CPUGPU_TIMER;
		//backend::ref().flush_rt_shader_resources();
	}

}

void stage_lights::render_light( light* l, bool shadowers_pass)
{
	if (l->is_shadower && !shadowers_pass)
		return;
	
	if (!l->is_shadower && shadowers_pass)
		return;
	
	if (l->get_type()==light_type_parallel)
		return;
	
	if ((l->get_type()==light_type_point  || 
		 l->get_type()==light_type_sphere ||
		 l->get_type()==light_type_obb) && l->is_cast_shadows() && options::ref().m_enabled_local_light_shadows)
	{
		render_shadowed_light(l);
		return;
	}
	
	// Common
	float light_range				= l->range;
	float3 light_color				= l->color;
	float3 const light_position		= m_context->get_v().transform_position( l->position );
	float3 const light_direction	= m_context->get_v().transform_direction( l->direction );
	
	// Xforms
	l->xform_calc					( );
	m_context->set_w				( l->m_xform );
	
	ref_geometry geometry;
	switch ( l->get_type() ) {
		case light_type_point : {
			geometry				= m_sphere_geometry.geometry;
			break;
		}
		case light_type_spot : {
			geometry				= m_pyramid_geometry.geometry;
			break;
		}
		case light_type_obb : {
			geometry				= m_obb_geometry.geometry;
			break;
		}
		case light_type_capsule : {
			geometry				= m_obb_geometry.geometry;
			break;
		}
		case light_type_parallel : break;
		case render::light_type_sphere : {
			geometry				= m_sphere_geometry.geometry;
			break;
		}
		case render::light_type_plane_spot : {
			geometry				= m_obb_geometry.geometry;
			break;
		}
		default : NODEFAULT( );
	}

	switch ( l->get_type() ) {
		case light_type_spot : {
			if (l->is_cast_shadows() && options::ref().m_enabled_local_light_shadows)
				make_spot_light_shadowmap(0, l);
			break;
		}
		case light_type_plane_spot : {
			if (l->is_cast_shadows() && options::ref().m_enabled_local_light_shadows)
				make_plane_spot_light_shadowmap(0, l);
			break;
		}
	}
	
	m_context->set_w				( l->m_xform );
	
	BEGIN_CPUGPU_TIMER(statistics::ref().lights_stat_group.accumulate_lighting_time);
	
	backend::ref().set_render_targets	( &*m_context->m_targets->m_rt_accumulator_diffuse, &*m_context->m_targets->m_rt_accumulator_specular, 0, 0); 
	backend::ref().reset_depth_stencil_target();
	
	// Mask opaque geometry.
	m_effect_accum_mask->apply	(effect_light_mask::tech_mask_local_light);
	geometry->apply				();
	draw_geometry				(l);
	
	switch ( l->get_type() ) {
		case light_type_point : {
			for (u32 tech_index=0; tech_index<2; tech_index++)
			{
				if (l->is_shadower)
					m_point_light_shadower->apply( tech_index );
				else
					m_point_light_accumulator->apply( tech_index );
				
				backend::ref().set_ps_constant	( m_c_light_position,		light_position );
				backend::ref().set_ps_constant	( m_c_light_range,			light_range );
				backend::ref().set_ps_constant	( m_c_light_attenuation_power,	l->attenuation_power );
				
				backend::ref().set_ps_constant	( m_c_diffuse_influence_factor, l->diffuse_influence_factor );
				backend::ref().set_ps_constant	( m_c_specular_influence_factor, l->specular_influence_factor );
				
				backend::ref().set_ps_constant	( m_c_is_shadower, l->is_shadower ? 1.0f : 0.0f );
				
				backend::ref().set_ps_constant	( m_c_light_color, light_color );
				backend::ref().set_ps_constant	( m_c_light_intensity, l->intensity );
				backend::ref().set_ps_constant	( m_c_lighting_model, l->lighting_model );
				float3 const* const eye_rays	= m_context->get_eye_rays();
				backend::ref().set_ps_constant	( m_c_eye_ray_corner,	((float4*)eye_rays)[0] );
				backend::ref().set_vs_constant	( m_c_near_far, m_context->get_near_far());
				geometry->apply					( );
				draw_geometry					( l );
			}
			break;
		}
		case light_type_spot : {
			for (u32 tech_index=0; tech_index<2; tech_index++)
			{
				if (l->is_cast_shadows() && options::ref().m_enabled_local_light_shadows)
				{
					m_shadowed_spot_light_accumulator->apply( tech_index );
					backend::ref().set_ps_constant( m_c_shadow_transparency, l->shadow_transparency );
					backend::ref().set_ps_constant( m_c_view_to_light_matrix, transpose(m_view_to_light_matrix));
					backend::ref().set_ps_constant( m_c_shadow_z_bias, m_shadow_z_bias);
					backend::ref().set_ps_constant( m_c_shadow_map_size, m_shadow_map_size);
					backend::ref().set_ps_texture("shadowmap_texture", &*m_shadow_depth_stencil_texture[l->shadow_map_size_index]);
				}
				else
					m_spot_light_accumulator->apply	( tech_index );
				
				backend::ref().set_ps_constant	( m_c_light_position,		light_position );
				backend::ref().set_ps_constant	( m_c_light_direction,		light_direction );
				backend::ref().set_ps_constant	( m_c_light_range,			light_range/math::sin(l->spot_penumbra_angle/2.f) );
				backend::ref().set_ps_constant	( m_c_light_attenuation_power,	l->attenuation_power );
				
				backend::ref().set_ps_constant	( m_c_diffuse_influence_factor, l->diffuse_influence_factor );
				backend::ref().set_ps_constant	( m_c_specular_influence_factor, l->specular_influence_factor );
				backend::ref().set_ps_constant	( m_c_is_shadower, 0.0f );

				float const penumbra_half_angle_cosine	= math::cos( l->spot_penumbra_angle/2.f );
				backend::ref().set_ps_constant	( m_c_light_spot_penumbra_half_angle_cosine, penumbra_half_angle_cosine );
				float const umbra_half_angle_cosine	= math::cos( l->spot_umbra_angle/2.f );
				backend::ref().set_ps_constant	( m_c_light_spot_umbra_half_angle_cosine, umbra_half_angle_cosine );
				backend::ref().set_ps_constant	( m_c_light_spot_inversed_umbra_half_angle_cosine_minus_penumbra_half_angle_cosine,
					1.f/( math::max(umbra_half_angle_cosine - penumbra_half_angle_cosine, 0.0001f) )
				);
				backend::ref().set_ps_constant	( m_c_light_spot_falloff, l->spot_falloff );

				backend::ref().set_ps_constant	( m_c_light_color, light_color );
				backend::ref().set_ps_constant	( m_c_light_intensity, l->intensity );
				backend::ref().set_ps_constant	( m_c_lighting_model, l->lighting_model );
				
				float3 const* const eye_rays	= m_context->get_eye_rays();
				backend::ref().set_ps_constant	( m_c_eye_ray_corner,	((float4*)eye_rays)[0] );
				backend::ref().set_vs_constant	( m_c_near_far, m_context->get_near_far());
				geometry->apply					( );
				draw_geometry					( l );
			}
			
			break;
		}
		case light_type_obb : {
			for (u32 tech_index=0; tech_index<2; tech_index++)
			{
				m_obb_light_accumulator->apply	( tech_index );

				backend::ref().set_ps_constant	( m_c_light_position,		light_position );
				backend::ref().set_ps_constant	( m_c_light_range,			light_range );
				backend::ref().set_ps_constant	( m_c_light_attenuation_power,	l->attenuation_power );

				backend::ref().set_ps_constant	( m_c_diffuse_influence_factor, l->diffuse_influence_factor );
				backend::ref().set_ps_constant	( m_c_specular_influence_factor, l->specular_influence_factor );
				backend::ref().set_ps_constant	( m_c_is_shadower, 0.0f );

				float4x4 obb_world				= l->m_xform;
				obb_world.set_scale				( l->scale );
				backend::ref().set_ps_constant	( m_c_light_local_to_world,	obb_world * m_context->get_v() );

				backend::ref().set_ps_constant	( m_c_light_color, light_color );
				backend::ref().set_ps_constant	( m_c_light_intensity, l->intensity );
				backend::ref().set_ps_constant	( m_c_lighting_model, l->lighting_model );
				
				float3 const* const eye_rays	= m_context->get_eye_rays();
				backend::ref().set_ps_constant	( m_c_eye_ray_corner,	((float4*)eye_rays)[0] );
				backend::ref().set_vs_constant	( m_c_near_far, m_context->get_near_far());
				geometry->apply					( );
				draw_geometry					( l );
			}
			break;
		}
		case light_type_capsule : {
			for (u32 tech_index=0; tech_index<2; tech_index++)
			{
				m_capsule_light_accumulator->apply ( tech_index );

				backend::ref().set_ps_constant	( m_c_light_position,		light_position );
				backend::ref().set_ps_constant	( m_c_light_direction,		light_direction );
				backend::ref().set_ps_constant	( m_c_light_range,			light_range );
				backend::ref().set_ps_constant	( m_c_light_attenuation_power,	l->attenuation_power );

				backend::ref().set_ps_constant	( m_c_diffuse_influence_factor, l->diffuse_influence_factor );
				backend::ref().set_ps_constant	( m_c_specular_influence_factor, l->specular_influence_factor );
				backend::ref().set_ps_constant	( m_c_is_shadower, 0.0f );

				backend::ref().set_ps_constant	( m_c_light_capsule_half_width,	l->scale.z );
				backend::ref().set_ps_constant	( m_c_light_capsule_radius,	l->scale.x );

				backend::ref().set_ps_constant	( m_c_light_color, light_color );
				backend::ref().set_ps_constant	( m_c_light_intensity, l->intensity );
				backend::ref().set_ps_constant	( m_c_lighting_model, l->lighting_model );
				
				float3 const* const eye_rays	= m_context->get_eye_rays();
				backend::ref().set_ps_constant	( m_c_eye_ray_corner,	((float4*)eye_rays)[0] );
				backend::ref().set_vs_constant	( m_c_near_far, m_context->get_near_far());
				geometry->apply					( );
				draw_geometry					( l );
			}
			break;
		}
		case light_type_parallel : {
			NODEFAULT();
		}
		case render::light_type_sphere : {
			for (u32 tech_index=0; tech_index<2; tech_index++)
			{
				m_sphere_light_accumulator->apply( tech_index );

				backend::ref().set_ps_constant	( m_c_light_position,		light_position );
				backend::ref().set_ps_constant	( m_c_light_range,			light_range );
				backend::ref().set_ps_constant	( m_c_light_attenuation_power,	l->attenuation_power );

				backend::ref().set_ps_constant	( m_c_diffuse_influence_factor, l->diffuse_influence_factor );
				backend::ref().set_ps_constant	( m_c_specular_influence_factor, l->specular_influence_factor );
				backend::ref().set_ps_constant	( m_c_is_shadower, 0.0f );

				backend::ref().set_ps_constant	( m_c_light_sphere_radius,	l->scale.x );

				backend::ref().set_ps_constant	( m_c_light_color, light_color );
				backend::ref().set_ps_constant	( m_c_light_intensity, l->intensity );
				backend::ref().set_ps_constant	( m_c_lighting_model, l->lighting_model );
				
				float3 const* const eye_rays	= m_context->get_eye_rays();
				backend::ref().set_ps_constant	( m_c_eye_ray_corner,	((float4*)eye_rays)[0] );
				backend::ref().set_vs_constant	( m_c_near_far, m_context->get_near_far());
				geometry->apply					( );
				draw_geometry					( l );
			}
			break;
		}
		case render::light_type_plane_spot : {
			for (u32 tech_index=0; tech_index<2; tech_index++)
			{
				if (l->is_cast_shadows() && options::ref().m_enabled_local_light_shadows)
				{
					m_shadowed_plane_spot_light_accumulator->apply( tech_index );
					backend::ref().set_ps_constant( m_c_view_to_light_matrix, transpose( m_view_to_light_matrix ) );
					backend::ref().set_ps_constant( m_c_shadow_transparency, l->shadow_transparency );
					backend::ref().set_ps_constant( m_c_shadow_z_bias, m_shadow_z_bias);
					backend::ref().set_ps_constant( m_c_shadow_map_size, m_shadow_map_size);
					backend::ref().set_ps_texture("shadowmap_texture", &*m_shadow_depth_stencil_texture[l->shadow_map_size_index]);
				}
				else
					m_plane_spot_light_accumulator->apply	( tech_index );

				backend::ref().set_ps_constant	( m_c_light_position,		light_position );
				backend::ref().set_ps_constant	( m_c_light_direction,		light_direction );
				backend::ref().set_ps_constant	( m_c_light_range,			light_range/math::sin(l->spot_penumbra_angle/2.f) );
				backend::ref().set_ps_constant	( m_c_light_attenuation_power,	l->attenuation_power );

				backend::ref().set_ps_constant	( m_c_diffuse_influence_factor, l->diffuse_influence_factor );
				backend::ref().set_ps_constant	( m_c_specular_influence_factor, l->specular_influence_factor );
				backend::ref().set_ps_constant	( m_c_is_shadower, 0.0f );

				float const penumbra_half_angle_cosine	= math::cos( l->spot_penumbra_angle/2.f );
				backend::ref().set_ps_constant	( m_c_light_spot_penumbra_half_angle_cosine, penumbra_half_angle_cosine );
				float const umbra_half_angle_cosine	= math::cos( l->spot_umbra_angle/2.f );
				backend::ref().set_ps_constant	( m_c_light_spot_umbra_half_angle_cosine, umbra_half_angle_cosine );
				backend::ref().set_ps_constant	( m_c_light_spot_inversed_umbra_half_angle_cosine_minus_penumbra_half_angle_cosine,
					1.f/( math::max(umbra_half_angle_cosine - penumbra_half_angle_cosine, 0.0001f) )
				);
				backend::ref().set_ps_constant	( m_c_light_spot_falloff, l->spot_falloff );

				// plane spot light specific
				backend::ref().set_ps_constant	( m_c_light_local_to_world,	transpose( l->m_plane_spot_xform * m_context->get_v() ) );

				backend::ref().set_ps_constant	( m_c_light_color, light_color );
				backend::ref().set_ps_constant	( m_c_light_intensity, l->intensity );
				backend::ref().set_ps_constant	( m_c_lighting_model, l->lighting_model );
				
				float3 const* const eye_rays	= m_context->get_eye_rays();
				backend::ref().set_ps_constant	( m_c_eye_ray_corner,	((float4*)eye_rays)[0] );
				backend::ref().set_vs_constant	( m_c_near_far, m_context->get_near_far());
				geometry->apply					( );
				draw_geometry					( l );
			}
			break;
		}
		default : NODEFAULT( );
	}
	END_CPUGPU_TIMER;
	backend::ref().flush_rt_shader_resources();
}

void stage_lights::draw_geometry	( light* l )
{
	switch ( l->get_type() ) {
		case light_type_sphere :
		case light_type_point : {
			backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, DU_SPHERE_NUMFACES*3, 0, 0);
			break;
		}
		case light_type_spot : {
			backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 6*3, 0, 0);
			break;
		}
		case light_type_obb :
		case light_type_capsule :
		case light_type_plane_spot : {
			backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, geometry_utils::cube_solid::face_count*3, 0, 0);
			break;
		}
		default : NODEFAULT( );
	}
}

void stage_lights::debug_render	( )
{
	static bool s_debug_render = false;
	if ( !s_debug_render )
		return;

	math::color const color	= math::color(u32(0), 255, 32);
	//u16 indices[] = {
	//	0, 1, 2,
	//	0, 2, 4,
	//	0, 4, 3,
	//	0, 3, 1,
	//	1, 3, 2,
	//	2, 3, 4,
	//};

	backend::ref().set_render_targets	( &*m_context->m_targets->m_rt_present, 0, 0, 0); 

	typedef lights_db::lights_type	lights_type;
	lights_type const& lights	= m_context->scene()->lights().get_lights();
	for ( lights_type::const_iterator i = lights.begin(), e = lights.end(); i != e; ++i ) {
//		system_renderer::ref().draw_aabb	( (*i).light->m_aabb, math::color( u32(255), 0, 0 ) );
		system_renderer::ref().draw_obb		( (*i).light->m_collision_geometry->get_matrix(), math::color( u32(255), 0, 0 ) );
		if ( (*i).light->get_type() != light_type_plane_spot )
			continue;

		//float4x4 const& scale	= math::create_scale(
		//	float3(
		//		(*i).light->range*math::tan((*i).light->spot_penumbra_angle/2.f),
		//		(*i).light->range*math::tan((*i).light->spot_penumbra_angle/2.f),
		//		(*i).light->range
		//	)
		//);
		//float4x4 const& rotation = math::create_rotation(
		//	(*i).light->direction,
		//	(*i).light->right
		//);
		//float4x4 const& translation = math::create_translation( (*i).light->position );
		//float4x4 transform		= scale * rotation * translation;
		//vertex_colored vertices[]	= {
		//	vertex_colored( transform.transform_position( float3(0.f, 0.f, 0.f) ), color ),
		//	vertex_colored( transform.transform_position( float3(-1.f,-1.f,1.f) ), color ),
		//	vertex_colored( transform.transform_position( float3(-1.f, 1.f,1.f) ), color ),
		//	vertex_colored( transform.transform_position( float3( 1.f,-1.f,1.f) ), color ),
		//	vertex_colored( transform.transform_position( float3( 1.f, 1.f,1.f) ), color ),
		//};
		//system_renderer::ref().draw_triangles(
		//	&vertices[0],
		//	&vertices[0] + array_size(vertices),
		//	&indices[0],
		//	&indices[0] + array_size(indices)
		//);
		float4x4 transform		= (*i).light->m_xform;
		vertex_colored vertices[8];
		for ( u32 i = 0; i<array_size(vertices); ++i )
			vertices[i]			= vertex_colored(
				transform.transform_position(
					float3(
						geometry_utils::cube_solid::vertices[3*i + 0],
						geometry_utils::cube_solid::vertices[3*i + 1],
						geometry_utils::cube_solid::vertices[3*i + 2]
					)
				),
				color
			);

		system_renderer::ref().draw_triangles(
			vertices,
			&vertices[0] + array_size(vertices),
			geometry_utils::cube_solid::faces,
			geometry_utils::cube_solid::faces + geometry_utils::cube_solid::index_count
		);
	}
}
