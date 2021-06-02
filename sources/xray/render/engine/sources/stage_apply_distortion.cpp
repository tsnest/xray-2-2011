////////////////////////////////////////////////////////////////////////////
//	Created		: 01.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "stage_apply_distortion.h"
#include <xray/render/core/effect_manager.h>
#include "effect_apply_distortion.h"
#include <xray/render/core/pix_event_wrapper.h>
#include <xray/render/core/backend.h>
#include "renderer_context.h"
#include "renderer_context_targets.h"
#include "vertex_formats.h"
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/res_effect.h>
#include <xray/render/core/effect_options_descriptor.h>
#include "statistics.h"

namespace xray {
namespace render {
	
stage_apply_distortion::stage_apply_distortion(renderer_context* context) : stage( context)
{
	effect_manager::ref().create_effect<effect_apply_distortion>(&m_sh_apply_distortion);
}

stage_apply_distortion::~stage_apply_distortion()
{
	
}

bool stage_apply_distortion::is_effects_ready() const
{
	return m_sh_apply_distortion.c_ptr() != NULL;
}

void stage_apply_distortion::execute()
{
	PIX_EVENT( stage_apply_distortion);
	
	if (!is_effects_ready())
		return;
	
	if (!is_enabled())
	{
		execute_disabled();
		return;
	}
	
	//backend::ref().flush_rt_shader_resources();
	
	BEGIN_CPUGPU_TIMER(statistics::ref().distortion_pass_stat_group.apply_time);
	
	backend::ref().set_render_targets( &*m_context->m_targets->m_rt_generic_0, 0, 0, 0);
	backend::ref().clear_render_targets(math::color(0,0,0,0));
	backend::ref().reset_depth_stencil_target();

	//D3D_VIEWPORT viewport;
	//backend::ref().get_viewport( viewport);
	
	float	w = float( backend::ref().target_width());			
	float	h = float( backend::ref().target_height());			
	
	u32		color = math::color_rgba( 255u, 255u, 255u, 255u );
	
	// Adjust texture coord that center of pixel matches center of texel
	float2	p0( 0.5f/w, 0.5f/h);
	float2	p1( ( w+0.5f)/w, ( h+0.5f)/h);
	
	// Fill vertex buffer
	u32		offset;
	
	//FIX ME: May be we can fill this vb's once????
	vertex_formats::TL* pv = ( vertex_formats::TL*)backend::ref().vertex.lock( 4, sizeof(vertex_formats::TL)/*m_context->m_g_quad_uv->get_stride()*/, offset);
	pv->set( 0, h, 0, 1.0, color, p0.x, p1.y); pv++;
	pv->set( 0, 0, 0, 1.0, color, p0.x, p0.y); pv++;
	pv->set( w, h, 0, 1.0, color, p1.x, p1.y); pv++;
	pv->set( w, 0, 0, 1.0, color, p1.x, p0.y); pv++;
	backend::ref().vertex.unlock();
	
	// Setup rendering states and objects
	m_context->m_g_quad_uv->apply();
	m_sh_apply_distortion->apply();
	backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 2*3, 0, offset);
	
	END_CPUGPU_TIMER;
	
	m_context->set_w					( float4x4().identity() );


	backend::ref().reset_render_targets();
	backend::ref().reset_depth_stencil_target();
}

} // namespace render
} // namespace xray
