////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "stage_pre_lighting.h"


namespace xray {
namespace render{

void stage_pre_lighting::execute( )
{
	PIX_EVENT( stage_pre_lighting );
	
	m_context->m_light_marker_id = stencil_light_marker_start;
	set_rt	(m_context->m_rt_accumulator, 0, 0, m_context->m_base_zb);
	backend::get_ref().clear(D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
}

} // namespace render
} // namespace xray
