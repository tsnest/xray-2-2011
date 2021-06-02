////////////////////////////////////////////////////////////////////////////
//	Created		: 12.01.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "stage_view_mode.h"
#include <xray/render/core/effect_manager.h>
#include <xray/render/facade/scene_view_mode.h>
#include "effect_editor_wireframe_accumulation.h"
#include "effect_editor_shader_complexity.h"
#include "effect_editor_texture_density.h"
#include "effect_editor_apply_wireframe.h"
#include "effect_editor_show_miplevel.h"
#include "effect_editor_geometry_complexity.h"
#include <xray/render/core/backend.h>
#include <xray/render/core/render_target.h>
#include "renderer_context.h"
#include "vertex_formats.h"
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/pix_event_wrapper.h>
#include "renderer_context_targets.h"
#include "scene.h"
#include "render_model.h"
#include <xray/particle/world.h>
#include "render_particle_emitter_instance.h"
#include "stage_particles.h"
#include <xray/render/core/res_effect.h>
#include <xray/render/core/effect_options_descriptor.h>
#include <xray/render/core/res_declaration.h>
#include <xray/render/facade/vertex_input_type.h>
#include <xray/render/core/destroy_data_helper.h>
#include "statistics.h"
#include "speedtree_forest.h"
#include "speedtree_tree.h"

#include "terrain_render_model.h"
#include "terrain.h"

#include "material.h"

#include <xray/render/core/options.h>

#include "effect_editor_accumulate_overdraw.h"
#include "effect_editor_show_overdraw.h"

namespace xray {
namespace render {

stage_view_mode::stage_view_mode( renderer_context* context): 
	stage( context)
{
	for (u32 i=0; i<num_vertex_input_types; i++)
	{
		if (i==post_process_vertex_input_type)
			continue;

		byte data[Kb];
		effect_options_descriptor desc(data, Kb);
		desc["vertex_input_type"] = (enum_vertex_input_type)i;
		
		effect_manager::ref().create_effect<effect_editor_wireframe_accumulation>(&m_editor_wireframe_accumulation_effect[i], desc);
		effect_manager::ref().create_effect<effect_editor_texture_density>(&m_editor_texture_density_effect[i], desc);
		effect_manager::ref().create_effect<effect_editor_shader_complexity>(&m_editor_shader_complexity_effect[i], desc);
		effect_manager::ref().create_effect<effect_editor_show_miplevel>(&m_editor_show_miplevel_effect[i], desc);
		effect_manager::ref().create_effect<effect_editor_accumulate_overdraw>(&m_editor_accumulate_overdraw_effect[i], desc);
		effect_manager::ref().create_effect<effect_editor_geometry_complexity>(&m_editor_geometry_complexity_effect[i], desc);
	}
	
	effect_manager::ref().create_effect<effect_editor_apply_wireframe>(&m_editor_apply_wireframe_shader);
	effect_manager::ref().create_effect<effect_editor_show_overdraw>(&m_editor_show_overdraw_shader);
	
	
	m_shader_complexity_parameter				= backend::ref().register_constant_host("shader_complexity", rc_float);
	m_shader_complexity_min_parameter			= backend::ref().register_constant_host("shader_complexity_min", rc_float);
	m_shader_complexity_max_parameter			= backend::ref().register_constant_host("shader_complexity_max", rc_float);
	m_geometry_complexity_parameters			= backend::ref().register_constant_host("geometry_complexity_parameters", rc_float);
	m_current_max_texture_dimension_parameter	= backend::ref().register_constant_host("current_max_texture_dimension", rc_float);

	m_c_start_corner							= backend::ref().register_constant_host( "start_corner", rc_float);
}

bool stage_view_mode::is_effects_ready() const
{
	for (u32 i = 0; i < num_vertex_input_types; i++)
	{
		if (i==post_process_vertex_input_type)
			continue;
		
		if (   m_editor_wireframe_accumulation_effect[i].c_ptr()==NULL 
			|| m_editor_texture_density_effect[i].c_ptr()==NULL 
			|| m_editor_shader_complexity_effect[i].c_ptr()==NULL 
			|| m_editor_show_miplevel_effect[i].c_ptr()==NULL 
			|| m_editor_geometry_complexity_effect[i].c_ptr()==NULL
			|| m_editor_accumulate_overdraw_effect[i].c_ptr()==NULL)
		return false;
	}
	
	return m_editor_apply_wireframe_shader.c_ptr()!=NULL && m_editor_show_overdraw_shader.c_ptr()!=NULL;
}

bool stage_view_mode::is_support_view_mode(scene_view_mode view_mode) const
{
	switch (view_mode)
	{
		case geometry_complexity_view_mode:
		case shader_complexity_view_mode:
		case texture_density_view_mode:
		case wireframe_two_sided_view_mode:
		case wireframe_view_mode:
		case miplevel_view_mode:
		case overdraw_view_mode:
			return true;
	}
	return false;
}

// TODO: Make as a general functions.
static void fill_surface(ref_rt surf, renderer_context* context, bool is_need_clear = true)
{
	float w = float(surf->width());
	float h = float(surf->height());
	float z = 0.0f;
	
	backend::ref().set_render_targets( &*surf, 0, 0, 0);
	
	if (is_need_clear)
	{
		backend::ref().clear_render_targets( math::color(1.0f, 1.0f, 1.0f, 0.0f));
		backend::ref().reset_depth_stencil_target();
	}
	
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

void stage_view_mode::execute(scene_view_mode view_mode)
{
	PIX_EVENT( stage_view_mode);
	
	if (!is_effects_ready())
		return;
	
	if (!is_enabled())
	{
		execute_disabled();
		return;
	}
	
	u32 max_texture_size_x, max_texture_size_y;
	
	float shader_complexity_min = 25.0f;
	float shader_complexity_max = 50.0f;
	
	if (view_mode==wireframe_view_mode || view_mode==wireframe_two_sided_view_mode)
	{
		backend::ref().set_render_targets( &*m_context->m_targets->m_rt_generic_0, 0, 0, 0);
		backend::ref().clear_render_targets( math::color( 0.f, 0.f, 0.f, 0.f));
	}
	//else if (view_mode == overdraw_view_mode)
	//{
	//	backend::ref().set_render_targets( 0, 0, 0, 0);
	//}
	else
	{
		backend::ref().set_render_targets( &*m_context->m_targets->m_rt_present, 0, 0, 0);
		backend::ref().clear_render_targets( math::color( 0.f, 0.f, 0.f, 0.f));
	}
	
	backend::ref().reset_depth_stencil_target();
	backend::ref().clear_depth_stencil( D3D_CLEAR_DEPTH | D3D_CLEAR_STENCIL, 1.0f, 0);	
	
	xray::particle::world* part_world = m_context->scene()->particle_world();
	
	m_context->scene()->select_models( m_context->get_culling_vp(), m_dynamic_visuals);
	
	render_surface_instances::iterator it_d			= m_dynamic_visuals.begin();
	render_surface_instances::const_iterator	end_d	= m_dynamic_visuals.end();
	
	// TODO: get geom type from instance?
	
	for ( ; it_d != end_d; ++it_d)
	{
		render_surface_instance& instance = *(*it_d);
		material_effects& me		= instance.m_render_surface->get_material_effects();
		render_geometry& geometry	= instance.m_render_surface->m_render_geometry;
		
		if (!geometry.geom.c_ptr())
			continue;
		
		m_context->set_w						( *instance.m_transform );
		instance.set_constants					( );
		
		u32 instruction_count = 0;
		if (me.stage_enable[geometry_render_stage])
		{
			instruction_count = me.get_render_complexity();//m_effects[geometry_render_stage]->get_technique(0)->get_pass(0)->get_ps()->hardware_shader()->data().instruction_count;
		}
		
		enum_vertex_input_type vit				= instance.m_render_surface->get_vertex_input_type();
		
		switch (view_mode)
		{
			case wireframe_view_mode:
				m_editor_wireframe_accumulation_effect[vit]->apply(0);
				break;
			
			case wireframe_two_sided_view_mode:
				m_editor_wireframe_accumulation_effect[vit]->apply(1);
				break;
			
			case shader_complexity_view_mode:
				m_editor_shader_complexity_effect[vit]->apply();
				backend::ref().set_ps_constant(m_shader_complexity_parameter, float(instruction_count));
				backend::ref().set_ps_constant(m_shader_complexity_min_parameter, shader_complexity_min);
				backend::ref().set_ps_constant(m_shader_complexity_max_parameter, shader_complexity_max);
				break;
			
			case overdraw_view_mode:
				m_editor_accumulate_overdraw_effect[vit]->apply();
				break;
			
			case miplevel_view_mode:
				m_editor_show_miplevel_effect[vit]->apply();
				me.get_max_used_texture_dimension(max_texture_size_x, max_texture_size_y);
				backend::ref().set_ps_constant(m_current_max_texture_dimension_parameter, float4(float(max_texture_size_x), float(max_texture_size_y), 0.0f, 0.0f));
				break;
			
			case texture_density_view_mode:
				m_editor_texture_density_effect[vit]->apply();
				me.get_max_used_texture_dimension(max_texture_size_x, max_texture_size_y);
				backend::ref().set_ps_constant(m_current_max_texture_dimension_parameter, float4(float(max_texture_size_x), float(max_texture_size_y), 0.0f, 0.0f));
				break;

			case geometry_complexity_view_mode:
				m_editor_geometry_complexity_effect[vit]->apply();
				backend::ref().set_ps_constant(m_geometry_complexity_parameters, float4(float(geometry.primitive_count),0.0f,0.0f,0.0f));
				break;
		}
		
		geometry.geom->apply();
		
		backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, geometry.primitive_count*3, 0, 0);
	}
	
	if (options::ref().m_enabled_draw_speedtree)
	{
		speedtree_forest::tree_render_info_array_type visible_trees;
		math::float3 view_position = m_context->get_v_inverted().c.xyz();
		m_context->scene()->get_speedtree_forest()->get_visible_tree_components(m_context, view_position, true, visible_trees);

		for (speedtree_forest::tree_render_info_array_type::iterator it=visible_trees.begin(); it!=visible_trees.end(); ++it)
		{
			u32 instruction_count = 0;
			
			if (it->tree_component->get_geometry_type()==SpeedTree::GEOMETRY_TYPE_VERTICAL_BILLBOARDS)
				continue;
			
			if (it->tree_component->get_material_effects().stage_enable[geometry_render_stage])
			{
				instruction_count = it->tree_component->get_material_effects().get_render_complexity();
			}
			
			u32 const vertex_input_type_index = u32(it->tree_component->get_material_effects().get_vertex_input_type());
			
			switch (view_mode)
			{
				case wireframe_view_mode:
					m_editor_wireframe_accumulation_effect[vertex_input_type_index]->apply(0);
					break;

				case wireframe_two_sided_view_mode:
					m_editor_wireframe_accumulation_effect[vertex_input_type_index]->apply(1);
					break;
				
				case shader_complexity_view_mode:
					m_editor_shader_complexity_effect[vertex_input_type_index]->apply();
					backend::ref().set_ps_constant(m_shader_complexity_parameter, float(instruction_count));
					backend::ref().set_ps_constant(m_shader_complexity_min_parameter, shader_complexity_min);
					backend::ref().set_ps_constant(m_shader_complexity_max_parameter, shader_complexity_max);
					break;
				
				case miplevel_view_mode:
					m_editor_show_miplevel_effect[vertex_input_type_index]->apply();
					it->tree_component->get_material_effects().get_max_used_texture_dimension(max_texture_size_x, max_texture_size_y);
					backend::ref().set_ps_constant(m_current_max_texture_dimension_parameter, float4(float(max_texture_size_x), float(max_texture_size_y), 0.0f, 0.0f));
					break;
				
				case overdraw_view_mode:
					m_editor_accumulate_overdraw_effect[vertex_input_type_index]->apply();
					break;
				
				case texture_density_view_mode:
					m_editor_texture_density_effect[vertex_input_type_index]->apply();
					it->tree_component->get_material_effects().get_max_used_texture_dimension(max_texture_size_x, max_texture_size_y);
					backend::ref().set_ps_constant(m_current_max_texture_dimension_parameter, float4(float(max_texture_size_x), float(max_texture_size_y), 0.0f, 0.0f));
					break;

				case geometry_complexity_view_mode:
					m_editor_geometry_complexity_effect[vertex_input_type_index]->apply();
					backend::ref().set_ps_constant(m_geometry_complexity_parameters, float4(float(it->tree_component->m_render_geometry.primitive_count),0.0f,0.0f,0.0f));
					break;
			}		
			
			m_context->scene()->get_speedtree_forest()->get_speedtree_wind_parameters().set		(it->tree_component->m_parent->GetWind());
			m_context->scene()->get_speedtree_forest()->get_speedtree_common_parameters().set	(m_context, it->tree_component, view_position);
			if (it->instance)
				m_context->set_w																	(
					m_context->scene()->get_speedtree_forest()->get_instance_transform(*it->instance)
				);
			else
				m_context->set_w																	(math::float4x4().identity());

			
	// 		if (it->tree_component->get_geometry_type()==SpeedTree::GEOMETRY_TYPE_VERTICAL_BILLBOARDS)
	// 		{
	// 			//
	// 		//	m_context->scene()->get_speedtree_forest()->get_speedtree_billboard_parameters().set(m_context, it->tree_component);
	// 		//	it->tree_component->m_render_geometry.geom->apply();
	// 		//	backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, it->tree_component->m_render_geometry.index_count, 0, 0);
	// 		}
	// 		else
			{
				m_context->scene()->get_speedtree_forest()->get_speedtree_tree_parameters().set(it->tree_component, it->instance, it->instance_lod);
				it->tree_component->m_render_geometry.geom->apply();
				backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, it->lod->num_indices, it->lod->start_index, 0);
				
				statistics::ref().visibility_stat_group.num_triangles.value += it->lod->num_indices / 3;
				statistics::ref().visibility_stat_group.num_speedtree_instances.value++;
			}
		}
	}
/*	
//////////////////////////////////////////////////////////////////////////
	typedef render::vector<terrain_render_model_instance_ptr>		terrain_ptr_cells;
	typedef terrain_ptr_cells::const_iterator		terrain_ptr_cells_cit;

	terrain_ptr_cells									terrain_cells;

////// Rendering terrain cells ///////////////////////////////////////////

	if( options::ref().m_enabled_draw_terrain && m_context->scene()->terrain())
	{
		m_context->scene()->select_terrain_cells( m_context->get_culling_vp(), terrain_cells);

		terrain_ptr_cells_cit it_tr = terrain_cells.begin();
		terrain_ptr_cells_cit en_tr = terrain_cells.end();

		m_context->set_w( math::float4x4().identity() );

		for( ; it_tr != en_tr; ++it_tr)
		{
			//(*it_tr)->m_terrain_model->effect()->apply();
			//(*it_tr)->m_terrain_model->render_geometry().geom->apply();
			
			u32 instruction_count = 20;
			u32 primitive_count = 0;
			float3	start_corner( 0.f, 0.f, 0.f);
			start_corner = (*it_tr)->m_terrain_model->transform().transform_position( start_corner);

			float3 distance_vector = start_corner + float3( (*it_tr)->m_terrain_model->physical_size()/2, 0, -(*it_tr)->m_terrain_model->physical_size()/2);
			distance_vector -= m_context->get_view_pos();

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

			u32 const vertex_input_type_index = u32(terrain_vertex_input_type);

			switch (view_mode)
			{
			case wireframe_view_mode:
				m_editor_wireframe_accumulation_effect[vertex_input_type_index]->apply(0);
				break;
			case wireframe_two_sided_view_mode:
				m_editor_wireframe_accumulation_effect[vertex_input_type_index]->apply(1);
				break;

			case shader_complexity_view_mode:
				m_editor_shader_complexity_effect[vertex_input_type_index]->apply();
				backend::ref().set_ps_constant(m_shader_complexity_parameter, float(instruction_count));
				backend::ref().set_ps_constant(m_shader_complexity_min_parameter, shader_complexity_min);
				backend::ref().set_ps_constant(m_shader_complexity_max_parameter, shader_complexity_max);
				break;

			case miplevel_view_mode:
				m_editor_show_miplevel_effect[vertex_input_type_index]->apply();
				(*it_tr)->m_terrain_model->effect()->get_max_used_texture_dimension(max_texture_size_x, max_texture_size_y);
				backend::ref().set_ps_constant(m_current_max_texture_dimension_parameter, float4(float(max_texture_size_x), float(max_texture_size_y), 0.0f, 0.0f));
				break;

			case texture_density_view_mode:
				m_editor_texture_density_effect[vertex_input_type_index]->apply();
				(*it_tr)->m_terrain_model->effect()->get_max_used_texture_dimension(max_texture_size_x, max_texture_size_y);
				backend::ref().set_ps_constant(m_current_max_texture_dimension_parameter, float4(float(max_texture_size_x), float(max_texture_size_y), 0.0f, 0.0f));
				break;

			case geometry_complexity_view_mode:
				m_editor_geometry_complexity_effect[vertex_input_type_index]->apply();
				backend::ref().set_ps_constant(m_geometry_complexity_parameters, float4(float(primitive_count),0.0f,0.0f,0.0f));
				break;
			}		

			backend::ref().set_vs_constant( m_c_start_corner, float4( start_corner, 0));
			backend::ref().set_ps_constant( m_c_start_corner, float4( start_corner, 0));
			
			statistics::ref().visibility_stat_group.num_triangles.value += primitive_count / 3;
		}
	}
*/	
	
	if (part_world)
	{
		
		particle::render_particle_emitter_instances_type	emitters( g_allocator );
		
		part_world->get_render_emitter_instances(m_context->get_culling_vp(), emitters);
		
		for (particle::render_particle_emitter_instances_type::const_iterator it=emitters.begin(); it!=emitters.end(); ++it)
		{
			render::render_particle_emitter_instance& instance	= static_cast< render::render_particle_emitter_instance& >( **it );
			u32 const num_particles										= instance.get_num_particles();
			
			if (!num_particles)
				continue;
			
			bool draw_particles = true;
			u32 instruction_count = 0;
			if (instance.get_material_effects().stage_enable[forward_render_stage])
			{
				instruction_count = instance.get_material_effects().get_render_complexity();//m_effects[forward_render_stage]->get_technique(0)->get_pass(0)->get_ps()->hardware_shader()->data().instruction_count;
			}
			
			switch (view_mode)
			{
				case wireframe_view_mode:
					if (instance.vertex_type()==particle::particle_vertex_type_billboard)
						m_editor_wireframe_accumulation_effect[particle_vertex_input_type]->apply(0);
					else if (instance.vertex_type()==particle::particle_vertex_type_billboard_subuv)
						m_editor_wireframe_accumulation_effect[particle_subuv_vertex_input_type]->apply(0);
					else
						m_editor_wireframe_accumulation_effect[particle_beamtrail_vertex_input_type]->apply(0);
				break;
				case wireframe_two_sided_view_mode:
					if (instance.vertex_type()==particle::particle_vertex_type_billboard)
						m_editor_wireframe_accumulation_effect[particle_vertex_input_type]->apply(1);
					else if (instance.vertex_type()==particle::particle_vertex_type_billboard_subuv)
						m_editor_wireframe_accumulation_effect[particle_subuv_vertex_input_type]->apply(1);
					else
						m_editor_wireframe_accumulation_effect[particle_beamtrail_vertex_input_type]->apply(1);
				break;
				
				case shader_complexity_view_mode:
					if (instance.vertex_type()==particle::particle_vertex_type_billboard)
						m_editor_shader_complexity_effect[particle_vertex_input_type]->apply();
					else if (instance.vertex_type()==particle::particle_vertex_type_billboard_subuv)
						m_editor_shader_complexity_effect[particle_subuv_vertex_input_type]->apply(0);
					else
						m_editor_shader_complexity_effect[particle_beamtrail_vertex_input_type]->apply();
					backend::ref().set_ps_constant(m_shader_complexity_parameter, float(instruction_count));
					backend::ref().set_ps_constant(m_shader_complexity_min_parameter, shader_complexity_min);
					backend::ref().set_ps_constant(m_shader_complexity_max_parameter, shader_complexity_max);
				break;
				
				case miplevel_view_mode:
					instance.get_material_effects().get_max_used_texture_dimension(max_texture_size_x, max_texture_size_y);

					if (instance.vertex_type()==particle::particle_vertex_type_billboard)
						m_editor_show_miplevel_effect[particle_vertex_input_type]->apply();
					else if (instance.vertex_type()==particle::particle_vertex_type_billboard_subuv)
						m_editor_show_miplevel_effect[particle_subuv_vertex_input_type]->apply(0);
					else
						m_editor_show_miplevel_effect[particle_beamtrail_vertex_input_type]->apply();

					backend::ref().set_ps_constant(m_current_max_texture_dimension_parameter, float4(float(max_texture_size_x), float(max_texture_size_y), 0.0f, 0.0f));
				break;
				
				// TODO: fix for particles.
				case texture_density_view_mode:
					instance.get_material_effects().get_max_used_texture_dimension(max_texture_size_x, max_texture_size_y);
					
					if (instance.vertex_type()==particle::particle_vertex_type_billboard)
						m_editor_texture_density_effect[particle_vertex_input_type]->apply();
					else if (instance.vertex_type()==particle::particle_vertex_type_billboard_subuv)
						m_editor_texture_density_effect[particle_subuv_vertex_input_type]->apply(0);
					else
						m_editor_texture_density_effect[particle_beamtrail_vertex_input_type]->apply();
					
					backend::ref().set_ps_constant(m_current_max_texture_dimension_parameter, float4(float(max_texture_size_x), float(max_texture_size_y), 0.0f, 0.0f));
				break;

				case geometry_complexity_view_mode:
					if (instance.vertex_type()==particle::particle_vertex_type_billboard)
						m_editor_geometry_complexity_effect[particle_vertex_input_type]->apply();
					else if (instance.vertex_type()==particle::particle_vertex_type_billboard_subuv)
						m_editor_geometry_complexity_effect[particle_subuv_vertex_input_type]->apply(0);
					else
						m_editor_geometry_complexity_effect[particle_beamtrail_vertex_input_type]->apply();
					
					backend::ref().set_ps_constant(m_geometry_complexity_parameters, float4(float(instance.get_num_particles() * 2),0.0f,0.0f,0.0f));
				break;
				
				case overdraw_view_mode:
					if (instance.vertex_type()==particle::particle_vertex_type_billboard)
						m_editor_accumulate_overdraw_effect[particle_vertex_input_type]->apply();
					else if (instance.vertex_type()==particle::particle_vertex_type_billboard_subuv)
						m_editor_accumulate_overdraw_effect[particle_subuv_vertex_input_type]->apply(0);
					else
						m_editor_accumulate_overdraw_effect[particle_beamtrail_vertex_input_type]->apply();
					break;
					
				default: 
					draw_particles = false;
				break;
			}
			
			if (draw_particles)
			{
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
		}
	}
	
	if (view_mode == wireframe_view_mode || view_mode == wireframe_two_sided_view_mode)
	{
		m_editor_apply_wireframe_shader->apply();
		fill_surface(m_context->m_targets->m_rt_present, m_context);
	}
	else if (view_mode == overdraw_view_mode)
	{
		backend::ref().set_render_targets( &*m_context->m_targets->m_rt_present, 0, 0, 0);
		backend::ref().clear_render_targets(0.0f, 0.25f, 0.0f, 0.0f);
		
		for (u32 layer_index = 1; layer_index < effect_editor_show_overdraw::num_overdraw_layers; layer_index++)
		{
			m_editor_show_overdraw_shader->apply(layer_index);
			fill_surface(m_context->m_targets->m_rt_present, m_context, false);
		}
	}
	
	m_context->set_w				(float4x4().identity());
	/*
	if( options::ref().m_enabled_draw_terrain && m_context->scene()->terrain())
	{
		typedef render::vector<render_model_terrain_instance_ptr>		terrain_ptr_cells;
		typedef terrain_ptr_cells::const_iterator		terrain_ptr_cells_cit;
		
		terrain_ptr_cells									terrain_cells;
		
		m_context->scene()->select_terrain_cells( m_context->get_culling_vp(), terrain_cells);
		
		terrain_ptr_cells_cit it_tr = terrain_cells.begin();
		terrain_ptr_cells_cit en_tr = terrain_cells.end();
		
		m_context->set_w( math::float4x4().identity() );
		
		for( ; it_tr != en_tr; ++it_tr)
		{
			switch (view_mode)
			{
				case wireframe_view_mode:
					m_editor_wireframe_accumulation_effect->apply(effect_editor_wireframe_accumulation::terrain_vertex);
				break;
			}
			
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
			
			constant_host const* m_c_start_corner = backend::ref().find_constant_host( "start_corner");
			backend::ref().set_vs_constant( m_c_start_corner, float4( start_corner, 0));
			
			backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, primitive_count, 0, 0);
		}
	}
*/
}

} // namespace render
} // namespace xray
