////////////////////////////////////////////////////////////////////////////
//	Created		: 05.07.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/backend.h>
#include "stage_decals_accumulate.h"
#include "renderer_context.h"
#include "renderer_context_targets.h"
#include <xray/render/core/pix_event_wrapper.h>
#include "scene.h"
#include "render_model.h"
#include <xray/render/core/res_effect.h>
#include <xray/render/core/res_geometry.h>
#include "statistics.h"
#include "scene_view.h"

#include <xray/render/core/effect_manager.h>
#include <xray/render/core/render_target.h>
#include "vertex_formats.h"
#include <xray/render/core/effect_options_descriptor.h>
#include <xray/render/core/untyped_buffer.h>

#include <xray/render/core/res_texture.h>
#include <xray/render/core/custom_config.h>
#include <xray/render/core/resource_manager.h>
#include "material.h"
#include "effect_decal_mask.h"

#include <xray/collision/space_partitioning_tree.h>

#include <xray/render/core/options.h>

namespace xray {
namespace render {
	
stage_decals_accumulate::stage_decals_accumulate(renderer_context* context):
	stage(context)
{
	effect_manager::ref().create_effect<effect_decal_mask>(&m_opaque_geometry_mask_effect);
	
	m_enabled = options::ref().m_enabled_decals_accumulate_stage;
}

stage_decals_accumulate::~stage_decals_accumulate()
{
	
}

bool stage_decals_accumulate::is_effects_ready() const
{
	return m_opaque_geometry_mask_effect.c_ptr() != NULL;
}

void stage_decals_accumulate::execute()
{
	PIX_EVENT( stage_decals_accumulate);
	
	if (!is_effects_ready())
		return;
	
	if (!is_enabled())
	{
		execute_disabled();
		return;
	}
	
	BEGIN_TIMER(statistics::ref().deferred_decals_stat_group.execute_time);
	
	scene::decal_instance_list_type& decals		= m_context->scene()->decals();
	//collision::objects_type decals_objects		= g_allocator;
	
	backend::ref().set_render_targets			( 
		&*m_context->m_targets->m_rt_decals_diffuse, // DXGI_FORMAT_R8G8B8A8_UNORM
		&*m_context->m_targets->m_rt_decals_normal,	 // DXGI_FORMAT_R16G16B16A16_FLOAT
		0,
		0
	);
	backend::ref().reset_depth_stencil_target	();
	math::color const clear_color				(0.0f, 0.0f, 0.0f, 0.0f);
	backend::ref().clear_render_targets			(
		clear_color,
		clear_color,
		clear_color,
		clear_color
	);
	
	//decals_objects.reserve						(decals.size());
	
	//math::frustum frustum						= m_context->get_culling_vp();
	decal_instance* decal						= decals.front();
	//for ( collision::objects_type::iterator i = decals_objects.begin(), e = decals_objects.end(); i != e; ++i )
	while (decal)
	{
		//decal_instance* decal					= static_cast_checked<decal_instance*>((*i)->user_data());
		
		statistics::ref().deferred_decals_stat_group.num_decal_draw_calls.value += 
			decal->draw(m_context, m_opaque_geometry_mask_effect, decals_accumulate_render_stage);
		
		statistics::ref().deferred_decals_stat_group.num_decals.value++;
		decal									= decals.get_next_of_object(decal);
	}
	backend::ref().reset_render_targets();
	backend::ref().reset_depth_stencil_target();
	END_TIMER;
}

} // namespace render
} // namespace xray


	
	
	
#if 0
	PIX_EVENT( stage_decals_accumulate);
	
	backend::ref().set_render_targets			( 
		&*m_context->m_targets->m_rt_decals_diffuse, // DXGI_FORMAT_R8G8B8A8_UNORM
		&*m_context->m_targets->m_rt_decals_normal,	 // DXGI_FORMAT_R16G16B16A16_FLOAT
		0,
		0
	);
	backend::ref().reset_depth_stencil_target	();
	math::color const clear_color				(0.0f, 0.0f, 0.0f, 0.0f);
	backend::ref().clear_render_targets			(
		clear_color,
		clear_color,
		clear_color,
		clear_color
	);
	
	scene::decal_instance_list_type& decals		= m_context->scene()->decals();
	
	collision::objects_type decals_objects		= g_allocator;
	decals_objects.reserve						(decals.size());
	
	math::frustum frustum						= m_context->get_culling_vp();
	m_context->scene()->decals_tree().cuboid_query(u32(-1), frustum, decals_objects);
	
	//for ( collision::objects_type::const_iterator i = decals_objects.begin(), e = decals_objects.end(); i != e; ++i )
	decal_instance* decal						= decals.front();
	while (decal)
	{
		//decal_instance* decal					= static_cast<decal_instance*>((*i)->user_data());
		
		// TODO:
		if (!decal->get_properties().projection_on_static_geometry)
			continue;
		
		float4x4 decal_world_matrix				= decal->get_properties().transform;
		decal_world_matrix.set_scale			(float3(1.0f, 1.0f, 1.0f));
		
		float3 const scale						= decal->get_properties().transform.get_scale();
		float3 const decal_direction			= math::normalize_safe(decal_world_matrix.k.xyz(), float3(0.0f, 0.0f, 1.0f));
		decal_world_matrix.c					= float4(decal_world_matrix.c.xyz() - decal_direction * scale.z, decal_world_matrix.c.w);
		float4x4 const decal_projection_matrix	= math::create_orthographic_projection(
			2.0f * scale.x, 
			2.0f * scale.y, 
			0.01f, 
			2.0f * scale.z
		);
		
		float4x4 decal_view_matrix				= math::float4x4().identity();
		decal_view_matrix.try_invert			(decal_world_matrix);
		float4x4 const world_to_decal_matrix	= decal_view_matrix * decal_projection_matrix;
		
		m_context->scene()->select_models(world_to_decal_matrix, m_dynamic_visuals);
		//m_context->scene()->select_models(m_context->get_culling_vp(), m_dynamic_visuals);
		
		render_surface_instances::iterator it_d			= m_dynamic_visuals.begin();
		render_surface_instances::const_iterator	end_d	= m_dynamic_visuals.end();
		
		for ( ; it_d != end_d; ++it_d)
		{
			render_surface_instance& instance	= (*it_d);
			render_geometry& geometry			= instance.m_render_surface->m_render_geometry;
			
			m_context->set_w						(*instance.m_transform);
			enum_vertex_input_type vertex_input_type = instance.m_render_surface->get_vertex_input_type();
			
			if (!decal->get_effects().effects[vertex_input_type].m_effects[decals_accumulate_render_stage])
				continue;
			
			decal->get_effects().effects[vertex_input_type].m_effects[decals_accumulate_render_stage]->apply();
			backend::ref().set_ps_constant			(m_world_to_decal_parameter, math::transpose(world_to_decal_matrix));
			geometry.geom->apply	();
			
			backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, geometry.primitive_count*3, 0, 0);
		}
		decal										= decals.get_next_of_object(decal);
	}
#endif // #if 0
