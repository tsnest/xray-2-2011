////////////////////////////////////////////////////////////////////////////
//	Created		: 16.03.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/custom_config.h>
#include "stage_debug.h"
#include <xray/render/core/backend.h>
#include "renderer_context.h"
#include "renderer_context_targets.h"
#include <xray/render/core/pix_event_wrapper.h>
#include "scene.h"
#include "render_model.h"
#include <xray/render/core/res_effect.h>
#include <xray/render/core/res_geometry.h>
#include "material.h"


namespace xray {
namespace render {
	
stage_debug::stage_debug(renderer_context* context) : stage( context)
{
	
}

stage_debug::~stage_debug()
{
	
}

void stage_debug::execute()
{
	if (!is_enabled())
	{
		execute_disabled();
		return;
	}
	backend::ref().set_render_targets( &*m_context->m_targets->m_rt_present, 0, 0, 0);
	
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

			if ( !me.stage_enable[debug_render_stage])
				continue;
			
			m_context->set_w						( *instance.m_transform );
			me.m_effects[debug_render_stage]->apply();
			geometry.geom->apply();
			backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, geometry.primitive_count*3, 0, 0);
		}
	}
	backend::ref().reset_render_targets();
	m_context->set_w				(float4x4().identity());
}

} // namespace render
} // namespace xray
