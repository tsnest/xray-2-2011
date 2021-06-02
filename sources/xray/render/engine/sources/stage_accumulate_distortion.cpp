////////////////////////////////////////////////////////////////////////////
//	Created		: 01.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/custom_config.h>
#include "stage_accumulate_distortion.h"
#include <xray/render/core/backend.h>
#include "renderer_context.h"
#include "renderer_context_targets.h"
#include <xray/render/core/pix_event_wrapper.h>
#include "scene.h"
#include "render_particle_emitter_instance.h"
#include <xray/particle/world.h>
#include <xray/render/core/res_effect.h>
#include "stage_particles.h"
#include "render_model.h"
#include <xray/render/core/res_geometry.h>
#include "statistics.h"
#include "material.h"

#include <xray/render/core/options.h>

namespace xray {
namespace render {
	
stage_accumulate_distortion::stage_accumulate_distortion(renderer_context* context) : stage( context)
{
	m_enabled = options::ref().m_enabled_distortion_stage;
}

stage_accumulate_distortion::~stage_accumulate_distortion()
{
	
}

void stage_accumulate_distortion::execute()
{
	//resource_manager::ref().copy2D(m_context->m_t_generic_0.c_ptr(), 0, 0, m_context->m_t_generic_1.c_ptr(), 0, 0, m_context->m_t_generic_1->width(), m_context->m_t_generic_1->height());
	
	if (!is_enabled())
	{
		execute_disabled();
		return;
	}
	
	BEGIN_CPUGPU_TIMER(statistics::ref().distortion_pass_stat_group.accumulate_time);
	
	backend::ref().set_render_targets( &*m_context->m_targets->m_rt_distortion, 0, 0, 0);
	backend::ref().clear_render_targets	( math::color( 0.0f, 0.0f, 0.0f, 0.f));
	
	{
		PIX_EVENT( stage_accumulate_distortion);
		
		xray::particle::world* part_world = m_context->scene()->particle_world();
		
		if (part_world)
		{
			particle::render_particle_emitter_instances_type emitters( g_allocator );
			
			part_world->get_render_emitter_instances(m_context->get_culling_vp(), emitters);
			
			for (particle::render_particle_emitter_instances_type::const_iterator it=emitters.begin(); it!=emitters.end(); ++it)
			{
				render::render_particle_emitter_instance& instance		= static_cast< render::render_particle_emitter_instance& >( **it );
				u32 const						  num_particles	= instance.get_num_particles();
				
				if (!num_particles || !instance.get_material_effects().stage_enable[accumulate_distortion_render_stage] || !instance.get_material_effects().m_effects[accumulate_distortion_render_stage])
					continue;
				
				instance.get_material_effects().m_effects[accumulate_distortion_render_stage]->apply();
				
				particle_shader_constants::ref().set(
					m_context->get_v_inverted().transform_direction(float3(0,1000,0)).normalize(),
					m_context->get_v_inverted().transform_direction(float3(1000,0,0)).normalize(),
					m_context->get_v_inverted().lines[3].xyz(),
					instance.locked_axis(),
					instance.screen_alignment()
				);
				particle_shader_constants::ref().set_time(m_context->m_current_time);
				
				m_context->set_w( instance.transform() );
				
				instance.render(m_context->get_v_inverted().lines[3].xyz(), num_particles);
			}
			backend::ref().flush();
		}
	}
	
	{
		PIX_EVENT( stage_distortion_models);
		
		m_context->scene()->select_models( m_context->get_culling_vp(), m_dynamic_visuals);
		
		render_surface_instances::iterator it_d			= m_dynamic_visuals.begin();
		render_surface_instances::const_iterator	end_d	= m_dynamic_visuals.end();
		
		for ( ; it_d != end_d; ++it_d)
		{
			render_surface_instance& instance = *(*it_d);
			material_effects& me		= instance.m_render_surface->get_material_effects();
			render_geometry& geometry	= instance.m_render_surface->m_render_geometry;

			if ( !me.stage_enable[accumulate_distortion_render_stage])
				continue;
			
			m_context->set_w						( *instance.m_transform );
			me.m_effects[accumulate_distortion_render_stage]->apply();
			geometry.geom->apply();
			backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, geometry.primitive_count*3, 0, 0);
		}
	}
	
	END_CPUGPU_TIMER;
	backend::ref().reset_render_targets();
	backend::ref().reset_depth_stencil_target();
	m_context->set_w					( float4x4().identity() );
}

} // namespace render
} // namespace xray
