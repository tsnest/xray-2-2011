////////////////////////////////////////////////////////////////////////////
//	Created		: 02.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/backend.h>
#include "stage_forward.h"
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

#include "speedtree.h"
#include "speedtree_tree.h"
#include "speedtree_forest.h"
#include "speedtree_convert_type.h"

#include <xray/render/core/res_effect.h>
#include "material.h"

#include "effect_decal_mask.h"

#include <xray/collision/space_partitioning_tree.h>

#include <xray/render/core/options.h>

namespace xray {
namespace render {
	
stage_forward::stage_forward(renderer_context* context) : stage( context)
{
	m_tree_position_and_scale_parameter		= backend::ref().register_constant_host( "tree_position_and_scale", rc_float);
	m_tree_rotation_parameter				= backend::ref().register_constant_host( "tree_rotation", rc_float);
	m_far_fog_color_and_distance			= backend::ref().register_constant_host( "far_fog_color_and_distance", rc_float );
	m_near_fog_distance						= backend::ref().register_constant_host( "near_fog_distance", rc_float );
	
	effect_manager::ref().create_effect<effect_decal_mask>(&m_opaque_geometry_mask_effect);
	
	m_enabled								=options::ref().m_enabled_forward_stage;
}

bool stage_forward::is_effects_ready() const
{
	return m_opaque_geometry_mask_effect.c_ptr() != NULL;
}

stage_forward::~stage_forward()
{
	
}

void stage_forward::execute()
{
	if (!is_effects_ready())
		return;
	
	if (!is_enabled())
	{
		execute_disabled();
		return;
	}
	
	BEGIN_CPUGPU_TIMER(statistics::ref().forward_stage_stat_group.execute_time);
	
	backend::ref().set_render_targets			(&*m_context->m_targets->m_rt_generic_0, 0, 0, 0);
	backend::ref().reset_depth_stencil_target	();
	
	// TODO: Get dynamic visuals once.
	{
		PIX_EVENT( stage_forward);
		
		m_context->scene()->select_models( m_context->get_culling_vp(), m_dynamic_visuals);
		
		render_surface_instances::iterator it_d			= m_dynamic_visuals.begin();
		render_surface_instances::const_iterator	end_d	= m_dynamic_visuals.end();
		
		for ( ; it_d != end_d; ++it_d)
		{
			render_surface_instance& instance = *(*it_d);
			material_effects& me		= instance.m_render_surface->get_material_effects();
			render_geometry& geometry	= instance.m_render_surface->m_render_geometry;
			
			if ( !me.stage_enable[forward_render_stage])
				continue;
			
			m_context->set_w( *instance.m_transform );
			me.m_effects[forward_render_stage]->apply();
			geometry.geom->apply();
			
			//backend::ref().set_ps_constant( m_far_fog_color_and_distance, float4(m_context->scene_view()->post_process_parameters().environment_far_fog_color, m_context->scene_view()->post_process_parameters().environment_far_fog_distance));
			//backend::ref().set_ps_constant( m_near_fog_distance, m_context->scene_view()->post_process_parameters().environment_near_fog_distance);
			backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, geometry.primitive_count*3, 0, 0);
		}
	}
	
	// SpeedTree
	
	/*if (options::ref().m_enabled_draw_speedtree)
	{
		speedtree_forest::tree_render_info_array_type visible_trees;
		math::float3 const& view_position = m_context->get_v_inverted().c.xyz();
		m_context->scene()->get_speedtree_forest()->get_visible_tree_components(m_context, view_position, true, visible_trees);
		
		for (speedtree_forest::tree_render_info_array_type::iterator it=visible_trees.begin(); it!=visible_trees.end(); ++it)
		{
			if (!it->tree_component->get_material_effects().stage_enable[forward_render_stage] || !it->tree_component->get_material_effects().m_effects[forward_render_stage])
				continue;
			
			
			// TODO: fix
			//if (it->tree_component->get_geometry_type()==SpeedTree::GEOMETRY_TYPE_VERTICAL_BILLBOARDS)
			//	continue;
			
			it->tree_component->get_material_effects().m_effects[forward_render_stage]->apply();
			
			//backend::ref().set_ps_constant( m_object_transparency_scale_parameter, 1.0f);
			
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
	*/
	
	BEGIN_TIMER(statistics::ref().forward_decals_stat_group.execute_time);
	
	// Decals
	scene::decal_instance_list_type& decals		= m_context->scene()->decals();
	collision::objects_type decals_objects		= g_allocator;
	
	backend::ref().reset_depth_stencil_target	();
	
	decals_objects.reserve						(decals.size());
	
	math::frustum frustum						= m_context->get_culling_vp();
	
	m_context->scene()->decals_tree().cuboid_query(u32(-1), frustum, decals_objects);
	
	for ( collision::objects_type::iterator i = decals_objects.begin(), e = decals_objects.end(); i != e; ++i )
	{
		decal_instance* decal					= static_cast_checked<decal_instance*>((*i)->user_data());
		
		statistics::ref().forward_decals_stat_group.num_decal_draw_calls.value += 
			decal->draw(m_context, m_opaque_geometry_mask_effect, forward_render_stage);
		
		statistics::ref().forward_decals_stat_group.num_decals.value++;
	}
	
	END_TIMER;

	END_CPUGPU_TIMER;
	
	m_context->set_w					( float4x4().identity() );

	backend::ref().reset_render_targets();
	backend::ref().reset_depth_stencil_target();
}

} // namespace render
} // namespace xray
