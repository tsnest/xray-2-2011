////////////////////////////////////////////////////////////////////////////
//	Created		: 10.02.2009
//	Author		: Igor Lobanchikov
//	Copyright ( color) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "renderer.h"
#include <xray/render/facade/scene_view_mode.h>
#include <xray/console_command.h>
#include <xray/render/core/utils.h>
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/sampler_state_descriptor.h>
#include "renderer_context.h"
#include "render_model.h"
#include "material.h"
#include "vertex_formats.h"
#include <xray/render/core/backend.h>
#include <xray/render/core/effect_manager.h>
#include "effect_editor_gbuffer_to_screen.h"

// render stages
#include "stage_gbuffer.h"
#include "stage_pre_lighting.h"
#include "stage_shadow_direct.h"
#include "stage_sun.h"
#include "stage_lights.h"
#include "stage_skybox.h"
#include "stage_accumulate_distortion.h"
#include "stage_apply_distortion.h"
#include "stage_forward.h"
#include "stage_particles.h"
#include "stage_postprocess.h"
#include "stage_screen_image.h"
#include "stage_view_mode.h"
#include "stage_debug.h"
#include "stage_decals_accumulate.h"
#include "stage_ambient_occlusion.h"
#include "stage_light_propagation_volumes.h"

#include "system_renderer.h"

#include "speedtree_forest.h"

#include <xray/render/core/render_target.h>
#include <xray/render/core/res_geometry.h>
#include "render_output_window.h"
#include "scene.h"
#include <xray/particle/world.h>
#include "shared_names.h"
#include "scene_view.h"
#include <xray/render/core/res_effect.h>
#include <xray/render/core/effect_options_descriptor.h>

#include <xray/ui/ui.h>
// for ui::vertex
#include <xray/render/facade/ui_renderer.h>

#include "effect_complex_post_process_blend.h"
#include "stage_clouds.h"

namespace xray {
namespace render {

static u32 s_view_mode_value = u32(lit_view_mode);
static xray::console_commands::cc_u32 s_view_mode("render_view_mode", 
												   s_view_mode_value, 
												   0, 
												   num_view_modes, 
												   true, 
												   xray::console_commands::command_type_engine_internal);

static bool s_disabled_shader_constansts_set = false;
static xray::console_commands::cc_bool s_disabled_shader_constansts_set_cc("disabled_shader_constansts_set", s_disabled_shader_constansts_set, false, xray::console_commands::command_type_engine_internal);

bool renderer::is_effects_ready() const
{
	return m_gbuffer_to_screen_shader.c_ptr() != NULL;
}

static statistics m_statistics;

renderer::renderer( renderer_context * renderer_context ):
	m_renderer_context	( renderer_context ),
	m_last_frame_time	( 0.f ),
	m_current_time		( 0.f )
{
#pragma message (RENDER_TODO("Implement filter none for mip, also anisotropic with linear."))
	
	static float4 dummy_data ( 0.f, 0.f, 0.f, 0.f);

	RENDER_TODO("This need to be moved when constants buffer are organized.");
	resource_manager::ref().register_constant_binding( shader_constant_binding( "hemi_cube_pos_faces",	&dummy_data));
	resource_manager::ref().register_constant_binding( shader_constant_binding( "hemi_cube_neg_faces",	&dummy_data));
	resource_manager::ref().register_constant_binding( shader_constant_binding( "dt_params",	&dummy_data));
	
	// Here some initialization code will go into more general initialization stage 
	sampler_state_descriptor sampler_sim;
	sampler_sim.set( D3D_FILTER_ANISOTROPIC, D3D_TEXTURE_ADDRESS_WRAP);
	sampler_sim.set_max_anisotropy(16);
	resource_manager::ref().register_sampler( "s_base", resource_manager::ref().create_sampler_state( sampler_sim));
	
	sampler_sim.set( D3D_FILTER_ANISOTROPIC );
	sampler_sim.set_max_anisotropy(16);
	resource_manager::ref().register_sampler( "s_base_hud", resource_manager::ref().create_sampler_state( sampler_sim) );

	sampler_sim.set( D3D_FILTER_ANISOTROPIC, D3D_TEXTURE_ADDRESS_WRAP);
	sampler_sim.set_max_anisotropy(16);
	resource_manager::ref().register_sampler( "s_detail", resource_manager::ref().create_sampler_state( sampler_sim) );
	
	sampler_sim.set( D3D_FILTER_MIN_MAG_MIP_POINT, D3D_TEXTURE_ADDRESS_CLAMP );
	resource_manager::ref().register_sampler( "s_position", resource_manager::ref().create_sampler_state( sampler_sim) );
	//resource_manager::ref().register_sampler( "s_normal", resource_manager::ref().create_sampler_state( sampler_sim) );
	resource_manager::ref().register_sampler( "s_diffuse", resource_manager::ref().create_sampler_state( sampler_sim) );
	resource_manager::ref().register_sampler( "s_accumulator", resource_manager::ref().create_sampler_state( sampler_sim) );

	sampler_sim.set( D3D_FILTER_ANISOTROPIC, D3D_TEXTURE_ADDRESS_WRAP);
	sampler_sim.set_max_anisotropy(16);
	resource_manager::ref().register_sampler( "s_material", resource_manager::ref().create_sampler_state( sampler_sim) );
	resource_manager::ref().register_sampler( "s_normal", resource_manager::ref().create_sampler_state( sampler_sim) );

	sampler_sim.set( D3D_FILTER_MIN_MAG_MIP_LINEAR, D3D_TEXTURE_ADDRESS_CLAMP);
	resource_manager::ref().register_sampler( "s_material1", resource_manager::ref().create_sampler_state( sampler_sim) );

	sampler_sim.set( D3D_FILTER_MIN_MAG_MIP_POINT, D3D_TEXTURE_ADDRESS_WRAP);
	resource_manager::ref().register_sampler( "s_nofilter", resource_manager::ref().create_sampler_state( sampler_sim) );

	sampler_sim.set( D3D_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D_TEXTURE_ADDRESS_CLAMP);
	resource_manager::ref().register_sampler( "s_rtlinear", resource_manager::ref().create_sampler_state( sampler_sim) );

	sampler_sim.set( D3D_FILTER_MIN_MAG_MIP_LINEAR, D3D_TEXTURE_ADDRESS_WRAP);
	resource_manager::ref().register_sampler( "s_linear", resource_manager::ref().create_sampler_state( sampler_sim) );
	
	sampler_sim.set( D3D_FILTER_MIN_MAG_MIP_LINEAR, D3D_TEXTURE_ADDRESS_BORDER);
	resource_manager::ref().register_sampler( "s_border", resource_manager::ref().create_sampler_state( sampler_sim) );

	sampler_sim.set( D3D_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D_TEXTURE_ADDRESS_CLAMP );
	sampler_sim.set_border_color( float4( 1.f, 1.f, 1.f, 0.f));
	sampler_sim.set_comparison_function( D3D_COMPARISON_LESS_EQUAL);
	resource_manager::ref().register_sampler( "s_shmap", resource_manager::ref().create_sampler_state( sampler_sim) );
	sampler_sim.reset();
	
// 	sampler_sim.set( D3D_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D_TEXTURE_ADDRESS_CLAMP );
// 	sampler_sim.set_border_color( float4( 1.f, 1.f, 1.f, 0.f));
// 	sampler_sim.set_comparison_function( D3D_COMPARISON_LESS_EQUAL);
// 	resource_manager::ref().register_sampler( "s_shmap_clamp", resource_manager::ref().create_sampler_state( sampler_sim) );
// 	sampler_sim.reset();
	
	effect_manager::ref().create_effect<effect_editor_gbuffer_to_screen>(&m_gbuffer_to_screen_shader);
	
	m_gbuffer_to_screen_type				= backend::ref().register_constant_host("gbuffer_to_screen_type", rc_float);
	
	m_stages.resize(num_render_stages);
	
	for (stages_type::iterator it=m_stages.begin(); it!=m_stages.end(); ++it)
		*it = 0;
	
	// TODO: Stages profiling with gpu and cpu timers.
	
	m_stage_debug									= 0;
	m_view_mode_stage								= 0;
	m_present_stage									= 0;
	
	m_stages[geometry_pre_pass_render_stage]		= NEW(stage_gbuffer)					(m_renderer_context, true);
	m_stages[decals_accumulate_render_stage]		= NEW(stage_decals_accumulate)			(m_renderer_context);
	m_stages[accumulate_distortion_render_stage]	= NEW(stage_accumulate_distortion)		(m_renderer_context);
	m_stages[pre_lighting_stage]					= NEW(stage_pre_lighting)				(m_renderer_context);
	m_stages[sun_shadows_accumulate_render_stage]	= NEW(stage_shadow_direct)				(m_renderer_context);
	m_stages[sun_render_stage]						= NEW(stage_sun)						(m_renderer_context);
	m_stages[deferred_lighting_render_stage]		= NEW(stage_lights)						(m_renderer_context, false);
	m_stages[ambient_occlusion_render_stage]		= NEW(stage_ambient_occlusion)			(m_renderer_context);
	m_stages[light_propagation_volumes_render_stage]= NEW(stage_light_propagation_volumes)	(m_renderer_context);
	m_stages[geometry_render_stage]					= NEW(stage_gbuffer)					(m_renderer_context, false);
	m_stages[skybox_render_stage]					= NEW(stage_skybox)						(m_renderer_context);
	//m_stages[apply_distortion_render_stage]			= NEW(stage_apply_distortion)			(m_renderer_context);
	//m_stages[clouds_render_stage]					= NEW(stage_clouds)						(m_renderer_context);
	m_stages[forward_render_stage]					= NEW(stage_forward)					(m_renderer_context);
	m_stages[lighting_render_stage]					= NEW(stage_lights)						(m_renderer_context, true);
	m_stages[particles_render_stage]				= NEW(stage_particles)					(m_renderer_context);
	m_stages[post_process_render_stage]				= NEW(stage_postprocess)				(m_renderer_context);
	
	m_stage_debug									= NEW(stage_debug)						(m_renderer_context);
	m_present_stage									= NEW(stage_screen_image)				(m_renderer_context);
	m_view_mode_stage								= NEW(stage_view_mode)					(m_renderer_context);
	
	m_timer.start();
}

renderer::~renderer()
{
	DELETE			(m_view_mode_stage);
	DELETE			(m_present_stage);
	DELETE			(m_stage_debug );
	
	for (stages_type::reverse_iterator it=m_stages.rbegin(), e=m_stages.rend(); it!=e; ++it)
		DELETE		(*it);
}

#ifndef MASTER_GOLD
// TODO: Make as a general functions.
static void fill_surface(ref_rt surf, renderer_context* context)
{
	float w = float(surf->width());
	float h = float(surf->height());
	float z = 0.0f;
	
	backend::ref().set_render_targets( &*surf, 0, 0, 0);
	backend::ref().reset_depth_stencil_target();
	backend::ref().clear_render_targets( xray::math::color( 1.0f, 1.0f, 1.0f, 0.0f));
	
	float2	p0( 0,0);
	float2	p1( 1,1);
	
	u32		offset;
	
	float3 const* eye_rays = context->get_eye_rays();
	
	// Fill vertex buffer
	vertex_formats::Tquad* pv = backend::ref().vertex.lock<vertex_formats::Tquad>( 4, offset);
	pv->set( 0, h, z, 1.0, eye_rays[1].x, eye_rays[1].y, eye_rays[1].z, p0.x, p1.y); pv++;
	pv->set( 0, 0, z, 1.0, eye_rays[0].x, eye_rays[0].y, eye_rays[0].z, p0.x, p0.y); pv++;
	pv->set( w, h, z, 1.0, eye_rays[3].x, eye_rays[3].y, eye_rays[3].z, p1.x, p1.y); pv++;
	pv->set( w, 0, z, 1.0, eye_rays[2].x, eye_rays[2].y, eye_rays[2].z, p1.x, p0.y); pv++;
	backend::ref().vertex.unlock();
	
	context->m_g_quad_eye_ray->apply();
	
	backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 2*3, 0, offset);
}
#endif // #ifndef MASTER_GOLD

void renderer::setup_render_output_window ( render_output_window_ptr in_output_window, viewport_type const& viewport)
{
	R_ASSERT									( in_output_window );
	render_output_window* const output_window	= (render_output_window*)in_output_window.c_ptr();
		
	m_renderer_context->set_target_context		( &output_window->target_context());
	backend::ref().set_render_output			( output_window->render_output());
	backend::ref().reset_depth_stencil_target	();

	viewport_type res_viewport;
	res_viewport = viewport_type( math::float2(0, 0), math::float2(1.f, 1.f));

	R_ASSERT			( viewport.width() );
	R_ASSERT			( viewport.height() );

	res_viewport.left	= math::max( res_viewport.left,		viewport.left);
	res_viewport.right	= math::min( res_viewport.right,	viewport.right);
	res_viewport.top	= math::max( res_viewport.top,		viewport.top);
	res_viewport.bottom	= math::min( res_viewport.bottom,	viewport.bottom);

	u32 const window_width	= backend::ref().target_width( );
	u32 const window_height	= backend::ref().target_height( );
	
#pragma warning(push)
#pragma warning(disable:4244)
	D3D_VIEWPORT d3d_viewport	= { window_width * res_viewport.left, window_height * res_viewport.top, window_width * res_viewport.width(), window_height * res_viewport.height(), 0.f, 1.f};
#pragma warning(pop)

	backend::ref().set_viewport				( d3d_viewport);
}

void renderer::toggle_render_stage( enum_render_stage_type stage_type, bool toggle )
{
	if (m_stages[stage_type])
		m_stages[stage_type]->set_enabled(toggle);
}

void renderer::execute_stages()
{
	for (stages_type::iterator it = m_stages.begin(); it != m_stages.end(); ++it)
	{
		stage* current_stage = *it;
		
		if (current_stage)
			current_stage->execute();
	}
}

void renderer::render(scene_ptr const& in_scene,
					  scene_view_ptr const& in_view,
					  render_output_window_ptr const& output_window,
					  viewport_type const& viewport,
					  boost::function< void ( bool ) > const& on_draw_scene,
					  bool draw_debug_terrain)
{
	backend::ref().disabled_shader_constansts_set	= s_disabled_shader_constansts_set;
	
	xray::render::scene*		scene	= static_cast_checked<xray::render::scene*>(in_scene.c_ptr());
	xray::render::scene_view*	view	= static_cast_checked<xray::render::scene_view*>(in_view.c_ptr());

	view->inc_render_frame_index		();
	
 	m_current_time						= m_timer.get_elapsed_sec();
	
	float time_delta					= m_current_time - m_last_frame_time;
	time_delta							= xray::math::max(time_delta, 0.0f) * scene->get_slomo();
	
	m_renderer_context->set_current_time(m_last_frame_time);
	m_renderer_context->set_time_delta  (time_delta );
	
	m_renderer_context->set_scene		(scene);
	m_renderer_context->set_scene_view	(view);
	
	if (!is_effects_ready() || !material::is_nomaterial_material_ready()) {
		xray::render::scene* const scene	= static_cast_checked<xray::render::scene*>(in_scene.c_ptr());
		scene->flush						( on_draw_scene );
		return;
	}
	
	statistics::ref().start();
	
	BEGIN_CPUGPU_TIMER(statistics::ref().general_stat_group.render_frame_time);
	BEGIN_TIMER(statistics::ref().general_stat_group.cpu_render_frame_time);
	
	static_cast_checked< render::render_output_window* > ( output_window.c_ptr() )->resize( );
	
	setup_render_output_window				( output_window, viewport);
	
	m_renderer_context->m_light_marker_id = 1;
	
	m_renderer_context->scene()->update_models();
	
	xray::particle::world* part_world = m_renderer_context->scene()->particle_world();
	
	scene->get_speedtree_forest()->tick(m_renderer_context);
	
	if (part_world)
		part_world->tick( time_delta, m_renderer_context->get_v() );
	
	backend::ref().reset();
	//backend::ref().flush();
	
	scene_view_mode view_mode = view->get_view_mode();
	
	backend::ref().set_render_targets	( &*m_renderer_context->m_targets->m_rt_position, &*m_renderer_context->m_targets->m_rt_normal, 0, 0);
	backend::ref().reset_depth_stencil_target( );
	backend::ref().clear_render_targets	( math::color( 1.f, 1.f, 1.f, 1.f), math::color( 0.f, 0.f, 0.f, 0.f), math::color( 0.f, 0.f, 0.f, 0.f), math::color( 0.f, 0.f, 0.f, 0.f));
	
	backend::ref().reset_depth_stencil_target();
	backend::ref().clear_depth_stencil	(D3D_CLEAR_DEPTH|D3D_CLEAR_STENCIL, 1.0f, 0);
	
#ifdef MASTER_GOLD
	execute_stages();
	XRAY_UNREFERENCED_PARAMETER(view_mode);
#else // #ifdef MASTER_GOLD
	
	if (m_view_mode_stage && m_view_mode_stage->is_support_view_mode(view_mode))
	{
		// opaque pass
		if (m_stages[geometry_pre_pass_render_stage])
			m_stages[geometry_pre_pass_render_stage]->execute();
		
		m_view_mode_stage->execute(view_mode);
		statistics::ref().visibility_stat_group.num_total_rendered_triangles.value = backend::ref().num_total_rendered_triangles;
	}
	else
	{
		float view_mode_type = float(u32(view_mode)) + 0.5f;
		switch (view_mode)
		{
			case lit_view_mode:
			case lit_with_histogram_view_mode:
			{
				execute_stages();
				
				if (view_mode==lit_with_histogram_view_mode)
				{
					// Copy final scene color to albedo texture.
					resource_manager::ref().copy2D(
						&*m_renderer_context->m_targets->m_t_color,
						0,
						0,
						&*m_renderer_context->m_targets->m_t_present,
						0,
						0,
						m_renderer_context->m_targets->m_t_present->width(),
						m_renderer_context->m_targets->m_t_present->height()
					);
					
					m_gbuffer_to_screen_shader->apply();
					backend::ref().set_ps_constant(m_gbuffer_to_screen_type, view_mode_type);
					fill_surface(m_renderer_context->m_targets->m_rt_present, m_renderer_context);
				}
				
				break;
			}
			case unlit_view_mode:
			case unlit_with_ao_view_mode:
			{
				m_stages[accumulate_distortion_render_stage]->set_enabled(false);
				m_stages[pre_lighting_stage]->set_enabled(false);
				m_stages[sun_shadows_accumulate_render_stage]->set_enabled(false);
				m_stages[sun_render_stage]->set_enabled(false);
				m_stages[deferred_lighting_render_stage]->set_enabled(false);
				m_stages[sun_shadows_accumulate_render_stage]->set_enabled(false);
				m_stages[post_process_render_stage]->set_enabled(false);
				m_stages[lighting_render_stage]->set_enabled(false);
				
				execute_stages();
				
				m_stages[accumulate_distortion_render_stage]->set_enabled(true);
				m_stages[pre_lighting_stage]->set_enabled(true);
				m_stages[sun_shadows_accumulate_render_stage]->set_enabled(true);
				m_stages[sun_render_stage]->set_enabled(true);
				m_stages[deferred_lighting_render_stage]->set_enabled(true);
				m_stages[sun_shadows_accumulate_render_stage]->set_enabled(true);
				m_stages[post_process_render_stage]->set_enabled(true);
				m_stages[lighting_render_stage]->set_enabled(true);
				
				m_gbuffer_to_screen_shader->apply();
				backend::ref().set_ps_constant(m_gbuffer_to_screen_type, view_mode_type);
				fill_surface(m_renderer_context->m_targets->m_rt_present, m_renderer_context);
				
				break;
			}
			case normals_view_mode:
			case specular_intencity_view_mode:
			case specular_power_view_mode:
			{
				if (m_stages[geometry_pre_pass_render_stage])
					m_stages[geometry_pre_pass_render_stage]->execute();
				
				if (m_stages[decals_accumulate_render_stage])
					m_stages[decals_accumulate_render_stage]->execute();
				
				m_gbuffer_to_screen_shader->apply();
				backend::ref().set_ps_constant(m_gbuffer_to_screen_type, view_mode_type);
				fill_surface(m_renderer_context->m_targets->m_rt_present, m_renderer_context);
				break;
			}
			case lighting_view_mode:
			case lighting_diffuse_view_mode:
			case lighting_specular_view_mode:
			case emissive_only_view_mode:
			case distortion_only_view_mode:
			{
				
				if (m_stages[post_process_render_stage])
					m_stages[post_process_render_stage]->set_enabled(false);
				
				execute_stages();
				
				if (m_stages[post_process_render_stage])
					m_stages[post_process_render_stage]->set_enabled(true);
				
				m_gbuffer_to_screen_shader->apply();
				
				backend::ref().set_ps_constant(m_gbuffer_to_screen_type, view_mode_type);
				fill_surface(m_renderer_context->m_targets->m_rt_present, m_renderer_context);
				break;
			}
			case indirect_lighting_mode:
			{
				m_stages[geometry_pre_pass_render_stage]->execute();
				m_stages[decals_accumulate_render_stage]->execute();
				m_stages[accumulate_distortion_render_stage]->execute();
				m_stages[pre_lighting_stage]->execute();
				m_stages[sun_render_stage]->execute_disabled();
				m_stages[sun_shadows_accumulate_render_stage]->execute_disabled();
				m_stages[ambient_occlusion_render_stage]->execute();
				m_stages[light_propagation_volumes_render_stage]->execute();
				
				m_gbuffer_to_screen_shader->apply();
				
				backend::ref().set_ps_constant(m_gbuffer_to_screen_type, view_mode_type);
				fill_surface(m_renderer_context->m_targets->m_rt_present, m_renderer_context);
				break;
			}
			case ambient_occlusion_only_view_mode:
			{
				if (m_stages[geometry_pre_pass_render_stage])
					m_stages[geometry_pre_pass_render_stage]->execute();
					
				if (m_stages[decals_accumulate_render_stage])
					m_stages[decals_accumulate_render_stage]->execute();
					
				if (m_stages[ambient_occlusion_render_stage])
					m_stages[ambient_occlusion_render_stage]->execute();
				
				m_gbuffer_to_screen_shader->apply();
				
				backend::ref().set_ps_constant(m_gbuffer_to_screen_type, view_mode_type);
				fill_surface(m_renderer_context->m_targets->m_rt_present, m_renderer_context);
				break;
			}
		}
	}
	
	backend::ref().reset_depth_stencil_target();
	
	if (m_stages[light_propagation_volumes_render_stage])
		((stage_light_propagation_volumes*)m_stages[light_propagation_volumes_render_stage])->draw_debug();

	if (m_stages[deferred_lighting_render_stage])
		((stage_lights*)m_stages[deferred_lighting_render_stage])->debug_render();
	

	if (m_stage_debug)
		m_stage_debug->execute			();
	
	backend::ref().set_render_targets( &*m_renderer_context->m_targets->m_rt_present, 0, 0, 0);
	
	scene->flush					( on_draw_scene );

	backend::ref().reset_depth_stencil_target();
	
	if (m_stages[lighting_render_stage])
		m_stages[lighting_render_stage]->debug_render	( );
	
#endif // #ifdef MASTER_GOLD
	
	END_TIMER;
	END_CPUGPU_TIMER;
	
	statistics::ref().visibility_stat_group.num_total_rendered_triangles.value = backend::ref().num_total_rendered_triangles;
	statistics::ref().visibility_stat_group.num_total_rendered_points.value = backend::ref().num_total_rendered_points;
	
	double const es = statistics::ref().general_stat_group.render_frame_time.cpu_time.average() / 1000.0;
	double const es2 = statistics::ref().general_stat_group.cpu_render_frame_time.average() / 1000.0;
	
	statistics::ref().general_stat_group.fps.value = math::floor(float(es > 0.0 ? (1.0 / es) : 0.0));
	statistics::ref().general_stat_group.cpu_fps.value = math::floor(float(es2 > 0.0 ? (1.0 / es2) : 0.0));
	
	statistics::ref().general_stat_group.num_setted_shader_constants.value = backend::ref().num_setted_shader_constants;
	statistics::ref().visibility_stat_group.num_draw_calls.value = backend::ref().num_draw_calls;
	
	backend::ref().disabled_shader_constansts_set = false;
	
	if (draw_debug_terrain)
		system_renderer::ref().draw_debug_terrain();
	
	present							( output_window, viewport );
}

void renderer::present	( render_output_window_ptr in_output_window, viewport_type const& viewport )
{
	render_output_window* const output_window = static_cast_checked< render_output_window* >( in_output_window.c_ptr() );
	setup_render_output_window			( output_window, viewport );
	
	// Present the final image to base render target
	m_present_stage->execute( m_renderer_context->m_t_present);
	
	if( output_window )
	{
		output_window->render_output()->present();
	}
	
	backend::ref().reset_depth_stencil_target();
	backend::ref().clear_depth_stencil	( D3D_CLEAR_DEPTH|D3D_CLEAR_STENCIL, 1.0f, 0);
	
	backend::ref().reset();
	backend::ref().flush();
	
	m_last_frame_time		 = m_current_time;
}

void renderer::draw_text( pcstr text, xray::math::float2 const& position, xray::ui::font* const in_font, xray::math::color const& in_color )
{
	XRAY_UNREFERENCED_PARAMETERS(text, &position, in_font, &in_color);
	// TODO:
// 	vectora<xray::render::ui::vertex>	ui_vertices(g_allocator);
// 
// 	u32 symb_count			= strlen(text);
// 	u32 max_verts			= symb_count*4; // use TriangleList+ IndexBuffer(0-1-2, 3-2-1)
// 	
// 	pcstr ch				= text;
// 	float const height		= in_font->get_height();
// 	float height_ts			= in_font->get_height_ts();
// 
// 	float curr_word_len		= 0.0f;
// 	pcstr next_word			= NULL;
// 
// 	//if(m_mode==tm_multiline)
// 	in_font->parse_word			(ch, curr_word_len, next_word);
// 
// 	
// 	//xray::render::ui::command* cmd = render.create_command(max_verts, prim_type, point_type);
// 	for(u32 i=0; i<symb_count; ++i, ++ch)
// 	{
// 		u32 clr = in_color;//(i>=(u32)idx1 && i<(u32)idx2)?sel_clr:in_color;
// 
// 		float3 uv			= in_font->get_char_tc_ts(*ch);
// 		float3 const& tc	= in_font->get_char_tc(*ch);
// 
// 		//if((m_mode==tm_multiline) && (ch==next_word))
// 		//{
// 		//	parse_word		(ch, m_font, curr_word_len, next_word);
// 		//	if(pos_rt.x+curr_word_len >get_size().x)
// 		//	{
// 		//		pos_rt.x		= 0.0f;
// 		//		pos_rt.y		+= height;
// 		//	}
// 		//}
// 
// 		cmd->push_point		(pos.x+pos_rt.x,		pos.y+pos_rt.y+height,	0.0f, clr, uv.x,		uv.y+height_ts);
// 		cmd->push_point		(pos.x+pos_rt.x,		pos.y+pos_rt.y,			0.0f, clr, uv.x,		uv.y);
// 		cmd->push_point		(pos.x+pos_rt.x+tc.z,	pos.y+pos_rt.y+height,	0.0f, clr, uv.x+uv.z,	uv.y+height_ts);
// 		cmd->push_point		(pos.x+pos_rt.x+tc.z,	pos.y+pos_rt.y,			0.0f, clr, uv.x+uv.z,	uv.y);
// 
// 		pos_rt.x			+= tc.z;
// 	}
// //	cmd->set_shader();
// //	cmd->set_geom();
// //	w.ui().destroy_command(cmd);
// 	render.push_command		(cmd);
}

} // namespace render 
} // namespace xray 
