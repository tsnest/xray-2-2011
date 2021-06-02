////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright ( C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/custom_config.h>
#include "stage_gbuffer.h"
#include "model_manager.h"

#include <xray/render/core/backend.h>
#include <xray/render/core/pix_event_wrapper.h>
#include "renderer_context.h"
#include "renderer_context_targets.h"
#include "scene.h"
#include "render_model.h"
#include <xray/render/core/res_effect.h>
#include <xray/render/core/res_geometry.h>
#include "terrain_render_model.h"
#include "terrain.h"
#include <xray/particle/world.h>
#include "stage_particles.h"
#include "scene_view.h"
#include "render_particle_emitter_instance.h"
#include <xray/render/facade/particles.h>
#include "statistics.h"

#include "speedtree.h"
#include "speedtree_tree.h"
#include "speedtree_forest.h"
#include "speedtree_convert_type.h"

#include "grass.h"

#include "material.h"

#include <xray/render/core/resource_manager.h>

#include "geometry_type.h"

#include <xray/render/core/options.h>

namespace xray {
namespace render {

stage_gbuffer::stage_gbuffer( renderer_context* context, bool is_pre_pass):
	stage(context),
	m_is_pre_pass(is_pre_pass)
{
	m_object_transparency_scale_parameter	= backend::ref().register_constant_host("object_transparency_scale", rc_float);
	m_c_start_corner						= backend::ref().register_constant_host("start_corner", rc_float);
	m_far_fog_color_and_distance			= backend::ref().register_constant_host("far_fog_color_and_distance", rc_float);
	m_near_fog_distance						= backend::ref().register_constant_host("near_fog_distance", rc_float);
	m_ambient_color							= backend::ref().register_constant_host("ambient_color", rc_float );
	m_c_environment_skylight_upper_color	= backend::ref().register_constant_host("environment_skylight_upper_color", rc_float);
	m_c_environment_skylight_lower_color	= backend::ref().register_constant_host("environment_skylight_lower_color", rc_float);
	m_c_environment_skylight_parameters		= backend::ref().register_constant_host("environment_skylight_parameters", rc_float);
	m_c_gs_test_constant					= backend::ref().register_constant_host("gs_test_constant", rc_float);
	
	m_enabled								= options::ref().m_enabled_g_stage;
	
	if (m_enabled)
	{
		if (m_is_pre_pass)
			m_enabled						= options::ref().m_enabled_g_stage_pre_pass;
		else
			m_enabled						= options::ref().m_enabled_g_stage_material_pass;
	}
}

stage_gbuffer::~stage_gbuffer()
{
}

void stage_gbuffer::execute()
{
	BEGIN_CPUGPU_TIMER(
		m_is_pre_pass ? 
		statistics::ref().gbuffer_stat_group.pre_pass_execute_time : 
		statistics::ref().gbuffer_stat_group.material_pass_execute_time
	);
	
	PIX_EVENT( stage_gbuffer);
	
	if (!is_enabled())
	{
		execute_disabled();
		return;
	}
	
	u32 tech_index							= m_is_pre_pass ? 0 : 1;
	
	if (m_is_pre_pass)
	{
		backend::ref().set_render_targets	( 
			&*m_context->m_targets->m_rt_position,
			&*m_context->m_targets->m_rt_normal,
			&*m_context->m_targets->m_rt_tangents,
			0
		);
		backend::ref().clear_render_targets	(
			math::color(10000.0f, 10000.0f, 10000.0f, 1.0f),
			math::color(0.0f, 0.0f, 0.0f, 1.0f),
			math::color(0.0f, 0.0f, 0.0f, 1.0f),
			math::color(0.0f, 0.0f, 0.0f, 1.0f)
		);
	}
	else
	{
		backend::ref().set_render_targets(
			&*m_context->m_targets->m_rt_generic_0, // DXGI_FORMAT_R16G16B16A16_FLOAT
			0,
			0,
			0
		);
		backend::ref().clear_render_targets(enum_target_rt0, math::color(0.0f, 0.0f, 0.0f, 1.0f));
	}
	
	m_context->scene()->select_models( m_context->get_culling_vp(), m_dynamic_visuals);

	render_surface_instances::iterator it_d			= m_dynamic_visuals.begin();
	render_surface_instances::const_iterator	end_d	= m_dynamic_visuals.end();
	
	for ( ; it_d != end_d; ++it_d)
	{
		render_surface_instance& instance = *(*it_d);
		material_effects& me		= instance.m_render_surface->get_material_effects();
		render_geometry& geometry	= instance.m_render_surface->m_render_geometry;
		
		if ( !me.stage_enable[geometry_render_stage] || 
			 !me.m_effects[geometry_render_stage].c_ptr())
			continue;
		
		if (!geometry.geom.c_ptr())
			continue;
		
		m_context->set_w					( *instance.m_transform );
		
		me.m_effects[geometry_render_stage]->apply(tech_index);
		backend::ref().set_ps_constant(m_object_transparency_scale_parameter, 1.0f);
		
		instance.set_constants		( );
		geometry.geom->apply		( );

		backend::ref().set_ps_constant(
			m_far_fog_color_and_distance,
			float4(
				m_context->scene_view()->post_process_parameters().environment_far_fog_color,
				m_context->scene_view()->post_process_parameters().environment_far_fog_distance
			)
		);
		
		backend::ref().set_ps_constant(
			m_near_fog_distance,
			m_context->scene_view()->post_process_parameters().environment_near_fog_distance
		);
		
		backend::ref().set_ps_constant(
			m_ambient_color, 
			float4(
				m_context->scene_view()->post_process_parameters().environment_ambient_color,
				0
			)
		);
		// TODO: remove skylight from sun pass, add the skylight_pass
		backend::ref().set_ps_constant( m_c_environment_skylight_upper_color, m_context->get_scene_view()->post_process_parameters().environment_skylight_upper_color );
		backend::ref().set_ps_constant( m_c_environment_skylight_lower_color, m_context->get_scene_view()->post_process_parameters().environment_skylight_lower_color );
		backend::ref().set_ps_constant( m_c_environment_skylight_parameters, m_context->get_scene_view()->post_process_parameters().environment_skylight_parameters );
		
		if (tech_index==0)
		{
			if (instance.m_render_surface && instance.m_render_surface->get_vertex_input_type()==skeletal_mesh_vertex_input_type)
				backend::ref().set_stencil_ref(all_geometry_type + skeleton_geometry_type);
			else
				backend::ref().set_stencil_ref(all_geometry_type + static_geometry_type);
		}
		
		//backend::ref().set_gs_constant(m_c_gs_test_constant, float4(20.0f, 10.0f, 5.0f, 0.0f));
		
		backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, geometry.primitive_count*3, 0, 0);

		if(tech_index==1)
			statistics::ref().visibility_stat_group.num_models.value++;
	}

	//
//////////////////////////////////////////////////////////////////////////
	typedef render::vector<terrain_render_model_instance_ptr>		terrain_ptr_cells;
	typedef terrain_ptr_cells::const_iterator		terrain_ptr_cells_cit;

	terrain_ptr_cells									terrain_cells;

////// Rendering terrain cells ///////////////////////////////////////////

	if (options::ref().m_enabled_draw_terrain && m_context->scene()->terrain())
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
			
			(*it_tr)->m_terrain_model->effect()->apply(tech_index);
			//(*it_tr)->m_terrain_model->render_geometry().geom->apply();
			
			float3	start_corner( 0.f, 0.f, 0.f);
			start_corner = (*it_tr)->m_terrain_model->transform().transform_position( start_corner);

			float3 distance_vector = start_corner + float3( (*it_tr)->m_terrain_model->physical_size()/2, 0, -(*it_tr)->m_terrain_model->physical_size()/2);
			distance_vector -= m_context->get_view_pos();

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
			
			backend::ref().set_ps_constant( m_object_transparency_scale_parameter, 1.0f);
			backend::ref().set_vs_constant( m_c_start_corner, float4( start_corner, 0));
			backend::ref().set_ps_constant( m_c_start_corner, float4( start_corner, 0));
			
			backend::ref().set_ps_constant(
				m_far_fog_color_and_distance,
				float4(
					m_context->scene_view()->post_process_parameters().environment_far_fog_color,
					m_context->scene_view()->post_process_parameters().environment_far_fog_distance
				)
			);
			
			backend::ref().set_ps_constant(
				m_near_fog_distance,
				m_context->scene_view()->post_process_parameters().environment_near_fog_distance
			);
			
			backend::ref().set_ps_constant(
				m_ambient_color, 
				float4(
					m_context->scene_view()->post_process_parameters().environment_ambient_color,
					0
				)
			);
			// TODO: remove skylight from sun pass, add the skylight_pass
			backend::ref().set_ps_constant( m_c_environment_skylight_upper_color, m_context->get_scene_view()->post_process_parameters().environment_skylight_upper_color );
			backend::ref().set_ps_constant( m_c_environment_skylight_lower_color, m_context->get_scene_view()->post_process_parameters().environment_skylight_lower_color );
			backend::ref().set_ps_constant( m_c_environment_skylight_parameters, m_context->get_scene_view()->post_process_parameters().environment_skylight_parameters );

			if (tech_index==0)
				backend::ref().set_stencil_ref(all_geometry_type + terrain_geometry_type);

			backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, primitive_count/*(*it_tr)->m_terrain_model->render_geometry().primitive_count*3*/, 0, 0);
			statistics::ref().visibility_stat_group.num_triangles.value += primitive_count / 3;
		}
	}
	
	
	// SpeedTree
	if (options::ref().m_enabled_draw_speedtree)
	{
		speedtree_forest::tree_render_info_array_type visible_trees;
		math::float3 const& view_position = m_context->get_v_inverted().c.xyz();
		m_context->scene()->get_speedtree_forest()->get_visible_tree_components(m_context, view_position, true, visible_trees);
		
		BEGIN_CPUGPU_TIMER(statistics::ref().speedtree_stat_group.render_time);
		for (speedtree_forest::tree_render_info_array_type::iterator it=visible_trees.begin(); it!=visible_trees.end(); ++it)
		{
			if (!it->tree_component->get_material_effects().stage_enable[geometry_render_stage] || !it->tree_component->get_material_effects().m_effects[geometry_render_stage])
				continue;
			
			//if (it->tree_component->get_geometry_type()==SpeedTree::GEOMETRY_TYPE_VERTICAL_BILLBOARDS)
			//	continue;
			
			it->tree_component->get_material_effects().m_effects[geometry_render_stage]->apply(tech_index);
			
			backend::ref().set_ps_constant( m_object_transparency_scale_parameter, 1.0f);
			
			m_context->scene()->get_speedtree_forest()->get_speedtree_wind_parameters().set		(it->tree_component->m_parent->GetWind());
			m_context->scene()->get_speedtree_forest()->get_speedtree_common_parameters().set	(m_context, it->tree_component, view_position);
			if (it->instance)
				m_context->set_w																	(
					m_context->scene()->get_speedtree_forest()->get_instance_transform(*it->instance)
				);
			else
				m_context->set_w																	(math::float4x4().identity());

			
			backend::ref().set_ps_constant(
				m_far_fog_color_and_distance,
				float4(
					m_context->scene_view()->post_process_parameters().environment_far_fog_color,
					m_context->scene_view()->post_process_parameters().environment_far_fog_distance
				)
			);
			
			backend::ref().set_ps_constant(
				m_near_fog_distance,
				m_context->scene_view()->post_process_parameters().environment_near_fog_distance
			);

			backend::ref().set_ps_constant(
				m_ambient_color, 
				float4(
					m_context->scene_view()->post_process_parameters().environment_ambient_color,
					0
				)
			);
			// TODO: remove skylight from sun pass, add the skylight_pass
			backend::ref().set_ps_constant( m_c_environment_skylight_upper_color, m_context->get_scene_view()->post_process_parameters().environment_skylight_upper_color );
			backend::ref().set_ps_constant( m_c_environment_skylight_lower_color, m_context->get_scene_view()->post_process_parameters().environment_skylight_lower_color );
			backend::ref().set_ps_constant( m_c_environment_skylight_parameters, m_context->get_scene_view()->post_process_parameters().environment_skylight_parameters );
			
			if (tech_index==0)
				backend::ref().set_stencil_ref(all_geometry_type + speedtree_geometry_type);
			
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
		END_CPUGPU_TIMER;
	}
	
	m_context->scene()->get_grass()->render(m_context, (u32)geometry_render_stage);
	
	xray::particle::world* part_world = m_context->scene()->particle_world();
	
	if (part_world)
	{
		particle::render_particle_emitter_instances_type emitters( g_allocator );
		
		part_world->get_render_emitter_instances(m_context->get_culling_vp(), emitters);
		
		for (particle::render_particle_emitter_instances_type::const_iterator it=emitters.begin(); it!=emitters.end(); ++it)
		{
			render_particle_emitter_instance* instance		= static_cast_checked<render_particle_emitter_instance*>(*it);
			u32 const						  num_particles	= instance->get_num_particles();
			
			if (!num_particles)
				continue;
			
			xray::particle::enum_particle_render_mode particle_render_mode = m_context->scene_view()->get_particles_render_mode();
			
			if (particle_render_mode==particle::normal_particle_render_mode && instance->get_material_effects().stage_enable[geometry_render_stage])
			{
				instance->get_material_effects().m_effects[geometry_render_stage]->apply(tech_index);
				
				particle_shader_constants::ref().set(
					m_context->get_v_inverted().transform_direction(float3(0,1000,0)).normalize(),
					m_context->get_v_inverted().transform_direction(float3(1000,0,0)).normalize(),
					m_context->get_v_inverted().lines[3].xyz(),
					instance->locked_axis(),
					instance->screen_alignment()
				);
				particle_shader_constants::ref().set_time(m_context->m_current_time);
				
				backend::ref().set_ps_constant(
					m_far_fog_color_and_distance,
					float4(
						m_context->scene_view()->post_process_parameters().environment_far_fog_color,
						m_context->scene_view()->post_process_parameters().environment_far_fog_distance
					)
				);
				
				backend::ref().set_ps_constant(
					m_near_fog_distance,
					m_context->scene_view()->post_process_parameters().environment_near_fog_distance
				);

				backend::ref().set_ps_constant(
					m_ambient_color, 
					float4(
						m_context->scene_view()->post_process_parameters().environment_ambient_color,
						0
					) 
				);
				m_context->set_w(instance->transform());
				// TODO: remove skylight from sun pass, add the skylight_pass
				backend::ref().set_ps_constant( m_c_environment_skylight_upper_color, m_context->get_scene_view()->post_process_parameters().environment_skylight_upper_color );
				backend::ref().set_ps_constant( m_c_environment_skylight_lower_color, m_context->get_scene_view()->post_process_parameters().environment_skylight_lower_color );
				backend::ref().set_ps_constant( m_c_environment_skylight_parameters, m_context->get_scene_view()->post_process_parameters().environment_skylight_parameters );

				if (tech_index==0)
					backend::ref().set_stencil_ref(all_geometry_type + particle_geometry_type);
				
				instance->render(m_context->get_v_inverted().lines[3].xyz(), num_particles);
				
				statistics::ref().visibility_stat_group.num_particles.value++;
			}
			else
			{
				instance->draw_debug(m_context->get_v(), particle_render_mode);
			}
		}
	}
/*	
	/*if (!m_is_pre_pass)
	{
		device::ref().d3d_context()->Flush();
		
		resource_manager::ref().copy2D		(
			&*m_context->m_targets->m_t_generic_1,
			0,
			0,
			&*m_context->m_targets->m_t_generic_0,
			0,
			0,
			m_context->m_targets->m_t_generic_0->width(),
			m_context->m_targets->m_t_generic_0->height()
		);
	}*/
	END_CPUGPU_TIMER;
	
	//if (m_is_pre_pass)
	backend::ref().reset_render_targets();
	backend::ref().reset_depth_stencil_target();
	
	m_context->set_w					(float4x4().identity());
}

} // namespace render
} // namespace xray

