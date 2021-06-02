////////////////////////////////////////////////////////////////////////////
//	Created		: 18.11.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "stage_shadow_direct.h"
#include <xray/render/core/effect_manager.h>
#include "effect_shadow_map.h"
#include <xray/render/core/backend.h>
#include <xray/render/core/pix_event_wrapper.h>
#include "light.h"
#include "renderer_context.h"
#include "scene.h"
#include "lights_db.h"
#include "shadow_cascade_volume.h"
#include "render_model.h"
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/res_effect.h>
#include <xray/render/core/effect_options_descriptor.h>
#include "system_renderer.h"

#include <xray/console_command.h>
#include <xray/console_command_processor.h>

#include <xray/render/core/options.h>
#include "engine_options.h"
#include "statistics.h"

#include "speedtree_forest.h"
#include "speedtree_tree.h"

#include "material.h"
#include "scene_view.h"

#include "terrain_render_model.h"
#include "terrain.h"

namespace xray {
namespace render {

static bool s_sun_moving_value = false;
static xray::console_commands::cc_bool s_sun_moving("sun_moving", 
															s_sun_moving_value, 
															false, 
															xray::console_commands::command_type_engine_internal);

static float s_sun_shadow_z_bias_value[4] = { 0.0001f, 0.0001f, 0.0001f, 0.0001f };
static xray::console_commands::cc_float sun_shadow_cascad_0_z_bias("sun_shadow_cascad_0_z_bias", s_sun_shadow_z_bias_value[0], 0.0f, 1.0f, true, xray::console_commands::command_type_engine_internal);
static xray::console_commands::cc_float sun_shadow_cascad_1_z_bias("sun_shadow_cascad_1_z_bias", s_sun_shadow_z_bias_value[1], 0.0f, 1.0f, true, xray::console_commands::command_type_engine_internal);
static xray::console_commands::cc_float sun_shadow_cascad_2_z_bias("sun_shadow_cascad_2_z_bias", s_sun_shadow_z_bias_value[2], 0.0f, 1.0f, true, xray::console_commands::command_type_engine_internal);
static xray::console_commands::cc_float sun_shadow_cascad_3_z_bias("sun_shadow_cascad_3_z_bias", s_sun_shadow_z_bias_value[3], 0.0f, 1.0f, true, xray::console_commands::command_type_engine_internal);

namespace
{
	//////////////////////////////////////////////////////////////////////////
	// tables to calculate view-frustum bounds in world space
	// note: D3D uses [0..1] range for Z
	static float3		corners [8]			= {
		float3( -1, -1,  0), float3( -1, -1, +1), 
		float3( -1, +1, +1), float3( -1, +1,  0), 
		float3( +1, +1, +1), float3( +1, +1,  0), 
		float3( +1, -1, +1), float3( +1, -1,  0)
	};

	static int			facetable[6][4]		= {
		{ 6, 7, 5, 4 },		{ 1, 0, 7, 6 },
		{ 1, 2, 3, 0 },		{ 3, 2, 4, 5 },		
		// near and far planes
		{ 0, 3, 5, 7 },		{  1, 6, 4, 2 },
	};
}

stage_shadow_direct::stage_shadow_direct( renderer_context* context): stage( context)
{
	m_old_shadow_map_size   = 0;
	
	effect_manager::ref().create_effect<effect_shadow_map>(&m_effect_shadow_direct);
	
	m_c_light_direction		= backend::ref().register_constant_host( "light_direction", rc_float );
	m_c_light_position		= backend::ref().register_constant_host( "light_position", rc_float );
//	m_sunmask				= backend::ref().register_constant_host( "sunmask");
	m_c_light_attenuation_power	= backend::ref().register_constant_host( "light_attenuation", rc_float );
	m_c_start_corner		= backend::ref().register_constant_host("start_corner", rc_float);
	m_enabled				= options::ref().m_enabled_sun_shadows_stage;
}

bool stage_shadow_direct::is_effects_ready() const
{
	return m_effect_shadow_direct.c_ptr() != NULL;
}

stage_shadow_direct::~stage_shadow_direct()
{
	
}

u32	stage_shadow_direct::index_to_shadow_size( u32 size_index ) const
{
	switch (size_index)
	{
		case 0:
			return 2048;
		case 1:
			return 1024;
		case 2:
			return 512;
		case 3:
			return 256;
		case 4:
			return 128;
		default: NODEFAULT( return 1024 );
	}
}

void stage_shadow_direct::execute_disabled()
{
	if (!is_effects_ready())
		return;

	light*	sun = &*m_context->scene()->lights().get_sun().c_ptr();
	if (!sun)
		return;
	
	u32 first_cascade = u32(sun_cascade::num_max_sun_shadow_cascades) - u32(sun->num_sun_cascades);
	u32 last_cascade  = u32(sun_cascade::num_max_sun_shadow_cascades);
	
	for (u32 cascade_id=first_cascade, cascade_index=0; cascade_id<last_cascade; cascade_id++, cascade_index++)
	{
		backend::ref().set_render_targets		( 0, 0, 0, 0);
		backend::ref().set_depth_stencil_target	( &*m_context->m_rt_shadow_map[cascade_index]);	
		backend::ref().clear_depth_stencil		( D3D_CLEAR_DEPTH|D3D_CLEAR_STENCIL, 1.0f, 0);		
	}
}

void stage_shadow_direct::execute()
{
	if (!is_effects_ready())
		return;
	
	if (!is_enabled())
	{
		execute_disabled();
		return;
	}
	
	light*	sun = &*m_context->scene()->lights().get_sun().c_ptr();
	if (!sun)
		return;
	
	static math::float3 const s_prev_sun_direction = sun->direction;
	
	if (s_sun_moving_value)
	{
		//float x = math::cos(m_context->get_current_time()) * 0.2f;
		//float z = math::sin(m_context->get_current_time()) * 0.2f;
		//sun->direction = float3(x, -1.0f, z);
		//sun->direction =  math::normalize(sun->direction);
	}
	else
	{
		//sun->direction = s_prev_sun_direction;
	}
	
	u32 request_shadow_map_size = index_to_shadow_size(sun->shadow_map_size_index);//math::floor(sun->sun_shadow_map_size);
	
	if (m_old_shadow_map_size!=request_shadow_map_size)
	{
		m_old_shadow_map_size = request_shadow_map_size;
		m_context->create_casceded_shadow_map_buffers(m_old_shadow_map_size);
	}
	
	u32 first_cascade = u32(sun_cascade::num_max_sun_shadow_cascades) - u32(sun->num_sun_cascades);
	u32 last_cascade  = u32(sun_cascade::num_max_sun_shadow_cascades);
	
	for (u32 cascade_id=first_cascade, cascade_index=0; cascade_id<last_cascade; cascade_id++, cascade_index++)
		execute_cascade(cascade_id, cascade_index, request_shadow_map_size);

	backend::ref().reset_render_targets();
	backend::ref().reset_depth_stencil_target();
}

#if 0
static void draw_line(float3 const& start, float3 const& end, xray::math::color clr)
{
	float3 line[2];
	line[0] = start;
	line[1] = end;
	system_renderer::ref().draw_screen_lines(line, 2, clr, 1, 0xffffffff, true, false);
}
#endif // #if 0

void	stage_shadow_direct::draw_debug(u32 in_cascade_id)
{
	(void)&in_cascade_id;
	
#if 0
	
	enum_sun_cascade cascade_id = (enum_sun_cascade)in_cascade_id;
	
	light*	sun = &*m_context->scene()->lights().get_sun().c_ptr();
	if (!sun)
		return;
	
	float const size						= m_context->m_sun_cascades[cascade_id].size;
	
	// TODO: global variable?
	float const sun_distance				= 300.f;
	
	float3 sun_position						=	m_context->get_view_pos() - sun->direction*sun_distance;
	float3 tmp_vec							=	m_context->get_view_pos() - sun_position;
	
	float4x4 light_view_transform			=	create_camera_direction( sun_position, sun->direction, float3( 1, 0, 0) /*this need to be north direction*/);
	float4x4 light_projection_transform		=	math::create_orthographic_projection( size, size, 0.1f, sun_distance +/*sqrt(2)*/1.41421f*size );
	
	shadow_cascade_volume::cascade_volume_planes cull_planes;
	float3	light_shift_xz( 0.f, 0.f, 0.f);
	
	shadow_cascade_volume	cascade_volume;
	
	//initialize shadow_cascade_volume
	//////////////////////////////////////////////////////////////////////////
	
	if ( cascade_id == 0  || m_context->m_sun_cascades[cascade_id].reset_chain )
		for( int i = 0; i < 4; i++)	
		{
			float3 eye_ray = m_context->get_eye_rays()[i];
			eye_ray.normalize();
			eye_ray = m_context->get_v_inverted().transform_direction(eye_ray);
			cascade_volume.view_frustum_rays.push_back	( ray( m_context->get_view_pos() + m_context->get_near()*eye_ray, eye_ray) );
		}
	else
		cascade_volume.view_frustum_rays	= m_context->m_sun_cascades[cascade_id].rays;
	
	cascade_volume.view_ray.origin			= m_context->get_view_pos();
	cascade_volume.view_ray.direction		= m_context->get_view_dir();
	cascade_volume.light_ray.origin			= sun_position;
	cascade_volume.light_ray.direction		= sun->direction;
	
	float4x4 light_full_transform = light_view_transform * light_projection_transform;
	
	float4x4 light_full_transform_invert = invert4x3( light_full_transform);
	
	for	( int p = 0; p < 8; p++)	{
		float4	xf	= light_full_transform_invert.transform( float4( corners[p], 1.f));
		cascade_volume.light_cuboid_points[p] = xf.xyz();
	}
	
	{
		static math::color colors_x[12] = 
		{
			math::color(1.0f, 0.0f, 0.0f, 1.f),
			math::color(0.0f, 1.0f, 0.0f, 1.f),
			math::color(0.0f, 0.0f, 1.0f, 1.f),
			math::color(1.0f, 0.0f, 1.0f, 1.f),
			math::color(0.5f, 0.0f, 0.0f, 1.f),
			math::color(0.0f, 0.5f, 0.0f, 1.f),
			math::color(0.0f, 0.0f, 0.5f, 1.f),
			math::color(0.5f, 0.0f, 0.5f, 1.f),
			math::color(0.5f, 0.5f, 0.5f, 1.f),
			math::color(0.5f, 0.5f, 0.0f, 1.f),
			math::color(1.0f, 1.0f, 0.0f, 1.f),
			math::color(1.0f, 1.0f, 1.0f, 1.f),
		};
		
		draw_line(cascade_volume.light_cuboid_points[0], cascade_volume.light_cuboid_points[1], colors_x[0]);
		draw_line(cascade_volume.light_cuboid_points[0], cascade_volume.light_cuboid_points[3], colors_x[1]);
		draw_line(cascade_volume.light_cuboid_points[0], cascade_volume.light_cuboid_points[7], colors_x[2]);
		draw_line(cascade_volume.light_cuboid_points[1], cascade_volume.light_cuboid_points[2], colors_x[3]);
		draw_line(cascade_volume.light_cuboid_points[1], cascade_volume.light_cuboid_points[6], colors_x[4]);
		draw_line(cascade_volume.light_cuboid_points[2], cascade_volume.light_cuboid_points[3], colors_x[5]);
		draw_line(cascade_volume.light_cuboid_points[2], cascade_volume.light_cuboid_points[4], colors_x[6]);
		draw_line(cascade_volume.light_cuboid_points[3], cascade_volume.light_cuboid_points[5], colors_x[7]);
		draw_line(cascade_volume.light_cuboid_points[4], cascade_volume.light_cuboid_points[5], colors_x[8]);
		draw_line(cascade_volume.light_cuboid_points[4], cascade_volume.light_cuboid_points[6], colors_x[9]);
		draw_line(cascade_volume.light_cuboid_points[5], cascade_volume.light_cuboid_points[7], colors_x[10]);
		draw_line(cascade_volume.light_cuboid_points[6], cascade_volume.light_cuboid_points[7], colors_x[11]);
	}

	// only side planes
	for (int plane=0; plane < 4; plane++)	
		for (int pt=0; pt < 4; pt++)	
		{
			int asd = facetable[plane][pt];
			cascade_volume.light_cuboid_polys[plane].points[pt] = asd;
		}
	
	cascade_volume.compute_caster_model_fixed( cull_planes, light_shift_xz, size, true, true, true, true);
	//light_shift_xz *= 0.0f;

	float3 adjastment		= compute_aligment( light_shift_xz, light_view_transform * light_projection_transform, 1024);

	for (u32 it=0; it< LIGHT_CUBOIDSIDEPOLYS_COUNT; it++)
	{
		shadow_cascade_volume::polygon&	poly	=	cascade_volume.light_cuboid_polys[it];
		float3 line[2];
		line[0] = float3(0.0f, 0.0f, 0.0f);
		line[1] = poly.plane.normal * poly.plane.d;
		static math::color colors_x[4] = 
		{
			math::color(1.0f, 0.0f, 0.0f, 1.f),
			math::color(0.0f, 1.0f, 0.0f, 1.f),
			math::color(0.0f, 0.0f, 1.0f, 1.f),
			math::color(1.0f, 0.0f, 1.0f, 1.f),
		};
		system_renderer::ref().draw_screen_lines(line, 2, colors_x[it], 1, 0xffffffff, true, false);
	}
	
	float3 line[2];
	line[0] = float3(0.0f, 0.0f, 0.0f);
	line[1] = cascade_volume.view_ray.direction * 3.0f;
	system_renderer::ref().draw_screen_lines(line, 2, math::color(1.0f, 1.0f, 1.0f, 1.0f), 1, 0xffffffff, true, false);
	
	{
		draw_line(light_shift_xz + cascade_volume.light_cuboid_points[0], light_shift_xz + cascade_volume.light_cuboid_points[1], xray::math::color(1.0f, 1.0f, 0.0f, 1.0f));
		draw_line(light_shift_xz + cascade_volume.light_cuboid_points[0], light_shift_xz + cascade_volume.light_cuboid_points[3], xray::math::color(1.0f, 1.0f, 0.0f, 1.0f));
		draw_line(light_shift_xz + cascade_volume.light_cuboid_points[0], light_shift_xz + cascade_volume.light_cuboid_points[7], xray::math::color(1.0f, 1.0f, 0.0f, 1.0f));
		draw_line(light_shift_xz + cascade_volume.light_cuboid_points[1], light_shift_xz + cascade_volume.light_cuboid_points[2], xray::math::color(1.0f, 1.0f, 0.0f, 1.0f));
		draw_line(light_shift_xz + cascade_volume.light_cuboid_points[1], light_shift_xz + cascade_volume.light_cuboid_points[6], xray::math::color(1.0f, 1.0f, 0.0f, 1.0f));
		draw_line(light_shift_xz + cascade_volume.light_cuboid_points[2], light_shift_xz + cascade_volume.light_cuboid_points[3], xray::math::color(1.0f, 1.0f, 0.0f, 1.0f));
		draw_line(light_shift_xz + cascade_volume.light_cuboid_points[2], light_shift_xz + cascade_volume.light_cuboid_points[4], xray::math::color(1.0f, 1.0f, 0.0f, 1.0f));
		draw_line(light_shift_xz + cascade_volume.light_cuboid_points[3], light_shift_xz + cascade_volume.light_cuboid_points[5], xray::math::color(1.0f, 1.0f, 0.0f, 1.0f));
		draw_line(light_shift_xz + cascade_volume.light_cuboid_points[4], light_shift_xz + cascade_volume.light_cuboid_points[5], xray::math::color(1.0f, 1.0f, 0.0f, 1.0f));
		draw_line(light_shift_xz + cascade_volume.light_cuboid_points[4], light_shift_xz + cascade_volume.light_cuboid_points[6], xray::math::color(1.0f, 1.0f, 0.0f, 1.0f));
		draw_line(light_shift_xz + cascade_volume.light_cuboid_points[5], light_shift_xz + cascade_volume.light_cuboid_points[7], xray::math::color(1.0f, 1.0f, 0.0f, 1.0f));
		draw_line(light_shift_xz + cascade_volume.light_cuboid_points[6], light_shift_xz + cascade_volume.light_cuboid_points[7], xray::math::color(1.0f, 1.0f, 0.0f, 1.0f));

		light_shift_xz += adjastment;
		draw_line(light_shift_xz + cascade_volume.light_cuboid_points[0], light_shift_xz + cascade_volume.light_cuboid_points[1], xray::math::color(1.0f, 0.0f, 0.0f, 1.0f));
		draw_line(light_shift_xz + cascade_volume.light_cuboid_points[0], light_shift_xz + cascade_volume.light_cuboid_points[3], xray::math::color(1.0f, 0.0f, 0.0f, 1.0f));
		draw_line(light_shift_xz + cascade_volume.light_cuboid_points[0], light_shift_xz + cascade_volume.light_cuboid_points[7], xray::math::color(1.0f, 0.0f, 0.0f, 1.0f));
		draw_line(light_shift_xz + cascade_volume.light_cuboid_points[1], light_shift_xz + cascade_volume.light_cuboid_points[2], xray::math::color(1.0f, 0.0f, 0.0f, 1.0f));
		draw_line(light_shift_xz + cascade_volume.light_cuboid_points[1], light_shift_xz + cascade_volume.light_cuboid_points[6], xray::math::color(1.0f, 0.0f, 0.0f, 1.0f));
		draw_line(light_shift_xz + cascade_volume.light_cuboid_points[2], light_shift_xz + cascade_volume.light_cuboid_points[3], xray::math::color(1.0f, 0.0f, 0.0f, 1.0f));
		draw_line(light_shift_xz + cascade_volume.light_cuboid_points[2], light_shift_xz + cascade_volume.light_cuboid_points[4], xray::math::color(1.0f, 0.0f, 0.0f, 1.0f));
		draw_line(light_shift_xz + cascade_volume.light_cuboid_points[3], light_shift_xz + cascade_volume.light_cuboid_points[5], xray::math::color(1.0f, 0.0f, 0.0f, 1.0f));
		draw_line(light_shift_xz + cascade_volume.light_cuboid_points[4], light_shift_xz + cascade_volume.light_cuboid_points[5], xray::math::color(1.0f, 0.0f, 0.0f, 1.0f));
		draw_line(light_shift_xz + cascade_volume.light_cuboid_points[4], light_shift_xz + cascade_volume.light_cuboid_points[6], xray::math::color(1.0f, 0.0f, 0.0f, 1.0f));
		draw_line(light_shift_xz + cascade_volume.light_cuboid_points[5], light_shift_xz + cascade_volume.light_cuboid_points[7], xray::math::color(1.0f, 0.0f, 0.0f, 1.0f));
		draw_line(light_shift_xz + cascade_volume.light_cuboid_points[6], light_shift_xz + cascade_volume.light_cuboid_points[7], xray::math::color(1.0f, 0.0f, 0.0f, 1.0f));
	}
#endif // #if 0
// 	light_view_transform	=	create_camera_direction(  sun_position  + light_shift_xz/* + adjastment*/, sun->direction, float3( 1, 0, 0) /*this need to be north direction*/);
// 	light_full_transform_invert = invert4x3( light_full_transform);
// 
// 	for	( int p = 0; p < 8; p++)	{
// 		float4	xf	= light_full_transform_invert.transform( float4( corners[p], 1.f));
// 		cascade_volume.light_cuboid_points[p] = xf.xyz();
// 		
// 		float3 line[2];
// 		line[0] = sun_position  + light_shift_xz;
// 		line[1] = xf.xyz();
// 		system_renderer::ref().draw_screen_lines(line, 2, math::color(1.0f, 0.0f, 0.0f, 1.0f), 1, 0xffffffff, true, false);
// 		//system_renderer::ref().draw_3D_point(xf.xyz(), 100, math::color(0.0f, 0.0f, 0.0f, 1.0f), true);
// 	}

	m_context->set_w					( float4x4().identity() );
}

void stage_shadow_direct::execute_cascade( u32 cascade_id, u32 cascade_index, u32 shadow_map_size)
{
	PIX_EVENT( stage_shadow_direct);
	
	BEGIN_CPUGPU_TIMER(
		cascade_index == 0 ? statistics::ref().cascaded_sun_shadow_stat_group.execute_time_cascade_1:
			(cascade_index == 1 ? statistics::ref().cascaded_sun_shadow_stat_group.execute_time_cascade_2:
				(cascade_index == 2 ? statistics::ref().cascaded_sun_shadow_stat_group.execute_time_cascade_3:
				 statistics::ref().cascaded_sun_shadow_stat_group.execute_time_cascade_4
				)
			)
	);
	
	u32 cascade_shadow_map_size = shadow_map_size;
	
	light*	sun = &*m_context->scene()->lights().get_sun().c_ptr();
	
	if (!sun)
		return;
	
	backend::ref().set_render_targets		( 0, 0, 0, 0);
	backend::ref().set_depth_stencil_target	( &*m_context->m_rt_shadow_map[cascade_index]);	
	backend::ref().clear_depth_stencil		( D3D_CLEAR_DEPTH|D3D_CLEAR_STENCIL, 1.0f, 0);
	
	float const size						= m_context->m_sun_cascades[cascade_id].size;
	
	// TODO: global variable?
	float const sun_distance				= 300.f;
	
	sun->position							=	m_context->get_view_pos() - sun->direction*sun_distance;
	float3	tmp_vec							=	m_context->get_view_pos() - sun->position;
	//float	distance						=	(sun->direction.dot_product( tmp_vec) * sun->direction).length();

	float4x4 light_view_transform			=	create_camera_direction( sun->position, sun->direction, float3( 1, 0, 0) /*this need to be north direction*/);
	float4x4 light_projection_transform		=	math::create_orthographic_projection( size, size, 0.1f, sun_distance +/*sqrt(2)*/1.41421f*size );

	shadow_cascade_volume::cascade_volume_planes cull_planes;
	float3	light_shift_xz( 0.f, 0.f, 0.f);

	shadow_cascade_volume	cascade_volume;
	
	//initialize shadow_cascade_volume
	//////////////////////////////////////////////////////////////////////////

	if ( cascade_index==0 ) //cascade_id == 0 || m_context->m_sun_cascades[cascade_id].reset_chain )
		for( int i = 0; i < 4; i++)	
		{
			float3 eye_ray = m_context->get_eye_rays()[i];
			eye_ray.normalize();
			eye_ray = m_context->get_v_inverted().transform_direction(eye_ray);
			cascade_volume.view_frustum_rays.push_back	( ray( m_context->get_view_pos() + m_context->get_near()*eye_ray, eye_ray) );
		}
	else
		cascade_volume.view_frustum_rays	= m_context->m_sun_cascades[cascade_id].rays;

	cascade_volume.view_ray.origin			= m_context->get_view_pos();
	cascade_volume.view_ray.direction		= m_context->get_view_dir();
	cascade_volume.light_ray.origin			= sun->position;
	cascade_volume.light_ray.direction		= sun->direction;


	float4x4 light_full_transform = light_view_transform * light_projection_transform;

	float4x4 light_full_transform_invert = invert4x3( light_full_transform);

	for	( int p = 0; p < 8; p++)	{
		float4	xf	= light_full_transform_invert.transform( float4( corners[p], 1.f));
		cascade_volume.light_cuboid_points[p] = xf.xyz();
	}

	// only side planes
	for (int plane=0; plane < 4; plane++)	
		for (int pt=0; pt < 4; pt++)	
		{
			int asd = facetable[plane][pt];
			cascade_volume.light_cuboid_polys[plane].points[pt] = asd;
		}

	cascade_volume.compute_caster_model_fixed( cull_planes, light_shift_xz, size, true, true, true, false);
	
	// For shadow stabilization.
	float3 adjastment		= compute_aligment( light_shift_xz, light_view_transform * light_projection_transform, float(cascade_shadow_map_size));
	light_view_transform	= create_camera_direction(  sun->position  + light_shift_xz + adjastment, sun->direction, float3( 1, 0, 0) /*this need to be north direction*/);
	
	float3 view_pos = m_context->get_view_pos();
	
	m_context->push_set_v( light_view_transform);
	m_context->push_set_p( light_projection_transform);

 	m_context->scene()->select_models( m_context->get_culling_vp(), m_caster_model);

	render_surface_instances::iterator		it_d	= m_caster_model.begin();
	render_surface_instances::const_iterator	end_d	= m_caster_model.end();

	D3D_VIEWPORT orig_viewport;
	backend::ref().get_viewport( orig_viewport);

	D3D_VIEWPORT tmp_viewport;
	tmp_viewport.TopLeftX	= 0.0f;
	tmp_viewport.TopLeftY	= 0.0f;
	tmp_viewport.Width		= float(cascade_shadow_map_size);
	tmp_viewport.Height		= float(cascade_shadow_map_size);
	tmp_viewport.MinDepth	= 0;
	tmp_viewport.MaxDepth	= 1.f;
	
	backend::ref().set_viewport( tmp_viewport);
	
	for( ; it_d != end_d; ++it_d)
	{
		render_surface_instance& instance = *(*it_d);
		material_effects& me		= instance.m_render_surface->get_material_effects();
		render_geometry& geometry	= instance.m_render_surface->m_render_geometry;

		if ((!me.stage_enable[geometry_render_stage] && !me.stage_enable[lighting_render_stage]) || 
			!me.is_cast_shadow ||
			!geometry.geom.c_ptr())
			continue;
		
		if (!me.stage_enable[shadow_render_stage] || 
			!me.m_effects[shadow_render_stage])
		{
			m_effect_shadow_direct->apply();
		}
		else
		{
			me.m_effects[shadow_render_stage]->apply();
		}
		
		
		//m_effect_shadow_direct->apply();
		instance.set_constants		( );
		geometry.geom->apply	( );
		m_context->set_w						(  (*instance.m_transform) );
		
		backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, geometry.primitive_count*3, 0, 0);
		
		switch (cascade_index)
		{
			case 0: statistics::ref().cascaded_sun_shadow_stat_group.num_models_cascade_1.value++; break;
			case 1: statistics::ref().cascaded_sun_shadow_stat_group.num_models_cascade_2.value++; break;
			case 2:	statistics::ref().cascaded_sun_shadow_stat_group.num_models_cascade_3.value++; break;
			case 3:	statistics::ref().cascaded_sun_shadow_stat_group.num_models_cascade_4.value++; break;
		};
		
	}
	
	if (options::ref().m_enabled_draw_speedtree)
		render_speedtree_instances(view_pos, cascade_index);
	
	if (options::ref().m_enabled_draw_terrain && options::ref().m_enabled_terrain_shadows)
		render_terrain(view_pos);
	
	backend::ref().set_viewport( orig_viewport);
	
	float4x4 shadow_trans = m_context->get_vp();
	
	m_context->pop_v();
	m_context->pop_p();
	
	//m_context->get_shadow_map_z_bias();
	float ragne		= 1.f;
	float z_bias	= m_context->m_sun_cascades[cascade_id].bias;//s_sun_shadow_z_bias_value[cascade_index];//sun->shadow_z_bias * m_context->m_sun_cascades[cascade_id].bias;
	float4x4	texture_space(	float4( 0.5f,				0.0f,				0.0f,			0.0f),
								float4( 0.0f,				-0.5f,				0.0f,			0.0f),
								float4( 0.0f,				0.0f,				ragne,			0.0f),
								float4( 0.5f,				0.5f,				-z_bias,		1.0f));
	
	m_context->set_view2shadow( m_context->get_v_inverted() * shadow_trans * texture_space, cascade_index);
	
	float4	tmp( 0.f, 0.f, 0.f, 1.f);
	
	float4	other = m_context->get_view2shadow().transform( tmp);
	
	// Initialize rays for the next cascade
	if( cascade_id < m_context->m_sun_cascades.size()-1 )
		m_context->m_sun_cascades[cascade_id+1].rays = cascade_volume.view_frustum_rays;
	
	END_CPUGPU_TIMER;
}


void stage_shadow_direct::render_terrain(xray::math::float3 const& viewer_position)
{
//////////////////////////////////////////////////////////////////////////
	typedef render::vector<terrain_render_model_instance_ptr>		terrain_ptr_cells;
	typedef terrain_ptr_cells::const_iterator		terrain_ptr_cells_cit;

	terrain_ptr_cells									terrain_cells;

////// Rendering terrain cells ///////////////////////////////////////////

	if (m_context->scene()->terrain())
	{
		m_context->scene()->select_terrain_cells( m_context->get_culling_vp(), terrain_cells);

		terrain_ptr_cells_cit it_tr = terrain_cells.begin();
		terrain_ptr_cells_cit en_tr = terrain_cells.end();

		m_context->set_w( math::float4x4().identity() );

		for( ; it_tr != en_tr; ++it_tr)
		{
			// TODO: remove this check
			if (!(*it_tr)->m_terrain_model->effect())
				continue;
			
			(*it_tr)->m_terrain_model->effect()->apply(2);
			//(*it_tr)->m_terrain_model->render_geometry().geom->apply();
			
			float3	start_corner( 0.f, 0.f, 0.f);
			start_corner = (*it_tr)->m_terrain_model->transform().transform_position( start_corner);

			float3 distance_vector = start_corner + float3( (*it_tr)->m_terrain_model->physical_size()/2, 0, -(*it_tr)->m_terrain_model->physical_size()/2);
			distance_vector -= viewer_position;//m_context->get_view_pos();

			u32 primitive_count = 0;
			if( distance_vector.length() > 150)
			{
				m_context->scene()->terrain()->m_grid_geom_1->apply();
				primitive_count = m_context->scene()->terrain()->m_grid_geom_1->intex_buffer()->size()/(sizeof(u16));
			}
			else
			{
				m_context->scene()->terrain()->m_grid_geom_0->apply();
				primitive_count = m_context->scene()->terrain()->m_grid_geom_0->intex_buffer()->size()/(sizeof(u16));
			}
			
			backend::ref().set_vs_constant( m_c_start_corner, float4( start_corner, 0));
			backend::ref().set_ps_constant( m_c_start_corner, float4( start_corner, 0));
			
			backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, primitive_count/*(*it_tr)->m_terrain_model->render_geometry().primitive_count*3*/, 0, 0);
			statistics::ref().visibility_stat_group.num_triangles.value += primitive_count / 3;
		}
	}
}


float3 stage_shadow_direct::compute_aligment( float3 const & lightXZshift, float4x4 const & light_space_transform, float smap_res)
{
	float3 align_origin( 0.f, 0.f, 0.f); 

	// Moves align point near to view point
// 	align_origin = m_context->get_view_pos();
// 	const float		align_aim_step_coef = 8.f;
// 	align_origin.set( math::floor(align_origin.x/align_aim_step_coef)+align_aim_step_coef/2, math::floor(align_origin.y/align_aim_step_coef)+align_aim_step_coef/2, math::floor(align_origin.z/align_aim_step_coef)+align_aim_step_coef/2);
// 	align_origin *=	align_aim_step_coef;
	align_origin -= lightXZshift;

	float4x4 viewport(	float4(	smap_res/2.f,	0.0f,				0.0f,		0.0f),
						float4(	0.0f,			-smap_res/2.f,		0.0f,		0.0f),
						float4(	0.0f,			0.0f,				1.0f,		0.0f),
						float4(	smap_res/2.f,	smap_res/2.f,		0.0f,		1.0f));

	float4x4 viewport_invert				= math::invert4x3( viewport);
	float4x4 light_space_transform_invert	= math::invert4x3( light_space_transform);

	float4	origin_pixel	= light_space_transform.transform( float4( align_origin, 1.f) );
	origin_pixel			*= (1.f/origin_pixel.w);
	origin_pixel			= viewport.transform( origin_pixel );

	const float	align_granularity = 4.f;

	origin_pixel.x	= origin_pixel.x / align_granularity - floorf	(origin_pixel.x / align_granularity);
	origin_pixel.y	= origin_pixel.y / align_granularity - floorf	(origin_pixel.y / align_granularity);
	origin_pixel.x	*= align_granularity;
	origin_pixel.y	*= align_granularity;
	origin_pixel.z	= 0;

	origin_pixel.xyz() = viewport_invert.transform_direction	(origin_pixel.xyz());
	origin_pixel.xyz() = light_space_transform_invert.transform_direction	(origin_pixel.xyz());

	float3 diff		= origin_pixel.xyz();
	static float sign_test = 1.f;
	diff	*=	sign_test;

	return diff;
}

float3 stage_shadow_direct::compute_aligment1( float3 const & lightXZshift, float4x4 const & light_space_transform, float smap_res)
{
	float4x4 viewport_invert(	float4(	2.f/smap_res,	0.0f,				0.0f,		0.0f),
								float4(	0.0f,			2.f/smap_res,		0.0f,		0.0f),
								float4(	0.0f,			0.0f,				1.0f,		0.0f),
								float4(	0.0f,			0.0f,				0.0f,		1.0f));
	
	float4x4 light_space_transform_invert	= math::invert4x3( light_space_transform);
	
	float3 pixel_dimension( 1.f, 1.f, 0.f );
	pixel_dimension		= viewport_invert.transform_direction	( pixel_dimension);
	pixel_dimension		= light_space_transform_invert.transform_direction	( pixel_dimension);
	
	float3 pixel_dimension_norm = pixel_dimension;
	pixel_dimension_norm.normalize();
	
	float3 light_dir	=  light_space_transform_invert.transform_direction( float3( 0.f, 0.f, 1.f));
	light_dir.normalize();
	
	math::plane plane_xz = math::create_plane_normalized( float3( 0.f, 1.f, 0.f), float3( 0.f, 0.f, 0.f) );
	
	float3 projected_pixel_size;
	if (!plane_xz.intersect_ray( math::abs(pixel_dimension), light_dir, projected_pixel_size))
	{
		projected_pixel_size = float3(0.1f, 0.1f, 0.1f);
	}
	
	float3 align_origin( 0.f, 0.f, 0.f); 
	
	align_origin = m_context->get_view_pos();
	const float		align_aim_step_coef = 10.f;
	align_origin.set( math::floor(align_origin.x/align_aim_step_coef)+align_aim_step_coef/2, math::floor(align_origin.y/align_aim_step_coef)+align_aim_step_coef/2, math::floor(align_origin.z/align_aim_step_coef)+align_aim_step_coef/2);
	align_origin *=	align_aim_step_coef;
	
	float4 light_pos	=  light_space_transform_invert.transform( float4( 0.f, 0.f, 0.f, 1.f));
	light_pos.xyz()		+= lightXZshift - align_origin;
	
	float3 light_pos_proj;
	bool intersect_result = plane_xz.intersect_ray( light_pos.xyz(), light_dir, light_pos_proj);
	
	float3 light_pos_align = light_pos_proj;
	
	if (intersect_result)
	{
		light_pos_align.x	= math::floor( light_pos_align.x/projected_pixel_size.x) * projected_pixel_size.x;
		light_pos_align.z	= math::floor( light_pos_align.z/projected_pixel_size.z) * projected_pixel_size.z;
	}
	else
	{
		return float3(0,0,0);
	}
	
//	float3 one_pixel_move_back( 0.f, 0.f, 0.f);
//
// 	float3 view_pos_proj;
// 	plane_xz.intersect_ray( m_context->get_view_pos(), light_dir, view_pos_proj);
// 
// 	one_pixel_move_back = light_pos_proj;
//
// 	one_pixel_move_back.x = one_pixel_move_back.x == 0.f ? 0.f : ( one_pixel_move_back.x  < 0 ? -1.f : 1.f );
// 	one_pixel_move_back.z = one_pixel_move_back.z == 0.f ? 0.f : ( one_pixel_move_back.z  < 0 ? -1.f : 1.f );

	return ( light_pos_align - light_pos_proj );// + one_pixel_move_back*projected_pixel_size;
}

void stage_shadow_direct::render_speedtree_instances(math::float3 const& viewer_position, u32 cascade_index)
{
	speedtree_forest::tree_render_info_array_type visible_trees;
	
	m_context->scene()->get_speedtree_forest()->get_visible_tree_components(m_context, viewer_position, true, visible_trees);
	
	for (speedtree_forest::tree_render_info_array_type::iterator it=visible_trees.begin(); it!=visible_trees.end(); ++it)
	{
		if (!it->tree_component->get_material_effects().stage_enable[geometry_render_stage] || !it->tree_component->get_material_effects().is_cast_shadow)
			continue;
		
		if ( !it->tree_component->get_material_effects().stage_enable[shadow_render_stage] || 
			!it->tree_component->get_material_effects().m_effects[shadow_render_stage])
			continue;
		
		// TODO: fix
		if (it->tree_component->get_geometry_type()==SpeedTree::GEOMETRY_TYPE_VERTICAL_BILLBOARDS)
			continue;

		it->tree_component->get_material_effects().m_effects[shadow_render_stage]->apply();
		
		m_context->scene()->get_speedtree_forest()->get_speedtree_wind_parameters().set		(it->tree_component->m_parent->GetWind());
		m_context->scene()->get_speedtree_forest()->get_speedtree_common_parameters().set	(m_context, it->tree_component, viewer_position);
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
			
			switch (cascade_index)
			{
				case 0: statistics::ref().cascaded_sun_shadow_stat_group.num_models_cascade_1.value++; break;
				case 1: statistics::ref().cascaded_sun_shadow_stat_group.num_models_cascade_2.value++; break;
				case 2:	statistics::ref().cascaded_sun_shadow_stat_group.num_models_cascade_3.value++; break;
				case 3:	statistics::ref().cascaded_sun_shadow_stat_group.num_models_cascade_4.value++; break;
			};
		}
	}
}

} // namespace render
} // namespace xray
