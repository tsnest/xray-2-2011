////////////////////////////////////////////////////////////////////////////
//	Created		: 29.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/state_cache.h>
#include <xray/render/core/device.h>
#include "com_utils.h"

namespace xray {
namespace render {

template class state_cache<ID3DRasterizerState,		D3D_RASTERIZER_DESC>;
template class state_cache<ID3DDepthStencilState,	D3D_DEPTH_STENCIL_DESC>;		
template class state_cache<ID3DBlendState,			D3D_BLEND_DESC>	;			
template class state_cache<ID3DSamplerState,		D3D_SAMPLER_DESC>;			

template <class IDeviceState, class state_desc>
state_cache<IDeviceState, state_desc>::state_cache()
{
	static const int iMasRSStates = 10;
	states.reserve( iMasRSStates);
}

template <class IDeviceState, class state_desc>
state_cache<IDeviceState, state_desc>::~state_cache()
{
	clear_state_array();
//	ASSERT( states.empty());
}


template <class IDeviceState, class state_desc>
void state_cache<IDeviceState, state_desc>::clear_state_array()
{
	for ( u32 i=0; i<states.size(); ++i)
		safe_release( states[i].state);

	states.erase(states.begin(),states.end());
}

template <>
void state_cache<ID3DRasterizerState, D3D_RASTERIZER_DESC>::create_state( D3D_RASTERIZER_DESC desc, 
																		 ID3DRasterizerState** ppIState)
{
	CHECK_RESULT( device::ref().d3d_device()->CreateRasterizerState( &desc, ppIState));
}

template <>
void state_cache<ID3DDepthStencilState, D3D_DEPTH_STENCIL_DESC>::create_state( D3D_DEPTH_STENCIL_DESC desc, 
																			   ID3DDepthStencilState** ppIState)
{
	CHECK_RESULT( device::ref().d3d_device()->CreateDepthStencilState( &desc, ppIState));
}

template <>
void state_cache<ID3DBlendState, D3D_BLEND_DESC>::create_state( D3D_BLEND_DESC desc, 
																ID3DBlendState** ppIState)
{
	CHECK_RESULT( device::ref().d3d_device()->CreateBlendState( &desc, ppIState));
}

template <>
void state_cache<ID3DSamplerState, D3D_SAMPLER_DESC>::create_state( D3D_SAMPLER_DESC desc, 
																	ID3DSamplerState** ppIState)
{
	CHECK_RESULT( device::ref().d3d_device()->CreateSamplerState( &desc, ppIState));
}


/*
template <>
void 
state_cache<ID3DxxRasterizerState, D3D_RASTERIZER_DESC>
::ResetDescription( D3D_RASTERIZER_DESC &desc)
{
	ZeroMemory( &desc, sizeof( desc));
	desc.FillMode = D3D_FILL_SOLID;
	desc.CullMode = D3Dxx_CULL_BACK;
	desc.FrontCounterClockwise = FALSE;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;
	desc.SlopeScaledDepthBias = 0.0f;
	desc.DepthClipEnable = TRUE;
	desc.ScissorEnable = FALSE;
	desc.MultisampleEnable = FALSE;
	desc.AntialiasedLineEnable = FALSE;
}

template <>
void 
state_cache<ID3DxxDepthStencilState, D3D_DEPTH_STENCIL_DESC>
::ResetDescription( D3D_DEPTH_STENCIL_DESC &desc)
{
	ZeroMemory( &desc, sizeof( desc));
	desc.DepthEnable = TRUE;
	desc.DepthWriteMask = D3D_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3Dxx_COMPARISON_LESS;
	desc.StencilEnable = TRUE;
	desc.StencilReadMask = 0xFF;
	desc.StencilWriteMask = 0xFF;

	desc.FrontFace.StencilFailOp = D3Dxx_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp = D3Dxx_STENCIL_OP_KEEP;
	desc.FrontFace.StencilPassOp = D3Dxx_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFunc = D3Dxx_COMPARISON_ALWAYS;

	desc.BackFace.StencilFailOp = D3Dxx_STENCIL_OP_KEEP;
	desc.BackFace.StencilDepthFailOp = D3Dxx_STENCIL_OP_KEEP;
	desc.BackFace.StencilPassOp = D3Dxx_STENCIL_OP_KEEP;
	desc.BackFace.StencilFunc = D3Dxx_COMPARISON_ALWAYS;
}

template <>
void 
state_cache< ID3DxxBlendState , D3D_BLEND_DESC >
::ResetDescription( D3D_BLEND_DESC &desc)
{
	ZeroMemory( &desc, sizeof( desc));

	desc.AlphaToCoverageEnable = FALSE;
	desc.SrcBlend = D3Dxx_BLEND_ONE;
	desc.DestBlend = D3Dxx_BLEND_ZERO;
	desc.BlendOp = D3Dxx_BLEND_OP_ADD;
	desc.SrcBlendAlpha = D3Dxx_BLEND_ONE;
	desc.DestBlendAlpha = D3Dxx_BLEND_ZERO;
	desc.BlendOpAlpha = D3Dxx_BLEND_OP_ADD;

	for ( int i=0; i<8; ++i)
	{
		desc.BlendEnable[i] = FALSE;
		desc.RenderTargetWriteMask[i] = D3Dxx_COLOR_WRITE_ENABLE_ALL;
	}
}
*/

} // namespace render
} // namespace xray
