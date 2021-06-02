////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "stage_pre_lighting.h"
#include <xray/render/core/pix_event_wrapper.h>
#include "renderer_context.h"
#include <xray/render/core/backend.h>
#include "renderer_context_targets.h"

namespace xray {
namespace render {

void stage_pre_lighting::execute( )
{
	PIX_EVENT( stage_pre_lighting );
	
	if (!is_enabled())
	{
		execute_disabled();
		return;
	}

	m_context->m_light_marker_id = 1<<stencil_light_marker_shift;
	//backend::ref().set_render_targets( &*m_context->m_rt_accumulator, 0, 0, 0);
	backend::ref().set_render_targets( &*m_context->m_targets->m_rt_accumulator_diffuse, &*m_context->m_targets->m_rt_accumulator_specular, 0, 0);
	backend::ref().reset_depth_stencil_target();

	backend::ref().clear_render_targets( math::color( 0.0f, 0.0f, 0.0f, 0.0f));
//	backend::ref().clear_depth_stencil( D3D_CLEAR_DEPTH | D3D_CLEAR_STENCIL, 1.0f, 0);

	m_context->set_w					( float4x4().identity() );

	backend::ref().reset_render_targets();
	backend::ref().reset_depth_stencil_target();
}

} // namespace render
} // namespace xray
