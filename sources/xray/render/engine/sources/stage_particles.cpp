////////////////////////////////////////////////////////////////////////////
//	Created		: 27.07.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/custom_config.h>
#include "stage_particles.h"
#include <xray/render/facade/particles.h>
#include <xray/console_command.h>
#include <xray/render/core/backend.h>
#include <xray/particle/particle_data_type.h>
#include <xray/render/core/pix_event_wrapper.h>
#include "renderer_context.h"
#include "scene.h"
#include "renderer_context_targets.h"
#include <xray/particle/world.h>
#include "render_particle_emitter_instance.h"
#include "scene_view.h"
#include <xray/render/core/res_effect.h>
#include <xray/render/core/res_geometry.h>
#include "statistics.h"

#include <xray/render/core/res_texture.h>
#include <xray/render/core/custom_config.h>
#include "material.h"

#include <xray/render/core/options.h>

namespace xray {
namespace render {

static u32 s_particle_render_mode_value = xray::particle::enum_particle_render_mode(xray::particle::normal_particle_render_mode); // in percents
static xray::console_commands::cc_u32 s_particle_render_mode("particle_render_mode", 
															s_particle_render_mode_value, 
															0, 
															100, 
															true, 
															xray::console_commands::command_type_engine_internal);

particle_shader_constants::particle_shader_constants()
{
	m_right_view_vector				= backend::ref().register_constant_host( "right_view_vector", rc_float );
	m_up_view_vector				= backend::ref().register_constant_host( "up_view_vector", rc_float );
	m_view_location					= backend::ref().register_constant_host( "view_location", rc_float );
	m_rotation_fixed_axis			= backend::ref().register_constant_host( "rotation_fixed_axis", rc_float );
	
	m_current_time					= backend::ref().register_constant_host( "current_time", rc_float );
	
	m_use_align_by_dir				= backend::ref().register_constant_host( "use_align_by_dir", rc_float );
	m_use_fixed_axis				= backend::ref().register_constant_host( "use_fixed_axis", rc_float );
	m_locked_no_ratate_axis_index	= backend::ref().register_constant_host( "locked_no_ratate_axis_index", rc_float );
	
}

void particle_shader_constants::set_time(float time)
{
	backend::ref().set_ps_constant ( m_current_time, time);
}

void particle_shader_constants::set(float3 const up_vector,
									float3 const right_vector,
									float3 const view_location,
									particle::enum_particle_locked_axis locked_axis,
									particle::enum_particle_screen_alignment screen_alignment)
{
	backend::ref().set_vs_constant ( m_right_view_vector, right_vector);
	backend::ref().set_vs_constant ( m_up_view_vector, up_vector);
	backend::ref().set_vs_constant ( m_view_location, view_location);
	
	float f_use_align_by_dir			= 0.0f;
	float f_use_fixed_axis				= 0.0f;
	float f_locked_no_ratate_axis_index = -1.0f;
	
	float3 rotation_fixed_axis	(0,0,0);
	
	if (screen_alignment==particle::particle_screen_alignment_to_axis)
	{
		bool const is_rotate_x = locked_axis==particle::particle_locked_axis_rotate_x;
		bool const is_rotate_y = locked_axis==particle::particle_locked_axis_rotate_y;
		bool const is_rotate_z = locked_axis==particle::particle_locked_axis_rotate_z;
		
		rotation_fixed_axis = float3(
			is_rotate_x,
			is_rotate_y,
			is_rotate_z
		);
		
		if (is_rotate_x || is_rotate_y || is_rotate_z)
		{
			f_use_fixed_axis = 1.0f;
		}
		else
		{
			f_locked_no_ratate_axis_index = float(locked_axis);
		}
	}
	else
	{
		f_use_align_by_dir = (screen_alignment==particle::particle_screen_alignment_to_path) ? 1.0f : 0.0f;
	}
	
	backend::ref().set_vs_constant ( m_rotation_fixed_axis, rotation_fixed_axis);
	backend::ref().set_vs_constant ( m_locked_no_ratate_axis_index, f_locked_no_ratate_axis_index);
	backend::ref().set_vs_constant ( m_use_align_by_dir, f_use_align_by_dir);
	backend::ref().set_vs_constant ( m_use_fixed_axis, f_use_fixed_axis);
}

stage_particles::stage_particles( renderer_context* context): stage( context)
{
	m_enabled						= options::ref().m_enabled_particles_stage;
}

stage_particles::~stage_particles()
{
}

void stage_particles::execute( )
{
	if (!is_enabled())
	{
		execute_disabled();
		return;
	}
	
	BEGIN_CPUGPU_TIMER(statistics::ref().particles_stat_group.execute_time);

	PIX_EVENT( stage_particles);
	
	xray::particle::world* part_world = m_context->scene()->particle_world();
	
	if (!part_world)
	{
		m_context->set_w					( float4x4().identity() );
		return;
	}
	
	backend::ref().set_render_targets			(&*m_context->m_targets->m_rt_generic_0, 0, 0, 0);
	backend::ref().reset_depth_stencil_target	();

	particle::render_particle_emitter_instances_type emitters( g_allocator );
	
	part_world->get_render_emitter_instances(m_context->get_culling_vp(), emitters);
	
	for (particle::render_particle_emitter_instances_type::const_iterator it=emitters.begin(); it!=emitters.end(); ++it)
	{
		render::render_particle_emitter_instance* instance	= static_cast< render::render_particle_emitter_instance* >( *it );
		u32 const						  num_particles	= instance->get_num_particles();
		
		if (!num_particles)
			continue;
		
		xray::particle::enum_particle_render_mode particle_render_mode = m_context->scene_view()->get_particles_render_mode();
		
		if (particle_render_mode==particle::normal_particle_render_mode && instance->get_material_effects().stage_enable[forward_render_stage])
		{
			instance->get_material_effects().m_effects[forward_render_stage]->apply();
			
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
			
			statistics::ref().visibility_stat_group.num_particles.value++;
		}
		else
		{
			instance->draw_debug(m_context->get_v(), particle_render_mode);
		}
	}
	END_CPUGPU_TIMER;

	m_context->set_w					( float4x4().identity() );

	backend::ref().reset_render_targets();
	backend::ref().reset_depth_stencil_target();
}

} // namespace render
} // namespace xray
