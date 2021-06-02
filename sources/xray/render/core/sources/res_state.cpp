////////////////////////////////////////////////////////////////////////////
//	Created		: 29.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/res_state.h>
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/backend.h>

namespace xray {
namespace render {

res_state::res_state( ID3DRasterizerState*	rasterizer_state, 
						ID3DDepthStencilState*		depth_stencil_state, 
						ID3DBlendState*				blend_state, 
						u32							stencil_ref):
m_rasterizer_state			( rasterizer_state),
m_depth_stencil_state		( depth_stencil_state),
m_blend_state				( blend_state),
m_stencil_ref				( stencil_ref),
m_is_registered				( false )
{
	
}

res_state::~res_state()
{
	// m_state is released in resource_manager cache.
}

void res_state::destroy_impl	() const
{
	resource_manager::ref().release( this );
}

void res_state::apply() const
{
	backend::ref().set_rasterizer_state		( m_rasterizer_state);
	backend::ref().set_depth_stencil_state	( m_depth_stencil_state);
	backend::ref().set_blend_state			( m_blend_state);
	backend::ref().set_stencil_ref			( m_stencil_ref);
}

} // namespace render
} // namespace xray
